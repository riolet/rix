%{
#include <stdio.h>
#include <stdlib.h>
#include "ritc.h"

// stuff from flex that bison needs to know about:
//extern int yylex();
//extern int yyparse();
//extern FILE *yyin;

//void yyerror(const YYLTYPE l, const char* m);
void yyerror(const char* m);
%}

%locations
%define parse.error verbose
// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	int ival;
	float fval;
	char *sval;
}

// define the constant-string tokens:
%token ENDOFLINE
%token ENDOFFILE 0 "end of file"

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> IDENT
%token <sval> MATH_OP
%token <sval> ASSIGNMENT 

%type <sval> statement
%type <sval> expression
%type <sval> objects
%type <sval> object
%type <sval> subject
%type <ival> int_expression
%type <fval> float_expression
%%

// the first rule defined is the highest-level rule, which in our
// case is just the concept of a whole "snazzle file":

%start ritchie;
ritchie:
	statements
	| ENDOFFILE  { handleEOF(); }
	;
statements:
  simple_statement
  | statements simple_statement
  ;
simple_statement:
  ENDOFLINE              { handleEOL();}
  | statement ENDOFLINE  { handleEOL();}
  ;
statement:
  expression            {$$ = $1;}
  | "(" expression ")"  {$$ = $2;} /* This is probably wrong */
  ;
expression:
  subject ASSIGNMENT objects                 { assignDeclare($1); $$ = handleAssign($1, $3); }
  ;
subject:
  IDENT
  ;
objects:
  object
  | objects "," object
  ;
object:
  int_expression { $$ = objectInt($1); }
  | float_expression { $$ = objectFloat($1); }
  | IDENT { $$ = objectIdent($1); }
  ;
int_expression:
  INT MATH_OP INT { $$ = simplifyInt($1, $2, $3); }
  ;
float_expression:
    FLOAT MATH_OP INT   { $$ = simplifyFloat($1, $2, $3); }
  | INT MATH_OP FLOAT   { $$ = simplifyFloat($1, $2, $3); }
  | FLOAT MATH_OP FLOAT { $$ = simplifyFloat($1, $2, $3); }

%%


void yyerror(const char *s) {
	fprintf(stderr, "EEK, parse error!  Message: %s\n", s);
	// might as well halt now:
	exit(-1);
}

/*
void yyerror(const YYLTYPE l, const char* m) {
  fprintf(stderr, "EEK, parse error!  Message: %s\n", m);
	// might as well halt now:
	exit(-1);
}
*/
