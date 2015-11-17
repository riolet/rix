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
%token <sval> BOOLEANOP
%token <sval> FUNCDEC
%token <sval> PARAMCOMMA
%token <sval> RETURN
%token <sval> SELFIDENT
%token <sval> SLCOMMENT
%token <sval> COMPARISON
%token <ival> TERNARY

%type <oval> ritchie;
%type <oval> statements;
%type <oval> statement;
%type <oval> simple_statement;
%type <oval> parameters;
%type <oval> codeblock;
%type <oval> function_definition;
%type <oval> expr;
//%type <oval> exprlist;
%type <oval> object;

%{
void yyerror(YYLTYPE *locp, const char* msg);
//void yyerror(const char* msg);
%}

%right ASSIGNMENT MATHASSIGN VERB
%right PARAMCOMMA
%right BOOLEANOP
%right COMPARISON TERNARY
%right MATH_OP


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
  | statement ENDOFLINE codeblock { printf("parser: s_s-stCB\nstatement EOL\n"); closeBrace(); $$ = $1; }
  | function_definition ENDOFLINE codeblock { printf("parser: s_s-func\n"); doneFunction($1); }
  ;
function_definition:
  TYPE FUNCDEC IDENT parameters { printf("parser: func-def\n"); $$ = funcHeader($1, $3, $4); }
  | FUNCDEC IDENT parameters { printf("parser: func-void\n"); $$ = funcHeader(0, $2, $3); }
  ;
codeblock:
  INDENT statements UNINDENT { printf("parser: codeblock\n"); $$ = $2; }
  ;
parameters:
  %empty                              { printf("parser: param0\n"); $$ = CreateObject(0, 0, 0, Expression, 0); }
  | TYPE IDENT                        { printf("parser: param1\n"); $$ = funcParameters( 0, $1, $2); }
  | parameters PARAMCOMMA TYPE IDENT  { printf("parser: paramN\n"); $$ = funcParameters($1, $3, $4); }
  ;
statement:
  expr              { printf("parser: stmt-expr\n"); $$ = completeExpression(finalize($1)); }
  | RETURN expr     { printf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn($2)); }
  ;
expr:
  object                  { printf("parser: expr-obj\n");   $$ = $1; }
  | expr PARAMCOMMA expr  { printf("parser: expr-cma\n");   $$ = concatParams($1, $3); }
  | expr ASSIGNMENT expr  { printf("parser: expr-asn\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr MATHASSIGN expr  { printf("parser: expr-mas\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr COMPARISON expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr BOOLEANOP  expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr  TERNARY   expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1, verbTernary(), $3); }
  | expr  MATH_OP   expr  { printf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }
  | expr   VERB     expr  { printf("parser: expr-evb\n");   $$ = conjugate($1, verbIdent($2), $3); }
  |        VERB     expr  { printf("parser: expr-vbe\n");   $$ = conjugate( 0, verbIdent($1), $2); }
  | expr   VERB           { printf("parser: expr-evb\n");   $$ = conjugate($1, verbIdent($2), 0); }
  |        VERB           { printf("parser: expr- v \n");   $$ = conjugate( 0, verbIdent($1), 0); }
  | LPAREN expr RPAREN    { printf("parser: expr-prn\n");   $$ = parenthesize($2); }
  ;
object:
  INT       { printf("parser: object-int\n");       $$ = objectInt($1); }
  | FLOAT   { printf("parser: object-float\n");     $$ = objectFloat($1); }
  | IDENT   { printf("parser: object-identifer\n"); $$ = objectIdent($1); }
  | STRING  { printf("parser: object-string\n");    $$ = objectString($1); }
  | CONDITIONLINK { printf("parser: object-previous\n"); $$ = objectPrev(); }
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
