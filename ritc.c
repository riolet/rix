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

FILE *file;
FILE *outMainFile;
FILE *outHeaderFile;

bool hitEOF;

Object* root;
Object* scopeStack[MAXSCOPE];
int scope_idx = 0;
Object* current;
char* previous[MAXSCOPE];
int prev_idx = 0;

Object*  scope_pop() { current = scopeStack[--scope_idx];  return scopeStack[scope_idx+1]; }
void scope_push(Object* val) { current = scopeStack[++scope_idx] = val; }

void handleEOF() {
    hitEOF = true;
}

Object* beginClass(char* className, char* parentName) {
    if (!className || !parentName) {
        criticalError(ERROR_ParseError, "Class name mustn't be null.\n");
    }

    //build full name of class:  <ClassName><sep><ParentClassName>
    char fullname[BUFFLEN];
    snprintf(fullname, BUFFLEN, "%s" COMPILER_SEP "%s", className, parentName);

    //get link to parent class definition
    Object* parent = findByName(parentName);
    if (parent == 0) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find definition for '%s'\n", parentName);
        criticalError(ERROR_ParseError, error);
    }

    //TODO: should the type be BaseType or BaseType* ?
    Object* parentReference = CreateObject("parent", "parent", 0, Variable, parent->fullname);
    //Object* selfReference = CreateObject("self", "self", 0, Variable, fullname);
    Object* result = CreateObject(className, fullname, current, Type, 0);
    //addSymbol(result, selfReference);
    addSymbol(result, parentReference);
    addSymbol(current, result);
    scope_push(result);
}

void doneClass(Object* tree) {
    scope_pop();
}

Object* beginFunction(char* returnType, char* funcName, Object* parameters) {
  if (returnType == 0) {
    criticalError(ERROR_ParseError, "Return type mustn't be null.\n");
  }
  ListString* types = parameters->paramTypes;
  ListString* names = parameters->code;
  //TODO: check funcName is undefined or function type
  //TODO: check returnType is a valid Type

  char funcFullName[BUFFLEN];
  int funcFullName_pos = 0;

  if (current->type == Type) {
    funcFullName_pos += snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, "%s" COMPILER_SEP, current->name);
  }

  funcFullName_pos += snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, "%s", funcName);

  while(types != 0) {
    funcFullName_pos += snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, COMPILER_SEP "%s", types->value);
    types = types->next;
  }
  Object* parentScope;
  int i = scope_idx;
  while (i >= 0) {
    if (scopeStack[i]->type != Type) {
        break;
    }
    i--;
  }
  parentScope = scopeStack[i];

  Object* result = CreateObject(funcName, funcFullName, parentScope, Function, returnType);

  if (current->type == Type) {
    result->parentClass = current;
    char pointer[BUFFLEN];
    snprintf(pointer, BUFFLEN, "%s *", current->name);
    addParam(result, pointer);
    addSymbol(result, CreateObject("self", "self", 0, Variable, pointer));
  }

  //add parameters to the function
  types = parameters->paramTypes;
  //assuming for every type there is a name
  while(types != 0) {
    addSymbol(result, CreateObject(names->value, names->value, 0, Variable, types->value));
    addParam(result, types->value);
    names = names->next;
    types = types->next;
  }

  addSymbol(parentScope, result);
  scope_push(result);
  return result;
}

void doneFunction(Object* tree) {
    scope_pop();
}

Object* beginConstructor(Object* parameters) {
    if (current->type != Type) {
        criticalError(ERROR_ParseError, "Constructor can only exist inside a class.\n");
    }
    ListString* types = parameters->paramTypes;
    ListString* names = parameters->code;

    char funcFullName[BUFFLEN];
    int funcFullName_pos = 0;

    funcFullName_pos += snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, "%s" COMPILER_SEP "%s", current->name, current->name);

    while(types != 0) {
        funcFullName_pos += snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, COMPILER_SEP "%s", types->value);
        types = types->next;
    }
    Object* parentScope;
    int i = scope_idx;
    while (i >= 0) {
        if (scopeStack[i]->type != Type) {
            break;
        }
        i--;
    }
    parentScope = scopeStack[i];

    char returnType[BUFFLEN];
    snprintf(returnType, BUFFLEN, "%s *", current->name);

    Object* result = CreateObject(current->name, funcFullName, parentScope, Constructor, returnType);
    result->parentClass = current;



    //add parameters to the function
    types = parameters->paramTypes;
    //assuming for every type there is a name
    while(types != 0) {
        addSymbol(result, CreateObject(names->value, names->value, 0, Variable, types->value));
        addParam(result, types->value);
        names = names->next;
        types = types->next;
    }

    //Add allocation code
    char allocator[BUFFLEN];
    snprintf(allocator, BUFFLEN, "%s self = malloc(sizeof(%s));", returnType, current->name);
    addCode(result, allocator);

    addSymbol(parentScope, result);
    scope_push(result);
    return result;
}

void doneConstructor(Object* tree) {
    scope_pop();
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

Object* declareVariable(char* name, char* type) {
    Object* var = CreateObject(name, name, 0, Variable, type);
    addSymbol(current, var);
    return var;
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

Object* finalize(Object* expression) {
    Object* exprPrev;
    if (strcmp(expression->returnType, "void")) {
        char prevName[BUFFLEN];
        snprintf(prevName, BUFFLEN, COMPILER_SEP "prev" COMPILER_SEP "%s", expression->returnType);
        int length = 0;
        while (!(prevName[length] == ' ' || prevName[length] == '*' || prevName[length] == '\0')) {
            length++;
        }
        prevName[length] = '\0'; //drop the " *" off the end if it's there
        previous[prev_idx] = strdup(prevName);
        exprPrev = conjugateAssign(objectIdent(prevName), verbAssignment("="), expression);
        return exprPrev;
    }
    return expression;
}

void closeBrace() {
    addCode(current, "}");
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

void incPrev() {
    prev_idx++;
}

void decPrev() {
    prev_idx--;
    if (prev_idx < 0) {
        criticalError(ERROR_ParseError, "previous result tracker went below 0. (decPrev, ritc.c)\n");
    }
}

Object* conjugateAssign(Object* subject, Object* verb, Object* objects) {
    if (subject == 0) {
        criticalError(ERROR_ParseError, "Cannot assign to nothing.\n");
    }
    ListString* paramIter;
    Object* realVerb = 0;
    Object* result = 0;
    char verbname[BUFFLEN];
    int verbname_pos = 0;

    if (verb->type == Type) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s" COMPILER_SEP, verb->name);
    }

    //build base name of verb (e.g. "+" becomes "plus")
    if (!strcmp(verb->name, "=")) {
        //handling straight up assignment.
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "assign");
    } else if (!isalpha(verb->name[0])) {
        //handle augmented assigment operators recursively.
        char op[4];
        Object* augment;
        strcpy(op, verb->name);
        if (op[2] == '=') { op[2] = '\0'; }
        if (op[1] == '=') { op[1] = '\0'; }


        augment = conjugate(subject, verbMathOp(op), objects);
        return conjugateAssign(subject, verbAssignment("="), augment);
    } else {
        //handle assignment verbs
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name);
    }

    //build final name for verb given object
    if (objects) {
        paramIter = objects->paramTypes;
        while (paramIter != 0) {
            verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, COMPILER_SEP "%s", paramIter->value);
            paramIter = paramIter->next;
        }
    }

    //search for the definition of that object
    printf("ConjugateAssign: fullVerbName: %s\n", verbname);
    realVerb = findFunctionByFullName(verbname);
    if (realVerb == 0 && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find function named %s.\n", verbname);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (realVerb == 0) {
        //must be literal = or similar.
        if (!objects) {
            criticalError(ERROR_ParseError, "Object of assignment was not found.\n");
        }
        result = CreateObject(0, 0, 0, Expression, objects->paramTypes->value);
        addParam(result, objects->paramTypes->value);
        snprintf(verbname, BUFFLEN, "%s = %s", subject->code->value, objects->code->value);


        if (!subject->returnType) {
            Object* variable = CreateObject(subject->name, subject->fullname, 0, Variable, objects->paramTypes->value);
            addSymbol(current, variable);
        } else {
            //Check compatible types if Subject exists
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
        printf("\tConjugated: %s\n", verbname);
        return result;
    }

    //build code line statement invoking that verb.
    verbname_pos = 0;
    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s(", realVerb->fullname);
    if (subject && objects) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", subject->code->value);
    } else if (subject) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", subject->code->value);
    }
    if (objects) {
        paramIter = objects->code;
        while (paramIter != 0) {
            if (paramIter->next == 0) {
                verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", paramIter->value);
            } else {
                verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,", paramIter->value);
            }
            paramIter = paramIter->next;
        }
    }

    if (subject && subject->returnType == 0) {
        Object* variable = CreateObject(subject->name, subject->fullname, 0, Variable, verb->returnType);
        addSymbol(current, variable);
    }

    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, ")");
    printf("\tConjugated: %s\n", verbname);

    result = CreateObject(0, 0, 0, Expression, "void");

    addParam(result, realVerb->returnType);
    addCode(result, verbname);
    return result;

}

Object* conjugate(Object* subject, Object* verb, Object* objects) {
    ListString* paramIter = 0;
    ListString* codeIter = 0;
    Object* realVerb = 0;
    Object* result = 0;
    char verbname[BUFFLEN];
    char invocation[BUFFLEN];
    int verbname_pos = 0;
    int invoke_pos = 0;
    //if this is an assignment verb, treat it differently.
    if (getFlag(verb, FLAG_ASSIGNMENT)) {
        return conjugateAssign(subject, verb, objects);
    }


    //== Build the fullname for the verb ==
    if (subject) {
        if (subject->returnType == 0) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Variable '%s' used before definition\n", subject->code->value);
            criticalError(ERROR_UndefinedVariable, error);
        }
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", subject->returnType);
        while (verbname[verbname_pos-1] == '*' || verbname[verbname_pos-1] == ' ') {
            verbname_pos--;
        }
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, COMPILER_SEP);
    } else if (verb->type == Type) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s" COMPILER_SEP, verb->name);
    }

    //erase the " *" if it's there from the name

    //build base name of verb (e.g. "+" becomes "plus")
    if (!strcmp(verb->name, "+"))       { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "plus"); }
    else if (!strcmp(verb->name, "-"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "minus"); }
    else if (!strcmp(verb->name, "*"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "times"); }
    else if (!strcmp(verb->name, "/"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "slash"); }
    else if (!strcmp(verb->name, "^^")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "exponent"); }
    else if (!strcmp(verb->name, "<"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmplt"); }
    else if (!strcmp(verb->name, ">"))  { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpgt"); }
    else if (!strcmp(verb->name, "<=")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmplteq"); }
    else if (!strcmp(verb->name, ">=")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpgteq"); }
    else if (!strcmp(verb->name, "==")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpeq"); }
    else if (!strcmp(verb->name, "!=")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpneq"); }
    else if (!strcmp(verb->name, "<>")) { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "compare"); }
    else                                { verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name); }

    //verify objects exists and is a known variable or is an expression composed of known variables
    if (objects) {
        if (!objects->paramTypes) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Variable '%s' used before definition\n", objects->code->value);
            criticalError(ERROR_UndefinedVariable, error);
        }
        paramIter = objects->paramTypes;
        while (paramIter) {
            verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, COMPILER_SEP "%s", paramIter->value);
            paramIter = paramIter->next;
        }
    }

    //search for the definition of that object
    printf("Conjugate: fullVerbName: %s\n", verbname);
    if (verb->type == Type) {
        //look for ctor inside class
        realVerb = findByNameInScope(verb, verbname, true);
    }
    if (!realVerb) {
        realVerb = findFunctionByFullName(verbname);
    }
    if (!realVerb && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        if (subject)
            snprintf(error, BUFFLEN, "Type \"%s\" doesn't have member function \"%s\".\n", subject->returnType, &verbname[strlen(subject->returnType)+1]);
        else
            snprintf(error, BUFFLEN, "Cannot find function named \"%s\".\n", verbname);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (!realVerb) {
        warningMsg("Cannot find verb \"%s\". Assuming \"%s\" is infix operator in C.\n", verbname, verb->name);
        //must be + or / or such...
        if (!objects) {
            char error[BUFFLEN];
            ListString* code = subject->code;
            while (code!=0 && code->next!=0) { code = code->next; }
            snprintf(error, BUFFLEN, "Did you forget an operand? %s %s ???\n", subject->code->value, verb->fullname);
            criticalError(ERROR_InvalidArguments, error);
        }
        if (verb->returnType) {
            result = CreateObject(0, 0, 0, Expression, verb->returnType);
            addParam(result, verb->returnType);
        } else if (!strcmp(subject->returnType, "Float") || !strcmp(objects->paramTypes->value, "Float")) {
            result = CreateObject(0, 0, 0, Expression, "Float");
            addParam(result, "Float");
        } else {
            result = CreateObject(0, 0, 0, Expression, subject->returnType);
            addParam(result, subject->returnType);
        }

        invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN-invoke_pos, "%s %s %s", subject->code->value, verb->fullname, objects->code->value);
        addCode(result, invocation);

        printf("\tConjugated: \"%s\"\n", invocation);
        return result;
    }
    //build code line statement invoking that verb.
    invoke_pos = 0;
    invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s(", realVerb->fullname);
    result = CreateObject(0, 0, 0, Expression, realVerb->returnType);

    if (subject) {
        if (objects)
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s,", subject->code->value);
        else
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s", subject->code->value);
    }

    if (objects) {
        codeIter = objects->code;
        while (codeIter) {
            if (codeIter->next) {
                invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s,", codeIter->value);
            } else {
                //this is the last entry
                invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s", codeIter->value);
            }
            codeIter = codeIter->next;
        }
    }
    invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ")");


    addParam(result, realVerb->returnType);
    addCode(result, invocation);
    printf("\tConjugated: %s\n", invocation);
    return result;
}

Object* injectC(char* code) {
    addCode(current, code);
    return 0;
}



Object* verbMathOp(char* verb) {
    printf("verbMathOp(%s)\n", verb);
    Object* result = CreateObject(verb, verb, 0, Function, 0);
    return result;
}

Object* verbComparison(char* verb) {
    printf("verbComparison(%s)\n", verb);
    Object* result = CreateObject(verb, verb, 0, Function, "Boolean");
    return result;
}

Object* verbTernary() {
    printf("verbTernary(<>)\n");
    Object* result = CreateObject("<>", "<>", 0, Function, "Boolean");
    return result;
}

Object* verbAssignment(char* verb) {
    printf("verbAssignment(%s)\n", verb);
    Object* result = CreateObject(verb, verb, 0, Function, 0);
    setFlags(result, FLAG_ASSIGNMENT);
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

Object* verbCtor(char* type) {
    printf("verbCtor(%s)\n", type);
    Object* result = findByName(type);
    if (result == 0) {
        char error[BUFFLEN];
        sprintf(error, "Cannot find Class \"%s\".\n", type);
        criticalError(ERROR_UndefinedVerb, error);
    }
    return result;
}

Object* parenthesize(Object* expr) {
    printf("parenthesize(%s)\n", expr->code->value);
    char line[BUFFLEN];
    if (expr==0) {
        criticalError(ERROR_ParseError, "Object* expr was void in parenthesize. (ritc.c)\n");
    }

    Object* parenthesized = CreateObject(expr->name, expr->fullname, expr->parentScope, Expression, expr->returnType);

    if (expr->code == 0) {
        criticalError(ERROR_ParseError, "Cannot put parentheses around nothing. (ritc.c)\n");
    }

    snprintf(line, BUFFLEN, "(%s)", expr->code->value);
    addCode(parenthesized, line);

    addParam(parenthesized, expr->returnType);
    return parenthesized;
}

Object* objectIdent(char* ident) {
    printf("objectIdent(%s)\n", ident);
    Object* result;
    Object* identifier = findByName(ident);

    if (!identifier) {
        result = CreateObject(ident, ident, 0, Variable, Undefined);
    } else {
        result = CreateObject(identifier->name, identifier->fullname, 0, identifier->type, identifier->returnType);
        addParam(result, identifier->returnType);
    }
    addCode(result, identifier? identifier->fullname : ident);
    return result;
}

Object* objectFloat(float f) {
    printf("objectFloat(%f)\n", f);
    char buffer[128];
    snprintf(buffer, BUFFLEN, "%f", f);
    Object* result = CreateObject(0, 0, 0, Expression, "Float");
    addCode(result, buffer);
    addParam(result, "Float");
    return result;
}

Object* objectInt(int d) {
    printf("objectInt(%d)\n", d);
    char buffer[32]; // 20 = (log10(2^64))
    snprintf(buffer, 32, "%d", d);
    Object* result = CreateObject(0, 0, 0, Expression, "Integer");
    addCode(result, buffer);
    addParam(result, "Integer");
    return result;
}

Object* objectString(char* string) {
    printf("objectString(%s)\n", string);
	char buffer[BUFFLEN];
	snprintf(buffer, BUFFLEN, "String" COMPILER_SEP "stringlit(%s)", string);
    Object* result = CreateObject(0,0,0, Expression, "String");
	addCode(result, buffer);
	addParam(result, "String");
	return result;
}

Object* objectPrev() {
    printf("objectPrev(%s)\n", previous[prev_idx]);
    return objectIdent(previous[prev_idx]);

    //brute force search of text lines in current scope
    /*
    char* pattern = COMPILER_SEP "prev" COMPILER_SEP;
    char buffer[BUFFLEN];
    strncpy(buffer, pattern, BUFFLEN);
    int lenPattern = strlen(pattern);
    ListString* codeIter = current->code;
    char* line;
    while (codeIter != 0) {
        if (!strncmp(codeIter->value, pattern, lenPattern)) {
          line = codeIter->value;
        }
        codeIter = codeIter->next;
    }
    int lenToken = 0;
    while( !isspace(line[lenPattern]) ) {
        buffer[lenPattern] = line[lenPattern];
        lenPattern++;
    }
    buffer[lenPattern] = 0; //null-terminate the string
    printf("objectPrev found object '%s'\n", buffer);
    exit(0);
    return objectIdent(buffer);
    */
}



Object* findByName(char* name) {
    printf(ANSI_COLOR_YELLOW "Searching for %s in %s\n" ANSI_COLOR_RESET, name, current->name);
    Object* result = findByNameInScope(current, name, false);
    printf(ANSI_COLOR_YELLOW "Found: %s\n" ANSI_COLOR_RESET, result ? result->fullname : "(null)");
    return result;
}

Object* findFunctionByFullName(char* name) {
    printf(ANSI_COLOR_YELLOW "Searching by fullname for %s in %s\n" ANSI_COLOR_RESET, name, current->name);

    Object* result = findByNameInScope(current, name, true);

    printf(ANSI_COLOR_YELLOW "Found: %s\n" ANSI_COLOR_RESET, result ? result->fullname : "(null)");
    return result;
}

void defineRSLSymbols(Object* root) {
    Object *temp1, *temp2, *temp3, *rslFunc;

    // ==============  Built-in Types ===============

    temp1 = CreateObject("BaseType", "BaseType", 0, Type, 0);
    setFlags(temp1, FLAG_EXTERNAL);
    addSymbol(root, temp1);
    temp2 = CreateObject("Boolean", "Boolean" COMPILER_SEP "BaseType", temp1, Type, 0);
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp2 = CreateObject("Ternary", "Ternary" COMPILER_SEP "BaseType", temp1, Type, 0);
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp2 = CreateObject("Stream", "Stream" COMPILER_SEP "BaseType", temp1, Type, 0);
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp2 = CreateObject("String", "String" COMPILER_SEP "BaseType", temp1, Type, 0);
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp2 = CreateObject("Number", "Number" COMPILER_SEP "BaseType", temp1, Type, 0);
    setFlags(temp2, FLAG_EXTERNAL);
    addSymbol(root, temp2);
    temp3 = CreateObject("Integer", "Integer" COMPILER_SEP "Number", temp2, Type, 0);
    setFlags(temp3, FLAG_EXTERNAL);
    addSymbol(root, temp3);
    temp3 = CreateObject("Float", "Float" COMPILER_SEP "Number", temp2, Type, 0);
    setFlags(temp3, FLAG_EXTERNAL);
    addSymbol(root, temp3);

    // ==============  Exponent functions ===============

    rslFunc = CreateObject("exponent", "Float" COMPILER_SEP "exponent" COMPILER_SEP "Integer", 0, Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "Integer" COMPILER_SEP "exponent" COMPILER_SEP "Float", 0, Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "Float" COMPILER_SEP "exponent" COMPILER_SEP "Float", 0, Function, "Float");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("exponent", "Integer" COMPILER_SEP "exponent" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  String Functions ===============

    rslFunc = CreateObject("assign", "String" COMPILER_SEP "assign" COMPILER_SEP "String", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "String", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "Integer" COMPILER_SEP "plus" COMPILER_SEP "String", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "Integer", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "Float" COMPILER_SEP "plus" COMPILER_SEP "String", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("plus", "String" COMPILER_SEP "plus" COMPILER_SEP "Float", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);

    // ==============  Conditional Functions ===============

    rslFunc = CreateObject("if", "Boolean" COMPILER_SEP "if", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("elif", "Boolean" COMPILER_SEP "elif" COMPILER_SEP "Boolean", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("else", "Boolean" COMPILER_SEP "else", 0, Function, "Boolean");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("compare", "Integer" COMPILER_SEP "compare" COMPILER_SEP "Integer", 0, Function, "Ternary");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("pick", "Ternary" COMPILER_SEP "pick" COMPILER_SEP "String" COMPILER_SEP "String" COMPILER_SEP "String", 0, Function, "String");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Ternary");
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    // ==============  Looping Functions ===============

    rslFunc = CreateObject("while", "Boolean" COMPILER_SEP "while", 0, Function, "void");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Boolean");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("for", "for" COMPILER_SEP "Integer" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    setFlags(rslFunc, FLAG_ASSIGNMENT);
    addParam(rslFunc, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);

    // ==============  Print Functions ===============

    rslFunc = CreateObject("print", "print" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print" COMPILER_SEP "Float", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "Float", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "Stream" COMPILER_SEP "print" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);

    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "Float", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "echo" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "Integer", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "loat", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("echo", "Stream" COMPILER_SEP "echo" COMPILER_SEP "String", 0, Function, "Integer");
    setFlags(rslFunc, FLAG_EXTERNAL);
    addParam(rslFunc, "Stream");
    addParam(rslFunc, "String");
    addSymbol(root, rslFunc);
}

int main(int argc,char **argv)
{
    int c,i,fd,old_stdout;
    int errflg=0;
	int  printTreeBool = 0;
    char *ifile = NULL;
    char *ofile = NULL;
    extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, "o:t")) != -1) {
        switch (c) {
        case 't':
        	printf("hit -t arg\n");
        	printTreeBool = 1;
        break;

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




    errorMsg(ANSI_COLOR_MAGENTA "\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n" ANSI_COLOR_RESET);

    root = CreateObject("Undefined", "Undefined", 0, CodeBlock, "Integer");
    scopeStack[scope_idx] = root;
    current = scopeStack[scope_idx];
    defineRSLSymbols(root);

    yyin = file;

    //getln();
    hitEOF = false;
    while (!hitEOF) {
        yyparse();
    }
    printf("=============  Compiling Complete!  ==============\n");

    outMainFile=fopen(oMainFileName,"w");
    outHeaderFile=fopen(oHeaderFileName,"w");

    fprintf(outMainFile,"#include \"rsl.h\"\n");
    fprintf(outMainFile,"#include \"%s\"\n",oHeaderFileName);
    fprintf(outMainFile,"int main(void) {\n");

    writeTree(outMainFile, outHeaderFile, root);
	if ( printTreeBool == 1 ) {

		printTreeToFile(root, 0, "./treeOutput.txt");
	}

    fprintf(outMainFile,"  return 0;\n}\n");
    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(file);

    //printf("\n%s compiled successfully.\n", ifile);

    return 0;
}
