#ifndef RITC_H
#define RITC_H

#include "ObjectTree.h"
#include "errors.h"
#include "ctype.h"

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

Object* funcHeader(char* returnType, char* funcName, Object* parameters);
Object* funcParameters(Object* tree, char* paramType, char* paramName);
Object* concatParams(Object* existing, Object* newParam);
void doneFunction(Object* tree);

Object* conjugateAssign(Object* subject, Object* verb, Object* objects);
Object* conjugate(Object* lhs, Object* verb, Object* rhs);

Object* verbAssignment(char* verb);
Object* verbMathOp(char* verb);
Object* verbComparison(char* verb);
Object* verbTernary();
Object* verbIdent(char* verb);
Object* parenthesize(Object* expr);
Object* objectVerb(Object* verb);
Object* objectIdent(char* ident);
Object* subjectIdent(char* ident);
Object* objectFloat(float f);
Object* objectInt(int i);
Object* objectPrev();
Object* objectString(char* string);

float simplifyFloat(float left, char* op, float right);
int   simplifyInt  (int   left, char* op, int   right);

#endif
