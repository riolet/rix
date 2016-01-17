#include <stdbool.h>
#include "ObjectTree.h"

#include "stdlib.h"
#include "stdio.h"

//mallocs memory and returns a pointer to a new Object
Object *CreateObject(char *name, char *fullname, Object * parentScope, OBJ_TYPE type,
                     char *returnType)
{

    Object *result = (Object *) malloc(sizeof(Object));

    if (result == 0) {
        warningMsg("%s", "CreateObject couldn't allocate a new object. (ObjectTree.c)\n");
        return 0;               //malloc failed.
    }

    result->name = name ? strdup(name) : 0;
    result->fullname = fullname ? strdup(fullname) : 0;
    result->parentClass = 0;
    result->parentScope = parentScope;
    result->category = type;
    result->returnType = returnType ? strdup(returnType) : 0;
    result->paramTypes = 0;
    result->definedSymbols = 0;
    result->code = 0;
    return result;
}

//UNTESTED
/*
void cleanup(Object* object) {

  ListObject* lop, * loi;
  ListString* lsp, * lsi;

  if(object->name != 0)
    free(object->name);
  if(object->fullname != 0)
    free(object->fullname);
  if(object->returnType != 0)
    free(object->returnType);

  if(object->paramTypes != 0) {
    lsi = object->paramTypes;
      while(lsi != 0) {
        lsp = lsi->next;
        free(lsi->value);
        free(lsi->next);
        free(lsi);
        lsi = lsp;
      }
    }
  if(object->code != 0) {
    lsi = object->code;
      while(lsi != 0) {
        lsp = lsi->next;
        free(lsi->value);
        free(lsi->next);
        free(lsi);
        lsi = lsp;
      }
    }
  if(object->definedSymbols != 0) {
    loi = object->definedSymbols;
    while(loi != 0) {
      lop = loi->next;
      cleanup(loi);
      free(loi->value);
      free(loi->next);
      free(loi);
      loi = lop;
    }
  }
  if(object != 0)
    free(object);
}

*/
//append item to end of linked list
int addParam(Object * tree, char *type)
{

    ListString *node = malloc(sizeof(ListString));

    if (node == 0) {
        warningMsg("Allocation failed in addParam. (ObjectTree.c)\n");
        return 1;
    }

    node->value = strdup(type);
    node->next = 0;

    if (node->value == 0) {
        warningMsg("strdup failed in addParam. (ObjectTree.c)\n");
        return 2;
    }

    if (tree->paramTypes == 0) {
        tree->paramTypes = node;
        return 0;
    }

    ListString *tail = tree->paramTypes;

    while (tail->next != 0) {
        tail = tail->next;
    }

    tail->next = node;
    return 0;

}

//append item to end of linked list
int addGenericType(Object * tree, char *genericType, int genericTypeArgPos)
{
    tree->genericType = genericType ? strdup(genericType) : 0;
    tree->genericTypeArgPos = genericTypeArgPos;
    return 0;
}

int addSymbol(Object * tree, Object * leaf)
{
    ListObject *node = malloc(sizeof(ListObject));

    if (node == 0) {
        warningMsg("Allocation failed in addSymbol. (ObjectTree.c)\n");
        return 1;
    }

    node->value = leaf;
    node->next = 0;

    if (tree->definedSymbols == 0) {
        tree->definedSymbols = node;
        return 0;
    }

    ListObject *tail = tree->definedSymbols;

    while (tail->next != 0) {
        tail = tail->next;
    }

    tail->next = node;
    return 0;
}

ListString *pushCode(Object * tree, char *line)
{
    ListString *node = malloc(sizeof(ListString));
    if (node == 0) {
        warningMsg("Allocation failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    node->value = strdup(line);


    if (node->value == 0) {
        warningMsg("strdup failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    if (tree->code == 0) {
        tree->code = node;
        return node;
    }

    node->next  = tree->code;
    tree->code = node;
    return node;
}

ListString *insertCode(Object * tree, char *line)
{
    ListString *node = malloc(sizeof(ListString));
    if (node == 0) {
        warningMsg("Allocation failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    node->value = strdup(line);


    if (node->value == 0) {
        warningMsg("strdup failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    if (tree->code == 0) {
        tree->code = node;
        return node;
    }

    node->next  = tree->code;
    tree->code = node;
    return node;
}

ListString *addCode(Object * tree, char *line)
{
    ListString *node = malloc(sizeof(ListString));
    if (node == 0) {
        warningMsg("Allocation failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    node->value = strdup(line);
    node->next = 0;

    if (node->value == 0) {
        warningMsg("strdup failed in addCode. (ObjectTree.c)\n");
        return 0;
    }

    if (tree->code == 0) {
        tree->code = node;
        return node;
    }

    ListString *tail = tree->code;

    while (tail->next != 0) {
        tail = tail->next;
    }

    tail->next = node;

    return node;
}

int listlen(ListString * head)
{
    int count = 0;
    while (head != 0) {
        count++;
        head = head->next;
    }
    return count;
}

int setFlags(Object * tree, int flags)
{
    tree->flags |= flags;
}

int getFlag(Object * tree, int flag)
{
    return ((tree->flags & flag) == flag);
}

int setParentClass(Object * tree, Object * parentClass)
{
    tree->parentClass = parentClass;
}

/* In a CodeBlock,
   Search the parent scope
   for V, T, C, F, CB
*/
Object *searchCodeBlock(Object * scope, char *name, int bUseFullName)
{

    //printf("\tSearch: CodeBlock\n");
    Object *result = 0;

    //search locally first
    ListObject *iter = scope->definedSymbols;

    while (iter != 0) {

        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {

            result = iter->value;

            break;

        }

        iter = iter->next;

    }

    //try looking in the parent scope, if any
    if (!result && scope->parentScope != 0) {

        result = findByNameInScope(scope->parentScope, name, bUseFullName);

        if (result) {

            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
        } else {

            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }

    }
    //printf("\tsearched %s, returning %s\n", scope->fullname, result ? result->fullname : "(null)");
    return result;

}

/*In a Function,
    Search the owning class (if available)
      for V, T, C, F, CB
    Search the parent scope
      for T, F
*/


bool isVerb (Object *result) {
    return result->category == Function ||
            result->category == Constructor ||
            result->category == Destructor ||
            result->category == Method;
}


Object *searchFunction(Object * scope, char *name, int bUseFullName)
{

    //printf("\tSearch: Function\n");
    Object *result = 0;

    ListObject *iter = scope->definedSymbols;

    while (iter != 0) {
        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {
            result = iter->value;
            break;
        }
        iter = iter->next;
    }

    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->category == Variable) {
            char newFullName[BUFFLEN];
            snprintf(newFullName, BUFFLEN, "((%s *) (" IDENT_SELF_SELF "->obj))->%s /* %s %d %s */",
                     scope->parentClass->returnType, result->fullname, __FILE__, __LINE__, scope->parentClass->name);
            //TODO: memory leak. (allocating space that will never be freed)
            Object *temp =
                CreateObject(result->name, newFullName, result->parentScope, result->category,
                             result->returnType);
            result = temp;
        }

        if (result) {
            //printf("\t  searched %s's superclass(%s) and found %s\n", scope->fullname, scope->parentClass->fullname, result->fullname);
        } else {
            //printf("\t  searched %s's superclass(%s) and found (null)\n", scope->fullname, scope->parentClass->fullname);
        }
    }

    if (!result && scope->parentScope) {

        result = findByNameInScope(scope->parentScope, name, bUseFullName);

        if (result && result->category != Type && !isVerb(result)) {
            if (!getFlag(result,FLAG_GLOBAL)) {
                compilerDebugPrintf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname,
                                    scope->parentScope->fullname, result->fullname);
                result = 0;
            }
        } else {
            compilerDebugPrintf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }

    }
    //printf("\tsearched %s, returning %s\n", scope->fullname, result ? result->fullname : "(null)");
    return result;

}

/* In a Constructor,
   Search the owning class
      for V, T, C
   Search the parent scope
      for T, F
*/
Object *searchConstructor(Object * scope, char *name, int bUseFullName)
{

    //printf("\tSearch: Constructor\n");
    Object *result = 0;

    ListObject *iter = scope->definedSymbols;

    while (iter != 0) {

        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {

            result = iter->value;

            break;

        }

        iter = iter->next;

    }

    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->category != Variable && result->category != Type
            && !isVerb(result)) {
            //printf("\t  searched %s's superclass(%s) and rejected %s\n", scope->fullname, scope->parentClass->fullname, result->fullname);
            result = 0;

        } else if (result && result->category == Variable) {
            char newFullName[BUFFLEN];
            snprintf(newFullName, BUFFLEN, "/* %s %d */ ((%s * )(" IDENT_SELF_SELF "->obj))->%s", __FILE__, __LINE__, scope->returnType, result->fullname);
            //TODO: memory leak. (allocating space that will never be freed)
            Object *temp =
                CreateObject(result->name, newFullName, result->parentScope, result->category,
                             result->returnType);
            result = temp;
            //printf("\t  searched %s's superclass(%s) and found %s\n", scope->fullname, scope->parentClass->fullname, result ? result->fullname : "(null)");
        }

    }

    if (!result && scope->parentScope != 0) {
        result = findByNameInScope(scope->parentScope, name, bUseFullName);
        if (result && result->category != Type && result->category != Function) {
            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;
        } else {
            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }

    }
    //printf("\tsearched %s, returning %s\n", scope->fullname, result ? result->fullname : "(null)");
    return result;
}

/*In a Type
  Search the parent class
    for T, F, V
*/
Object *searchType(Object * scope, char *name, int bUseFullName)
{

    //printf("\tSearch Types\n");
    Object *result = 0;
    ListObject *iter = scope->definedSymbols;
    while (iter) {
        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {
            result = iter->value;
            break;
        }
        iter = iter->next;
    }

    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->category != Type && result->category != Function
            && result->category != Variable) {
            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;
        } else {
            if (result && result->category == Variable) {
                //Prepend "$super." to fullName
                char newFullName[BUFFLEN];
                snprintf(newFullName, BUFFLEN, IDENT_SUPER "_->%s /* %s %d */", result->fullname,__FILE__, __LINE__);
                //TODO: memory leak. (allocating space that will never be freed)
                Object *temp =
                    CreateObject(result->name, newFullName, result->parentClass,
                                 result->category, result->returnType);
                result = temp;
            }
            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }
    }

    if (!result && scope->parentScope) {

        result = findByNameInScope(scope->parentScope, name, bUseFullName);

        if (result && result->category != Type && result->category != Function) {

            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;

        } else {

            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }

    }
    //printf("\tsearched %s, returning %s\n", scope->fullname, result ? result->fullname : "(null)");
    return result;

}

Object *findByNameInScope(Object * scope, char *name, int bUseFullName)
{

    if (scope == 0 || name == 0) {

        warningMsg("Object or name was null in findByNameInScope. (ObjectTree.c)\n");

        return 0;

    }
    //printf("in findByName, searching %s for %s\n", scope->fullname, name);
    Object *result;

    switch (scope->category) {

    case CodeBlock:
        result = searchCodeBlock(scope, name, bUseFullName);
        break;

    case Function:
        //compilerDebugPrintf("Searching %s in function %s\n",name,scope->name);
        result = searchFunction(scope, name, bUseFullName);
        break;

    case Constructor:
        result = searchConstructor(scope, name, bUseFullName);
        break;

    case Type:
        result = searchType(scope, name, bUseFullName);
        break;

    default:
        warningMsg("cannot search within category %d\n", scope->category);
        break;

    }

    //printf("exiting findByName (%s), found %s\n", scope->fullname, result? result->fullname : "nothing.");
    return result;

}

Object *findFunctionMatch(Object * scope, char *name, int paramc, char **params)
{

    if (scope == 0 || name == 0) {

        warningMsg("Object or name was null in findFunctionMatch. (ObjectTree.c)\n");

        return 0;

    }

    int i;

    char *s;

    if (scope->definedSymbols == 0 && scope->parentScope == 0) {

        return 0;

    } else if (scope->definedSymbols == 0) {

        return findFunctionMatch(scope->parentScope, name, paramc, params);

    }

    ListObject *iter = scope->definedSymbols;

    while (iter != 0) {

        if (!strcmp(name, iter->value->name)) {

            ListString *iter_param = iter->value->paramTypes;

            for (i = 0; i < paramc && iter_param != 0; i++) {

                if (strcmp(params[i], iter_param->value)) {

                    break;

                }

                iter_param = iter_param->next;

            }

            if (i == paramc && iter_param == 0) {

                return iter->value;

            }

        }

        iter = iter->next;

    }

    if (scope->parentScope != 0) {

        return findFunctionMatch(scope->parentScope, name, paramc, params);

    }

    return 0;

}

OBJ_TYPE getIdentType(Object * scope, char *identifier)
{

    Object *obj = findByNameInScope(scope, identifier, 0);

    if (obj)

        return obj->category;

    return Undefined;

}

void writeTree(FILE * outc, FILE * outh, Object * tree)
{
    writeTypeDefs(outh, tree);
    writeForwardDeclarations(outh, tree);
    writeTreeHelper(outc, outh, tree, 0);
}

void writeTypeDefs(FILE * outh, Object * tree)
{
    ListObject *iter = tree->definedSymbols;
    while (iter) {
        if (iter->value->category == Type && !getFlag(iter->value, FLAG_EXTERNAL)) {
            fprintf(outh, "typedef struct " COMPILER_SEP "%s %s;\n", iter->value->name,
                    iter->value->name);
        }
        iter = iter->next;
    }
}

void writeTreeHelper(FILE * outc, FILE * outh, Object * tree, int indent)
{

    ListObject *oIter;
    ListString *sIter;

    if (tree == 0) {
        warningMsg("tree was null in writeTree. (ObjectTree.c)\n");
        return;
    }

    if (outc == 0 || outh == 0) {
        warningMsg("output file was null in writeTree. (ObjectTree.c)\n");
        return;
    }

    oIter = tree->definedSymbols;
    sIter = tree->paramTypes;

    //construct and print function header
    if (isVerb(tree) && !getFlag(tree, FLAG_EXTERNAL)) {
        compilerDebugPrintf("Writing function %s\n",tree->fullname);
        writeFunction(outh, tree, indent, false);
    } else if (tree->category == Type && !getFlag(tree, FLAG_EXTERNAL)) {
        writeClass(outc, outh, tree, indent);
    } else if (tree->category == Dummy) {
        //Dummy
    } else {
        writeOther(outc, outh, tree, indent);
    }

}

void writeDeclareVariable (ListObject *oIter, FILE * outFile, Object * tree) {
            Object * rType = findByNameInScope(tree,oIter->value->returnType,false);
            //compilerDebugPrintf("writing variable %s\n",oIter->value->name);
            if (!getFlag(oIter->value,FLAG_NO_CODEGEN)) {
                if (getFlag(rType, FLAG_PRIMITIVE)) {
                    if (oIter->value->genericType) {
                        Object * gType = findByNameInScope(tree,oIter->value->genericType,false);
                        if (getFlag(gType,FLAG_PRIMITIVE)) {
                            fprintf(outFile, "\t_$_%s_type_1(%s)  %s;\n", oIter->value->returnType, oIter->value->genericType,
                                    oIter->value->fullname);
                        } else {
                            fprintf(outFile, "\t_$_%s_type_0(" IDENT_MPTR ")  %s;\n", oIter->value->returnType,
                                    oIter->value->fullname);
                        }
                    } else {
                        fprintf(outFile, "\t%s %s;\n", oIter->value->returnType,
                                oIter->value->fullname);
                    }
                } else {
                    if (!strcmp(oIter->value->returnType, IDENT_MPTR)) {
                        fprintf(outFile, "\t_$_TEMP_OBJ(%s);\n", oIter->value->fullname);
                    } else if (!strcmp(oIter->value->returnType, IDENT_HEAP_MPTR)) {
                        fprintf(outFile, "\t_$_HEAP_VARIABLE(%s);\n", oIter->value->fullname);
                    }
                    else {
                        fprintf(outFile, "\t_$_VARIABLE(%s);\n", oIter->value->fullname);
                    }

                }
            }
}

void writeDeclareClassVariable (ListObject *oIter, FILE * outFile, Object * tree) {
    Object * rType = findByNameInScope(tree,oIter->value->returnType,false);
    if (!getFlag(oIter->value,FLAG_NO_CODEGEN)) {
        if (getFlag(rType, FLAG_PRIMITIVE)) {
            fprintf(outFile, "\t%s %s;\n", oIter->value->returnType,
                    oIter->value->fullname);
        } else {
            if (strcmp(oIter->value->fullname, IDENT_SUPER "_")) {
                fprintf(outFile, "\t" IDENT_MPTR "* %s;\n", oIter->value->fullname);
            } else {
                fprintf(outFile, "\t%s * %s;\n", oIter->value->returnType, oIter->value->fullname);
            }
        }
    }
}

void writeFunction(FILE * outh, Object * tree, int indent, bool sigOnly)
{
    ListObject *oIter;
    ListString *sIter;

    oIter = tree->definedSymbols;
    sIter = tree->paramTypes;

    compilerDebugPrintf("Looking up function %s\n",tree->fullname);
    Object * rType = findByNameInScope(tree,tree->returnType,false);

    compilerDebugPrintf ("Looking up category %s\n",tree->returnType);

    if (rType) {
        compilerDebugPrintf("Line%d %s\n",__LINE__, tree->fullname);
        if (getFlag(rType, FLAG_PRIMITIVE)) {
            fprintf(outh, "%s %s(", tree->returnType, tree->fullname);
        } else {
            fprintf(outh, IDENT_MPTR "* %s(", tree->fullname);
        }
    } else {
        fprintf(outh, "void %s(", tree->fullname);
    }

    //add each param
    Object *pType;
    char printComma = ' ';
    while (sIter != 0) {
        printComma = ',';
        pType = findByNameInScope(tree,sIter->value,false);
        //compilerDebugPrintf("Looking up symbol %d\n",oIter);
        if (pType) {
            if (getFlag(pType, FLAG_PRIMITIVE)) {
                fprintf(outh, "%s %s", sIter->value, oIter->value->fullname);
            } else {
                fprintf(outh, IDENT_MPTR " * %s", oIter->value->fullname);
            }
        }

        sIter = sIter->next;
        oIter = oIter->next;
        if (sIter !=0 ) {
            fprintf(outh, ",");
        }
    }


    if (rType&&!getFlag(rType,FLAG_PRIMITIVE)) {
        fprintf(outh, "%c " IDENT_MPTR " * " IDENT_MPTR "_in",printComma);
    }

    //finish
    if (sigOnly) {
        fprintf(outh, ");\n");
        return;
    } else {
        fprintf(outh, ") {\n");
    }

    while (oIter != 0) {
        if (oIter->value->category == Variable) {
            //declare all class variables
            writeDeclareVariable (oIter, outh, tree);
        }
        oIter = oIter->next;
    }

    //print each line of code.
    if (tree->code != 0 && tree->code->value != 0) {
        sIter = tree->code;
        while (sIter != 0) {
            fprintf(outh, " \t%s\n", sIter->value);
            sIter = sIter->next;
        }
    }
    fprintf(outh, "}\n");
}

void writeOther(FILE * outc, FILE * outh, Object * tree, int indent)
{

    ListObject *oIter;
    ListString *sIter;

    oIter = tree->definedSymbols;

    while (oIter != 0) {
        if (oIter->value->category == Variable) {
            //declare all local variables
            writeDeclareVariable (oIter, outc, tree);
        } else {
            writeTreeHelper(outc, outh, oIter->value, indent + 1);
        }

        oIter = oIter->next;
    }

    if (tree->code != 0 && tree->code->value != 0) {
        sIter = tree->code;
        while (sIter != 0) {
            fprintf(outc, "\t%s\n", sIter->value);
            sIter = sIter->next;
        }
    }

}

void writeClass(FILE * outc, FILE * outh, Object * tree, int indent)
{

    ListObject *oIter;

    oIter = tree->definedSymbols;
    //fprintf(outh, "%s %s " COMPILER_SEP "%s %s;\n", "typedef", "struct", tree->name,
    //        tree->name);
    fprintf(outh, "%s " COMPILER_SEP "%s {\n", "struct", tree->name);

    while (oIter != 0) {
        if (oIter->value->category == Variable) {
            writeDeclareClassVariable (oIter, outh, tree);
        } else {
            oIter = oIter->next;
            break;
        }
        oIter = oIter->next;
    }

    fprintf(outh, "};\n");



    while (oIter != 0) {
        if (isVerb(oIter->value)) {
            writeFunction(outh, tree, indent, false);
        } else {
            writeTreeHelper(outc, outh, tree, indent);
        }
    }

}

void writeForwardDeclarations (FILE * outh, Object * tree)
{
    ListObject *fiter;
    int indent = 0;
    fiter = tree->definedSymbols;
    //Do the forward declarations
    while (fiter) {
        if (isVerb(fiter->value)) {
            if (!getFlag(fiter->value, FLAG_EXTERNAL)) {
                compilerDebugPrintf("Forward decling %s\n", fiter->value->fullname);
                writeFunction(outh, fiter->value, indent, true);
            }
        }
        fiter = fiter->next;
    }
}

//================  Testing / Printing =================

void printSequential(ListString * list, int indent, int newlines)
{

    int i;

    if (!newlines) {
        if (list == 0) {
            printf("\n");
            return;
        }

        printf("%s, ", list->value);

        while (list->next != 0) {
            list = list->next;
            printf("%s, ", list->value);
        }

        printf("\n");
    } else {

        if (list == 0) {
            return;
        }

        for (i = 0; i < indent; i++) {
            printf("  ");
        }
        printf("\"%s\"\n", list->value);

        while (list->next != 0) {
            list = list->next;
            for (i = 0; i < indent; i++) {
                printf("  ");
            }
            printf("\"%s\"\n", list->value);
        }

    }

}

void printType(OBJ_TYPE type)
{

    switch (type) {

    case Undefined:
        printf("Undefined");
        break;

    case Variable:
        printf("Variable");
        break;

    case Type:
        printf("Type");
        break;

    case Constructor:
        printf("Constructor");
        break;

    case Function:
        printf("Function");
        break;

    case CodeBlock:
        printf("CodeBlock");
        break;

    case Expression:
        printf("Expression");
        break;

    default:
        printf("(missing from printType in ObjectTree.c)");
        break;

    }

}

void printTreeList(ListObject * trees, int indent)
{

    if (trees == 0) {

        return;

    }

    printTree(trees->value, indent);

    while (trees->next != 0) {

        trees = trees->next;

        printTree(trees->value, indent);

    }

}

void printTree(Object * tree, int indent)
{

    if (getFlag(tree, FLAG_EXTERNAL)) {
        return;
    }

    int i;
    if (tree == 0) {
        for (i = 0; i < indent; i++) {
            printf("  ");
        }
        printf("Object: (null)\n");
        return;
    }

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("Object: name: \"%s\"\n", tree->name);

    indent += 1;
    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("fullname: \"%s\"\n", tree->fullname);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("parentScope: %s\n",
           tree->parentScope ? tree->parentScope->fullname : "(null)");

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("parentClass: %s\n",
           tree->parentClass ? tree->parentClass->fullname : "(null)");

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("category: ");
    printType(tree->category);
    printf("\n");

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("returnType: \"%s\"\n", tree->returnType);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("paramTypes: ");

    printSequential(tree->paramTypes, indent + 1, 0);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("definedSymbols: \n");

    printTreeList(tree->definedSymbols, indent + 1);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("code: \n");

    printSequential(tree->code, indent + 1, 1);

}

//used to debug to file ( prints tree to file with fname )
void printTreeToFile(Object * tree, int indent, char *fname)
{

    int i;

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("Object: name: \"%s\"\n", tree->name);

    indent += 1;

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("fullname: \"%s\"\n", tree->fullname);

    if (tree->parentScope == 0) {

        for (i = 0; i < indent; i++) {
            printf("  ");
        }
        printf("parentObject: (null)\n");

    } else {

        for (i = 0; i < indent; i++) {
            printf("  ");
        }
        printf("parentObject: %s\n", tree->parentScope->fullname);

    }

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("category: ");
    printType(tree->category);
    printf("\n");

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("returnType: \"%s\"\n", tree->returnType);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("paramTypes: ");

    printSequential(tree->paramTypes, indent + 1, 0);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("definedSymbols: \n");

    printTreeList(tree->definedSymbols, indent + 1);

    for (i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("code: \n");

    printSequential(tree->code, indent + 1, 1);

}

//===============  Test / sample  ====================

int testmain()
{

    Object *root = CreateObject("Undefined", "Undefined", 0, CodeBlock, "int");
    Object *basetype = CreateObject("BaseType", "BaseType", 0, Type, 0);
    Object *rect = CreateObject("Rectangle", "BaseType_Rectangle", basetype, Type, 0);
    Object *rectConst =
        CreateObject("Rectangle", "Rectangle_Rectangle_Rectangle_int_int", 0,
                     Constructor, "Rectangle");
    Object *subexpr = CreateObject(0, 0, 0, Expression, "float");
    addCode(subexpr, "3.14159");
    addParam(rectConst, "int");
    addParam(rectConst, "int");
    addSymbol(rectConst, CreateObject("width", "width", 0, Variable, "int"));
    addSymbol(rectConst, CreateObject("height", "height", 0, Variable, "int"));
    addSymbol(rectConst, CreateObject("self", "self", 0, Variable, "Rectangle*"));
    addCode(rectConst,
            "Rectangle * Rectangle_Rectangle_Rectangle_int_int(int width, int height) {");
    addCode(rectConst, "    Rectangle * self = (Rectangle*)malloc(sizeof(Rectangle));");
    addCode(rectConst, "    self->w = width;");
    addCode(rectConst, "    self->h = height;");
    addCode(rectConst, "    return self;");

    addCode(rectConst, "}");

    addSymbol(rect, CreateObject("w", "w", 0, Variable, "int"));

    addSymbol(rect, CreateObject("h", "h", 0, Variable, "int"));

    addSymbol(rect, rectConst);

    addCode(rect, "typedef struct {");

    addCode(rect, "    BaseType parent;");

    addCode(rect, "    int w;");

    addCode(rect, "    int h;");

    addCode(rect, "} Rectangle;");

    addSymbol(root, basetype);

    addSymbol(root, rect);

    addSymbol(root, subexpr);

    printTree(root, 0);

    return 0;

}
