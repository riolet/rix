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
%token <sval> CHAR
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
%token <sval> LBRACE
%token <sval> RBRACE
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
%token <sval> LESSTHAN
%token <sval> GREATERTHAN
%token <ival> TERNARY
%token <sval> CODE_INSERT

%token <sval> CONDRETURN
%token <sval> ACCESSOR

%token <sval> DTV_EXTERNAL
%token <sval> DTV_ADDSOURCE

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
%type <oval> arguments;
%type <oval> arglist;
%type <oval> typeArgList;


%type <sval> parameterIdent;
%type <sval> anyIdent;
%type <sval> anyIdentOrVerb;
//%type <sval> function_type_entry;

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
%right ENDOFLINE INDENT
%right PARAMCOMMA
%right BOOLEANOP
%right COMPARISON LESSTHAN GREATERTHAN TERNARY
%right MATH_OP
%right ACCESSOR
%right DESTRUCTOR
%right LBRACKET RBRACKET LBRACE RBRACE
//%right class_definition

%%
%start ritchie;
ritchie:
	statements   { compilerDebugPrintf("parser: ritchie-stmts\n"); $$ = $1; }
	| ENDOFFILE  { compilerDebugPrintf("parser: ritchie-EOF\n"); $$ = 0; handleEOF(); }
	;
statements:
  simple_statement              { compilerDebugPrintf("parser: stmts-s_s\n"); $$ = $1; }
  | statements simple_statement { compilerDebugPrintf("parser: stmts-stmt,s_s\n"); $$ = $1; }
  ;

simple_statement:
  ENDOFLINE             { compilerDebugPrintf("parser: s_s-eol\nempty EOL\n"); $$ = 0; }
  | DTV_EXTERNAL STRING ENDOFLINE { compilerDebugPrintf("parser: dtv\n"); directive($1,$2);  }
  | DTV_ADDSOURCE STRING ENDOFLINE { compilerDebugPrintf("parser: dtv\n"); directive($1,$2);  }
  | statement ENDOFLINE { compilerDebugPrintf("parser: s_s-stmt\nstatement EOL\n"); $$ = $1; }
  | statement ENDOFLINE codeblock { compilerDebugPrintf("parser: s_s-stCB\nstatement EOL\n"); closeBrace(); $$ = $1; }
  | function_definition ASSIGNMENT statement ENDOFLINE {
          compilerDebugPrintf("parser: s_s-func - Funliner Defined! %s\n", $1->fullname);
          makeReturn($3); doneFunction($1); }
  | function_definition ENDOFLINE codeblock {
          compilerDebugPrintf("parser: s_s-func - Function Defined! %s\n", $1->fullname);
          doneFunction($1); }
  | CODE_INSERT ENDOFLINE     { compilerDebugPrintf("parser: code-insert\n"); $$ = injectC($1);  }
  | class_definition ENDOFLINE classblock {
          compilerDebugPrintf("parser: s_s-class - Class Defined! %s\n", $1->fullname);
          doneClass($1); }
  ;

statement:
  expr              { compilerDebugPrintf("parser: stmt-expr\n"); $$ = completeExpression(finalize($1)); }
  | RETURN expr     { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn($2)); }
  | RETURN          { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn(0)); }
  ;
expr:
  object                  { compilerDebugPrintf("parser: expr-obj\n");   $$ = $1; }
  | expr ASSIGNMENT expr  { compilerDebugPrintf("parser: expr-asn\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr MATHASSIGN expr  { compilerDebugPrintf("parser: expr-mas\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr LESSTHAN expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr GREATERTHAN expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr COMPARISON expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr BOOLEANOP  expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr  TERNARY   expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1,  verbTernary(), $3); }
  | expr  CONDRETURN   expr  { compilerDebugPrintf("parser: expr-crt\n");   $$ = conjugate($1,  verbCondReturn(), $3); }
  | expr  MATH_OP   expr  { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }

//  | expr   VERB     expr  { compilerDebugPrintf("parser: expr-svo\n");   $$ = conjugate($1,  verbIdent($2), $3); }
//  | expr   VERB           { compilerDebugPrintf("parser: expr-sv \n");   $$ = conjugate($1,  verbIdent($2),  0); }

  |        VERB     arguments  { compilerDebugPrintf("parser: expr- vo\n");   $$ = conjugate( 0,  verbIdent($1), $2); }
//  |        VERB                { compilerDebugPrintf("parser: expr- v \n");   $$ = conjugate( 0,  verbIdent($1),  0); }

//  |      STATICVERB expr  { compilerDebugPrintf("parser: expr- Xo\n");   $$ = conjugate( 0, sVerbIdent($1), $2); }
//  |      STATICVERB       { compilerDebugPrintf("parser: expr- X \n");   $$ = conjugate( 0, sVerbIdent($1),  0); }
  |        TYPE     arguments  { compilerDebugPrintf("parser: expr-sto\n");   $$ = conjugate( 0,   verbCtor($1,0), $2); }
  |        TYPE LBRACE TYPE RBRACE arguments  { compilerDebugPrintf("parser: expr-sto\n");   $$ = conjugate( 0,   verbCtor($1,$3), $5); }
  | LPAREN expr RPAREN    { compilerDebugPrintf("parser: expr-prn\n");   $$ = parenthesize($2); }
  | expr LBRACKET expr RBRACKET ASSIGNMENT expr { compilerDebugPrintf("parser: expr-prn\n");   $$ = conjugate($1,  verbPutObjAtIdx(), concatParams($3,$6); }
  | expr LBRACKET expr RBRACKET  { compilerDebugPrintf("parser: expr-prn\n");   $$ = conjugate($1,  verbGetObjAtIdx(), $3); }
  | expr ACCESSOR anyIdent { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugateAccessorIdent( $1, $3); }
//  | expr ACCESSOR VERB { compilerDebugPrintf("parser: expr-.v\n");   $$ = conjugate( $1, verbIdent($3), 0); }
  | expr ACCESSOR VERB arguments { compilerDebugPrintf("parser: expr-.vo\n");   $$ = conjugate( $1, verbIdent($3), $4); }
  | IDENT DESTRUCTOR      { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate(objectIdent($1),  verbDestructor(), 0); }
  ;

arguments:
  LPAREN RPAREN { compilerDebugPrintf("parser: arg-()\n");   $$ = 0; } //0-ary
  | LPAREN arglist RPAREN { compilerDebugPrintf("parser: arg(...)\n");   $$ = $2; } //n-ary
  ;

arglist:
  expr { compilerDebugPrintf("parser: arg-(x)\n");   $$ = $1; } //1-ary
  | expr PARAMCOMMA arglist { compilerDebugPrintf("parser: arg(x,y)\n");   $$ = concatParams($1,$3); } //n-ary
  ;

typeArgList:
  UNMARKEDNEWIDENT { compilerDebugPrintf("parser: arg-(x) %s\n",$1);   $$ = objectPlaceHolderType($1); } //1-ary
  | UNMARKEDNEWIDENT PARAMCOMMA UNMARKEDNEWIDENT { compilerDebugPrintf("parser: arg(x,y) %s, %s\n",$1,$3);   $$ = concatParams(objectPlaceHolderType($1),objectPlaceHolderType($3)); } //n-ary
  ;


object:
  INT       { compilerDebugPrintf("parser: object-int\n");       $$ = objectInt($1); }
  | CHAR       { compilerDebugPrintf("parser: object-int\n");       $$ = objectChar($1); }
  | FLOAT   { compilerDebugPrintf("parser: object-float\n");     $$ = objectfloat($1);}
  | IDENT   { compilerDebugPrintf("parser: object-identifer\n"); $$ = objectIdent($1); }
  | NEWIDENT   { compilerDebugPrintf("parser: object-new-identifer\n"); $$ = objectNewIdent($1); }
  | UNMARKEDNEWIDENT { compilerDebugPrintf("parser: object-unmarked-new-identifer\n"); $$ = objectUnmarkedNewIdent($1); }
  | STRING  { compilerDebugPrintf("parser: object-string\n");    $$ = objectString($1);  }
  | SELFIDENT { compilerDebugPrintf("parser: object-self\n");    $$ = objectSelfIdent($1);}
  | CONDITIONLINK { compilerDebugPrintf("parser: object-previous\n"); $$ = objectPrev();   }
  ;

anyIdent:
  IDENT   { compilerDebugPrintf("parser: IDENT\n"); $$ = $1; }
  | NEWIDENT   { compilerDebugPrintf("parser: NEWIDENT\n"); $$ = $1; }
  | UNMARKEDNEWIDENT { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

anyIdentOrVerb:
  IDENT   { compilerDebugPrintf("parser: IDENT\n"); $$ = $1; }
  | NEWIDENT   { compilerDebugPrintf("parser: NEWIDENT\n"); $$ = $1; }
  | UNMARKEDNEWIDENT { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  | VERB { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

function_definition:
  anyIdentOrVerb RETURN TYPE LPAREN parameters RPAREN { compilerDebugPrintf("parser: func-def\n"); $$ = beginFunction($3, $1, $5); }
  | anyIdentOrVerb RETURN LPAREN parameters RPAREN { compilerDebugPrintf("parser: func-void\n"); $$ = beginFunction("void", $1, $4); }
  ;

//function_type_entry:
//  TYPE                                  { compilerDebugPrintf("parser: TYPE\n"); $$ = $1; }
//  |NEWIDENT                             { compilerDebugPrintf("parser: NEWIDENT\n"); $$ = $1; }
//  ;

parameters:
  %empty                                { compilerDebugPrintf("parser: param0\n"); $$ = CreateObject(0, 0, 0, Expression, 0); }
  | TYPE parameterIdent                       { compilerDebugPrintf("parser: param1\n"); $$ = funcParameters( 0, $1, $2); }
  | parameters PARAMCOMMA TYPE parameterIdent { compilerDebugPrintf("parser: paramN\n"); $$ = funcParameters($1, $3, $4); }
  ;

parameterIdent:
  IDENT                                 { compilerDebugPrintf("parser: IDENT\n"); $$ = $1; }
  |UNMARKEDNEWIDENT                     { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

codeblock:
  INDENT statements UNINDENT { compilerDebugPrintf("parser: codeblock\n"); $$ = $2; }
  ;



class_definition:
  UNMARKEDNEWIDENT CLASSDEC TYPE { compilerDebugPrintf("parser: class-def\n"); $$ = beginClass($1, $3, 0); }
  | UNMARKEDNEWIDENT LBRACE typeArgList RBRACE CLASSDEC TYPE { compilerDebugPrintf("parser: gen-class-def\n"); $$ = beginClass($1, $6, $3); }
  ;

ctor_definition:
  SELFIDENT LPAREN parameters RPAREN { compilerDebugPrintf("parser: class-def\n"); $$ = beginConstructor($3); }
  ;
classblock:
  INDENT class_statements UNINDENT { $$ = $2; }
  ;
class_statements:
  class_statement
  | class_statements class_statement
  ;
class_statement:
  ENDOFLINE { compilerDebugPrintf("parser: c_s-eol\nempty EOL\n"); $$ = 0; }
  | UNMARKEDNEWIDENT ASSIGNMENT TYPE ENDOFLINE { compilerDebugPrintf("parser: c_s:varType\n"); $$ = declareVariable($1, $3); }
  | function_definition ENDOFLINE codeblock {
          compilerDebugPrintf("parser: c_s-func - Function Defined! %s\n", $1->fullname);
          doneFunction($1); }
  | function_definition ASSIGNMENT statement ENDOFLINE {
          compilerDebugPrintf("parser: c_s-func - Function Defined! %s\n", $1->fullname);
          makeReturn($3); doneFunction($1); }
  | ctor_definition ENDOFLINE codeblock {
          compilerDebugPrintf("parser: c_s-func - Constructor Defined! %s\n", $1->fullname);
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
