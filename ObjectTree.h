#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "errors.h"

#define xstr(a) str(a)
#define str(a) #a
#define cat(a,b) a ## b

#define debugPrintf //printf
#define compilerDebugPrintf //printf

#define FLAG_ASSIGNMENT 1
#define FLAG_SUBJECT    2
#define FLAG_EXTERNAL   4
#define FLAG_SAVERESULT     8
#define FLAG_RETURNS     16
#define FLAG_PRIMITIVE     32
#define BUFFLEN 1024
#define COMPILER_SEP "_$_"
#define IDENT_SELF "$"
#define IDENT_SUPER "$super"
#define IDENT_RETVAR_RAW _$_retvar
#define IDENT_HEAP_RETVAR_RAW _$_heap_retvar
#define IDENT_CTR_PTR _$_counter_pointer
#define RETVAR_POINT_RAW cat(IDENT_RETVAR_RAW,_point)
#define RETVAR_ASSIGN_RAW cat(IDENT_RETVAR_RAW,_assign)
#define IDENT_RETVAR_INITIALIZE_RAW cat(IDENT_RETVAR_RAW,_initialize)


#define IDENT_RETVAR xstr(IDENT_RETVAR_RAW)
#define IDENT_HEAP_RETVAR xstr(IDENT_HEAP_RETVAR_RAW)
#define IDENT_RETVAR_INITIALIZE xstr(IDENT_RETVAR_INITIALIZE_RAW)
#define RETVAR_POINT xstr(RETVAR_POINT_RAW)
#define RETVAR_ASSIGN xstr(RETVAR_ASSIGN_RAW)

#define _$_TEMP_OBJ(x) _$_retvar __attribute__ ((__cleanup__(_$_cleanup))) x; IDENT_RETVAR_INITIALIZE_RAW (&x,xstr(x));
#define _$_VARIABLE(x) _$_retvar __attribute__ ((__cleanup__(_$_cleanup_var))) * x = alloca(sizeof(IDENT_RETVAR_RAW)); IDENT_RETVAR_INITIALIZE_RAW(x,xstr(x));
#define _$_HEAP_VARIABLE(x) _$_retvar * x = malloc(sizeof(IDENT_RETVAR_RAW)); IDENT_RETVAR_INITIALIZE_RAW(x,xstr(x));

//typedef enum {false, true} bool;

typedef enum {
    Undefined,
    Variable,
    Type,
    Constructor,
    Function,
    CodeBlock,
    Expression,
    Dummy,
    NewMarkedIdent,
    NewUnmarkedIdent,
    Destructor,
} OBJ_TYPE;

typedef struct _Object Object;
typedef struct _ListObject ListObject;
typedef struct _ListString ListString;

struct _ListString {
    char *value;
    ListString *next;
};
struct _ListObject {
    Object *value;
    ListObject *next;
};

struct _Object {
    char *name;                 //symbol name     ("myInteger", "calcTotalArea ", "Rectangle")
    char *fullname;             //symbol fullname ("myInteger", "Integer_calcTotalArea_Rectangle_Rectangle", "BaseType_Rectangle")
    Object *parentClass;
    Object *parentScope;        //parent scope    (global scope, global scope, BaseType)
    OBJ_TYPE type;              //What is this?   (Variable, Function, Class)
    char *returnType;           //What value type?(Integer,  Integer,  NULL)
    char *genericType;          //What value type if the returnType is Generic?(Integer,  Integer,  NULL)
    int genericTypeArgPos;      //What value type if the returnType is Generic?(Integer,  Integer,  NULL)
    ListString *paramTypes;     //parameters?     (NULL,     [Integer, Integer], NULL)
    ListObject *definedSymbols; //Things inside?  (NULL, [Rectangle "r1", Rectangle "r2", Integer "a1", Integer "a2"], [Integer "w", Integer "h", Constructor "Rectangle", Function "Area"])
    ListString *code;           //CodeBlock       (NULL, "Integer ...calcTotalArea...(...) {...", "typedef struct...")
    int flags;
};

//mallocs memory and returns a pointer to a new Object
Object *CreateObject(char *name, char *fullname, Object * parentScope, OBJ_TYPE type,
                     char *returnType);

//append item to end of linked list
int addParam(Object * tree, char *type);
int addGenericType(Object * tree, char *genericType, int genericTypeArgPos);
int addSymbol(Object * tree, Object * leaf);
ListString *addCode(Object * tree, char *line);
int setFlags(Object * tree, int flags);
int getFlag(Object * tree, int flag);
int setParentClass(Object * tree, Object * parentClass);
int listlen(ListString * head);

//writes the code of root first, then children in order
void writeTree(FILE * outc, FILE * outh, Object * tree);
void writeTreeHelper(FILE * outc, FILE * outh, Object * tree, int indent);
void writeTypeDefs(FILE * outh, Object * tree);
void writeFunction(FILE * outh, Object * tree, int indent, bool sigOnly);
void writeOther(FILE * outc, FILE * outh, Object * tree, int indent);
void writeClass(FILE * outc, FILE * outh, Object * tree, int indent);
void writeForwardDeclarations (FILE * outh, Object * tree);

void printTree(Object * tree, int indent);
void printTreeToFile(Object * tree, int indent, char *fname);

//searches for identifier in current, and parent scope.
//returns Undefined if identifier isn't found.
OBJ_TYPE getIdentType(Object * scope, char *identifier);
//return null if name not found
Object *searchFunction(Object * scope, char *name, int bUseFullName);
Object *searchConstructor(Object * scope, char *name, int bUseFullName);
Object *searchType(Object * scope, char *name, int bUseFullName);
Object *searchCodeBlock(Object * scope, char *name, int bUseFullName);
Object *findByNameInScope(Object * scope, char *name, int bUseFullName);

Object *findFunctionMatch(Object * scope, char *name, int paramc, char **params);

#endif
