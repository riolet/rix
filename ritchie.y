%{
#include <stdio.h>
#include <stdlib.h>

// stuff from flex that bison needs to know about:
extern int yylex();
extern int yyparse();
extern FILE *yyin;

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


%%

// the first rule defined is the highest-level rule, which in our
// case is just the concept of a whole "snazzle file":

%start ritchie;
ritchie:
	statements       { printf("ritchie statements: \n"); }
	| ENDOFFILE      { printf("ritchie EOF: \n"); }
	;
statements:
  simple_statement { printf("simple_statement: \n"); }
  ;
simple_statement:
  ENDOFLINE              {printf("\n");}
  | statement ENDOFLINE  {printf("\n");}
  ;
statement:
  expression
  ;
expression:
  IDENT ASSIGNMENT expression { printf ("Ident(%s) Assignment(%s) ", $1, $2); }
  | value MATH_OP value       { printf ("math_op(%s) ", $2); }
  ;
value:
  INT      { printf ("int(%d) ", $1); }
  | FLOAT  { printf ("float(%f) ", $1); }
  ;

%%


int main(int argc, char** argv) {
	// open a file handle to a particular file:
	FILE *myfile = fopen("test.rit", "r");
	// make sure it's valid:
	if (!myfile) {
	  printf("I can't open test.rit!\n");
		return -1;
	}
	// set flex to read from it instead of defaulting to STDIN:
	yyin = myfile;

	// parse through the input until there is no more:
	do {
		yyparse();
	} while (!feof(yyin));
	
}

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
