#ifndef RITC_H
#define RITC_H

extern int yylex();
extern int yyparse();
extern FILE *yyin;

void handleEOF();

void handleEOL();

char* assignDeclare(char* text);
char* handleAssign(char* subject, char* object);

char* objectIdent(char* text);
char* objectFloat(float f);
char* objectInt(int i);

float simplifyFloat(float left, char* op, float right);
int   simplifyInt  (int   left, char* op, int   right);

#endif
