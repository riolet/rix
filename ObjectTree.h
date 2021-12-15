#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "errors.h"
#include <setjmp.h>
#include <signal.h>

#define xstr(a) str(a)
#define str(a) #a

#define xcat(a,b) cat(a,b)
#define cat(a,b) a ## b

#define debugPrintf(...) //printf("\n %d ",__LINE__);printf(__VA_ARGS__)
FILE *outCompilerLogFile;
#define compilerDebugPrintf(...) fprintf(outCompilerLogFile,__VA_ARGS__)

#define FLAG_ASSIGNMENT 1
#define FLAG_SUBJECT    2
#define FLAG_EXTERNAL   4
#define FLAG_SAVERESULT     8
#define FLAG_RETURNS     16
#define FLAG_PRIMITIVE     32
#define FLAG_GLOBAL     64
#define FLAG_NO_CODEGEN     128
#define FLAG_IDENT_SELF     256
#define FLAG_FOUND_PARENT     512
#define FLAG_ENUM     1024

#define BUFFLEN 1024
#define COMPILER_SEP "_$_"
#define IDENT_SELF "$"
#define IDENT_SELF_SELF "$self"
#define IDENT_SUPER "$super"


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
    Method,
    Field,
    Enum,
    EnumEntry,
    EnumTypeVariable
} OBJ_TYPE;

typedef struct _Object Object;
typedef struct _ListObject ListObject;
typedef struct _ListString ListString;
typedef struct _ListType ListType;

struct _ListString {
    char *value;
    ListString *next;
};

struct _ListType {
    char *type;
    ListType *genericType;
    ListType *next;
};

struct _ListObject {
    Object *value;
    ListObject *next;
};

struct _Object {
    char *name;                 //symbol name     ("myint", "calcTotalArea ", "Rectangle")
    char *fullname;             //symbol fullname ("myint", "int_calcTotalArea_Rectangle_Rectangle", "BaseType_Rectangle")
    Object *parentClass;
    Object *parentScope;        //parent scope    (global scope, global scope, BaseType)
    OBJ_TYPE category;              //What is this?   (Variable, Function, Class)
    char *returnType;           //What value category?(int,  int,  NULL)
    ListType *genericType;          //What value category if the returnType is Generic?(int,  int,  NULL)
    int genericTypeArgPos;      //What position of the returnType is Generic?(int,  int,  NULL)
    char *resolvedSpecificType;       //Generic type resolved as specific
    ListType *paramTypes;     //parameters?     (NULL,     [int, int], NULL)
    ListObject *definedSymbols; //Things inside?  (NULL, [Rectangle "r1", Rectangle "r2", int "a1", int "a2"], [int "w", int "h", Constructor "Rectangle", Function "Area"])
    ListString *code;           //CodeBlock       (NULL, "int ...calcTotalArea...(...) {...", "typedef struct...")
    int flags;
};

//mallocs memory and returns a pointer to a new Object
Object *CreateObject(char *name, char *fullname, Object * parentScope, OBJ_TYPE type,
                     char *returnType);

//append item to end of linked list
int addParam(Object * tree, char *type);
int addParamWithGenericType(Object * tree, char *type, ListType *genericType);
int addGenericType(Object * tree, ListType *genericType, int genericTypeArgPos);
int addSymbol(Object * tree, Object * leaf);
ListString *addCode(Object * tree, char *line);
ListString *pushCode(Object * tree, char *line);
int setFlags(Object * tree, int flags);
int getFlag(Object * tree, int flag);
int setParentClass(Object * tree, Object * parentClass);
int listStringlen(ListString * head);
int listTypelen(ListType * head);

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

bool isVerb (Object *result);

void writeEnums (FILE * outh, ListString *iter);

#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) ){
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1)

#endif
