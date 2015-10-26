#ifndef RITC_H
#define RITC_H

extern int yylex();
extern int yyparse();
extern FILE *yyin;

void handleEOF();

void handleEOL();

char* subjectIdent(char* subject);
char* verbIdent(char* verb);
char* verbAssignment(char* verb);
char* verbMathOp(char* verb);
char* objectIdent(char* object);
char* objectFloat(float f);
char* objectInt(int i);

float simplifyFloat(float left, char* op, float right);
int   simplifyInt  (int   left, char* op, int   right);

#endif
