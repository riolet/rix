#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>

#include "ritc.h"
#include "crsl.h"


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

Object *beginClass(char *className, char *parentName, Object *typeArgs)
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
        ListString * list = typeArgs->paramTypes;
        compilerDebugPrintf(" Paramtypes: ");
        while (list) {
            compilerDebugPrintf(" %s ", typeArgs->paramTypes->value);
            list = list->next;
        }
        compilerDebugPrintf(" Paramtypes\n");

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

Object *beginFunction(char *returnType, char *funcName, Object * parameters)
{
    if (returnType == 0) {
        criticalError(ERROR_ParseError, "Return category mustn't be null.\n");
    }
    ListString *types = parameters->paramTypes;
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
                         COMPILER_SEP "%s", types->value);
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
            addSymbol(result, CreateObject("self", "self", 0, Variable, pointer));
            //addSymbol(result, CreateObject(COMPILER_SEP "prev", COMPILER_SEP "prev", 0, Variable, COMPILER_SEP "Last"));
        }
    }
    //add parameters to the function
    types = parameters->paramTypes;
    //assuming for every category there is a name
    while (types != 0) {
        addSymbol(result,
                  CreateObject(names->value, names->value, 0, Variable, types->value));
        addParam(result, types->value);
        names = names->next;
        types = types->next;
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
    ListString *types = parameters->paramTypes;
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
        compilerDebugPrintf("Adding category %s\n",types->value);
        funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     COMPILER_SEP "%s", types->value);
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
        addSymbol(result,
                  CreateObject(names->value, names->value, 0, Variable, types->value));
        addParam(result, types->value);
        names = names->next;
        types = types->next;
    }

    compilerDebugPrintf("External ctor %d\n",external);
    if (external) {
        setFlags(result, FLAG_EXTERNAL);
    } else {
        //Add allocation code
        char allocator[BUFFLEN];
        //_$_returnAppointer($_mptr_in,newString,String_$_destructor_$_);
        snprintf(allocator, BUFFLEN, "%s * self_ = calloc(1, sizeof(%s));\n"
                         IDENT_MPTR " * self = _$_returnAppointer(_$_mptr_in,self_,%s_$_destructor_$_);", returnType,
                 current->name,returnType);
        addCode(result, allocator);


        if (!getFlag(current->parentClass,FLAG_PRIMITIVE)) {
            char retVarName[BUFFLEN];
            snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d", retVarNumber);
            retVarNumber++;
            Object *retVar =
                    CreateObject(retVarName, retVarName, 0, Variable, IDENT_HEAP_MPTR);
            addSymbol(result, retVar);

            //Todo: Handle heap variables

            snprintf(allocator, BUFFLEN, "self_->" IDENT_SUPER "= %s" COMPILER_SEP "%s" COMPILER_SEP "(%s);",
                     current->parentClass->name, current->parentClass->name, retVarName);

        } else {
//            snprintf(allocator, BUFFLEN, "self_->" IDENT_SUPER "= %s" COMPILER_SEP "%s" COMPILER_SEP "();",
//                      current->parentClass->name, current->parentClass->name);
        }
        addCode(result, allocator);

        snprintf(allocator, BUFFLEN, "self_->" IDENT_SUPER "_= self_->"IDENT_SUPER"->obj;");
        addCode(result, allocator);

        //Add field allocators
        ListObject *oIter;

        oIter = current->definedSymbols;

        //fprintf(outh, "%s %s " COMPILER_SEP "%s %s;\n", "typedef", "struct", tree->name,
        //        tree->name);
        //fprintf(outh, "%s " COMPILER_SEP "%s {\n", "struct", tree->name);

        while (oIter != 0) {

            if (strcmp(oIter->value->name,IDENT_SUPER)&&strcmp(oIter->value->name,IDENT_SUPER "_"))
            {
                if (oIter->value->category == Variable) {
                    Object * rType = findByName(oIter->value->returnType);
                    if (!getFlag(rType,FLAG_PRIMITIVE)) {

                        char retVarName[BUFFLEN];
                        snprintf(retVarName, BUFFLEN, IDENT_MPTR "%d", retVarNumber);
                        retVarNumber++;
                        Object *retVar =
                                CreateObject(retVarName, retVarName, 0, Variable, IDENT_HEAP_MPTR);
                        addSymbol(result, retVar);

                        //Todo: Handle heap variables
                        snprintf(allocator, BUFFLEN, "self_->%s= %s" COMPILER_SEP "%s" COMPILER_SEP "(%s);",
                                 oIter->value->name, oIter->value->returnType, oIter->value->returnType, retVarName);
                        addCode(result, allocator);

                    } else {
//                        snprintf(allocator, BUFFLEN, "self_->%s= %s" COMPILER_SEP "%s" "();",
//                                 oIter->value->name, oIter->value->returnType, oIter->value->returnType);
                    }

                } else {
                    oIter = oIter->next;
                    break;
                }
            }
            oIter = oIter->next;

        }
    }
    addSymbol(parentScope, result);
    scope_push(result);

    return result;
}

void doneConstructor(Object * tree)
{
    if (!external) {
        addCode(current, "return self;");
    }
    scope_pop();
}

Object *beginDestructor(Object * parameters)
{
    if (current->category != Type) {
        criticalError(ERROR_ParseError, "Destructor can only exist inside a class.\n");
    }
    ListString *types = parameters->paramTypes;
    ListString *names = parameters->code;

    char funcFullName[BUFFLEN];
    int funcFullName_pos = 0;

    funcFullName_pos +=
            snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                     "%s" COMPILER_SEP "%s" COMPILER_SEP, current->name, "destructor");

    while (types != 0) {
        funcFullName_pos +=
                snprintf(&funcFullName[funcFullName_pos], BUFFLEN - funcFullName_pos,
                         COMPILER_SEP "%s", types->value);
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
    snprintf(returnType, BUFFLEN, "%s", "destructor");

    Object *result =
            CreateObject(current->name, funcFullName, parentScope, Function, returnType);
    result->parentClass = current;

    ListObject *oIter;

    oIter = current->definedSymbols;
    char deallocator[BUFFLEN];

    while (oIter != 0) {

        if (strcmp(oIter->value->name,IDENT_SUPER "_"))
        {
            if (oIter->value->category == Variable) {
                Object * rType = findByName(oIter->value->returnType);
                if (!getFlag(rType,FLAG_PRIMITIVE)) {
                    snprintf(deallocator, BUFFLEN, "_$_cleanup(((%s *)" IDENT_MPTR "_in->obj)->%s);",
                             current->returnType, oIter->value->name);
                    addCode(result, deallocator);
                    snprintf(deallocator, BUFFLEN, "free(((%s *)" IDENT_MPTR "_in->obj)->%s);",
                             current->returnType, oIter->value->name);
                    addCode(result, deallocator);
                } else {
//                        snprintf(allocator, BUFFLEN, "self_->%s= %s" COMPILER_SEP "%s" "();",
//                                 oIter->value->name, oIter->value->returnType, oIter->value->returnType);
                }

            } else {
                oIter = oIter->next;
                break;
            }
        }
        oIter = oIter->next;

    }

    //self destruct
//    snprintf(deallocator, BUFFLEN, "free(((%s *)" IDENT_MPTR "_in->obj)->" IDENT_SUPER ");",current->returnType);
//    addCode(result, deallocator);

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

Object *funcParameters(Object * paramList, char *paramType, char *paramName)
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

    addParam(result, type->returnType);
    addCode(result, paramName);
    return result;
}

Object *concatParams(Object * existing, Object * newParam)
{
    Object *result = CreateObject(0, 0, 0, Expression, 0);
    ListString *paramIter;
    ListString *codeIter;
    int paramLen;
    int codeLen;
    char *lastCode;
    char *lastParam;
    char newCode[BUFFLEN];

    //insert any declarations from the old objects
    paramIter = existing->paramTypes;
    codeIter = existing->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while (codeLen > 0 && codeLen > paramLen) {
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert any declarations from the new object
    paramIter = newParam->paramTypes;
    codeIter = newParam->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while (codeLen > 0 && codeLen > paramLen) {
        addCode(result, codeIter->value);
        codeLen--;
        codeIter = codeIter->next;
    }

    //insert parameters of old objects
    paramIter = existing->paramTypes;
    codeIter = existing->code;
    paramLen = listlen(paramIter);
    codeLen = listlen(codeIter);
    while (codeLen > 0) {
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
    while (codeLen > 0) {
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

Object *declareVariable(char *name, char *type)
{
    Object *oType = findByName(type);
    Object *var = CreateObject(name, name, 0, Variable, oType->returnType);
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

Object *finalize(Object * expression)
{
    //compilerDebugPrintf("Expression %d at %d", expression,__LINE__);
    char prevName[BUFFLEN];
    int length =
        snprintf(prevName, BUFFLEN, COMPILER_SEP "prev.p" "%s", expression->returnType);
    while (prevName[length] == ' ' || prevName[length] == '*' || prevName[length] == '\0') {
        length--;
    }
    prevName[length + 1] = '\0';        //drop the " *" off the end if it's there
    previous[prev_idx] = strdup(prevName);
    compilerDebugPrintf("Finalize expression %s %s\n",expression->code->value, expression->returnType);
    prevType[prev_idx] = strdup(expression->returnType);
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
        snprintf(newCode, BUFFLEN, IDENT_MPTR "_prepare(%s, " IDENT_MPTR "_in);\n"
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
                      "previous result tracker went below 0. (decPrev, ritc.c)\n");
    }
}

Object *injectC(char *code)
{
    addCode(current, code);
    return 0;
}

Object *conjugateAssign(Object * subject, Object * verb, Object * objects)
{
    if (subject == 0) {
        criticalError(ERROR_ParseError, "Cannot assign to nothing.\n");
    }
    ListString *paramIter;
    Object *realVerb = 0;
    Object *result = 0;
    char verbname[BUFFLEN];
    char tempBuffer[BUFFLEN];
    int verbname_pos = 0;

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
                         COMPILER_SEP "%s", paramIter->value);
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
        if (!objects) {
            criticalError(ERROR_ParseError, "Object of assignment was not found.\n");
        }
        if (!objects->returnType) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Paramtypes not found for %s %d.\n", objects->name,__LINE__);
            criticalError(ERROR_ParseError, error);
        }
        result = CreateObject(0, 0, 0, Expression, objects->returnType);
        addParam(result, objects->returnType);
        if (objects->genericType) {
            result->genericType = strdup(objects->genericType);
        }

        if (!subject->returnType) {
            if (subject->category == NewMarkedIdent) {
                compilerDebugPrintf ("Creating new variable %s as %s\n",subject->name, objects->returnType);
                Object *variable =
                    CreateObject(subject->name, subject->fullname, 0, Variable,
                                 objects->returnType);

                    if (objects->genericType) {
                        variable->genericType = strdup(objects->genericType);
                    }
//                    compilerDebugPrintf("objects at %d = %d, variable %s %d\n",__LINE__,objects,variable->name,variable);
//                    if (variable->genericType)
//                        compilerDebugPrintf("variable %d generic type %s\n",variable,variable->genericType);
                    addSymbol(current, variable);
            } else {
                char error[1024];
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

        if (getFlag(rType,FLAG_PRIMITIVE)) {
            snprintf(verbname, BUFFLEN, "%s = %s", subject->code->value,
                     objects->code->value);
        } else {
            snprintf(verbname, BUFFLEN, MPTR_ASSIGN "(%s,%s)", subject->code->value,
                     objects->code->value);
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
        paramIter = objects->code;
        while (paramIter != 0) {
            if (paramIter->next == 0) {
                verbname_pos +=
                    snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s",
                             paramIter->value);
            } else {
                verbname_pos +=
                    snprintf(&verbname[verbname_pos], BUFFLEN - verbname_pos, "%s,",
                             paramIter->value);
            }
            paramIter = paramIter->next;
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
                variable->genericType = strdup(objects->genericType);
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
    ListString *paramIter = 0;
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
        while (verbname[verbname_pos - 1] == '*' || verbname[verbname_pos - 1] == ' ') {
            verbname_pos--;
        }
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
                         COMPILER_SEP "%s", paramIter->value);
            genericVerbNamePos +=
                snprintf(&genericVerbName[genericVerbNamePos],
                         BUFFLEN - genericVerbNamePos, COMPILER_SEP "%s%d", GENERIC_PARAM,
                         paramNumber);
            while (verbname[verbname_pos - 1] == '*' || verbname[verbname_pos - 1] == ' ') {
                verbname_pos--;
            }
            verbname[verbname_pos] = '\0';
            genericVerbName[genericVerbNamePos] = '\0';
            paramTypes[paramNumber] = strdup(paramIter->value);
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
    if (!realVerb) {
        realVerb = findFunctionByFullName(genericVerbName);
    }

    if (!realVerb && subject) {
        //This part looks for an inherited method via parent classes.
        //TODO: this seems too complex. It should be revisited.
        char newName[BUFFLEN];
        char newSubject[BUFFLEN];
        char paramTypes[BUFFLEN][BUFFLEN];
        int subject_idx = snprintf(newSubject, BUFFLEN,   "((%s *) (%s->obj))->",subject->returnType, subject->code->value);
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
                   || !strcmp(objects->paramTypes->value, "float")) {
            result = CreateObject(0, 0, 0, Expression, "float");
            addParam(result, "float");
        } else {
            result = CreateObject(0, 0, 0, Expression, subject->returnType);
            if (subject->genericType) {
                result->genericType = strdup(subject->genericType);
            }
            addParam(result, subject->returnType);
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
        snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s(",
                 realVerb->fullname);

    //Get the generic return category
    char *returnType;
    if (strcmp(realVerb->returnType, "Generic_$$")) {
        returnType = strdup(realVerb->returnType);
    } else {
        if (realVerb->genericType)
            //Preset return category
            returnType = strdup(realVerb->genericType);
        else
            //Positional return category
            returnType=paramTypes[realVerb->genericTypeArgPos - 1];
    }

    result = CreateObject(0, 0, 0, Expression, returnType);

    if (subject) {
        if (objects)
            invoke_pos +=
                snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s,",
                         subject->code->value);
        else
            invoke_pos +=
                snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s",
                         subject->code->value);
    } else if (strstr(verb->name, COMPILER_SEP)) {
        //This is assumed to be a static verb of some class. Inject a 0 as first argument.
        if (objects) {
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "0,");
        } else {
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "0");
        }
    }

    if (objects) {
        codeIter = objects->code;
        while (codeIter) {
            if (codeIter->next) {
                invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s,",
                             codeIter->value);
            } else {
                //this is the last entry
                invoke_pos +=
                    snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s",
                             codeIter->value);
            }
            codeIter = codeIter->next;
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
        if (subject||objects) {
            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ", ");
        }
        invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "&%s", retVarName);
    }

    //== Generic shenanigans ==
    //compilerDebugPrintf("Conjverb Generic type %s\n",verb->genericType);
    if (verb->genericType) {
//        if (!getFlag(rType,FLAG_PRIMITIVE)) {
//            invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ", ");
//        }
//        invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, "%s", verb->genericType);
        result->genericType = strdup(verb->genericType);
    }

    //Close
    invoke_pos += snprintf(&invocation[invoke_pos], BUFFLEN - invoke_pos, ")");

    if (!strcmp(realVerb->returnType, "Generic_$$")) {
        if (realVerb->genericType) {
            result->genericType = strdup(realVerb->genericType);
            addParam(result, realVerb->genericType);
        } else {
            result->genericType = strdup(paramTypes[realVerb->genericTypeArgPos - 1]);
            addParam(result, paramTypes[realVerb->genericTypeArgPos - 1]);
        }
    } else if (!strcmp(realVerb->returnType, "Generic_YTYPE$$")) {
        //compilerDebugPrintf("Subject %s at %d = %d\n",subject->name,__LINE__,subject);
        if (subject->genericType) {
            result->genericType = strdup(subject->genericType);
            addParam(result, subject->genericType);
        } else {
            compilerDebugPrintf("Subject %s has no generic type at %d\n", subject->name, __LINE__);
        }
    } else {
        addParam(result, realVerb->returnType);
    }

    if (realVerb && getFlag(realVerb, FLAG_SAVERESULT)) {
        char temp[BUFFLEN];
        snprintf(temp, BUFFLEN, COMPILER_SEP "prev.pbool = %s", invocation);
        addCode(result, temp);
    } else {
        addCode(result, invocation);
    }
    compilerDebugPrintf("\tConjugated: (%d) %s at \n", __LINE__, invocation);
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

Object *verbCtor(char *type, char *ytype)
{
    compilerDebugPrintf("verbCtor(%s)\n", type);
    Object *result = findByName(type);
    if (result == 0) {
        char error[BUFFLEN];
        sprintf(error, "Cannot find Class \"%s\".\n", type);
        criticalError(ERROR_UndefinedVerb, error);
    }
    if (ytype) {
        compilerDebugPrintf("Setting generic ytype (%s)\n", ytype);
        result->genericType=ytype;
    }
    return result;
}

Object *parenthesize(Object * expr)
{
    compilerDebugPrintf("parenthesize(%s)\n", expr->code->value);
    char line[BUFFLEN];
    if (expr == 0) {
        criticalError(ERROR_ParseError,
                      "Object* expr was void in parenthesize. (ritc.c)\n");
    }

    Object *parenthesized =
        CreateObject(expr->name, expr->fullname, expr->parentScope, Expression,
                     expr->returnType);

    if (expr->code == 0) {
        criticalError(ERROR_ParseError,
                      "Cannot put parentheses around nothing. (ritc.c)\n");
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
        addParam(result, identifier->returnType);
        if (identifier->genericType)
            addParam(result, identifier->genericType);
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
        addParam(result, identifier->returnType);
        if (identifier->genericType)
            result->genericType = strdup(identifier->genericType);
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
    Object *identifier;

    if (strlen(ident) == 1) {
        //must be $ by itself.
        result = CreateObject(0, 0, 0, Expression, scopeStack[scope_idx - 1]->returnType);
        addParam(result, scopeStack[scope_idx - 1]->returnType);
        char code[BUFFLEN];
        snprintf(code,BUFFLEN, "((%s *) (self->obj))",current->returnType);
        addCode(result, code);
    } else {
        ident += 2;             // bypass the "$."
        identifier = findByNameInScope(scopeStack[scope_idx - 1], ident, false);

        result = CreateObject(ident, ident, 0, Variable, identifier->returnType);
        addParam(result, identifier->returnType);
        char code[BUFFLEN];
        snprintf(code, BUFFLEN, "((%s *) (self->obj))->%s", current->returnType,ident);
        addCode(result, code);
    }
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
    Object *result = CreateObject(0, 0, 0, Expression, "Char");
    addCode(result, buffer);
    addParam(result, "Char");
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

    if (subject->category == Type) {
        //build verb name
        char verbname[BUFFLEN];
        snprintf(verbname, BUFFLEN, "%s" COMPILER_SEP "%s", subject->name, field);
        Object *result = CreateObject(verbname, verbname, 0, Function, 0);
        return result;
    } else {

        //verify parent is defined
        char *parent = subject->code->value;
        compilerDebugPrintf("Parent %s\n", parent);

        Object *oParent = subject;
        if (!oParent) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "Cannot find object named %s\n", parent);
            criticalError(ERROR_UndefinedVariable, error);
        }

        char *parentType = oParent->returnType;
        if (!strcmp(parentType,"Generic_YTYPE$$")) {
            if (subject->genericType) {
                parentType = subject->genericType;
                //compilerDebugPrintf("Setting gentype %s\n", parent);
            } else {
                errorMsg("No generic type for %s\n",parent);
                criticalError(ERROR_ParseError,"Generic Type not found\n");
            }
        }
        //remove " *" from the end if present.
        int length = 0;
        while (parentType[length] != '\0' && parentType[length] != ' '
               && parentType[length] != '*') {
            length++;
        }
        parentType[length] = '\0';

        Object *oParentType = findByName(parentType);
        if (!oParentType) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN,
                     "Cannot find \"%s\" class description for object %s\n", parentType,
                     parent);
            criticalError(ERROR_UndefinedVariable, error);
        }

        Object *oField = 0;
        //verify the category of parent has defined a variable named field
        ListObject *oIter = oParentType->definedSymbols;
        while (oIter) {
            if (!strcmp(oIter->value->name, field)) {
                oField = oIter->value;
                break;
            }
            oIter = oIter->next;
        }

        if (!oField) {
            char error[BUFFLEN];
            snprintf(error, BUFFLEN, "%s %s has no member named %s->\n", parentType,
                     parent, field);
            criticalError(ERROR_UndefinedVariable, error);
        }

        Object *result = CreateObject(field, field, 0, Expression, oField->returnType);
        char accessCode[BUFFLEN];
        compilerDebugPrintf ("Parent field %s %s\n",parent, field);
        snprintf(accessCode, BUFFLEN, "((%s *)%s->obj)->%s", parentType, parent, field);
        addParam(result, oField->returnType);
        addCode(result, accessCode);
        return result;
    }
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
        fprintf(outMakeFile, "${RITCHIE_HOME}/%s ",value);
    }
    Object *result = CreateObject(0, 0, 0, Expression, "void");
    return result;
}

int main(int argc, char **argv)
{
    int c, i, fd, old_stdout;
    int errflg = 0;
    int printTreeBool = 0;
    int numline = 0;
    char *ifile = NULL;
    char *ofile = NULL;
    extern char *optarg;
    extern int optind, optopt;
    FILE *ritTempFile;
    bool quiet = false;

    while ((c = getopt(argc, argv, "o:t")) != -1) {
        switch (c) {
        case 't':
            compilerDebugPrintf("hit -t arg\n");
            printTreeBool = 1;
            break;

        case 'o':
            ofile = optarg;
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
//      file = fopen("helloworld.rit", "r");
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

//    errorMsg(ANSI_COLOR_MAGENTA "\n"
//             "**********************************\n"
//             "**********************************\n"
//             "**********************************\n"
//             "**********************************\n" ANSI_COLOR_RESET);

    root = CreateObject("RootScope", "RootScope", 0, CodeBlock, "int");
    //addSymbol(root, CreateObject(COMPILER_SEP "prev", COMPILER_SEP "prev", 0, Variable, COMPILER_SEP "Last"));
    scopeStack[scope_idx] = root;
    current = scopeStack[scope_idx];
    defineRSLSymbols(root);

    ritTempFile = fopen("ritchie_temp_file.rit", "w");
    if (ritTempFile == 0) {
        perror("fopen");
        return 1;
    }

    outCompilerLogFile = fopen(oCompilerLogFileName, "w");
    compilerDebugPrintf("%s\n", ifile);

    //Read RSL
    readFile("rsl/rsl.rit", ritTempFile, &numline);

    //compilerDebugPrintf("Lines read %d\n",numline);
    //Read mainfile
    readFile(ifile, ritTempFile, &numline);

    fprintf(ritTempFile,"\n"); //END OF FILE GUARANTEE!
    fclose(ritTempFile);

    file = fopen("ritchie_temp_file.rit", "r+");

    yyin = file;

    outMainFile = fopen(oMainFileName, "w");
    outHeaderFile = fopen(oHeaderFileName, "w");
    outMakeFile = fopen(oMakeFileName, "w");


    fprintf(outMakeFile, "gcc -lm -I /home/rohana/Projects/ritchie -ggdb -o %s.out "
            "%s.c ${RITCHIE_HOME}/rsl/rsl.c ${RITCHIE_HOME}/errors.c ", ofile, ofile);
    //getln();
    hitEOF = false;
    while (!hitEOF) {
        yyparse();
    }
    fprintf(outMakeFile, " -lm");

    if (!quiet)
        printf("=============  Compilation Complete!  ==============\n");



    fprintf(outMainFile, "#include \"rsl/rsl.h\"\n");
    fprintf(outMainFile, "#include \"%s\"\n", oHeaderFileName);
    //fprintf(outMainFile,"int _$$_argc;\n");
    //fprintf(outMainFile,"char **_$$_argv;\n");
    fprintf(outMainFile, "int main(int _$$_argc_, char **_$$_argv_) {\n");
    fprintf(outMainFile, "    _$$_argc=_$$_argc_;\n");
    fprintf(outMainFile, "    _$$_argv=_$$_argv_;\n");

    writeTree(outMainFile, outHeaderFile, root);
    if (printTreeBool == 1) {

        printTreeToFile(root, 0, "./treeOutput.txt");
    }

    fprintf(outMainFile, "  return 0;\n}\n");
    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(outMakeFile);
    fclose(file);
    remove("ritchie_temp_file.rit");

    //compilerDebugPrintf("\n%s compiled successfully.\n", ifile);

    return 0;
}
