#ifndef RITC_H
#define RITC_H

#include "ObjectTree.h"
#include "preproc.h"
#include "errors.h"
#include "ctype.h"
#include "crsl.h"

#define COMPILER_SEP "_$_"
#define GENERIC_PARAM "Generic_$$"

extern int yylex();
extern int yyparse();
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
Object *beginFunction(char *funcName, char *returnType, char *returnGenericType, Object * parameters);
void doneFunction(Object * tree);
Object *beginConstructor(Object * parameters);
void doneConstructor(Object * tree);
Object *beginDestructor(Object * parameters);
void doneDestructor(Object * tree);

Object *funcParameters(Object * tree, char *paramType, char *paramName, char *generiType);
Object *concatParams(Object * existing, Object * newParam);
Object *declareVariable(char *name, char *type, char * genericType);

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
Object *verbCtor(char *type, char *ytype);
Object *parenthesize(Object * expr);
Object *objectIdent(char *ident);
Object *objectNewIdent(char *ident);
Object *objectUnmarkedNewIdent(char *ident);
Object *objectSelfIdent(char *ident);
Object *objectfloat(float f);
Object *objectInt(int i);
Object *objectChar(char *c);
Object *objectPrev();
Object *objectPlaceHolderType(char *ident);
Object *objectString(char *string);
Object *conjugateAccessorIdent(Object *subject, char *field);

Object *createCodeBlock(Object * expression);

float simplifyfloat(float left, char *op, float right);
int simplifyInt(int left, char *op, int right);

Object *concatGenerics(Object * existing, Object * newGeneric);
Object *genericOfGeneric(Object * parent, Object * child);

Object * directive(char *key, char *value);
#endif
