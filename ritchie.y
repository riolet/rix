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
%token <sval> NEWIDENT
%token <sval> UNMARKEDNEWIDENT
%token <sval> FIELD
%token <sval> VERB
%token <sval> STATICVERB
%token <sval> TYPE
%token <sval> MATH_OP
%token <sval> ASSIGNMENT
%token <sval> LPAREN
%token <sval> RPAREN
%token <sval> LBRACKET
%token <sval> RBRACKET
%token <ival> INDENT
%token <ival> UNINDENT
%token <sval> CONDITIONLINK
%token <sval> MATHASSIGN
%token <sval> BITWISEOP
%token <sval> BOOLEANOP
%token <sval> COLON
%token <sval> CLASSDEC
%token <sval> CTORDEC
%token <sval> PARAMCOMMA
%token <sval> RETURN
%token <sval> SELFIDENT
%token <sval> SLCOMMENT
%token <sval> COMPARISON
%token <ival> TERNARY
%token <sval> CODE_INSERT

%token <sval> CONDRETURN

%type <oval> ritchie;
%type <oval> statements;
%type <oval> statement;
%type <oval> simple_statement;
%type <oval> class_statements;
%type <oval> class_statement;
%type <oval> parameters;
%type <oval> codeblock;
%type <oval> classblock;
%type <oval> function_definition;
%type <oval> class_definition;
%type <oval> ctor_definition;
%type <oval> expr;
%type <oval> object;

%type <sval> parameterIdent

%{
void yyerror(YYLTYPE *locp, const char* msg);
//void yyerror(const char* msg);
%}

//  Precedence for the following tokens.
//  Higher tokens happen last
//  Lower tokens happen first / take priority
//  Multiple tokens means treat as the same precedence
//     and rely on associativity.
//  %right (as opposed to %left) means,
//    given a compound expression,
//    evaluate from right to left.
%right ASSIGNMENT MATHASSIGN VERB TYPE STATICVERB CONDRETURN
%right PARAMCOMMA
%right BOOLEANOP
%right COMPARISON TERNARY
%right MATH_OP


%%
%start ritchie;
ritchie:
	statements   { printf("parser: ritchie-stmts\n"); $$ = $1; }
	| ENDOFFILE  { printf("parser: ritchie-EOF\n"); $$ = 0; handleEOF(); }
	;
statements:
  simple_statement              { printf("parser: stmts-s_s\n"); $$ = $1; }
  | statements simple_statement { printf("parser: stmts-stmt,s_s\n"); $$ = $1; }
  ;
simple_statement:
  ENDOFLINE             { printf("parser: s_s-eol\nempty EOL\n"); $$ = 0; }
  | statement ENDOFLINE { printf("parser: s_s-stmt\nstatement EOL\n"); $$ = $1; }
  | statement ENDOFLINE codeblock { printf("parser: s_s-stCB\nstatement EOL\n"); closeBrace(); $$ = $1; }
  | function_definition ASSIGNMENT statement ENDOFLINE {
          printf("parser: s_s-func - Function Defined! %s\n", $1->fullname);
          makeReturn($3); doneFunction($1); }
  | function_definition ENDOFLINE codeblock {
          printf("parser: s_s-func - Function Defined! %s\n", $1->fullname);
          doneFunction($1); }
  | CODE_INSERT ENDOFLINE     { printf("parser: code-insert\n"); $$ = injectC($1);  }
  | class_definition ENDOFLINE classblock {
          printf("parser: s_s-class - Class Defined! %s\n", $1->fullname);
          doneClass($1); }
  ;
statement:
  expr              { printf("parser: stmt-expr\n"); $$ = completeExpression(finalize($1)); }
  | RETURN expr     { printf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn($2)); }
  | RETURN          { printf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn(0)); }
  ;
expr:
  object                  { printf("parser: expr-obj\n");   $$ = $1; }
  | expr PARAMCOMMA expr  { printf("parser: expr-cma\n");   $$ = concatParams($1, $3); }
  | expr ASSIGNMENT expr  { printf("parser: expr-asn\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr MATHASSIGN expr  { printf("parser: expr-mas\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr COMPARISON expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr BOOLEANOP  expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr  TERNARY   expr  { printf("parser: expr-cmp\n");   $$ = conjugate($1,  verbTernary(), $3); }
  | expr  CONDRETURN   expr  { printf("parser: expr-crt\n");   $$ = conjugate($1,  verbCondReturn(), $3); }
  | expr  MATH_OP   expr  { printf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }

  | expr   VERB     expr  { printf("parser: expr-svo\n");   $$ = conjugate($1,  verbIdent($2), $3); }
  | expr   VERB           { printf("parser: expr-sv \n");   $$ = conjugate($1,  verbIdent($2),  0); }
  |        VERB     expr  { printf("parser: expr- vo\n");   $$ = conjugate( 0,  verbIdent($1), $2); }
  |        VERB           { printf("parser: expr- v \n");   $$ = conjugate( 0,  verbIdent($1),  0); }
  |      STATICVERB expr  { printf("parser: expr- Xo\n");   $$ = conjugate( 0, sVerbIdent($1), $2); }
  |      STATICVERB       { printf("parser: expr- X \n");   $$ = conjugate( 0, sVerbIdent($1),  0); }
  |        TYPE     expr  { printf("parser: expr-sto\n");   $$ = conjugate( 0,   verbCtor($1), $2); }
  |        TYPE           { printf("parser: expr-sto\n");   $$ = conjugate( 0,   verbCtor($1),  0); }
  | LPAREN expr RPAREN    { printf("parser: expr-prn\n");   $$ = parenthesize($2); }
  ;
object:
  INT       { printf("parser: object-int\n");       $$ = objectInt($1); }
  | FLOAT   { printf("parser: object-float\n");     $$ = objectFloat($1);}
  | IDENT   { printf("parser: object-identifer\n"); $$ = objectIdent($1); }
  | NEWIDENT   { printf("parser: object-new-identifer\n"); $$ = objectNewIdent($1); }
  | UNMARKEDNEWIDENT { printf("parser: object-unmarked-new-identifer\n"); $$ = objectUnmarkedNewIdent($1); }
  | FIELD   { printf("parser: object-field\n");     $$ = objectField($1);  }
  | STRING  { printf("parser: object-string\n");    $$ = objectString($1);  }
  | SELFIDENT { printf("parser: object-self\n");    $$ = objectSelfIdent($1);}
  | CONDITIONLINK { printf("parser: object-previous\n"); $$ = objectPrev();   }
  ;



function_definition:
  UNMARKEDNEWIDENT RETURN TYPE COLON parameters { printf("parser: func-def\n"); $$ = beginFunction($3, $1, $5); }
  | VERB RETURN TYPE COLON parameters { printf("parser: func-def\n"); $$ = beginFunction($3, $1, $5); }
  | UNMARKEDNEWIDENT RETURN COLON parameters { printf("parser: func-void\n"); $$ = beginFunction("void", $1, $4); }
  | VERB RETURN COLON parameters { printf("parser: func-void\n"); $$ = beginFunction("void", $1, $4); }
  ;
parameters:
  %empty                                { printf("parser: param0\n"); $$ = CreateObject(0, 0, 0, Expression, 0); }
  | TYPE parameterIdent                       { printf("parser: param1\n"); $$ = funcParameters( 0, $1, $2); }
  | parameters PARAMCOMMA TYPE parameterIdent { printf("parser: paramN\n"); $$ = funcParameters($1, $3, $4); }
  ;

parameterIdent:
  IDENT                                 { printf("parser: IDENT\n"); $$ = $1; }
  |UNMARKEDNEWIDENT                     { printf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

codeblock:
  INDENT statements UNINDENT { printf("parser: codeblock\n"); $$ = $2; }
  ;



class_definition:
  UNMARKEDNEWIDENT CLASSDEC TYPE { printf("parser: class-def\n"); $$ = beginClass($1, $3); }
  ;
ctor_definition:
  CTORDEC parameters { printf("parser: class-def\n"); $$ = beginConstructor($2); }
  ;
classblock:
  INDENT class_statements UNINDENT { $$ = $2; }
  ;
class_statements:
  class_statement
  | class_statements class_statement
  ;
class_statement:
  ENDOFLINE { printf("parser: c_s-eol\nempty EOL\n"); $$ = 0; }
  | UNMARKEDNEWIDENT ASSIGNMENT TYPE ENDOFLINE { printf("parser: c_s:varType\n"); $$ = declareVariable($1, $3); }
  | function_definition ENDOFLINE codeblock {
          printf("parser: c_s-func - Function Defined! %s\n", $1->fullname);
          doneFunction($1); }
  | ctor_definition ENDOFLINE codeblock {
          printf("parser: c_s-func - Constructor Defined! %s\n", $1->fullname);
          doneConstructor($1); }
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
