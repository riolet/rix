#ifndef RITC_H
#define RITC_H

#include "ObjectTree.h"
#include "errors.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;

Object* findByName(char* name);

void handleEOF();

Object* completeExpression(Object* expression);

char* subjectIdent(char* subject);

Object* exprSVO(char* lhs, Object* verb, Object* rhs);
Object* conjugate(Object* lhs, Object* verb, Object* rhs);
Object* verbAssignment(char* verb);
Object* verbMathOp(char* verb);
Object* verbIdent(char* verb);
Object* parenthesize(Object* expr);
Object* objectVerb(Object* verb);
Object* objectIdent(char* ident);
Object* objectFloat(float f);
Object* objectInt(int i);

float simplifyFloat(float left, char* op, float right);
int   simplifyInt  (int   left, char* op, int   right);

#endif
