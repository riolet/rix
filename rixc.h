#ifndef RITC_H
#define RITC_H

#include "ObjectTree.h"
#include "errors.h"
#include "ctype.h"
#include "crsl.h"

#define COMPILER_SEP "_$_"
#define GENERIC_PARAM "Generic_$$"
#define BASETYPE "BaseType"

extern int yylex();
extern int yyparse();
extern int yylineno;
char yyinFileName[256];
extern FILE *yyin;

//void defineRSLSymbols(Object *);

Object *findByName(char *name);
Object *findFunctionByFullName(char *name);

void handleEOF();

Object *completeExpression(Object * expression);
Object *finalize(Object * expression);
void closeBrace();
Object *makeReturn(Object * expression);
void checkPrevExists();
void incPrev();
void decPrev();

Object *beginClass(char *className, char *parentName, Object *typeArgs, bool isPrimitive);
void doneClass(Object * tree);
Object *beginFunction(char *funcName, ListType *returnType, Object * parameters);
void doneFunction(Object * tree);
Object *beginConstructor(Object * parameters);
void doneConstructor(Object * tree);
Object *beginDestructor(Object * parameters);
void doneDestructor(Object * tree);

Object *funcParameters(Object * tree, ListType *paramType, char *paramName);
Object *concatParams(Object * existing, Object * newParam);
Object *declareVariable(char *name, ListType *type);

Object *conjugateNewVarAssignment(char * ident, Object * verb, Object * objects);
Object *conjugateAssign(Object * subject, Object * verb, Object * objects);
Object *conjugate(Object * lhs, Object * verb, Object * rhs);
Object *conjugateConditional(Object * lhs, Object * verb, Object * rhs);
Object *injectC(char *code);

Object *verbNewVarAssignment(char *verb);
Object *verbAssignment(char *verb);
Object *verbMathOp(char *verb);
Object *verbComparison(char *verb);
Object *verbTernary();
Object *verbCondReturn();
Object *verbIdent(char *verb);
Object *verbDestructor();
Object *verbGetObjAtIdx();
Object *verbPutObjAtIdx();
Object *sVerbIdent(char *staticVerb);
ListType * createListType (char *type);
char * genericNameToString (ListType *genericType);
Object *verbCtor(ListType *Type);
Object *parenthesize(Object * expr);
Object *objectIdent(char *ident);
Object *objectNewIdent(char *ident);
Object *objectUnmarkedNewIdent(char *ident);
Object *objectSelfIdent(char *ident);
Object *objectdouble(double f);
Object *objectInt(int i);
Object *objectChar(char *c);
Object *objectPrev();
Object *objectPlaceHolderType(char *ident);
Object *objectString(char *string);
Object *conjugateAccessorIdent(Object *subject, char *field);

Object *createCodeBlock(Object * expression);

double simplifydouble(double left, char *op, double right);
int simplifyInt(int left, char *op, int right);

ListType *concatGenerics(ListType * existing, ListType * newGeneric);
ListType *genericOfGeneric(ListType * parent, ListType * child);

Object *directive(char *key, char *value);

bool beginsWith(const char *haystack, const char *needle);

void * displayDefinedSymbols(Object *tree);
#endif
