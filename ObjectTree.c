#include "ObjectTree.h"

#include "stdlib.h"
#include "stdio.h"

//mallocs memory and returns a pointer to a new Object
Object * CreateObject(char* name, char* fullname, Object* parentScope, OBJ_TYPE type, char* returnType) {
  Object* result = (Object*)malloc(sizeof(Object));
  if (result == 0) {
    warningMsg("%s", "CreateObject couldn't allocate a new object. (ObjectTree.c)\n");
    return 0; //malloc failed.
  }
  result->name           = name ? strdup(name) : 0;
  result->fullname       = fullname? strdup(fullname) : 0;
  result->parentClass    = 0;
  result->parentScope    = parentScope;
  result->type           = type;
  result->returnType     = returnType ? strdup(returnType) : 0;
  result->paramTypes     = 0;
  result->definedSymbols = 0;
  result->code           = 0;
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
int addParam(Object* tree, char* type) {
  ListString* node = malloc(sizeof(ListString));
  if (node == 0) {
    warningMsg("Allocation failed in addParam. (ObjectTree.c)\n");
    return 1;
  }
  node->value = strdup(type);
  if (node->value == 0) {
    warningMsg("strdup failed in addParam. (ObjectTree.c)\n");
    return 2;
  }
  if (tree->paramTypes == 0) {
    tree->paramTypes = node;
    return 0;
  }
  ListString* tail = tree->paramTypes;
  while(tail->next != 0) {
    tail = tail->next;
  }
  tail->next = node;
  return 0;
}

int addSymbol(Object* tree, Object* leaf) {
  ListObject* node = malloc(sizeof(ListObject));
  if (node == 0) {
    warningMsg("Allocation failed in addSymbol. (ObjectTree.c)\n");
    return 1;
  }
  node->value = leaf;
  if(tree->definedSymbols == 0) {
    tree->definedSymbols = node;
    return 0;
  }
  ListObject* tail = tree->definedSymbols;
  while(tail->next != 0) { tail = tail->next; }
  tail->next = node;
  return 0;
}

int addCode(Object* tree, char* line) {
  ListString* node = malloc(sizeof(ListString));
  if (node == 0) {
    warningMsg("Allocation failed in addCode. (ObjectTree.c)\n");
    return 1;
  }
  node->value = strdup(line);
  if (node->value == 0) {
    warningMsg("strdup failed in addCode. (ObjectTree.c)\n");
    return 2;
  }
  if(tree->code == 0) {
    tree->code = node;
    return 0;
  }
  ListString* tail = tree->code;
  while(tail->next != 0) { tail = tail->next; }
  tail->next = node;
  return 0;

}

int listlen(ListString* head) {
    int count = 0;
    while (head != 0) {
        count++;
        head = head->next;
    }
    return count;

}

int setFlags(Object* tree, int flags) {
    tree->flags |= flags;
}

int getFlag(Object* tree, int flag) {
    return ((tree->flags & flag) == flag);
}

int setParentClass(Object* tree, Object* parentClass) {
    tree->parentClass = parentClass;
}

/* In a CodeBlock,
   Search the parent scope
   for V, T, C, F, CB
*/
Object* searchCodeBlock(Object* scope, char* name, int bUseFullName) {
    //printf("\tSearch: CodeBlock\n");
    Object* result = 0;

    //search locally first
    ListObject* iter = scope->definedSymbols;
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
Object* searchFunction(Object* scope, char* name, int bUseFullName) {
    //printf("\tSearch: Function\n");
    Object* result = 0;

    ListObject* iter = scope->definedSymbols;
    while (iter != 0) {
        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {
            result = iter->value;
            break;
        }
        iter = iter->next;
    }

    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->type == Variable) {
            //Prepend "self->" to fullName
            char newFullName[BUFFLEN];
            snprintf(newFullName, BUFFLEN, "self->%s", result->fullname);
            //TODO: memory leak. (allocating space that will never be freed)
            Object* temp = CreateObject(result->name, newFullName, result->parentScope, result->type, result->returnType);
            result = temp;
        }
        if (result) {
            //printf("\t  searched %s's superclass(%s) and found %s\n", scope->fullname, scope->parentClass->fullname, result->fullname);
        } else {
            //printf("\t  searched %s's superclass(%s) and found (null)\n", scope->fullname, scope->parentClass->fullname);
        }
    }

    if (!result && scope->parentScope != 0) {
        result = findByNameInScope(scope->parentScope, name, bUseFullName);
        if (result && result->type != Type && result->type != Function) {
            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;
        } else {
            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
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
Object* searchConstructor(Object* scope, char* name, int bUseFullName) {
    //printf("\tSearch: Constructor\n");
    Object* result = 0;
    ListObject* iter = scope->definedSymbols;
    while (iter != 0) {
        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {
            result = iter->value;
            break;
        }
        iter = iter->next;
    }

    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->type != Variable && result->type != Type && result->type != Constructor) {
            //printf("\t  searched %s's superclass(%s) and rejected %s\n", scope->fullname, scope->parentClass->fullname, result->fullname);
            result = 0;
        } else if (result && result->type == Variable) {
            char newFullName[BUFFLEN];
            snprintf(newFullName, BUFFLEN, "self->%s", result->fullname);
            //TODO: memory leak. (allocating space that will never be freed)
            Object* temp = CreateObject(result->name, newFullName, result->parentScope, result->type, result->returnType);
            result = temp;
            //printf("\t  searched %s's superclass(%s) and found %s\n", scope->fullname, scope->parentClass->fullname, result ? result->fullname : "(null)");
        }
    }

    if (!result && scope->parentScope != 0) {
        result = findByNameInScope(scope->parentScope, name, bUseFullName);
        if (result && result->type != Type && result->type != Function) {
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
Object* searchType(Object* scope, char* name, int bUseFullName) {
    //printf("\tSearch Types\n");
    Object* result = 0;
    ListObject* iter = scope->definedSymbols;
    while (iter) {
        if (!strcmp(name, bUseFullName ? iter->value->fullname : iter->value->name)) {
            result = iter->value;
            break;
        }
        iter = iter->next;
    }
    if (!result && scope->parentClass) {
        result = findByNameInScope(scope->parentClass, name, bUseFullName);
        if (result && result->type != Type && result->type != Function && result->type != Variable) {
            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;
        } else {
            if (result && result->type == Variable) {
                //Prepend "parent." to fullName
                char newFullName[BUFFLEN];
                snprintf(newFullName, BUFFLEN, "parent.%s", result->fullname);
                //TODO: memory leak. (allocating space that will never be freed)
                Object* temp = CreateObject(result->name, newFullName, result->parentClass, result->type, result->returnType);
                result = temp;
            }
            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }
    }
    if (!result && scope->parentScope) {
        result = findByNameInScope(scope->parentScope, name, bUseFullName);
        if (result && result->type != Type && result->type != Function) {
            //printf("\t  searched %s's parent(%s) and rejected %s\n", scope->fullname, scope->parentScope->fullname, result->fullname);
            result = 0;
        } else {
            //printf("\t  searched %s's parent(%s) and found %s\n", scope->fullname, scope->parentScope->fullname, result ? result->fullname : "(null)");
        }
    }
    //printf("\tsearched %s, returning %s\n", scope->fullname, result ? result->fullname : "(null)");
    return result;
}


Object* findByNameInScope(Object* scope, char* name, int bUseFullName) {
    if (scope == 0 || name == 0) {
        warningMsg("Object or name was null in findByNameInScope. (ObjectTree.c)\n");
        return 0;
    }
    //printf("in findByName, searching %s for %s\n", scope->fullname, name);
    Object* result;
    switch(scope->type) {
    case CodeBlock:
        result = searchCodeBlock(scope, name, bUseFullName);
        break;
    case Function:
        result = searchFunction(scope, name, bUseFullName);
        break;
    case Constructor:
        result = searchConstructor(scope, name, bUseFullName);
        break;
    case Type:
        result = searchType(scope, name, bUseFullName);
        break;
    default:
        warningMsg("cannot search within type %d\n", scope->type);
        break;
    }
    //printf("exiting findByName (%s), found %s\n", scope->fullname, result? result->fullname : "nothing.");
    return result;
}

Object* findFunctionMatch(Object* scope, char* name, int paramc, char** params) {
  if (scope == 0 || name == 0) {
    warningMsg("Object or name was null in findFunctionMatch. (ObjectTree.c)\n");
    return 0;
  }
  int i;
  char* s;
  if (scope->definedSymbols == 0 && scope->parentScope == 0) {
    return 0;
  } else if (scope->definedSymbols == 0) {
    return findFunctionMatch(scope->parentScope, name, paramc, params);
  }

  ListObject* iter = scope->definedSymbols;
  while (iter != 0) {
    if (!strcmp(name, iter->value->name)) {
      ListString* iter_param = iter->value->paramTypes;
      for(i = 0; i < paramc && iter_param != 0; i++) {
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

OBJ_TYPE getIdentType(Object* scope, char* identifier) {
  Object* obj = findByNameInScope(scope, identifier, 0);
  if (obj)
    return obj->type;
  return Undefined;
}

void writeTree(FILE* outc, FILE* outh, Object* tree) {
    writeTreeHelper(outc, outh, tree, 0);
}

void writeTreeHelper(FILE* outc, FILE* outh, Object* tree, int indent) {
    FILE* output;
    ListObject* oIter;
    ListString* sIter;

    if (tree == 0) {
      warningMsg("tree was null in writeTree. (ObjectTree.c)\n");
      return;
    }
    if (outc == 0 || outh == 0) {
      warningMsg("output file was null in writeTree. (ObjectTree.c)\n");
      return;

    }

    if (tree->type == Function || tree->type == Constructor) {
        output = outh;
    } else {
        output = outc;
    }

    oIter = tree->definedSymbols;
    sIter = tree->paramTypes;
    //construct and print function header
    if ((tree->type == Function || tree->type == Constructor) && !getFlag(tree, FLAG_EXTERNAL)) {
        fprintf(output, "%s %s(", tree->returnType, tree->fullname);

        //add each param
        while (sIter != 0) {
            if (sIter->next == 0) {
                fprintf(output, "%s %s", sIter->value, oIter->value->fullname);
            } else {
                fprintf(output, "%s %s, ", sIter->value, oIter->value->fullname);
            }
            sIter = sIter->next;
            oIter = oIter->next;
        }
        //finish
        fprintf(output, ") {\n");
    }

    //oIter may not be the head of the list of symbols, on purpose
    //   Due to the function header above declaring parameters.
    while (oIter != 0) {
        if (oIter->value->type == Variable) {
            //declare all local variables
            fprintf(output, "    %s %s;\n", oIter->value->returnType, oIter->value->fullname);
        } else {
            writeTreeHelper(outc, outh, oIter->value, indent+1);
        }
        oIter = oIter->next;
    }


    //print each line of code.
    if (tree->code != 0 && tree->code->value != 0) {

        sIter = tree->code;
        while (sIter != 0) {
            fprintf(output, "    %s\n", sIter->value);
            sIter = sIter->next;
        }
    }
    if (tree->type == Constructor && !getFlag(tree, FLAG_EXTERNAL)) {
        fprintf(output, "    return self;\n"
            "}\n");
    }

    if (tree->type == Function && !getFlag(tree, FLAG_EXTERNAL)) {
        fprintf(output, "}\n");
    }
}


//================  Testing / Printing =================

void printSequential(ListString* list, int indent, int newlines) {
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
    for(i = 0; i < indent; i++) { printf("  "); } printf("\"%s\"\n", list->value);
    while (list->next != 0) {
      list = list->next;
      for(i = 0; i < indent; i++) { printf("  "); } printf("\"%s\"\n", list->value);
    }
  }
}

void printType(OBJ_TYPE type) {
  switch(type) {
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

void printTreeList(ListObject* trees, int indent) {
  if (trees == 0) {
    return;
  }
  printTree(trees->value, indent);
    while (trees->next != 0) {
      trees = trees->next;
      printTree(trees->value, indent);
    }
}

void printTree(Object* tree, int indent) {
  //if (getFlag(tree, FLAG_EXTERNAL)) {
  //  return;
  //}
  int i;
  if (tree == 0) {
    for(i = 0; i < indent; i++) { printf("  "); }   printf("Object: (null)\n");
    return;
  }
  for(i = 0; i < indent; i++) { printf("  "); }   printf("Object: name: \"%s\"\n", tree->name);
  indent += 1;
  for(i = 0; i < indent; i++) { printf("  "); }   printf("fullname: \"%s\"\n", tree->fullname);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("parentScope: %s\n", tree->parentScope ? tree->parentScope->fullname : "(null)");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("parentClass: %s\n", tree->parentClass ? tree->parentClass->fullname : "(null)");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("type: "); printType(tree->type); printf("\n");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("returnType: \"%s\"\n", tree->returnType);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("paramTypes: ");
  printSequential(tree->paramTypes, indent+1, 0);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("definedSymbols: \n");
  printTreeList(tree->definedSymbols, indent+1);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("code: \n");
  printSequential(tree->code, indent+1, 1);
}

//used to debug to file ( prints tree to file with fname )
void printTreeToFile(Object* tree, int indent, char* fname) {
  int i;
  for(i = 0; i < indent; i++) { printf("  "); }   printf("Object: name: \"%s\"\n", tree->name);
  indent += 1;
  for(i = 0; i < indent; i++) { printf("  "); }   printf("fullname: \"%s\"\n", tree->fullname);
  if(tree->parentScope == 0) {
    for(i = 0; i < indent; i++) { printf("  "); } printf("parentObject: (null)\n");
  } else {
    for(i = 0; i < indent; i++) { printf("  "); } printf("parentObject: %s\n", tree->parentScope->fullname);
  }
  for(i = 0; i < indent; i++) { printf("  "); }   printf("type: "); printType(tree->type); printf("\n");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("returnType: \"%s\"\n", tree->returnType);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("paramTypes: ");
  printSequential(tree->paramTypes, indent+1, 0);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("definedSymbols: \n");
  printTreeList(tree->definedSymbols, indent+1);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("code: \n");
  printSequential(tree->code, indent+1, 1);
}

//===============  Test / sample  ====================

int testmain() {
  Object* root = CreateObject("Undefined", "Undefined", 0, CodeBlock, "Integer");
  Object* basetype = CreateObject("BaseType", "BaseType", 0, Type, 0);
  Object* rect = CreateObject("Rectangle", "BaseType_Rectangle", basetype, Type, 0);
  Object* rectConst = CreateObject("Rectangle", "Rectangle_Rectangle_Rectangle_Integer_Integer", 0, Constructor, "Rectangle");
  Object* subexpr = CreateObject(0, 0, 0, Expression, "Float");
  addCode(subexpr, "3.14159");

  addParam(rectConst, "Integer");
  addParam(rectConst, "Integer");
  addSymbol(rectConst, CreateObject("width", "width", 0, Variable, "Integer"));
  addSymbol(rectConst, CreateObject("height", "height", 0, Variable, "Integer"));
  addSymbol(rectConst, CreateObject("self", "self", 0, Variable, "Rectangle*"));
  addCode(rectConst, "Rectangle * Rectangle_Rectangle_Rectangle_Integer_Integer(Integer width, Integer height) {");
  addCode(rectConst, "    Rectangle * self = (Rectangle*)malloc(sizeof(Rectangle));");
  addCode(rectConst, "    self->w = width;");
  addCode(rectConst, "    self->h = height;");
  addCode(rectConst, "    return self;");
  addCode(rectConst, "}");

  addSymbol(rect, CreateObject("w", "w", 0, Variable, "Integer"));
  addSymbol(rect, CreateObject("h", "h", 0, Variable, "Integer"));
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










