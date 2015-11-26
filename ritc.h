#ifndef RITC_H
#define RITC_H

#include "ObjectTree.h"
#include "preproc.h"
#include "errors.h"
#include "ctype.h"

#define COMPILER_SEP "_$_"

extern int yylex();
extern int yyparse();
extern FILE *yyin;

void defineRSLSymbols(Object*);

Object* findByName(char* name);
Object* findFunctionByFullName(char* name);

void handleEOF();

Object* completeExpression(Object* expression);
Object* finalize(Object* expression);
void closeBrace();
Object* makeReturn(Object* expression);
void incPrev();
void decPrev();

Object* beginClass(char* className, char* parentName);
void doneClass(Object* tree);
Object* beginFunction(char* returnType, char* funcName, Object* parameters);
void doneFunction(Object* tree);
Object* beginConstructor(Object* parameters);
void doneConstructor(Object* tree);
Object* funcParameters(Object* tree, char* paramType, char* paramName);
Object* concatParams(Object* existing, Object* newParam);
Object* declareVariable(char* name, char* type);

Object* conjugateAssign(Object* subject, Object* verb, Object* objects);
Object* conjugate(Object* lhs, Object* verb, Object* rhs);
Object* conjugateConditional(Object* lhs, Object* verb, Object* rhs);
Object* injectC(char* code);

Object* verbAssignment(char* verb);
Object* verbMathOp(char* verb);
Object* verbComparison(char* verb);
Object* verbTernary();
Object* verbIdent(char* verb);
Object* sVerbIdent(char* staticVerb);
Object* verbCtor(char* type);
Object* parenthesize(Object* expr);
Object* objectIdent(char* ident);
Object* objectSelfIdent(char* ident);
Object* objectFloat(float f);
Object* objectInt(int i);
Object* objectPrev();
Object* objectString(char* string);
Object* objectField(char* fullname);

float simplifyFloat(float left, char* op, float right);
int   simplifyInt  (int   left, char* op, int   right);

#endif
