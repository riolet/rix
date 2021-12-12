%{
#include <stdio.h>
#include <stdlib.h>
#include "ObjectTree.h"
#include "rixc.h"
#include "errors.h"

#define YYPARSE_PARAM scanner
#define YYLEX_PARAM   scanner
#define YYDEBUG 1

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
	double   fval;
	char*   sval;
	Object* oval;
  ListType* ltval;
  ListString* lsval;
}

// define the constant-string tokens:
%token ENDOFLINE
%token ENDOFFILE 0 "end of file"

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <sval> CHAR
%token <fval> DOUBLE
%token <sval> STRING
%token <sval> IDENT
%token <sval> NEWIDENT
%token <sval> UNMARKEDNEWIDENT
%token <sval> FIELD
%token <sval> VERB
%token <sval> STATICVERB
%token <sval> TYPE
%token <sval> MATH_OP
%token <sval> ASTERISK
%token <sval> ASSIGNMENT
%token <sval> NEWVARASSIGNMENT
%token <sval> LPAREN
%token <sval> RPAREN
%token <sval> LBRACKET
%token <sval> RBRACKET
%token <sval> RBRACKETASSIGN
%token <ival> INDENT
%token <sval> LBRACE
%token <sval> RBRACE
%token <ival> UNINDENT
%token <sval> CONDITIONLINK
%token <sval> UNARYNEGATE
%token <sval> MATHASSIGN
%token <sval> BITWISEOP
%token <sval> BOOLEANOP
%token <sval> UNARYBOOLNEGATE
%token <sval> COLON
%token <sval> CLASSDEC
%token <sval> CLASSDECPRIM
%token <sval> PARAMCOMMA
%token <sval> RETURN
%token <sval> SELFIDENT
%token <sval> SLCOMMENT
%token <sval> COMPARISON
%token <sval> LESSTHAN
%token <sval> GREATERTHAN
%token <ival> TERNARY
%token <sval> CODE_INSERT
%token <sval> ENUM_DEC
%token <sval> IMPORT
%token <sval> ACCESSOR
%token <sval> ACCESSOR_IDENT
%token <sval> BITWISEEXP

%token <sval> RANGE

%token <sval> DTV_EXTERNAL
%token <sval> DTV_ADDSOURCE


%type <oval> rix;
%type <oval> statements;
%type <oval> statement;
%type <oval> simple_statement;
%type <oval> class_statements;
%type <oval> class_statement;
%type <oval> parameters;
%type <oval> codeblock;
%type <oval> funcblock;
%type <oval> classblock;
%type <oval> function_definition;
%type <oval> class_definition;
%type <oval> ctor_definition;
%type <oval> expr;
%type <oval> object;
%type <oval> arguments;
%type <oval> arglist;
%type <oval> verbEndOfLine;
%type <ltval> typeExpression;

%type <sval> parameterIdent;
%type <sval> anyIdentOrVerb;

%type <lsval> enumblock;
%type <lsval> enum_statements;
%type <sval> enum_statement;


%{
void yyerror(YYLTYPE *locp, const char* msg);
%}

//  Precedence for the following tokens.
//  Higher tokens happen last
//  Lower tokens happen first / take priority
//  Multiple tokens means treat as the same precedence
//     and rely on associativity.
//  %right (as opposed to %left) means,
//    given a compound expression,
//    evaluate from right to left.
%right ASSIGNMENT MATHASSIGN NEWVARASSIGNMENT
%left VERB TYPE STATICVERB RBRACKETASSIGN
%left ENDOFLINE INDENT
%left PARAMCOMMA
%left BOOLEANOP
%left COMPARISON LESSTHAN GREATERTHAN TERNARY
%left ASTERISK
%left MATH_OP
%left UNARYNEGATE UNARYBOOLNEGATE
%left RANGE
%left ACCESSOR ACCESSOR_IDENT CONDITIONLINK
%left DESTRUCTOR
%left LBRACKET RBRACKET LBRACE RBRACE 
%left LPAREN RPAREN
//%right class_definition

%%
%start rix;
rix:
	statements   { compilerDebugPrintf("parser: rix-stmts\n"); $$ = $1; }
	| ENDOFFILE  { compilerDebugPrintf("parser: rix-EOF\n"); $$ = 0; handleEOF(); }
	;
statements:
  simple_statement              { compilerDebugPrintf("parser: stmts-s_s\n"); $$ = $1; }
  | statements simple_statement { compilerDebugPrintf("parser: stmts-stmt,s_s\n"); $$ = $1; }
  ;

simple_statement:
  ENDOFLINE             { compilerDebugPrintf("parser: s_s-eol\nempty EOL\n"); $$ = 0; }
  | verbEndOfLine codeblock     { compilerDebugPrintf("parser: VERB ENDOFLINE codeblock\n"); closeBrace();}
  | IMPORT STRING ENDOFLINE{ compilerDebugPrintf("parser: import\n"); $$ = 0; }
  | DTV_EXTERNAL STRING ENDOFLINE { compilerDebugPrintf("parser: dtv\n"); directive($1,$2);  }
  | DTV_ADDSOURCE STRING ENDOFLINE { compilerDebugPrintf("parser: dtv\n"); directive($1,$2);  }
  | statement ENDOFLINE { compilerDebugPrintf("parser: s_s-stmt\nstatement EOL\n"); $$ = $1; }
  | statement ENDOFLINE codeblock { compilerDebugPrintf("parser: s_s-stCB\nstatement EOL\n"); closeBrace(); $$ = $1; }
  | function_definition ASSIGNMENT statement ENDOFLINE {
          compilerDebugPrintf("parser: s_s-func - Funliner Defined! %s\n", $1->fullname);
          makeReturn($3); doneFunction($1); }
  | function_definition ENDOFLINE funcblock {
          compilerDebugPrintf("parser: s_s-func - Function Defined! %s\n", $1->fullname);
          doneFunction($1); }
  | CODE_INSERT ENDOFLINE     { compilerDebugPrintf("parser: code-insert\n"); $$ = injectC($1);  }
  | class_definition ENDOFLINE classblock {
          compilerDebugPrintf("parser: s_s-class - Class Defined! %s\n", $1->fullname);
          doneClass($1); }
  | UNMARKEDNEWIDENT ENUM_DEC ENDOFLINE enumblock {
        compilerDebugPrintf("parser: s_s-class - Enum Defined! %s\n", $1);
        beginEnum($4,$1); }
  | expr              { compilerDebugPrintf("parser: stmt-expr\n"); $$ = completeExpression(finalize($1)); }
  | RETURN expr     { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn($2)); }
  | RETURN          { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn(0)); }
  ;

verbEndOfLine:
  VERB ENDOFLINE  {$$ = completeExpression(finalize(conjugate( 0,  verbIdent($1), 0))); }
  ;
//Todo-Proper handling of codeblocks
//Verb calls with Codeblocks
//  |        VERB     arguments ENDOFLINE codeblock { compilerDebugPrintf("parser: expr- vo-cb\n");   $$ = completeExpression(conjugate( 0,  verbIdent($1), $2)); }
//  |        TYPE     arguments ENDOFLINE codeblock { compilerDebugPrintf("parser: expr-sto-cb\n");   $$ = conjugate( 0,   verbCtor($1,0), $2); }
//  | expr ACCESSOR VERB arguments ENDOFLINE codeblock { compilerDebugPrintf("parser: expr-.vo-cb\n");   $$ = conjugate( $1, verbIdent($3), $4); }
//  | TYPE LBRACE TYPE RBRACE arguments  ENDOFLINE codeblock { compilerDebugPrintf("parser: expr-sto-cb\n");   $$ = conjugate( 0,   verbCtor($1,$3), $5); }
////Verb calls without Codeblocks
//  |        VERB     arguments ENDOFLINE  { compilerDebugPrintf("parser: expr- vo-nocb\n");   $$ = completeExpression(conjugate( 0,  verbIdent($1), $2)); }
//  |        TYPE     arguments ENDOFLINE  { compilerDebugPrintf("parser: expr-sto-noccb\n");   $$ = completeExpression(conjugate( 0,   verbCtor($1,0), $2)); }
//  | expr ACCESSOR VERB arguments ENDOFLINE  { compilerDebugPrintf("parser: expr-.vo-noccb\n");   $$ = completeExpression(conjugate( $1, verbIdent($3), $4)); }
//  | TYPE LBRACE TYPE RBRACE arguments  ENDOFLINE  { compilerDebugPrintf("parser: expr-sto-noccb\n");   $$ = completeExpression(conjugate( 0,   verbCtor($1,$3), $5)); }
  ;

statement:
   expr              { compilerDebugPrintf("parser: stmt-expr\n"); $$ = completeExpression(finalize($1)); }
  | RETURN expr     { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn($2)); }
  | RETURN          { compilerDebugPrintf("parser: stmt-rtEx\n"); $$ = completeExpression(makeReturn(0)); }
  ;

expr:
  object                  { compilerDebugPrintf("parser: expr-obj\n");   $$ = $1; }
  | typeExpression UNMARKEDNEWIDENT { compilerDebugPrintf("parser: c_s:varType\n"); $$ = declareVariable($2, $1); }
  | typeExpression BITWISEEXP UNMARKEDNEWIDENT { compilerDebugPrintf("parser: c_s:varType\n"); $$ = declareGlobalVariable($3, $1); }
  | UNMARKEDNEWIDENT NEWVARASSIGNMENT expr  { compilerDebugPrintf("parser: expr-asn\n");   $$ = conjugateNewVarAssignment($1, verbAssignment($2), $3); }
  | typeExpression arguments  { compilerDebugPrintf("parser: expr-sto\n");   $$ = conjugate( 0,   verbCtor($1), $2); }
  | expr ASSIGNMENT expr  { compilerDebugPrintf("parser: expr-asn\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr MATHASSIGN expr  { compilerDebugPrintf("parser: expr-mas\n");   $$ = conjugate($1, verbAssignment($2), $3); }
  | expr LESSTHAN expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr GREATERTHAN expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr COMPARISON expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr BOOLEANOP  expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1, verbComparison($2), $3); }
  | expr  TERNARY   expr  { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate($1,  verbTernary(), $3); }
  | expr  MATH_OP   expr  { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }
  | expr  ASTERISK   expr  { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }
  | expr  UNARYNEGATE   expr  { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate($1, verbMathOp($2), $3); }
  | UNARYNEGATE expr { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate($2, verbMathOp("*"), objectInt(-1)); }
  | UNARYBOOLNEGATE expr { compilerDebugPrintf("parser: expr-mth\n");   $$ = conjugate(0, verbComparison($1), $2); }
  |        VERB     arguments  { compilerDebugPrintf("parser: expr- vo\n");   $$ = conjugate( 0,  verbIdent($1), $2); }
  | LPAREN expr RPAREN    { compilerDebugPrintf("parser: expr-prn\n");   $$ = parenthesize($2); }
  | expr LBRACKET expr RBRACKETASSIGN expr { compilerDebugPrintf("parser: expr-prn\n");   $$ = conjugate($1,  verbPutObjAtIdx(), concatParams($3,$5)); }
  | expr LBRACKET expr RBRACKET  { compilerDebugPrintf("parser: expr-prn\n");   $$ = conjugate($1,  verbGetObjAtIdx(), $3); }
  | TYPE ACCESSOR_IDENT { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugateAccessorEnum( $1, $2, Field); }
  | expr ACCESSOR_IDENT { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugateAccessorIdent( $1, $2, Field); }
  | expr ACCESSOR_IDENT arguments { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugate($1, conjugateAccessorIdent( $1, $2, Method),$3); }
  | expr RANGE expr { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugate($1, verbRange($2), $3); }
  |      RANGE expr { compilerDebugPrintf("parser: exp-.i\n");   $$ = conjugate(0, verbRange($1), $2); }
  | IDENT DESTRUCTOR      { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate(objectIdent($1),  verbDestructor(), 0); }
  | IDENT CONDITIONLINK   { compilerDebugPrintf("parser: expr-cmp\n");   $$ = conjugate(objectIdent($1),  verbIsDefined($2), 0); }
  ;

typeExpression:
  TYPE { compilerDebugPrintf("Single TYpe Generic\n");   $$ = createListType($1); } //1-ary
  | TYPE LESSTHAN typeExpression GREATERTHAN { compilerDebugPrintf("Generic of Generic\n");   $$ = genericOfGeneric(createListType($1),$3); } //1-ary
  | typeExpression PARAMCOMMA typeExpression { compilerDebugPrintf("Multitype of Generic\n");   $$ = concatGenerics($1,$3); } //1-ary
  ;

arguments:
  /* %empty { compilerDebugPrintf("parser: arg\n");   $$ = 0; } //0-ary */
  LPAREN RPAREN { compilerDebugPrintf("parser: arg-()\n");   $$ = 0; } //0-ary
  | LPAREN arglist RPAREN { compilerDebugPrintf("parser: arg(...)\n");   $$ = $2; } //n-ary
  ;

arglist:
  expr { compilerDebugPrintf("parser: arg-(x)\n");   $$ = $1; } //1-ary
  | expr PARAMCOMMA arglist { compilerDebugPrintf("parser: arg(x,y)\n");   $$ = concatParams($1,$3); } //n-ary
  ;

object:
  INT       { compilerDebugPrintf("parser: object-int\n");       $$ = objectInt($1); }
  | CHAR       { compilerDebugPrintf("parser: object-int\n");       $$ = objectChar($1); }
  | DOUBLE   { compilerDebugPrintf("parser: object-double\n");     $$ = objectdouble($1);}
  | IDENT   { compilerDebugPrintf("parser: object-identifer\n"); $$ = objectIdent($1); }
  | NEWIDENT   { compilerDebugPrintf("parser: object-new-identifer\n"); $$ = objectNewIdent($1); }
  | STRING  { compilerDebugPrintf("parser: object-string\n");    $$ = objectString($1);  }
  | SELFIDENT { compilerDebugPrintf("parser: object-self\n");    $$ = objectSelfIdent($1);}
  /* | CONDITIONLINK { compilerDebugPrintf("parser: object-previous\n"); $$ = objectPrev();   } */
  ;

anyIdentOrVerb:
  IDENT   { compilerDebugPrintf("parser: IDENT\n"); $$ = $1; }
  | NEWIDENT   { compilerDebugPrintf("parser: NEWIDENT\n"); $$ = $1; }
  | UNMARKEDNEWIDENT { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  | VERB { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

function_definition:
  anyIdentOrVerb RETURN typeExpression LPAREN parameters RPAREN { compilerDebugPrintf("parser: func-def\n"); $$ = beginFunction($1, $3, $5); }
  | anyIdentOrVerb RETURN LPAREN parameters RPAREN { compilerDebugPrintf("parser: func-void\n"); $$ = beginFunction($1, createListType("void"), $4); }
  ;

parameters:
  %empty                                { compilerDebugPrintf("parser: param0\n"); $$ = CreateObject(0, 0, 0, Expression, 0); }
  | typeExpression parameterIdent                       { compilerDebugPrintf("parser: param1\n"); $$ = funcParameters( 0, $1, $2); }
  | parameters PARAMCOMMA typeExpression parameterIdent { compilerDebugPrintf("parser: paramN\n"); $$ = funcParameters($1, $3, $4); }
  ;

parameterIdent:
  IDENT                                 { compilerDebugPrintf("parser: IDENT\n"); $$ = $1; }
  |UNMARKEDNEWIDENT                     { compilerDebugPrintf("parser: UNMARKEDNEWIDENT\n"); $$ = $1; }
  ;

funcblock:
  INDENT statements UNINDENT { compilerDebugPrintf("parser: codeblock\n"); $$ = $2; }
  ;

codeblock:
//Todo-Proper handling of codeblocks
//  INDENT statements UNINDENT { compilerDebugPrintf("parser: codeblock\n"); $$ = createCodeBlock($2); }
  INDENT statements UNINDENT { compilerDebugPrintf("parser: codeblock\n"); $$ = $2; }
  ;



class_definition:
  UNMARKEDNEWIDENT CLASSDEC { compilerDebugPrintf("parser: class-def\n"); $$ = beginClass($1, BASETYPE, 0, false); }
  | UNMARKEDNEWIDENT CLASSDEC TYPE { compilerDebugPrintf("parser: class-def\n"); $$ = beginClass($1, $3, 0, false); }
  | UNMARKEDNEWIDENT CLASSDECPRIM { compilerDebugPrintf("parser: class-def\n"); $$ = beginClass($1, BASETYPE, 0, true); }
  | UNMARKEDNEWIDENT CLASSDECPRIM TYPE { compilerDebugPrintf("parser: class-def\n"); $$ = beginClass($1, $3, 0, true); }
  ;

ctor_definition:
  SELFIDENT LPAREN parameters RPAREN { compilerDebugPrintf("parser: class-def\n"); $$ = beginConstructor($3); }
  |SELFIDENT LPAREN ASTERISK RPAREN { compilerDebugPrintf("parser: class-def\n"); $$ = beginConstructorAll(); }
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
  | typeExpression UNMARKEDNEWIDENT ENDOFLINE { compilerDebugPrintf("parser: c_s:varType\n"); $$ = declareVariable($2, $1); }
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

enumblock:
  INDENT enum_statements UNINDENT { compilerDebugPrintf("parser: enumDeclared\n"); $$=$2; }
  ;

enum_statements:
  enum_statement { compilerDebugPrintf("parser: enumLine\n"); $$=concatEnums(0,$1); }
  | enum_statements enum_statement { compilerDebugPrintf("parser: enumLine\n"); $$=concatEnums($1,$2); }
  ;

enum_statement:
  ENDOFLINE { compilerDebugPrintf("parser: c_s-eol\nempty EOL\n"); $$=0; }
  | parameterIdent ENDOFLINE  { compilerDebugPrintf("parser: enumLine\n"); $$=$1; }
  ;

%%

//void yyerror(char const *msg) {
void yyerror(YYLTYPE *locp, const char* msg) {
  char errorMsg[1024];
  snprintf(errorMsg, 256, "In File: %s. %s\n", yyinFileName, msg);
  criticalError(ERROR_ParseError, errorMsg);
	// might as well halt now:
	exit(-1);
}
