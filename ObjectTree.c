#include "ObjectTree.h"

#include "stdlib.h"
#include "stdio.h"

//mallocs memory and returns a pointer to a new Object
Object * CreateObject(char* name, char* fullname, Object* parent, OBJ_TYPE type, char* returnType) {
  Object* result = (Object*)malloc(sizeof(Object));
  result->name           = name;
  result->fullname       = fullname;
  result->parentScope    = parent;
  result->type           = type;
  result->returnType     = returnType;
  result->paramTypes     = 0;
  result->definedSymbols = 0;
  result->code           = 0;
  return result;
}

//append item to end of linked list
void addParam(Object* tree, char* type) {
  ListString* node = malloc(sizeof(ListString));
  node->value = type;
  if (tree->paramTypes == 0) {
    tree->paramTypes = node;
    return;
  }
  ListString* tail = tree->paramTypes;
  while(tail->next != 0) {
    tail = tail->next;
  }
  tail->next = node;
}

void addSymbol(Object* tree, Object* leaf) {
  ListObject* node = malloc(sizeof(ListObject));
  node->value = leaf;
  if(tree->definedSymbols == 0) {
    tree->definedSymbols = node;
    return;
  }
  ListObject* tail = tree->definedSymbols;
  while(tail->next != 0) { tail = tail->next; }
  tail->next = node;
}

void addCode(Object* tree, char* line) {
  ListString* node = malloc(sizeof(ListString));
  node->value = line;
  if(tree->code == 0) {
    tree->code = node;
    return;
  }
  ListString* tail = tree->code;
  while(tail->next != 0) { tail = tail->next; }
  tail->next = node;
}

Object* findByNameInScope(Object* scope, char* name) {
  if (scope->definedSymbols == 0 && scope->parentScope == 0) {
    return 0;
  } else if (scope->definedSymbols == 0) {
    return findByNameInScope(scope->parentScope, name);
  }

  if (!strcmp(name, scope->definedSymbols->value->name)) {
    return scope->definedSymbols->value;
  }
  ListObject* iter = scope->definedSymbols;
  while (iter->next != 0) {
    iter = iter->next;
    if (!strcmp(name, iter->value->name)) {
      return iter->value;
    }
  }
  if (scope->parentScope != 0) {
    return findByNameInScope(scope->parentScope, name);
  }
  return 0;
}

Object* findFunctionMatch(Object* scope, char* name, int paramc, char** params) {
  int i;
  char* s;
  printf("searching for %s\n", name);
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
  Object* obj = findByNameInScope(scope, identifier);
  if (obj)
    return obj->type;
  return Undefined;
}

void writeTree(FILE* outc, FILE* outh, Object* tree) {
    FILE* output;
    if (tree->type == Function)
        output = outh;
    else
        output = outc;

    ListObject* iter = tree->definedSymbols;
    while (iter != 0) {
      writeTree(outc, outh, iter->value);
      iter = iter->next;
    }

    //print each line of code.
    if (tree->code != 0 && tree->code->value != 0) {
        ListString* iter = tree->code;
        fprintf(output, "%s\n", iter->value);
        while (iter->next != 0) {
            iter = iter->next;
            fprintf(output, "%s\n", iter->value);
        }
    }
    //TODO: recurse over each definedSymbol.
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
  int i;
  for(i = 0; i < indent; i++) { printf("  "); }   printf("Object: name: \"%s\"\n", tree->name);
  indent += 1;
  for(i = 0; i < indent; i++) { printf("  "); }   printf("fullname: \"%s\"\n", tree->fullname);
  if(tree->parentScope == 0) {
    for(i = 0; i < indent; i++) { printf("  "); } printf("parentObject: NULL\n");
  } else {
    for(i = 0; i < indent; i++) { printf("  "); } printf("parentObject: %s\n", tree->parentScope->fullname);
  }
  for(i = 0; i < indent; i++) { printf("  "); }   printf("type: "); printType(tree->type); printf("\n");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("returnType: \"%s\"\n", tree->returnType != 0 ? tree->returnType : "NULL");
  for(i = 0; i < indent; i++) { printf("  "); }   printf("paramTypes: ");
  printSequential(tree->paramTypes, indent+1, 0);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("definedSymbols: \n");
  printTreeList(tree->definedSymbols, indent+1);
  for(i = 0; i < indent; i++) { printf("  "); }   printf("code: \n");
  printSequential(tree->code, indent+1, 1);
}

//writes the code of root first, then children in order
//void writeTree(FILE* outc, FILE* outh, Object* tree);

//searches for identifier in current, and parent scope.
//returns Undefined if identifier isn't found.


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










