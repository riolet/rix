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
Object* current;


void handleEOF() {
    hitEOF = true;
}

Object* funcHeader(char* returnType, char* funcName, Object* parameters) {
  //TODO: check funcName is undefined or function type
  //TODO: check returnType is a valid Type
  //TODO: change current to equal result
  Object* result = CreateObject(funcName, funcName, 0, Function, returnType);
  return result;
}

Object* funcParameters(Object* paramList, char* paramType, char* paramName) {
    //TODO: check if type is actually a defined type
    //TODO: check paramType is a valid Type
    Object* result;
    if (paramList == 0)
        Object* result = CreateObject(0, 0, 0, Undefined, 0);
    else
        resut = paramList;
    addParam(result, paramType);
    addCode(result, paramName);
    return result;
}

Object* completeExpression(Object* expression) {
    char* buffer = malloc(BUFFLEN);
    snprintf(buffer, BUFFLEN, "%s;", expression->code->value);
    addCode(current, buffer);
}

char* subjectIdent(char* subject) {
    printf("subjectIdent(%s)\n", subject);
    return subject;
}

Object* exprSVO(char* lhs, Object* verb, Object* rhs) {
    if (rhs == 0) {
        printf("exprSVO(%s %s)\n", lhs, verb->name);
    } else {
        printf("exprSVO(%s %s %s)\n", lhs, verb->name, rhs->code->value);
    }
    char* expr = malloc(BUFFLEN);
    Object* result;
    if (verb->name[0] == '=' && verb->name[1] == '\0') {
        //Assignment
        Object* left = findByName(lhs);
        if (!left) {
            //lhs is new variable.
            addSymbol(current, CreateObject(lhs, lhs, 0, Variable, rhs->returnType));
            snprintf(expr, BUFFLEN, "%s %s = %s", rhs->returnType, lhs, rhs->code->value);
            result = CreateObject(0, 0, 0, Expression, rhs->returnType);
            addCode(result, expr);
        } else {
            //lhs is an existing vavriable
            if (strcmp(left->returnType, rhs->returnType)) {
                char error[BUFFLEN];
                sprintf(error, "%s (%s) doesn't match %s (%s).\n", lhs, left->returnType, rhs->code->value, rhs->returnType);
                criticalError(ERROR_IncompatibleTypes, error);
            }
            snprintf(expr, BUFFLEN, "%s = %s", lhs, rhs->code->value);
            result = CreateObject(0, 0, 0, Expression, rhs->returnType);
            addCode(result, expr);
        }
    } else if (verb->returnType == 0) {
        Object* left = findByName(lhs);
        if (!left) {
            char error[BUFFLEN];
            sprintf(error, "Variable \"%s\" used before declaration.\n", lhs);
            criticalError(ERROR_UndefinedVariable, error);
        }

        if (!strcmp(left->returnType, "Integer") && !strcmp(rhs->returnType, "Integer")) {
            result = CreateObject(0, 0, 0, Expression, "Integer");
            snprintf(expr, BUFFLEN, "(%s %s %s)", left->name, verb->fullname, rhs->code->value);
            addCode(result, expr);
        } else if ((!strcmp(left->returnType, "Integer") || !strcmp(left->returnType, "Float")) && (!strcmp(rhs->returnType, "Integer") || !strcmp(rhs->returnType, "Float"))) {
            result = CreateObject(0, 0, 0, Expression, "Float");
            snprintf(expr, BUFFLEN, "(%s %s %s)", left->name, verb->fullname, rhs->code->value);
            addCode(result, expr);
        } else {
            criticalError(ERROR_IncompatibleTypes, "exprSVO(l,v,r) failed. Types weren't float or int.");
        }
    }
    return result;
}

Object* conjugate(Object* lhs, Object* verb, Object* rhs) {
    if (lhs == 0 && rhs == 0) {
        printf("conjugate(%s)\n", verb->name);
    } else if (lhs == 0) {
        printf("conjugate(%s %s)\n", verb->name, rhs->code->value);
    } else {
        printf("conjugate(%s %s %s)\n", lhs->code->value, verb->name, rhs->code->value);
    }

    Object* result;
    const char* ltype = 0;
    const char* rtype = 0;
    if (lhs != 0) ltype = lhs->returnType;
    if (rhs != 0) rtype = rhs->returnType;
    char* expr = malloc(BUFFLEN);
    if (verb->returnType == 0 && strcmp(verb->name,"=") ) {
        //TODO: assuming infix MathOp, but that could be a dangerous assumption
        if (!strcmp(ltype, "Integer") && !strcmp(rtype, "Integer")) {
            result = CreateObject(0, 0, 0, Expression, "Integer");
            snprintf(expr, BUFFLEN, "(%s %s %s)", lhs->code->value, verb->fullname, rhs->code->value);
            addCode(result, expr);
        } else if ((!strcmp(ltype, "Integer") || !strcmp(ltype, "Float")) && (!strcmp(rtype, "Integer") || !strcmp(rtype, "Float"))) {
            result = CreateObject(0, 0, 0, Expression, "Float");
            snprintf(expr, BUFFLEN, "(%s %s %s)", lhs->code->value, verb->fullname, rhs->code->value);
            addCode(result, expr);
        } else {
            criticalError(ERROR_IncompatibleTypes, "Conjugate(l,v,r) failed. Types weren't float or int.");
        }
    } else if (verb->returnType == 0) {
        //Assignment operator
    } else {
        //proper verb?
        //TODO: check for overloaded variations
        if (lhs != 0 && rhs != 0) {
            char *params[2];
            params[0] = lhs->returnType;
            params[1] = rhs->returnType;
            verb = findFunctionMatch(current, verb->name, 2, params);
            result = CreateObject(0, 0, 0, Expression, verb->returnType);
            snprintf(expr, BUFFLEN, "%s(%s, %s)", verb->fullname, lhs->code->value, rhs->code->value);
            addCode(result, expr);
        } else if (rhs != 0) {
            char *params[1];
            params[0] = rhs->returnType;
            verb = findFunctionMatch(current, verb->name, 1, params);
            result = CreateObject(0, 0, 0, Expression, verb->returnType);
            snprintf(expr, BUFFLEN, "%s(%s)", verb->fullname, rhs->code->value);
            addCode(result, expr);
        } else {
            result = CreateObject(0, 0, 0, Expression, verb->returnType);
            snprintf(expr, BUFFLEN, "%s()", verb->fullname);
            addCode(result, expr);
        }
    }
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
    char* old = expr->code->value;
    char* buffer = malloc(strlen(old) + 3); // old + ()\0
    sprintf(buffer, "(%s)", old);
    //free(old);
    expr->code->value = buffer;
    return expr;
}

Object* objectVerb(Object* verb) {
    printf("objectVerb(%s)\n", verb->name);
    if (verb->paramTypes != 0) {
        criticalError(ERROR_InvalidArguments, "Only zero-argument verbs can be used as objects.");
    }
    Object* result = CreateObject(0, 0, 0, Expression, verb->returnType);
    char* buffer = malloc(BUFFLEN);
    snprintf(buffer, BUFFLEN, "%s()", verb->fullname);
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
    return result;
}

Object* objectFloat(float f) {
    printf("objectFloat(%f)\n", f);
    char* buffer = malloc(256);
    sprintf(buffer, "%f", f);
    Object* result = CreateObject(0, 0, 0, Expression, "Float");
    addCode(result, buffer);
    return result;
}

Object* objectInt(int d) {
    printf("objectInt(%d)\n", d);
    char* buffer = malloc(32); // 21 = (log10(2^64)+1)
    snprintf(buffer, 32, "%d", d);
    Object* result = CreateObject(0, 0, 0, Expression, "Integer");
    addCode(result, buffer);
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
    return findByNameInScope(current, name);
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



    errorMsg(ANSI_COLOR_MAGENTA "**********************************\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n" ANSI_COLOR_RESET);
    fprintf(outMainFile,"#include \"rsl.h\"\n");
    fprintf(outMainFile,"#include \"%s\"\n",oHeaderFileName);
    fprintf(outMainFile,"int main(void) {\n");

    root = CreateObject("Undefined", "Undefined", 0, CodeBlock, "Integer");
    Object* rslFunc;
    rslFunc = CreateObject("print", "print_Integer", 0, Function, "Integer");
    addParam(rslFunc, "Integer");
    addSymbol(root, rslFunc);
    rslFunc = CreateObject("print", "print_Float", 0, Function, "Integer");
    addParam(rslFunc, "Float");
    addSymbol(root, rslFunc);

    printf("root is loaded.\n");
    current = root;

    yyin = file;

    //getln();
    hitEOF = false;
    while (!hitEOF) {
        yyparse();
    }

    writeTree(outMainFile, outHeaderFile, root);

    fprintf(outMainFile,"  return 0;\n}\n");
    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(file);

    printf("\n%s compiled successfully.\n", ifile);

    return 0;
}
