#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>

#include "rixc.h"
#include "crsl.h"
#include "ext/List/cliblist.h"

#define BUFFLEN 1024
#define LABELMAX 8096
#define STACKDEP 1024
#define MAXSCOPE 64
#define EVAL_BUFF_MAX_LEN 4096

FILE *file;
FILE *outMainFile;
FILE *outHeaderFile;
FILE *outMakeFile;


bool hitEOF;

Object *root;
Object *scopeStack[MAXSCOPE];
int scope_idx = 0;
Object *current;
char *previous[MAXSCOPE];
char *prevType[MAXSCOPE];
char prevExists[MAXSCOPE];
ListString *prevNode[MAXSCOPE];
int prev_idx = 0;
bool external = false;
int retVarNumber = 0;
int codeBlockNumber = 0;

Object *scope_pop()
{
    current = scopeStack[--scope_idx];
    return scopeStack[scope_idx + 1];
}

void scope_push(Object * val)
{
    current = scopeStack[++scope_idx] = val;
}

void handleEOF()
{
    hitEOF = true;
}

Object *beginClass(char *className, char *parentName, Object *typeArgs, bool isPrimitive)
{
    if (!className || !parentName) {
        criticalError(ERROR_ParseError, "Class name mustn't be null.\n");
    }
    //build full name of class:  <ClassName><sep><ParentClassName>
    char fullname[BUFFLEN];
    char codename[BUFFLEN];
    snprintf(fullname, BUFFLEN, "%s" COMPILER_SEP "%s", className, parentName);

    //get link to parent class definition
    Object *parent = findByName(parentName);
    if (parent == 0) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find definition for '%s'\n", parentName);
        criticalError(ERROR_ParseError, error);
    }

    snprintf(codename, BUFFLEN, "%s", className);

    Object *parentReference = CreateObject(IDENT_SUPER, IDENT_SUPER, 0, Variable, IDENT_MPTR);
    Object *parentReference_ = CreateObject(IDENT_SUPER"_", IDENT_SUPER"_", 0, Variable, parent->name);
    Object *result = CreateObject(className, fullname, current, Type, codename);
    compilerDebugPrintf("External class %d\n",external);
    if (external) {
        setFlags(result, FLAG_EXTERNAL);
    } else {
        setParentClass(result, parent);
        addSymbol(result, parentReference);
        addSymbol(result, parentReference_);
    }

    if (typeArgs) {
        ListType * list = typeArgs->paramTypes;
        compilerDebugPrintf(" Paramtypes: ");
        while (list) {
            compilerDebugPrintf(" %s ", typeArgs->paramTypes->type);
            list = list->next;
        }
        compilerDebugPrintf(" Paramtypes\n");

    }
    if (isPrimitive) {
        setFlags(result,FLAG_PRIMITIVE);
    }
    addSymbol(current, result);
    scope_push(result);
    return result;
}

void doneClass(Object * tree)
{

    if (!external) {
        //check for destructor.
        //if no destructor exists, add one.
        char dtorName[BUFFLEN];
        snprintf(dtorName, BUFFLEN, "%s" COMPILER_SEP "%s" COMPILER_SEP, current->name, "destructor");
        Object *dtor = findByNameInScope(current, dtorName, true);
        if (!dtor) {
            compilerDebugPrintf ("Beginning Destructor %s!\n",dtorName);
            beginDestructor(CreateObject(0, 0, 0, Expression, 0));
            doneDestructor(0);
        }

        //check for 0-arg constructor.
        //if no 0-arg ctor exists, add one.

        char ctorName[BUFFLEN];
        snprintf(ctorName, BUFFLEN, "%s" COMPILER_SEP "%s" COMPILER_SEP, current->name, current->name);
        Object *ctor = findByNameInScope(current, ctorName, true);
        compilerDebugPrintf("Finding ctor for %s\n", ctorName);
        if (!ctor) {
            beginConstructor(CreateObject(0, 0, 0, Expression, 0));
            doneConstructor(0);
        } else {
            compilerDebugPrintf("Ctor found for %s\n", ctorName);
        }
    }
    scope_pop();
}

Object *beginFunction(char *funcName, char *returnType, ListType *returnGenericType, Object * parameters)
{
    if (returnType == 0) {
        criticalError(ERROR_ParseError, "Return category mustn't be null.\n");
    }
    ListType *types = parameters->paramTypes;
    ListString *names = parameters->code;
    //TODO: check funcName is undefined or function category
    //TODO: check returnType is a valid Type

    char funcFullName[BUFFLEN];
    int funcFullName_pos = 0;

    //Build full name.
    // full name starts with parent class, if available
    if (current->category == Type) {
        funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     "%s" COMPILER_SEP, current->name);
    }

    funcFullName_pos +=
        snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos, "%s",
                 funcName);

    //add each of the parameters to the fullname
    if (types != 0) {
        while (types != 0) {
            funcFullName_pos +=
                snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                         COMPILER_SEP "%s", types->type);
            while (funcFullName[funcFullName_pos - 1] == ' '
                   || funcFullName[funcFullName_pos - 1] == '*') {
                funcFullName_pos--;
            }
            funcFullName[funcFullName_pos] = '\0';
            types = types->next;
        }
    } else {
        funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     COMPILER_SEP);
        funcFullName[funcFullName_pos] = '\0';
    }

    Object *parentScope;
    int i = scope_idx;
    while (i >= 0) {
        if (scopeStack[i]->category != Type) {
            break;
        }
        i--;
    }
    parentScope = scopeStack[i];

    Object *result =
        CreateObject(funcName, funcFullName, parentScope, Function, returnType);

    compilerDebugPrintf("External func %d\n",external);
    if (external) {
        setFlags(result, FLAG_EXTERNAL);
    } else {
        if (current->category == Type) {
            result->parentClass = current;
            char pointer[BUFFLEN];
            snprintf(pointer, BUFFLEN, "%s", current->name);
            addParam(result, pointer);
            addSymbol(result, CreateObject(IDENT_SELF_SELF, IDENT_SELF_SELF, 0, Variable, pointer));
            //addSymbol(result, CreateObject(COMPILER_SEP "prev", COMPILER_SEP "prev", 0, Variable, COMPILER_SEP "Last"));
        }
    }
    //add parameters to the function
    types = parameters->paramTypes;
    //assuming for every Type there is a name
    while (types != 0) {
        if (!external) {
            addSymbol(result,
                      CreateObject(names->value, names->value, 0, Variable, types->type));
        }
        addParam(result, types->type);
        names = names->next;
        types = types->next;
    }

    //If the return type is a generic type...
    if (returnGenericType) {
        //ToDo better generic handling
        compilerDebugPrintf("Setting generic ytype (%s)\n", returnGenericType->type);
        result->resolvedSpecificType=returnGenericType->type;
    }

    addSymbol(parentScope, result);
    scope_push(result);

    return result;
}

void doneFunction(Object * tree)
{
    scope_pop();
}

Object *beginConstructor(Object * parameters)
{
    if (current->category != Type) {
        criticalError(ERROR_ParseError, "Constructor can only exist inside a class.\n");
    }
    ListType *types = parameters->paramTypes;
    ListString *names = parameters->code;

    char funcFullName[BUFFLEN];
    int funcFullName_pos = 0;

    funcFullName_pos +=
        snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                 "%s" COMPILER_SEP "%s" , current->name, current->name);

    compilerDebugPrintf("Doing constructor %s\n",funcFullName);
    if (!types) {
        compilerDebugPrintf("Adding no types\n");
        funcFullName_pos +=
                snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                         COMPILER_SEP);
    }

    while (types != 0) {
        compilerDebugPrintf("Adding category %s\n",types->type);
        funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     COMPILER_SEP "%s", types->type);
        while (funcFullName[funcFullName_pos - 1] == ' '
               || funcFullName[funcFullName_pos - 1] == '*') {
            funcFullName_pos--;
        }
        funcFullName[funcFullName_pos] = '\0';
        types = types->next;
    }
    Object *parentScope;
    int i = scope_idx;
    while (i >= 0) {
        if (scopeStack[i]->category != Type) {
            break;
        }
        i--;
    }
    parentScope = scopeStack[i];

    char returnType[BUFFLEN];
    snprintf(returnType, BUFFLEN, "%s", current->name);

    compilerDebugPrintf("Adding ctor %s\n",funcFullName);
    Object *result =
        CreateObject(current->name, funcFullName, parentScope, Constructor, returnType);
    result->parentClass = current;

    //add parameters to the function
    types = parameters->paramTypes;
    //assuming for every category there is a name
    while (types != 0) {
        if (!external) {
            addSymbol(result,
                      CreateObject(names->value, names->value, 0, Variable, types->type));
        }
        addParam(result, types->type);
        names = names->next;
        types = types->next;
    }

    compilerDebugPrintf("External ctor %d\n",external);
    if (external) {
        setFlags(result, FLAG_EXTERNAL);
    } else {
        //Add allocation code
        char allocator[BUFFLEN];
        snprintf(allocator, BUFFLEN, "%s * " IDENT_SELF_SELF "_ = calloc(1, sizeof(%s));\n"
                         IDENT_MPTR " * " IDENT_SELF_SELF " = _$_returnAppointer(_$_mptr_in," IDENT_SELF_SELF "_,%s_$_destructor_$_);",
                 returnType, current->name,returnType);
        addCode(result, allocator);


    //     if (!getFlag(current->parentClass,FLAG_PRIMITIVE)) {
    //         char retVarName[BUFFLEN];
    //         snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d", retVarNumber);
    //         retVarNumber++;
    //         Object *retVar =
    //                 CreateObject(retVarName, retVarName, 0, Variable, IDENT_HEAP_MPTR);
    //         addSymbol(result, retVar);

    //         //Todo: Handle heap variables

    //         snprintf(allocator, BUFFLEN, IDENT_SELF_SELF "_->" IDENT_SUPER "= %s" COMPILER_SEP "%s" COMPILER_SEP "(%s);",
    //                  current->parentClass->name, current->parentClass->name, retVarName);

    //     }
    //     addCode(result, allocator);

    //     snprintf(allocator, BUFFLEN, IDENT_SELF_SELF "_->" IDENT_SUPER "_= " IDENT_SELF_SELF "_->"IDENT_SUPER"->obj;");
    //     addCode(result, allocator);

    //     //Add field allocators
    //     ListObject *oIter;

    //     oIter = current->definedSymbols;

    //     while (oIter != 0) {

    //         if (strcmp(oIter->value->name,IDENT_SUPER)&&strcmp(oIter->value->name,IDENT_SUPER "_"))
    //         {
    //             if (oIter->value->category == Variable) {
    //                 Object * rType = findByName(oIter->value->returnType);
    //                 if (!getFlag(rType,FLAG_PRIMITIVE)) {

    //                     char retVarName[BUFFLEN];
    //                     snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d", retVarNumber);
    //                     retVarNumber++;
    //                     Object *retVar =
    //                             CreateObject(retVarName, retVarName, 0, Variable, IDENT_HEAP_MPTR);
    //                     addSymbol(result, retVar);

    //                     //Todo: Handle heap variables
    //                     snprintf(allocator, BUFFLEN, IDENT_SELF_SELF "_->%s= %s" COMPILER_SEP "%s" COMPILER_SEP "(%s);",
    //                              oIter->value->name, oIter->value->returnType, oIter->value->returnType, retVarName);
    //                     addCode(result, allocator);

    //                 }

    //             } else {
    //                 oIter = oIter->next;
    //                 break;
    //             }
    //         }
    //         oIter = oIter->next;

    //     }
    }
    addSymbol(parentScope, result);
    scope_push(result);

    return result;
}

void doneConstructor(Object * tree)
{
    if (!external) {
        addCode(current, "return " IDENT_SELF_SELF ";");
    }
    scope_pop();
}

Object *beginDestructor(Object * parameters)
{
    if (current->category != Type) {
        criticalError(ERROR_ParseError, "Destructor can only exist inside a class.\n");
    }

    char funcFullName[BUFFLEN];
    int funcFullName_pos = 0;

    funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     "%s" COMPILER_SEP "%s" COMPILER_SEP, current->name, "destructor");

    Object *parentScope;
    int i = scope_idx;
    while (i >= 0) {
        if (scopeStack[i]->category != Type) {
            break;
        }
        i--;
    }
    parentScope = scopeStack[i];

    Object *result =
            CreateObject(current->name, funcFullName, parentScope, Function, IDENT_MPTR);
    result->parentClass = current;

    ListObject *oIter;

    oIter = current->definedSymbols;
    char deallocator[BUFFLEN];

    // while (oIter != 0) {
    //     if (strcmp(oIter->value->name,IDENT_SUPER "_"))
    //     {
    //         if (oIter->value->category == Variable) {
    //             Object * rType = findByName(oIter->value->returnType);
    //             if (!getFlag(rType,FLAG_PRIMITIVE)) {
    //                 snprintf(deallocator, BUFFLEN, "_$_cleanup(((%s *)" IDENT_MPTR "_in->obj)->%s);",
    //                          current->returnType, oIter->value->name);
    //                 addCode(result, deallocator);
    //                 snprintf(deallocator, BUFFLEN, "free(((%s *)" IDENT_MPTR "_in->obj)->%s);",
    //                          current->returnType, oIter->value->name);
    //                 addCode(result, deallocator);
    //             }
    //         } else {
    //             oIter = oIter->next;
    //             break;
    //         }
    //     }
    //     oIter = oIter->next;
    // }

    snprintf(deallocator, BUFFLEN, "free(((%s *)" IDENT_MPTR "_in->obj));",current->returnType);
    addCode(result, deallocator);

    addSymbol(parentScope, result);
    scope_push(result);

    return result;
}

void doneDestructor(Object * tree)
{
    scope_pop();
}

Object *funcParameters(Object * paramList, char *paramType, char *paramName, ListType *genericType)
{
    //TODO: check if category is actually a defined category
    //TODO: check paramType is a valid Type
    Object *type = findByName(paramType);
    if (!type || type->category != Type) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find category '%s'\n", paramType);
        criticalError(ERROR_UndefinedType, error);
    }

    Object *result;
    if (paramList == 0) {
        result = CreateObject(0, 0, 0, Undefined, 0);
    } else {
        result = paramList;
    }
    addParamWithGenericType(result, type->returnType,genericType);            
    addCode(result, paramName);
    return result;
}

Object *concatParams(Object * existing, Object * newParam)
{
    Object *result = CreateObject(0, 0, 0, Expression, 0);
    ListType *paramIter;
    ListString *codeIter;
    int paramLen;
    int codeLen;
    char *lastCode;
    char *lastParam;
    char newCode[BUFFLEN];

    compilerDebugPrintf("Concating exist: %s new: %s\n",existing->paramTypes->type,newParam->paramTypes->type);
    //insert any declarations from the old objects
    paramIter = existing->paramTypes;    
    codeIter = existing->code;
    paramLen = listTypelen(paramIter);
    compilerDebugPrintf("Existing param count and code ciybt %d %d\n",paramLen,codeLen);
    codeLen = listStringlen(codeIter);
    while (codeLen > 0 && codeLen > paramLen) {
        compilerDebugPrintf("Adding code %s\n",codeIter->value);
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert any declarations from the new object
    paramIter = newParam->paramTypes;
    codeIter = newParam->code;
    paramLen = listTypelen(paramIter);
    codeLen = listStringlen(codeIter);
    compilerDebugPrintf("New param count and code ciybt %d %d\n",paramLen,codeLen);
    while (codeLen > 0 && codeLen > paramLen) {
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert parameters of old objects
    paramIter = existing->paramTypes;
    codeIter = existing->code;
    paramLen = listTypelen(paramIter);
    codeLen = listStringlen(codeIter);
    while (codeLen > 0) {
        if (codeLen > paramLen) {
            codeLen--;
            codeIter = codeIter->next;
            continue;
        }
        addCode(result, codeIter->value);
        addParamWithGenericType(result, paramIter->type,paramIter->genericType);
        codeLen--;
        codeIter = codeIter->next;
        paramIter = paramIter->next;
    }

    //insert parameters of new objects
    paramIter = newParam->paramTypes;
    codeIter = newParam->code;
    paramLen = listTypelen(paramIter);
    codeLen = listStringlen(codeIter);
    while (codeLen > 0) {
        if (codeLen > paramLen) {
            codeLen--;
            codeIter = codeIter->next;
            continue;
        }
        compilerDebugPrintf("Adding code %s\n",codeIter->value);
        addCode(result, codeIter->value);
        addParamWithGenericType(result, paramIter->type,paramIter->genericType);
        codeLen--;
        codeIter = codeIter->next;
        paramIter = paramIter->next;
    }

    return result;
}

Object *declareVariable(char *name, char *type, ListType *genericType)
{
    Object *oType = findByName(type);
    Object *var = CreateObject(name, name, 0, Variable, oType->returnType);
    if (genericType) {
        var->genericType=genericType;
    }
    addSymbol(current, var);
    return var;
}

Object *completeExpression(Object * expression)
{
    if (expression == 0) {
        warningMsg("expression was null\n");
        return 0;
    }

    char buffer[BUFFLEN];
    ListString *code = expression->code;
    while (code != 0) {
        snprintf(buffer, BUFFLEN, "/*%s %d*/ %s;", __FILE__, __LINE__, code->value);
        prevNode[prev_idx] = addCode(current, buffer);
        code = code->next;
    }
    return current;
}

Object *createCodeBlock(Object * expression)
{
    if (expression == 0) {
        warningMsg("expression was null\n");
        return 0;
    }
    prevNode[prev_idx] = pushCode(expression, "({");
    prevNode[prev_idx] = addCode(expression, "});");
    return expression;
}

Object *finalize(Object * expression)
{
    return expression;
}

int prependPrev()
{
    //ensure variable is declared
    checkPrevExists();

    //prepend "previous[idx] = "
    char code[BUFFLEN];
    if (!prevNode[prev_idx]) {
        criticalError(ERROR_ParseError, "No previous value found.\n");
    }
    if (!strncmp(prevNode[prev_idx]->value, COMPILER_SEP "prev.", 8)) {
        return 0;
    }

    snprintf(code, BUFFLEN, "%s = %s", previous[prev_idx], prevNode[prev_idx]->value);
    free(prevNode[prev_idx]->value);
    prevNode[prev_idx]->value = strdup(code);

    return 0;
}

void closeBrace()
{
    addCode(current, "}");
}

Object *makeReturn(Object * expression)
{
    if (expression == 0) {
        Object *result = CreateObject(0, 0, 0, Expression, "void");
        addCode(result, "return");
        return result;
    }
    //add "return" to the last expression
    ListString *line = expression->code;
    if (line == 0) {
        warningMsg("makeReturn: Nothing to add a return to\n");
    }
    while (line->next != 0) {
        line = line->next;
    }
    char *oldCode = line->value;
    char newCode[BUFFLEN];
    Object * rType = findByName(expression->returnType);
    if (getFlag(rType,FLAG_PRIMITIVE)) {
        snprintf(newCode, BUFFLEN, "return %s", oldCode);
        //TODO: this violates encapsulation
        free(line->value);
        line->value = strdup(newCode);
    } else {
        //remove last semicolon
        if (line->value[strlen(line->value)-1]==';')
            line->value[strlen(line->value)-1]='\0';
        snprintf(newCode, BUFFLEN, "_$_object_ownership_transfer(%s, " IDENT_MPTR "_in);\n"
                "return " IDENT_MPTR "_in;", line->value);
        free(line->value);
        line->value = strdup(newCode);
        compilerDebugPrintf("Expression category %d code %s\n", expression->category, expression->code->value);
        setFlags(expression, FLAG_RETURNS);
    }
    return expression;
}

void checkPrevExists()
{
    ListString *oldNode;
    ListString *oldNext;
    if (!prevExists[prev_idx]) {
        char code[BUFFLEN];
        snprintf(code, BUFFLEN, COMPILER_SEP "Last " COMPILER_SEP "prev;");
        //addSymbol(current, CreateObject(COMPILER_SEP "prev", COMPILER_SEP "prev", 0, Variable, COMPILER_SEP "Last "));
        if (prevNode[prev_idx]) {
            oldNode = prevNode[prev_idx];
            oldNext = prevNode[prev_idx]->next;
            ListString *node = malloc(sizeof(ListString));
            oldNode->next = node;
            node->next = oldNext;
            node->value = oldNode->value;
            oldNode->value = strdup(code);
            prevNode[prev_idx] = node;
        } else {
            addCode(current, code);
        }

        prevExists[prev_idx] = 1;
    }
}

void incPrev()
{
    prev_idx++;
    //addCode(current, COMPILER_SEP "Last " COMPILER_SEP "prev;");
}

void decPrev()
{
    prevExists[prev_idx] = 0;
    prevNode[prev_idx] = 0;
    prevType[prev_idx] = 0;
    prev_idx--;
    if (prev_idx < 0) {
        criticalError(ERROR_ParseError,
                      "previous result tracker went below 0. (decPrev, rixc.c)\n");
    }
}

Object *injectC(char *code)
{
    addCode(current, code);
    return 0;
}

Object *conjugateNewVarAssignment(char * ident, Object * verb, Object * objects)
{
    if (ident == 0) {
        criticalError(ERROR_ParseError, "Cannot assign to nothing.\n");
    }
    Object* exists = findByName(ident);
    if (!exists) {
        Object * newIdent = objectNewIdent(ident);
        Object * verb = verbAssignment("=");
        return conjugateAssign(newIdent, verb, objects);
    } else {
        char error[1024];
        snprintf(error, 1024, "Trying to recreate identifier %s\n", ident);
        criticalError(ERROR_ParseError, error);
    }
}


Object *conjugateAssign(Object * subject, Object * verb, Object * objects)
{
    if (subject == 0) {
        criticalError(ERROR_ParseError, "Cannot assign to nothing.\n");
    }
    ListType *paramIter;
    Object *realVerb = 0;
    Object *result = 0;
    char verbname[BUFFLEN];
    char tempBuffer[BUFFLEN];
    int verbname_pos = 0;
    ListString *codeIter;

    if (verb->category == Type) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s" COMPILER_SEP,
                     verb->name);
    }
    //build base name of verb (e.g. "+" becomes "plus")
    if (!strcmp(verb->name, "=")) {
        //handling straight up assignment.
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "assign");
    } else if (!isalpha(verb->name[0])) {
        //handle augmented assigment operators recursively.
        char op[4];
        Object *augment;
        strcpy(op, verb->name);
        if (op[2] == '=') {
            op[2] = '\0';
        }
        if (op[1] == '=') {
            op[1] = '\0';
        }

        augment = conjugate(subject, verbMathOp(op), objects);
        return conjugateAssign(subject, verbAssignment("="), augment);
    } else {
        //handle assignment verbs
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name);
    }

    //build final name for verb given object
    if (objects) {
        paramIter = objects->paramTypes;
        while (paramIter != 0) {
            verbname_pos +=
                snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos,
                         COMPILER_SEP "%s", paramIter->type);
            paramIter = paramIter->next;
        }
    }
    //search for the definition of that object
    compilerDebugPrintf("ConjugateAssign: fullVerbName: %s\n", verbname);
    realVerb = findFunctionByFullName(verbname);
    if (realVerb == 0 && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find function named %s %d.\n", verbname,__LINE__);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (realVerb == 0) {
        //must be literal = or similar.
        compilerDebugPrintf("must be literal = or similar: %s (%d) \n", verbname,__LINE__);
        if (!objects) {
            criticalError(ERROR_ParseError, "Object of assignment was not found.\n");
        }
        // if (!objects->returnType) {
        //     char error[BUFFLEN];
        //     snprintf(error, BUFFLEN, "Paramtypes not found for %s %d.\n", objects->name,__LINE__);
        //     criticalError(ERROR_ParseError, error);
        // }
        result = CreateObject(0, 0, 0, Expression, objects->returnType);
        addParamWithGenericType(result, objects->returnType,result->genericType);
        if (!subject->returnType) {
            if (subject->category == NewMarkedIdent) {
                compilerDebugPrintf ("Creating new variable %s as %s\n",subject->name, objects->returnType);
                Object *variable =
                    CreateObject(subject->name, subject->fullname, 0, Variable,
                                 objects->returnType);

                    if (objects->genericType) {
                        variable->genericType = objects->genericType;
                    }
//                    compilerDebugPrintf("objects at %d = %d, variable %s %d\n",__LINE__,objects,variable->name,variable);
//                    if (variable->genericType)
//                        compilerDebugPrintf("variable %d generic type %s\n",variable,variable->genericType);
                    addSymbol(current, variable);
            } else {
                char error[1024];
                g_lineNum--;
                snprintf(error, 1024, "Unknown identifier %s\n", subject->name);
                criticalError(ERROR_ParseError, error);
            }
        } else {
            //Check compatible types if Subject exists
            if (strcmp(subject->returnType, objects->returnType)) {
//                if (!((strcmp(subject->returnType, "int")
//                       || strcmp(subject->returnType, "float")
//                      ) && (strcmp(objects->paramTypes->value, "int")
//                            || strcmp(objects->paramTypes->value, "float")
//                      ))) {
                    char error[BUFFLEN];
                    snprintf(error, BUFFLEN, "%s (%s) cannot be assigned category %s\n",
                             subject->code->value, subject->returnType,
                             objects->returnType);
                    criticalError(ERROR_IncompatibleTypes, error);
//                }
            }
        }

        Object * rType = findByName(objects->returnType);

        if (!rType) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Cannot find category for %s\n",objects->returnType);
            criticalError(ERROR_ParseError, error);
        }
        //TODO: This ignores the classes assign method
        if (getFlag(rType,FLAG_PRIMITIVE)) {
            snprintf(verbname, BUFFLEN, "%s = %s", subject->code->value,
                     objects->code->value);
        } else {
            //if (subject->name)
            //ToDo handle member initializations
            compilerDebugPrintf("Is it FLAG_IDENT_SELF %d\n",getFlag(subject,FLAG_IDENT_SELF));
            if (getFlag(subject,FLAG_IDENT_SELF)) {                
                snprintf(verbname, BUFFLEN, MPTR_ASSIGN_ALLOC "(&%s,%s,\"subject->code->value\")", subject->code->value,
                     objects->code->value);
            } else {
                snprintf(verbname, BUFFLEN, MPTR_ASSIGN "(%s,%s)", subject->code->value,
                     objects->code->value);
            }
        }

        addCode(result, verbname);
        compilerDebugPrintf("\tConjugated: (%d) %s at \n", __LINE__, verbname);
//        compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//        if (result->genericType)
//            compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
        return result;
    }
    //build code line statement invoking that verb.
    verbname_pos = 0;
    verbname_pos +=
        snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s(",
                 realVerb->fullname);
    if (subject && objects) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,",
                     subject->code->value);
    } else if (subject) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s",
                     subject->code->value);
    }
    if (objects) {
        codeIter = objects->code;
        while (codeIter != 0) {
            if (codeIter->next == 0) {
                verbname_pos +=
                    snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s",
                             codeIter->value);
            } else {
                verbname_pos +=
                    snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,",
                             codeIter->value);
            }
            codeIter = codeIter->next;
        }
    }

    if (subject && subject->returnType == 0) {
        if (subject->category == NewMarkedIdent) {
            compilerDebugPrintf("NewMarkedIdent %s", subject->name);
            Object *variable = CreateObject(subject->name, subject->fullname, 0, Variable,
                                            verb->returnType);
            addSymbol(current, variable);
            compilerDebugPrintf("Subject at %d = %s\n",__LINE__,subject->code->value);
            if (objects->genericType) {
                variable->genericType = objects->genericType;
            }
        } else {
            char error[1024];
            snprintf(error, 1024, "Unknown identifier %s\n", subject->name);
            criticalError(ERROR_ParseError, error);
        }
    }

    verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, ")");
    compilerDebugPrintf("\tConjugated: (%d) %s at \n", __LINE__, verbname);

    result = CreateObject(0, 0, 0, Expression, "void");

    addParam(result, realVerb->returnType);
    addCode(result, verbname);
//    compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//    if (result->genericType)
//        compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
    return result;

}

Object *conjugate(Object * subject, Object * verb, Object * objects)
{
    ListType *paramIter = 0;
    ListString *codeIter = 0;
    Object *realVerb = 0;
    Object *result = 0;
    char verbname[BUFFLEN];
    char genericVerbName[BUFFLEN];
    char invocation[BUFFLEN];
    char *paramTypes[BUFFLEN];
    int verbname_pos = 0;
    int genericVerbNamePos = 0;
    int invoke_pos = 0;
    //if this is an assignment verb, treat it differently.
    if (getFlag(verb, FLAG_ASSIGNMENT)) {
        return conjugateAssign(subject, verb, objects);
    }


    //== Build the fullname for the verb ==

    if (subject) {
        if (subject->returnType == 0) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Variable '%s' used before definition\n",
                     subject->code->value);
            criticalError(ERROR_UndefinedVariable, error);
        }

        verbname_pos +=
                snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s",
                         subject->returnType);

        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, COMPILER_SEP);
    } else if (verb->category == Type) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s" COMPILER_SEP,
                     verb->name);
    }
    //erase the " *" if it's there from the name

    //build base name of verb (e.g. "+" becomes "plus")
    if (!strcmp(verb->name, "+")) {
        verbname_pos += snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "plus");
    } else if (!strcmp(verb->name, "-")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "minus");
    } else if (!strcmp(verb->name, "*")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "times");
    } else if (!strcmp(verb->name, "/")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "slash");
    } else if (!strcmp(verb->name, "%")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "modulus");
    } else if (!strcmp(verb->name, "^^")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "exponent");
    } else if (!strcmp(verb->name, "<")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmplt");
    } else if (!strcmp(verb->name, ">")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpgt");
    } else if (!strcmp(verb->name, "<=")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmplteq");
    } else if (!strcmp(verb->name, ">=")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpgteq");
    } else if (!strcmp(verb->name, "==")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpeq");
    } else if (!strcmp(verb->name, "!=")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "cmpneq");
    } else if (!strcmp(verb->name, "<>")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "compare");
    } else if (!strcmp(verb->name, "-->")) {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "condreturn");
    } else {
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s", verb->name);
    }

    //verify objects exists and is a known variable or is an expression composed of known variables
    verbname[verbname_pos] = '\0';
    strncpy(genericVerbName, verbname, (verbname_pos < BUFFLEN) ? verbname_pos : BUFFLEN);
    genericVerbName[verbname_pos] = '\0';
    genericVerbNamePos = verbname_pos;

    int paramNumber = 0;
    if (objects) {
        if (!objects->paramTypes) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Variable '%s' used before definition as object\n",
                     objects->code->value);
            criticalError(ERROR_UndefinedVariable, error);
        }
        paramIter = objects->paramTypes;
        while (paramIter) {
            verbname_pos +=
                snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos,
                         COMPILER_SEP "%s", paramIter->type);
            genericVerbNamePos +=
                snprintf(&genericVerbName[genericVerbNamePos],
                         BUFFLEN - genericVerbNamePos, COMPILER_SEP "%s", GENERIC_PARAM);
            while (verbname[verbname_pos - 1] == '*' || verbname[verbname_pos - 1] == ' ') {
                verbname_pos--;
            }
            verbname[verbname_pos] = '\0';
            genericVerbName[genericVerbNamePos] = '\0';
            paramTypes[paramNumber] = strdup(paramIter->type);
            paramIter = paramIter->next;
            paramNumber++;
        }
    } else {
        //To stop getting verb names getting confused with keywords
        verbname_pos +=
            snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, COMPILER_SEP);
        verbname[verbname_pos] = '\0';
    }

    //search for the definition of that object
    compilerDebugPrintf("Conjugate: fullVerbName: %s\n", verbname);
    compilerDebugPrintf("Conjugate: genericVerbName: %s\n", genericVerbName);
    if (verb->category == Type) {
        //look for ctor inside class
        realVerb = findByNameInScope(verb, verbname, true);
    }
    if (!realVerb) {
        realVerb = findFunctionByFullName(verbname);
    }
    //Is this a generic function
    bool genericVerb = false;
    if (!realVerb) {
        realVerb = findFunctionByFullName(genericVerbName);
        genericVerb = true;
        compilerDebugPrintf("%s is a Generic Verb\n",genericVerbName);
    }

    if (!realVerb && subject) {
        //This part looks for an inherited method via parent classes.
        //TODO: this seems too complex. It should be revisited.
        char newName[BUFFLEN];
        char newSubject[BUFFLEN];
        char paramTypes[BUFFLEN][BUFFLEN];
        int subject_idx = snprintf(newSubject, BUFFLEN,   "/* %d */ ((%s *) ((%s)->obj))->",__LINE__,
                                   subject->returnType, subject->code->value);
        int offset = snprintf(newName, BUFFLEN, "%s", subject->returnType);
        while (newName[offset - 1] == '*' || newName[offset - 1] == ' ') {
            offset--;
        }
        newName[offset] = '\0';
        Object *parent = findByName(newName);
        if (parent && parent->parentClass) {
            //skip first level (already checked there)
            parent = parent->parentClass;
        }

        while (!realVerb && parent && parent->category == Type) {
            snprintf(newName, BUFFLEN, "%s%s", parent->name, &verbname[offset]);
            compilerDebugPrintf("Trying parent class: %s\n", newName);
            realVerb = findFunctionByFullName(newName);
            char * oldSubject = strdup(newSubject);

            subject_idx +=
                snprintf(&newSubject[subject_idx], BUFFLEN - subject_idx,  IDENT_SUPER "_->");



            parent = parent->parentClass;
        }
        newSubject[subject_idx-3] = '\0';
        if (realVerb) {
            //TODO: violates encapsulation. (just this once!)
            free(subject->code->value);
            subject->code->value = strdup(newSubject);
        }
    }

    if (!realVerb && isalpha(verb->name[0])) {
        char error[BUFFLEN];
        if (subject)
            snprintf(error, BUFFLEN, "Type \"%s\" doesn't have member function \"%s\".\n",
                     subject->returnType, &verbname[strlen(subject->returnType) + 1]);
        else
            snprintf(error, BUFFLEN, "Cannot find function named %s %d.\n", verbname,__LINE__);
        criticalError(ERROR_UndefinedVerb, error);
    } else if (!realVerb) {
        compilerDebugPrintf("Cannot find verb \"%s\". Assuming \"%s\" is infix operator in C.\n",
                   verbname, verb->name);
        //must be + or / or such...
        if (!objects) {
            char error[BUFFLEN];
            ListString *code = subject->code;
            while (code != 0 && code->next != 0) {
                code = code->next;
            }
            snprintf(error, BUFFLEN, "Did you forget an operand? %s %s ???\n",
                     subject->code->value, verb->fullname);
            criticalError(ERROR_InvalidArguments, error);
        }
        if (verb->returnType) {
            result = CreateObject(0, 0, 0, Expression, verb->returnType);
            if (verb->genericType) {
                result->genericType=verb->genericType;
            }
            addParam(result, verb->returnType);
        } else if (!strcmp(subject->returnType, "float")
                   || !strcmp(objects->paramTypes->type, "float")) {
            result = CreateObject(0, 0, 0, Expression, "float");
            addParam(result, "float");
        } else {
            result = CreateObject(0, 0, 0, Expression, subject->returnType);
            addParamWithGenericType(result, subject->returnType, subject->genericType);
        }

        invoke_pos +=
            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s %s %s",
                     subject->code->value, verb->fullname, objects->code->value);
        addCode(result, invocation);

        compilerDebugPrintf("\tConjugated: (%d) %s at \n", __LINE__, invocation);
        return result;
    }
    //Hack to allow if statements without codeblocks working yet.
    //TODO: remove this.
    if (realVerb && !strcmp(realVerb->fullname, "bool_$_if")
        || !strcmp(realVerb->fullname, "bool_$_else")
        || !strcmp(realVerb->fullname, "bool_$_elif_$_bool")) {
        return conjugateConditional(subject, realVerb, objects);
    }
    //build code line statement invoking that verb.
    invoke_pos = 0;
    invoke_pos +=
        snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ %s( ",
                 __LINE__,realVerb->fullname);

    //Get the generic return category
    char *returnType;
    if (strcmp(realVerb->returnType, "Generic_$$")) {
        returnType = strdup(realVerb->returnType);
    } else {
        //ToDo better generic handling
        if (realVerb->genericType)
            //Preset return category
            returnType = realVerb->genericType->type;
        else
            //Positional return category
            returnType=paramTypes[realVerb->genericTypeArgPos - 1];
    }

    result = CreateObject(0, 0, 0, Expression, returnType);

    bool hasParams = false;
    if (subject) {
            invoke_pos +=
                snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ %s", __LINE__, subject->code->value);
        hasParams = true;
    } else if (strstr(verb->name, COMPILER_SEP)) {
        //This is assumed to be a static verb of some class. Inject a 0 as first argument.
        invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ 0",__LINE__);
        hasParams = true;
    }


    if (objects) {
        codeIter = objects->code;
        paramIter = objects->paramTypes;
        while (codeIter) {
            if (hasParams) {
                invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ", ");
            }

            if (genericVerb) {
                Object * pType = findByName(paramIter->type);

                if (pType&&getFlag(pType,FLAG_PRIMITIVE)) {
//                    invoke_pos +=
//                            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "(" IDENT_MPTR "* )");
                    invoke_pos +=
                            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ %s", __LINE__, codeIter->value);
                    invoke_pos +=
                            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",true");
                } else {
                    invoke_pos +=
                            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ %s", __LINE__, codeIter->value);
                    invoke_pos +=
                            snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",false");
                }
            } else {
                invoke_pos +=
                        snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "/* %d */ %s", __LINE__, codeIter->value);
            }
            codeIter = codeIter->next;
            paramIter = paramIter->next;
            hasParams = true;
        }
    }
    //== RetVar shenanigans ==
    Object * rType = findByName(realVerb->returnType);
    if (rType&&!getFlag(rType,FLAG_PRIMITIVE)) {
        char retVarName[BUFFLEN];
        snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d_%d", scope_idx, retVarNumber);
        retVarNumber++;
        Object *retVar =
                CreateObject(retVarName, retVarName, 0, Variable, IDENT_MPTR);
        addSymbol(current, retVar);
        if (hasParams) {
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ", ");
        }
        invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "&%s", retVarName);
        hasParams = true;
    }

    //== Generic shenanigans ==
    //compilerDebugPrintf("Conjverb Generic type %s\n",verb->genericType);

    if (verb->resolvedSpecificType) {
        result->genericType = createGeneric(verb->resolvedSpecificType);
    } else if (verb->genericType) {
            result->genericType = verb->genericType;

            if (hasParams) {
                invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ", ");
            }
            //Todo better Generics handling
            Object *pType = findByName(result->genericType->type);
            if (pType && getFlag(pType, FLAG_PRIMITIVE)) {
                invoke_pos +=
                        snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "true");
            } else {
                invoke_pos +=
                        snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "false");
            }
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",%s", result->genericType->type);
    }



    if (!strcmp(realVerb->returnType, GENERIC_PARAM)) {
        if (realVerb->genericType) {
            //Todo better Generics handling
            result->returnType = realVerb->genericType->type;            
            addParam(result, realVerb->genericType->type);
        } else {
            result->genericType = createGeneric(paramTypes[realVerb->genericTypeArgPos - 1]);
            addParam(result, paramTypes[realVerb->genericTypeArgPos - 1]);
        }
        Object * pType = findByName(result->returnType);
        if (pType&&getFlag(pType,FLAG_PRIMITIVE)) {
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",true");
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",%s",result->returnType);
        } else {
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",false");
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "," IDENT_MPTR );
        }
    } else if (!strcmp(realVerb->returnType, "Generic_YTYPE$$")) {
        //compilerDebugPrintf("Subject %s at %d = %d\n",subject->name,__LINE__,subject);
        if (subject->genericType) {
            //Todo better Generics handling
            result->returnType = strdup(subject->genericType->type);
            addParam(result, subject->genericType->type);
        } else {
            compilerDebugPrintf("Subject %s has no generic type at %d\n", subject->name, __LINE__);
        }
        Object * pType = findByName(result->returnType);
        if (pType&&getFlag(pType,FLAG_PRIMITIVE)) {
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",true");
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",%s",result->returnType);
        } else {
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ",false");
            invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "," IDENT_MPTR );
        }

    } else {
        addParam(result, realVerb->returnType);
    }

    //Close
    invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ")");
    if (realVerb && getFlag(realVerb, FLAG_SAVERESULT)) {
        char temp[BUFFLEN];
        snprintf(temp, BUFFLEN, COMPILER_SEP "prev.pbool = %s", invocation);
        addCode(result, temp);
    } else {
        addCode(result, invocation);
    }
    compilerDebugPrintf("\tConjugated: (%d) %s  -> %s\n", __LINE__, invocation, result->returnType);
//    compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//    if (result->genericType)
//        compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
    return result;
}

Object *conjugateConditional(Object * subject, Object * realverb, Object * objects)
{
    //do the stuff.
    char code[BUFFLEN];

    checkPrevExists();

    //if statement:
    // A if
    // becomes
    // _$_prev = A; bool_if(_$_prev)
    if (!strcmp(realverb->fullname, "bool_$_if")) {
        snprintf(code, BUFFLEN,
                 COMPILER_SEP "prev.pbool = %s; if(" COMPILER_SEP "prev.pbool) {",
                 subject->code->value);
    }
    //elif statement:
    // A elif B
    // becomes
    // _$_prev = A; bool_elif_bool(!_$_prev && B)
    else if (!strcmp(realverb->fullname, "bool_$_elif_$_bool")) {

        snprintf(code, BUFFLEN,
                 COMPILER_SEP "prev.pbool = %s; %s(!" COMPILER_SEP
                 "prev.pbool && %s)", subject->code->value, realverb->fullname,
                 objects->code->value);
    }
    //else statement:
    // A else
    // becomes
    // _$_prev = A; bool_else(!_$_prev)
    else {
        snprintf(code, BUFFLEN,
                 COMPILER_SEP "prev.pbool = %s; %s(!" COMPILER_SEP "prev.pbool)",
                 subject->code->value, realverb->fullname);
    }

    Object *result = CreateObject(0, 0, 0, Expression, realverb->returnType);
    addCode(result, code);
    addParam(result, realverb->returnType);
    return result;
}

Object *verbMathOp(char *verb)
{
    compilerDebugPrintf("verbMathOp(%s)\n", verb);
    Object *result = CreateObject(verb, verb, 0, Function, 0);
    return result;
}

Object *verbComparison(char *verb)
{
    compilerDebugPrintf("verbComparison(%s)\n", verb);
    Object *result = CreateObject(verb, verb, 0, Function, "bool");
    return result;
}

Object *verbTernary()
{
    compilerDebugPrintf("verbTernary(<>)\n");
    Object *result = CreateObject("<>", "<>", 0, Function, "bool");
    return result;
}

Object *verbDestructor()
{
    compilerDebugPrintf("verbDestructor(#)\n");
    Object *result = CreateObject("destructor", "destructor", 0, Function, 0);
    return result;
}

Object *verbGetObjAtIdx()
{
    compilerDebugPrintf("verbGetObjAtIdx(#)\n");
    Object *result = CreateObject("getObjectAtIndex", "getObjectAtIndex", 0, Function, 0);
    return result;
}

Object *verbPutObjAtIdx()
{
    compilerDebugPrintf("verbPutObjAtIdx(#)\n");
    Object *result = CreateObject("putObjectAtIndex", "putObjectAtIndex", 0, Function, 0);
    return result;
}

Object *verbCondReturn()
{
    compilerDebugPrintf("verbCondReturn(-->)\n");
    Object *result = CreateObject("-->", "-->", 0, Function, "bool");
    return result;
}

Object *verbAssignment(char *verb)
{
    compilerDebugPrintf("verbAssignment(%s)\n", verb);
    Object *result = CreateObject(verb, verb, 0, Function, 0);
    setFlags(result, FLAG_ASSIGNMENT);
    return result;
}

Object *verbIdent(char *verb)
{
    compilerDebugPrintf("verbIdent(%s)\n", verb);
    Object *result = findByName(verb);
    if (result == 0) {
        char error[BUFFLEN];
        sprintf(error, "Function \"%s(..)\" used before declaration.\n", verb);
        criticalError(ERROR_UndefinedVerb, error);
    }
    return result;
}

Object *sVerbIdent(char *staticVerb)
{
    char *type;
    char *field;
    type = strtok(staticVerb, ".");
    field = strtok(0, ".");

    //verify category exists
    Object *oType = findByName(type);
    if (!oType) {
        char error[BUFFLEN];
        snprintf(error, BUFFLEN, "Cannot find category %s\n", type);
        criticalError(ERROR_UndefinedVariable, error);
    }
    //build verb name
    char verbname[BUFFLEN];
    snprintf(verbname, BUFFLEN, "%s" COMPILER_SEP "%s", oType->name, field);

    Object *result = CreateObject(verbname, verbname, 0, Function, 0);
    return result;
}

ListType * createGeneric (char *type) {
    ListType *node = malloc(sizeof(ListType));
    node->type=type;
    node->next;
}

Object *verbCtor(char *type, ListType *genericType)
{
    compilerDebugPrintf("verbCtor(%s)\n", type);
    Object *result = findByName(type);
    if (result == 0) {
        char error[BUFFLEN];
        sprintf(error, "Cannot find Class \"%s\".\n", type);
        criticalError(ERROR_UndefinedVerb, error);
    }
    if (genericType) {
        compilerDebugPrintf("Setting generic ytype (%s)\n", genericType->type);
        result->genericType=genericType;
    }
    return result;
}

Object *parenthesize(Object * expr)
{
    compilerDebugPrintf("parenthesize(%s)\n", expr->code->value);
    char line[BUFFLEN];
    if (expr == 0) {
        criticalError(ERROR_ParseError,
                      "Object* expr was void in parenthesize. (rixc.c)\n");
    }

    Object *parenthesized =
        CreateObject(expr->name, expr->fullname, expr->parentScope, Expression,
                     expr->returnType);

    if (expr->code == 0) {
        criticalError(ERROR_ParseError,
                      "Cannot put parentheses around nothing. (rixc.c)\n");
    }

    snprintf(line, BUFFLEN, "(%s)", expr->code->value);
    addCode(parenthesized, line);

    addParam(parenthesized, expr->returnType);
    return parenthesized;
}

Object *objectNewIdent(char *ident)
{
    compilerDebugPrintf("objectNewIdent(%s)\n", ident);
    Object *result;
    Object *identifier = findByName(ident);

    if (!identifier) {
        result = CreateObject(ident, ident, 0, NewMarkedIdent, Undefined);
    } else {
        char error[1024];
        snprintf(error, 1024, "Using an existing identifier as new %s\n", ident);
        criticalError(ERROR_ParseError, error);
    }
    addCode(result, identifier ? identifier->fullname : ident);
//    compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//    if (result->genericType)
//        compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
    return result;
}

Object *objectUnmarkedNewIdent(char *ident)
{
    compilerDebugPrintf("objectUnmarkedNewIdent(%s)\n", ident);
    Object *result;
    Object *identifier = findByName(ident);

    if (!identifier) {
        compilerDebugPrintf("objectUnmarkedNewIdent(%s) not found\n", ident);
        result = CreateObject(ident, ident, 0, NewUnmarkedIdent, Undefined);
    } else {
        result =
            CreateObject(identifier->name, identifier->fullname, 0, identifier->category,
                         identifier->returnType);
        addParamWithGenericType(result, identifier->returnType,identifier->genericType);
    }
    addCode(result, identifier ? identifier->fullname : ident);
//    compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//    if (result->genericType)
//        compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
    return result;
}

Object *objectIdent(char *ident)
{
    compilerDebugPrintf("objectIdent(%s)\n", ident);
    Object *result;
    Object *identifier = findByName(ident);
//    compilerDebugPrintf("identifier at %d = %d\n",__LINE__,identifier);
//    if (identifier->genericType)
//        compilerDebugPrintf("identifier %d generic type %s\n",identifier,identifier->genericType);
    if (!identifier) {
        char error[1024];
        snprintf(error, 1024, "Unknown identifier %s\n", ident);
        criticalError(ERROR_ParseError, error);
    } else {
        result =
            CreateObject(identifier->name, identifier->fullname, 0, identifier->category,
                         identifier->returnType);
        addParamWithGenericType(result, identifier->returnType,identifier->genericType);
    }
    compilerDebugPrintf("Ident full name %s\n",identifier->fullname);
    addCode(result, identifier ? identifier->fullname : ident);
//    compilerDebugPrintf("Result at %d = %d\n",__LINE__,result);
//    if (result->genericType)
//        compilerDebugPrintf("Result %d generic type %s\n",result,result->genericType);
    return result;
}

Object *objectSelfIdent(char *ident)
{
    compilerDebugPrintf("objectSelfIdent(%s)\n", ident);
    //must be Function or Constructor and be inside a class
    if ((current->category != Function && current->category != Constructor) || scope_idx == 0
        || scopeStack[scope_idx - 1]->category != Type) {
        criticalError(ERROR_ParseError,
                      "Cannot use self identifier ($) outside of class verbs.\n");
    }

    Object *result;

    result = CreateObject(0, 0, 0, Expression, scopeStack[scope_idx - 1]->returnType);
    addParam(result, scopeStack[scope_idx - 1]->returnType);
    char code[BUFFLEN];
    snprintf(code,BUFFLEN, IDENT_SELF_SELF);
    addCode(result, code);

    return result;
}

Object *objectfloat(float f)
{
    compilerDebugPrintf("objectfloat(%f)\n", f);
    char buffer[128];
    snprintf(buffer, BUFFLEN, "%f", f);
    Object *result = CreateObject(0, 0, 0, Expression, "float");
    addCode(result, buffer);
    addParam(result, "float");
    return result;
}

Object *objectInt(int d)
{
    compilerDebugPrintf("objectInt(%d)\n", d);
    char buffer[32];            // 20 = (log10(2^64))
    snprintf(buffer, 32, "%d", d);
    Object *result = CreateObject(0, 0, 0, Expression, "int");
    addCode(result, buffer);
    addParam(result, "int");
    return result;
}

Object *objectChar(char *c)
{
    compilerDebugPrintf("objectInt(%c)\n", c[1]);
    char buffer[4];            // 20 = (log10(2^64))
    snprintf(buffer, 4, "%s", c);
    Object *result = CreateObject(0, 0, 0, Expression, "char");
    addCode(result, buffer);
    addParam(result, "char");
    return result;
}

Object *objectString(char *string)
{
    //== RetVar shenanigans ==
    Object * rType = findByName("String");


    char retVarName[BUFFLEN];
    snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d_%d", scope_idx, retVarNumber);
    retVarNumber++;
    Object *retVar =
            CreateObject(retVarName, retVarName, 0, Variable, IDENT_MPTR);
    addSymbol(current, retVar);


    compilerDebugPrintf("objectString(%s)\n", string);
    char buffer[BUFFLEN];
    snprintf(buffer, BUFFLEN, "String" COMPILER_SEP "stringlit(%s,&%s)", string, retVarName);
    Object *result = CreateObject(0, 0, 0, Expression, "String");
    addCode(result, buffer);
    addParam(result, "String");
    return result;
}

Object *objectPrev()
{
    compilerDebugPrintf("objectPrev(%s)\n", previous[prev_idx]);
    Object *result = CreateObject(0, 0, 0, Expression, prevType[prev_idx]);
    addCode(result, previous[prev_idx]);
    addParam(result, prevType[prev_idx]);

    prependPrev();

    return result;

}

Object *objectPlaceHolderType(char *ident)
{
    compilerDebugPrintf("objectPlaceHolderType(%s)\n", ident);
    Object *result;
    Object *identifier = findByName(ident);

    if (!identifier) {
        compilerDebugPrintf("Creating type (%s)\n", ident);
        result = CreateObject(ident, ident, current, Type, "System");
    } else {
        criticalError(ERROR_ParseError,"Placeholder ident not available");
    }
    addCode(result, identifier ? identifier->fullname : ident);
    return result;
}

Object *conjugateAccessorIdent(Object *subject, char *field)
{
        //verify parent is defined
        char *subCodeValue = subject->code->value;
        compilerDebugPrintf("conjugateAccessorIdent subject->code->value %s\n", subCodeValue);

        //Object *oParent = subject;
        if (!subject) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Cannot find object named %s\n", subCodeValue);
            criticalError(ERROR_UndefinedVariable, error);
        }

        char *returnType = subject->returnType;
        compilerDebugPrintf("subject->code->value return type is %s %s\n", subCodeValue,returnType);

        if (!strcmp(returnType,"Generic_YTYPE$$")) {
            if (subject->genericType) {
                //ToDo Better Generics Handling
                returnType = subject->genericType->type;
                //compilerDebugPrintf("Setting gentype %s\n", parent);
            } else {
                errorMsg("No generic type for %s\n",subCodeValue);
                criticalError(ERROR_ParseError,"Generic Type not found\n");
            }
        }

        Object *oReturnType = findByName(returnType);

        Object *oField = 0;

        char newSubject[BUFFLEN];

        int subject_idx =0;
        subject_idx = snprintf(&newSubject[subject_idx], BUFFLEN - subject_idx,  "((%s *)( %s->obj))", returnType,
                               subCodeValue);


        //Todo - Make sure inheritance works with generics
        Object * parent = oReturnType;
        bool firstRound = true;
        while (!oField && parent) {
            compilerDebugPrintf("Trying parent class: %s\n", parent->name);
            ListObject *oIter = parent->definedSymbols;
            while (oIter) {
                if (!strcmp(oIter->value->name, field)) {
                    oField = oIter->value;
                    break;
                }
                oIter = oIter->next;
            }

            if (!oField) {
                subject_idx +=
                        snprintf(&newSubject[subject_idx], BUFFLEN - subject_idx, "->" IDENT_SUPER "_");
                parent = parent->parentClass;
            } else {
                break;
            }
        }

        if (!oField) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "%s %s has no member named %s\n", returnType,
                     subCodeValue, field);
            criticalError(ERROR_UndefinedVariable, error);
        }


        Object *result = CreateObject(field, field, 0, Expression, oField->returnType);
        char accessCode[BUFFLEN];
        compilerDebugPrintf ("Parent field %s %s\n",subCodeValue, field);
        snprintf(accessCode, BUFFLEN, "/* %d */ (%s)->%s", __LINE__, newSubject, field);
        //snprintf(accessCode, BUFFLEN, "/* %d */ ((%s *)(%s))->%s", __LINE__, parent->returnType, newSubject, field);
        addParam(result, oField->returnType);
        addCode(result, accessCode);
        setFlags(result,FLAG_IDENT_SELF);
        return result;

}

Object *findByName(char *name)
{
    Object *result = findByNameInScope(current, name, false);
    return result;
}

Object *findFunctionByFullName(char *name)
{
    Object *result = findByNameInScope(current, name, true);
    return result;
}

Object * directive(char *key, char *value) {
    compilerDebugPrintf ("Directive value %s\n",value);
    if (!strcmp(key,"##external")) {
        if (!strcmp(value,"\"\"")) {
            external = false;
        } else {
            external = true;
            fprintf(outMainFile, "#include %s\n",value);
        }
    }
    if (!strcmp(key,"##addsource")) {
        //No error checking
        external = true;
        fprintf(outMakeFile, "${RIX_HOME}/%s ",value);
    }
    Object *result = CreateObject(0, 0, 0, Expression, "void");
    return result;
}

#define LINESIZE 1024

int rixParse(FILE *fp)
{
    compilerDebugPrintf("Rix Parse %d\n",yylineno);    
        yylineno=1;
        yyin = fp;
        hitEOF = false;                                        
        while (!hitEOF) {
            compilerDebugPrintf("Read a line\n");    
            yyparse();
        }
}

int openFiles(char name[])
{
    FILE *fp;
    char line[LINESIZE], word[LINESIZE], word2[LINESIZE];
    size_t len = strlen(name);
    if (name[len - 1] == '"')
        name[len - 1] = '\0';
    
    if (name[0] == '"')
    {
        name++;
    }
    compilerDebugPrintf("Checking %s\n",name);     
    if ((fp = fopen(name, "r+")) == 0)
    {
        compilerDebugPrintf("Cannot find import file %s in working directory. Trying RIX_HOME\n", name);
        const char *RIX_HOME = getenv("RIX_HOME");
        char importPath[BUFFLEN];
        if (RIX_HOME != 0)
        {
            snprintf(importPath, BUFFLEN, "%s/%s", RIX_HOME, name);
            if ((fp = fopen(importPath, "r+")) == 0)
            {
                printf("Cannot find import file %s in working directory or RIX_HOME\n",
                       importPath);
                perror("open");
                return 1;
            } else {
                //compilerDebugPrintf("Found file %s RIX_HOME\n",name);
            }
        }
        else
        {
            criticalError(0, "RIX_HOME not set.\n");
            perror("open");
            return 1;
        }
    }
    compilerDebugPrintf("Reading %s\n",name);     
    while (fgets(line, LINESIZE, fp))
    {
        char *importPos = strstr(line, "import");
        if (importPos != NULL)
        {
            char *openParenPos = strchr(importPos, '(');
            if (openParenPos != NULL)
            {
                char *word2 = malloc(BUFFLEN);
                char *cursor = openParenPos + 1;
                int i = 0;
                while (*(cursor + i) != ')')
                {
                    word2[i] = *(cursor + i);
                    i++;
                }
                word2[i] = '\0';
                //compilerDebugPrintf("Recursing %s\n",word2);
                openFiles(word2);
            }
        }
    }    
    compilerDebugPrintf("Parsing %s\n",name);
    rewind(fp);
    rixParse(fp);
    return 0;        
}

int main(int argc, char **argv)
{
    int c, i, fd, old_stdout;
    int errflg = 0;
    bool printTreeBool = false;
    int numline = 0;
    char *ifile = NULL;
    char *ofile = NULL;
    extern char *optarg;
    extern int optind, optopt;
    FILE *ritTempFile;
    bool quiet = false;
    g_headerLines = 0;
    bool waferSupport = false;
    bool isLibrary = false;
    while ((c = getopt(argc, argv, "o:tqWL")) != -1) {
        switch (c) {
        case 't':
            compilerDebugPrintf("hit -t arg\n");
            printTreeBool = true;
            break;

        case 'o':
            ofile = optarg;
            break;

        case 'W':
            waferSupport = true;
            break;
            
        case 'L':
            isLibrary = true;
            break;
            
        case ':':              /* -f or -o without operand */
            fprintf(stderr, "Option -%c requires an operand\n", optopt);
            errflg++;
            break;

        case 'q':
            quiet = true;
        };
    }

    if (errflg) {
        fprintf(stderr, "usage: . . . ");
        exit(2);
    }

    for (i = 0; optind < argc; optind++, i++) {
        if (i == 0) {
            ifile = argv[optind];
        }
    }

    if (ifile == NULL) {
        errorMsg("No file to compile\n");
//      file = fopen("helloworld.rix", "r");
        criticalError(ERROR_ParseError, "No file to compile specified");
    } else {
        file = fopen(ifile, "r");
    }

    char oMainFileName[BUFFLEN];
    char oHeaderFileName[BUFFLEN];
    char oMakeFileName[BUFFLEN];
    char oCompilerLogFileName[BUFFLEN];

    if (ofile == NULL) {
        strcpy(oMainFileName, "out.c");
        strcpy(oHeaderFileName, "out.h");
        strcpy(oMakeFileName, "out.sh");
        strcpy(oCompilerLogFileName, "out.log");
    } else {
        snprintf(oMainFileName, BUFFLEN, "%s.c", ofile);
        snprintf(oHeaderFileName, BUFFLEN, "%s.h", ofile);
        snprintf(oMakeFileName, BUFFLEN, "%s.sh", ofile);
        snprintf(oCompilerLogFileName, BUFFLEN, "%s.log", ofile);
    }


    root = CreateObject("RootScope", "RootScope", 0, CodeBlock, "int");

    scopeStack[scope_idx] = root;
    current = scopeStack[scope_idx];

    outCompilerLogFile = fopen(oCompilerLogFileName, "w");
    if (!outCompilerLogFile) {
        errorMsg("Can't create log files\n");
//      file = fopen("helloworld.rix", "r");
        criticalError(ERROR_ParseError, "Can't create log");
    }

    defineRSLSymbols(root, waferSupport);


    compilerDebugPrintf("%s\n", ifile);
    
    outMainFile = fopen(oMainFileName, "w");
    outHeaderFile = fopen(oHeaderFileName, "w");
    outMakeFile = fopen(oMakeFileName, "w");

    fprintf(outMakeFile, "gcc -Wno-implicit-function-declaration -lm -I ${RIX_HOME} -ggdb -o %s.out "
            "%s.c ${RIX_HOME}/rsl/rsl.c ${RIX_HOME}/errors.c ", ofile, ofile);

        
    openFiles("rsl/rsl.rix");
    openFiles(ifile);


    fprintf(outMakeFile, " -lm");

    if (!quiet) {
        printf("=============  Compilation Complete!  ==============\n");
        printf("Wrote source file %s, header file %s, build file %s and compilation log file %s\n",
               oMainFileName,oHeaderFileName,oMakeFileName,oCompilerLogFileName);
    }


    fprintf(outMainFile, "#include \"rsl/rsl.h\"\n");
    fprintf(outMainFile, "#include \"%s\"\n", oHeaderFileName);

    if (waferSupport)
        fprintf(outMainFile, "void server(Request * __$$__request, Response * __$$__response) {\n"
                             "\t_$_U_VARIABLE(request);\n"
                             "\t_$_U_VARIABLE(response);\n"
                             "\tRequest_$_Request_$_(__$$__request, request);\n"
                             "\tResponse_$_Response_$_(__$$__response, response);\n");
    else if (isLibrary)
        fprintf(outMainFile, "/* This is a library */");                             
    else
        fprintf(outMainFile, "int main(int _$$_argc_, char **_$$_argv_) {\n"
                             "    _$$_argc=_$$_argc_;\n"
                             "    _$$_argv=_$$_argv_;\n");

    writeTree(outMainFile, outHeaderFile, root);
    if (printTreeBool == 1) {
        //printTreeToFile(root, 0, "./treeOutput.txt");
    }
    
	if (!isLibrary)
		fprintf(outMainFile, "  return 0;\n}\n");
		
    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(outMakeFile);
    fclose(file);
    remove("rix_temp_file.rix");

    //compilerDebugPrintf("\n%s compiled successfully.\n", ifile);

    return 0;
}

ListType * concatGenerics(char * existing, ListType * newGeneric) {return newGeneric;}

ListType * genericOfGeneric(char * parent, ListType * child) {return child;}

bool beginsWith(const char *haystack, const char *needle)
{
    return strncmp(needle, haystack, strlen(needle)) == 0;
}