%{
#include <stdio.h>
#include <stdlib.h>
#include "ObjectTree.h"
#include "ritc.h"
#include "errors.h"

#define YYPARSE_PARAM scanner
#define YYLEX_PARAM   scanner

%}

%glr-parser
%locations
%define api.pure //full
%define parse.error verbose

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	int     ival;
	float   fval;
	char*   sval;
	Object* oval;
}

// define the constant-string tokens:
%token ENDOFLINE
%token ENDOFFILE 0 "end of file"

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <sval> IDENT
%token <sval> VERB
%token <sval> TYPE
%token <sval> MATH_OP
%token <sval> ASSIGNMENT
%token <sval> LPAREN
%token <sval> RPAREN
%token <ival> INDENT
%token <ival> UNINDENT
%token <sval> CONDITIONLINK
%token <sval> MATHASSIGN
%token <sval> BITWISEOP
%token <sval> FUNCDEC
%token <sval> PARAMCOMMA
%token <sval> RETURN
%token <sval> SELFIDENT

%type <oval> ritchie;
%type <oval> statements;
%type <oval> statement;
%type <oval> simple_statement;
%type <oval> parameters;
%type <oval> codeblock;
%type <oval> function_definition;
%type <oval> expression
%type <oval> objects
%type <oval> object
%type <oval> verb
%type <oval> subject
%type <ival> int_expression
%type <fval> float_expression

%{
void yyerror(YYLTYPE *locp, const char* msg);
//void yyerror(const char* msg);
%}
%%
%start ritchie;
ritchie:
	statements   { printf("parser: ritchie-stmts\n"); $$ = $1; }
	| ENDOFFILE  { printf("parser: ritchie-EOF\n"); $$ = CreateObject(0, 0, 0, Expression, 0); handleEOF(); }
	;
statements:
  simple_statement              { printf("parser: stmts-s_s\n"); $$ = $1; }
  | statements simple_statement { printf("parser: stmts-stmt,s_s\n"); $$ = $1; }
  ;
simple_statement:
  ENDOFLINE             { printf("parser: s_s-eol\nempty EOL\n"); $$ = CreateObject(0, 0, 0, Expression, 0); }
  | statement ENDOFLINE { printf("parser: s_s-stmt\nstatement EOL\n"); $$ = $1; }
  | function_definition ENDOFLINE codeblock { printf("parser: s_s-func\n"); doneFunction($1); }
  ;
function_definition:
  TYPE FUNCDEC IDENT parameters { printf("parser: func-def\n"); $$ = funcHeader($1, $3, $4); }
  ;
codeblock:
  INDENT statements UNINDENT { printf("parser: codeblock\n"); $$ = $2; }
  ;
parameters:
  TYPE IDENT                          { printf("parser: param1\n"); $$ = funcParameters( 0, $1, $2); }
  | parameters PARAMCOMMA TYPE IDENT  { printf("parser: param2\n"); $$ = funcParameters($1, $3, $4); }
  ;
statement:
  expression          { printf("parser: stmt-expr\n");   $$ = completeExpression($1); }
  | RETURN expression { printf("parser: stmt-retEx\n"); $$ = completeExpression(makeReturn($2)); }
  | RETURN object     { printf("parser: stmt-retOb\n"); $$ = completeExpression(makeReturn($2)); }
  ;
expression:
  subject verb objects    { printf("parser: expr-svo\n"); $$ = conjugate($1, $2, $3); }
  | subject verb          { printf("parser: expr-sv\n");  $$ = conjugate($1, $2,  0); }
  | verb objects          { printf("parser: expr-vo\n");  $$ = conjugate( 0, $1, $2); }
  | verb                  { printf("parser: expr-v\n");   $$ = conjugate( 0, $1,  0); }
  ;
objects:
  object                  { printf("parser: objects-object\n"); $$ = $1; }
  | objects verb object   { printf("parser: objects-ovo\n"); $$ = conjugate($1, $2, $3); }
/*  | objects "," objects   /* This hasn't been handled properly yet. */
  ;
object:
  verb                       { printf("parser: object-ie\n"); $$ = objectVerb($1); }
  | int_expression           { printf("parser: object-ie\n"); $$ = objectInt($1); }
  | float_expression         { printf("parser: object-fe\n"); $$ = objectFloat($1); }
  | IDENT                    { printf("parser: object-ID\n"); $$ = objectIdent($1); }
  | LPAREN expression RPAREN { printf("parser: object-exp\n"); $$ = parenthesize($2); }
  ;
subject:
  IDENT  { printf("parser: subject-ident(%s)\n", $1); $$ = subjectIdent($1); }
  ;
verb:
  VERB         { printf("parser: verb-idnt\n"); $$ = verbIdent($1); }
  | ASSIGNMENT { printf("parser: verb-asgn\n"); $$ = verbAssignment($1); }
  | MATH_OP    { printf("parser: verb-math\n"); $$ = verbMathOp($1); }
  ;
int_expression:
  INT { printf("parser: ie-i\n");  $$ = $1; }
  /*
  | int_expression MATH_OP INT { printf("parser: ie-ie+i\n"); $$ = simplifyInt($1, $2, $3); }
  | INT MATH_OP INT            { printf("parser: ie-i+i\n");  $$ = simplifyInt($1, $2, $3); }
  */
  ;
float_expression:
  FLOAT { printf("parser: fe-f\n");   $$ = $1; }
  /*
  | float_expression MATH_OP FLOAT { printf("parser: fe-fe+f\n"); $$ = simplifyFloat($1, $2, $3); }
  | float_expression MATH_OP INT   { printf("parser: fe-fe+i\n"); $$ = simplifyFloat($1, $2, $3); }
  | int_expression MATH_OP FLOAT   { printf("parser: fe-ie+f\n"); $$ = simplifyFloat($1, $2, $3); }
  | FLOAT MATH_OP INT   { printf("parser: fe-f+i\n"); $$ = simplifyFloat($1, $2, $3); }
  | INT MATH_OP FLOAT   { printf("parser: fe-i+f\n"); $$ = simplifyFloat($1, $2, $3); }
  | FLOAT MATH_OP FLOAT { printf("parser: fe-f+f\n"); $$ = simplifyFloat($1, $2, $3); }
  */
  ;
%%

//void yyerror(char const *msg) {
void yyerror(YYLTYPE *locp, const char* msg) {
  char errorMsg[256];
  snprintf(errorMsg, 256, "%s\n", msg);
  criticalError(ERROR_ParseError, errorMsg);
	// might as well halt now:
	exit(-1);
}
