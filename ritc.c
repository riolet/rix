#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>

#include "ritc.h"

typedef enum {false, true} bool;

#define BUFFLEN 1024
#define LABELMAX 8096
#define STACKDEP 1024
#define MAXSCOPE 64
#define EVAL_BUFF_MAX_LEN 4096
#define COMPILER_SEP "_$_"

FILE *file;
FILE *outMainFile;
FILE *outHeaderFile;

bool hitEOF;

Object* root;
Object* current;


void handleEOF() {
    hitEOF = true;
}

Object* funcHeader(char* returnType, char* funcName, Object* parameters) {
  if (returnType == 0) {
    returnType = "void";
  }
  ListString* types = parameters->paramTypes;
  ListString* names = parameters->code;
  //TODO: check funcName is undefined or function type
  //TODO: check returnType is a valid Type
  //TODO: change current to equal result
  char funcdef[BUFFLEN];
  int funcdef_pos = 0;
  int fullname_pos = 0;
  funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "%s ", returnType);
  fullname_pos = funcdef_pos;
  funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "%s", funcName);

  while(types != 0) {
    funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "_%s", types->value);
    types = types->next;
  }


  printf("\tfunction full name: '%s'\n", &funcdef[fullname_pos]);
  Object* result = CreateObject(funcName, &funcdef[fullname_pos], current, Function, returnType);
  funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "(");

  //add parameters to the function
  types = parameters->paramTypes;
  //assuming for every type there is a name
  while(types != 0) {
    addSymbol(result, CreateObject(names->value, names->value, 0, Variable, types->value));
    addParam(result, types->value);
    if (types->next == 0) {
        funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "%s %s", types->value, names->value);
    } else {
        funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, "%s %s,", types->value, names->value);
    }
    names = names->next;
    types = types->next;
  }
  funcdef_pos += snprintf(&funcdef[funcdef_pos], BUFFLEN - funcdef_pos, ") {");

  printf("\tfunction code: '%s'\n", funcdef);
  addCode(result, funcdef);
  addSymbol(current, result);
  current = result;
  return result;
}

void doneFunction(Object* tree) {
  addCode(tree, "}");
  current = root;
}

Object* funcParameters(Object* paramList, char* paramType, char* paramName) {
    //TODO: check if type is actually a defined type
    //TODO: check paramType is a valid Type

    Object* result;
    if (paramList == 0) {
        result = CreateObject(0, 0, 0, Undefined, 0);
    } else {
        result = paramList;
    }
    addParam(result, paramType);
    addCode(result, paramName);
    return result;
}

Object* concatParams(Object* existing, Object* newParam) {
    Object* result = CreateObject(0, 0, 0, Expression, 0);
    ListString* paramIter;
    ListString* codeIter;
    int paramLen;
    int codeLen;
    char* lastCode;
    char* lastParam;
    char newCode[BUFFLEN];


    //insert any declarations from the old objects
    paramIter = existing->paramTypes;
    codeIter = existing->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while(codeLen > 0 && codeLen > paramLen) {
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert any declarations from the new object
    paramIter = newParam->paramTypes;
    codeIter = newParam->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while(codeLen > 0 && codeLen > paramLen) {
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert parameters of old objects
    paramIter = existing->paramTypes;
    codeIter = existing->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while(codeLen > 0) {
        if (codeLen > paramLen) {
            codeLen--;
            codeIter = codeIter->next;
            continue;
        }
        addCode(result, codeIter->value);
        addParam(result, paramIter->value);
        codeLen--;
        codeIter = codeIter->next;
        paramIter = paramIter->next;
    }

    //insert parameters of new objects
    paramIter = newParam->paramTypes;
    codeIter = newParam->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while(codeLen > 0) {
        if (codeLen > paramLen) {
            codeLen--;
            codeIter = codeIter->next;
            continue;
        }
        addCode(result, codeIter->value);
        addParam(result, paramIter->value);
        codeLen--;
        codeIter = codeIter->next;
        paramIter = paramIter->next;
    }

    return result;
}

Object* completeExpression(Object* expression) {
    if (expression == 0) {
        warningMsg("expression was null\n");
        return 0;
    }
    char buffer[BUFFLEN];
    ListString* code = expression->code;
    while (code != 0) {
        snprintf(buffer, BUFFLEN, "%s;", code->value);
        addCode(current, buffer);
        code = code->next;
    }
    return current;
}

Object* makeReturn(Object* expression) {
    if (expression == 0) {
        warningMsg("makeReturn: Cannot return null expression\n");
        return 0;
    }
    //add "return" to the last expression
    ListString* line = expression->code;
    if (line == 0) {
        warningMsg("makeReturn: Nothing to add a return to\n");
    }
    while (line->next != 0) {
        line = line->next;
    }
    char* oldCode = line->value;
    char newCode[BUFFLEN];
    snprintf(newCode, BUFFLEN, "return %s", oldCode);
    //TODO: this violates encapsulation
    free(line->value);
    line->value = strdup(newCode);
    return expression;
}

Object* conjugateAssign(Object* subject, Object* verb, Object* objects) {
    if (subject == 0) {
        criticalError(ERROR_ParseError, "Cannot assign to nothing.\n");
    }
    ListString* paramIter;
    Object* realVerb;
    Object* result;
    char verbname[BUFFLEN];
    int verbname_pos = 0;

    //build base name of verb (e.g. "+" becomes "plus")
    if (!strcmp(verb->name, "="))       { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "assign"); }
    else                                { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name); }

    //build final name for verb given object
    if (objects) {
        paramIter = objects->paramTypes;
        while (paramIter != 0) {
            verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "_%s", paramIter->value);
            paramIter = paramIter->next;
        }
    }

    //search for the definition of that object
    printf("asgn: fullVerbName: %s\n", verbname);
    realVerb = findFunctionByFullName(verbname);
    if (realVerb == 0 && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find function named %s.\n", verbname);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (realVerb == 0) {
        //must be literal = or similar.
        result = CreateObject(0, 0, 0, Expression, objects->paramTypes->value);
        addParam(result, objects->paramTypes->value);
        ListString* objectCode = objects->code;
        while (objectCode != 0) {
            if (objectCode->next == 0) {
              //last line
              snprintf(verbname, BUFFLEN, "%s = %s", subject->code->value, objectCode->value);
            } else {
              //every line except last
              addCode(result, objectCode->value);
            }
            objectCode = objectCode->next;
        }


        if (!subject->returnType) {
            ///Add Subject declaration if Subject didn't previously exist
            char declaration[BUFFLEN];
            snprintf(declaration, BUFFLEN, "%s %s", objects->paramTypes->value, subject->fullname);
            addCode(result, declaration);
            Object* variable = CreateObject(subject->name, subject->fullname, 0, Variable, objects->paramTypes->value);
            addSymbol(current, variable);
        } else {
            ///Check compatible types if Subject exists
            if (strcmp(subject->returnType, objects->paramTypes->value)) {
                if (!(
                    ( strcmp(subject->returnType, "Integer")
                      || strcmp(subject->returnType, "Float")
                    ) &&
                    ( strcmp(objects->paramTypes->value, "Integer")
                      || strcmp(objects->paramTypes->value, "Float")
                    )))
                {
                    char error[BUFFLEN];
                    snprintf(error, BUFFLEN, "%s (%s) cannot be assigned type %s\n", subject->name, subject->returnType, objects->paramTypes->value);
                    criticalError(ERROR_IncompatibleTypes, error);
                }
            }
        }

        addCode(result, verbname);
        printf("code line: %s\n", verbname);
        return result;
    }

    //build code line statement invoking that verb.
    verbname_pos = 0;
    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s(", realVerb->fullname);
    if (subject != 0) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", subject->code->value); }
    paramIter = objects->code;
    while (paramIter != 0) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", paramIter->value);
        paramIter = paramIter->next;
    }

    if (verbname[verbname_pos-1] == ',') { verbname_pos--; } //to overwrite the last comma
    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, ") {");
    printf("code line: %s", verbname);

    result = CreateObject(0, 0, 0, Expression, realVerb->returnType);
    addParam(result, realVerb->returnType);
    ///TDOO: add subject declaration if subject didn't previously exist!!!
    addCode(result, verbname);

    return result;

}

Object* conjugate(Object* subject, Object* verb, Object* objects) {
    ListString* paramIter;
    ListString* codeIter;
    Object* realVerb;
    Object* result;
    char verbname[BUFFLEN];
    int verbname_pos = 0;
    //if this is an assignment verb, treat it differently.
    if (!strcmp(verb->name, "=")) {
        return conjugateAssign(subject, verb, objects);
    }

    //build base name of verb (e.g. "+" becomes "plus")

    if (!strcmp(verb->name, "+"))       { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "plus"); }
    else if (!strcmp(verb->name, "-"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "minus"); }
    else if (!strcmp(verb->name, "*"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "times"); }
    else if (!strcmp(verb->name, "/"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "slash"); }
    else if (!strcmp(verb->name, "^^")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "exponent"); }
    else                                { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name); }


    //build final name for verb given object
    if (subject != 0) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "_%s", subject->returnType); }

    if (objects != 0) {
        paramIter = objects->paramTypes;
        while (paramIter != 0) {
            verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "_%s", paramIter->value);
            paramIter = paramIter->next;
        }
    }

    //search for the definition of that object
    printf("fullVerbName: %s\n", verbname);
    realVerb = findFunctionByFullName(verbname);
    if (realVerb == 0 && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find function named %s.\n", verbname);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (realVerb == 0) {
        //must be + or / or such...
        if (objects == 0) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Did you forget an operand? %s %s ???\n", subject->code->value, verb->name);
            criticalError(ERROR_InvalidArguments, error);
        }
        result = CreateObject(0, 0, 0, Expression, objects->paramTypes->value);
        if (!strcmp(subject->returnType, "Float") || !strcmp(objects->code->value, "Float")) {
            addParam(result, "Float");
        } else {
            addParam(result, objects->paramTypes->value);
        }
        snprintf(verbname, BUFFLEN, "%s %s %s", subject->code->value, verb->name, objects->code->value);

        addCode(result, verbname);
        return result;
    }
    //build code line statement invoking that verb.
    verbname_pos = 0;
    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s(", realVerb->fullname);
    result = CreateObject(0, 0, 0, Expression, realVerb->returnType);

    if (subject != 0) {
        //TODO: can there ever be a declaration in the subject? (i.e. multiple code lines?);
        if (subject->code->next != 0) {
            criticalError(ERROR_ParseError, "Multiple code lines found in subject.\n");
        }
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", subject->code->value);
    }

    if (objects != 0) {
        codeIter = objects->code;
        int numParams = listlen(objects->paramTypes);
        int numCodes = listlen(objects->code);
        while (numCodes > numParams) {
                //pull out all declarations separately.
                addCode(result, codeIter->value);
                codeIter = codeIter->next;
                numCodes--;
        }
        while (codeIter != 0) {
            if (codeIter->next == 0) {
                //this is the last entry
                verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", codeIter->value);
            } else {
                verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", codeIter->value);
            }
            codeIter = codeIter->next;
        }
    }
    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, ")");

    addParam(result, realVerb->returnType);
    addCode(result, verbname);

    return result;
}

Object* verbMathOp(char* verb) {
    printf("verbMathOp(%s)\n", verb);
    Object* result = CreateObject(verb, verb, 0, Function, 0);
    return result;
}

Object* verbAssignment(char* verb) {
    printf("verbAssignment(%s)\n", verb);
    Object* result = CreateObject(verb, verb, 0, Function, 0);
    return result;
}

Object* verbIdent(char* verb) {
    printf("verbIdent(%s)\n", verb);
    Object* result = findByName(verb);
    if (result == 0) {
        char error[BUFFLEN];
        sprintf(error, "Function \"%s(..)\" used before declaration.\n", verb);
        criticalError(ERROR_UndefinedVerb, error);
    }
    return result;
}

Object* parenthesize(Object* expr) {
    printf("parenthesize(%s)\n", expr->code->value);
    if (expr==0) {
        criticalError(ERROR_ParseError, "Object* expr was void in parenthesize. (ritc.c)\n");
    }

    Object* parenthesized = CreateObject(expr->name, expr->fullname, expr->parentScope, Expression, expr->returnType);

    if (expr->code == 0) {
        criticalError(ERROR_ParseError, "Cannot put parentheses around nothing. (ritc.c)\n");
    }

    //copy lines to new object until last line. Put last line in parentheses.
    ListString* code = expr->code;
    char line[BUFFLEN];
    while (code != 0) {
        if (code->next == 0) {
          //last line
          snprintf(line, BUFFLEN, "(%s)", code->value);
          addCode(parenthesized, line);
        } else {
          //all lines except last
          addCode(parenthesized, code->value);
        }
        code = code->next;
    }
    addParam(parenthesized, expr->returnType);
    return parenthesized;
}

Object* objectVerb(Object* verb) {
    printf("objectVerb(%s)\n", verb->name);
    if (verb->paramTypes != 0) {
        criticalError(ERROR_InvalidArguments, "Only zero-argument verbs can be used as objects.\n");
    }
    Object* result = CreateObject(0, 0, 0, Expression, verb->returnType);
    char* buffer = malloc(BUFFLEN);
    snprintf(buffer, BUFFLEN, "%s()", verb->fullname);
    addParam(result, verb->returnType);
    addCode(result, buffer);
    return result;
}

Object* objectIdent(char* ident) {
    printf("objectIdent(%s)\n", ident);
    //TODO: what if `object` is undefined?
    Object* identifier = findByName(ident);
    if (!identifier) {
        char error[BUFFLEN];
        sprintf(error, "Variable \"%s\" used before declaration.\n", ident);
        criticalError(ERROR_UndefinedVariable, error);
    }
    Object* result = CreateObject(0, 0, 0, Expression, identifier->returnType);
    addCode(result, ident);
    addParam(result, identifier->returnType);
    return result;
}

Object* subjectIdent(char* ident) {
    printf("subjectIdent(%s)\n", ident);
    //TODO: what if `object` is undefined?
    Object* result;
    Object* identifier = findByName(ident);

    if (!identifier) {
        result = CreateObject(ident, ident, 0, Variable, Undefined);
    } else {
        result = CreateObject(ident, ident, 0, identifier->type, identifier->returnType);
        addParam(result, identifier->returnType);
    }
    addCode(result, ident);
    return result;
}

Object* objectFloat(float f) {
    printf("objectFloat(%f)\n", f);
    char* buffer = malloc(256);
    sprintf(buffer, "%f", f);
    Object* result = CreateObject(0, 0, 0, Expression, "Float");
    addCode(result, buffer);
    addParam(result, "Float");
    return result;
}

Object* objectInt(int d) {
    printf("objectInt(%d)\n", d);
    char* buffer = malloc(32); // 21 = (log10(2^64)+1)
    snprintf(buffer, 32, "%d", d);
    char* returnBuffer = malloc(9);
    Object* result = CreateObject(0, 0, 0, Expression, "Integer");
    addCode(result, buffer);
    addParam(result, "Integer");
    return result;
}

float simplifyFloat(float left, char* op, float right){
    char error[50];

    switch (op[0]) {
    case '+':
        return left + right;
    case '-':
        return left - right;
    case '*':
        return left * right;
    case '/':
        return left / right;
    default:
        sprintf(error, "simplifyFloat encountered a '%c'\n", op[0]);
        criticalError(ERROR_UnrecognizedSymbol, error);
    }
    return 0.0f;
}

int simplifyInt(int left, char* op, int right){
    char error[50];

    switch (op[0]) {
    case '+':
        return left + right;
    case '-':
        return left - right;
    case '*':
        return left * right;
    case '/':
        return left / right;
    default:
        sprintf(error, "simplifyInt encountered a '%c'\n", op[0]);
        criticalError(ERROR_UnrecognizedSymbol, error);
    }
    return 0;
}

Object* findByName(char* name) {
    Object* result = findByNameInScope(current, name);
    return result;
}

Object* findFunctionByFullName(char* name) {
    Object* result = findByFullNameInScope(current, name);
    if (result == 0) {
        result = findByFullNameInScope(root, name);
    }
    return result;
}

void defineRSLSymbols(Object* root) {
    Object *temp1, *temp2, *temp3, *rslFunc;

    // ==============  Built-in Types ===============

    temp1 = CreateObject("BaseType", "BaseType", 0, Type, 0);
    addSymbol(root, temp1);
    temp2 = CreateObject("Boolean", "BaseType_Boolean", temp1, Type, 0);
    addSymbol(root, temp2);
    temp2 = CreateObject("Ternary", "BaseType_Ternary", temp1, Type, 0);
    addSymbol(root, temp2);
    temp2 = CreateObject("Stream", "BaseType_Stream", temp1, Type, 0);
    addSymbol(root, temp2);
    temp2 = CreateObject("String", "BaseType_String", temp1, Type, 0);
    addSymbol(root, temp2);
    temp2 = CreateObject("Number", "BaseType_Number", temp1, Type, 0);
    addSymbol(root, temp2);
    temp3 = CreateObject("Integer", "Number_Integer", temp2, Type, 0);
    addSymbol(root, temp3);
    temp3 = CreateObject("Float", "Number_Float", temp2, Type, 0);
    addSymbol(root, temp3);

    // ==============  Exponent functions ===============

    rslFunc = CreateObject("exponent", "exponent_Float_Integer", 0, Function, "Float");
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "exponent_Integer_Float", 0, Function, "Float");
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "exponent_Float_Float", 0, Function, "Float");
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "exponent_Integer_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  String Functions ===============

    rslFunc = CreateObject("assign", "assign_String_String", 0, Function, "String");
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "plus_Integer_String", 0, Function, "String");
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "plus_String_Integer", 0, Function, "String");
    addParam(rslFunc, "String");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "plus_Float_String", 0, Function, "String");
    addParam(rslFunc, "Float");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "plus_String_Float", 0, Function, "String");
    addParam(rslFunc, "String");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);

    // ==============  Print Functions ===============

    rslFunc = CreateObject("print", "print_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_Float", 0, Function, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_String", 0, Function, "Integer");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_Stream_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_Stream_Float", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_Stream_String", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("echo", "echo_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo_Float", 0, Function, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo_String", 0, Function, "Integer");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo_Stream_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo_Stream_Float", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo_Stream_String", 0, Function, "Integer");
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
}

int main(int argc,char **argv)
{
    int c,i;
    int errflg=0;
    char *ifile = NULL;
    char *ofile = NULL;
    extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
        case 'o':
            ofile = optarg;
            break;
        case ':':       /* -f or -o without operand */
            fprintf(stderr,
                    "Option -%c requires an operand\n", optopt);
            errflg++;
            break;
        };
    }

    if (errflg) {
        fprintf(stderr, "usage: . . . ");
        exit(2);
    }


    for (i=0; optind < argc; optind++,i++) {
        if (i==0) {
            ifile=argv[optind];
        }
    }


    if (ifile==NULL) {
        errorMsg("No file to compile\n");
        file=fopen("helloworld.rit","r");
    } else {
        file=fopen(ifile,"r");
    }

    char oMainFileName[BUFFLEN];
    char oHeaderFileName[BUFFLEN];

    if (ofile==NULL) {
        strcpy(oMainFileName,"out.c");
        strcpy(oHeaderFileName,"out.h");
    } else {
        snprintf(oMainFileName,BUFFLEN,"%s.c",ofile);
        snprintf(oHeaderFileName,BUFFLEN,"%s.h",ofile);
    }
    outMainFile=fopen(oMainFileName,"w");
    outHeaderFile=fopen(oHeaderFileName,"w");



    errorMsg(ANSI_COLOR_MAGENTA "\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n" ANSI_COLOR_RESET);
    fprintf(outMainFile,"#include \"rsl.h\"\n");
    fprintf(outMainFile,"#include \"%s\"\n",oHeaderFileName);
    fprintf(outMainFile,"int main(void) {\n");


    root = CreateObject("Undefined", "Undefined", 0, CodeBlock, "Integer");
    current = root;
    defineRSLSymbols(root);

    yyin = file;

    //getln();
    hitEOF = false;
    while (!hitEOF) {
        yyparse();
    }

    writeTree(outMainFile, outHeaderFile, root);
    //printTree(root, 0);
    fprintf(outMainFile,"  return 0;\n}\n");
    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(file);

    printf("\n%s compiled successfully.\n", ifile);

    return 0;
}
