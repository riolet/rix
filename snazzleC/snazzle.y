%{
#include <stdio.h>
#include <stdlib.h>

// stuff from flex that bison needs to know about:
extern int yylex();
extern int yyparse();
extern FILE *yyin;
 
void yyerror(const char *s);
%}

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
%token SNAZZLE TYPE
%token END

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%

// the first rule defined is the highest-level rule, which in our
// case is just the concept of a whole "snazzle file":
snazzle:
	header template body_section footer { printf("done with a snazzle file!\n"); }
	;
header:
	SNAZZLE FLOAT { printf("reading a snazzle file version %f\n", $2); }
	;
template:
	typelines
	;
typelines:
	typelines typeline
	| typeline
	;
typeline:
	TYPE STRING { printf("new defined snazzle type: %s\n", $2); }
	;
body_section:
	body_lines
	;
body_lines:
	body_lines body_line
	| body_line
	;
body_line:
	INT INT INT INT STRING { printf("new snazzle: %d, %d, %d, %d, %s\n", $1, $2, $3, $4, $5); }
	;
footer:
	END
	;

%%

int main(int argc, char** argv) {
	// open a file handle to a particular file:
	FILE *myfile = fopen("a.snazzle.file", "r");
	// make sure it's valid:
	if (!myfile) {
	  printf("I can't open a.snazzle.file!\n");
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
