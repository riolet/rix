%{
/* need this for the call to atof() below */
#include <math.h>
%}

alphabetic    [a-zA-Z]
numeral       [0-9]
alphanumeric  {alphabetic}|{numeral}
integer       {numeral}+
float         {numeral}+"."{numeral}*
identifier    {alphabetic}{alphanumeric}*

mathOp        "+"|"-"|"*"|"/"|"^^"
assignment    "="

%%

{integer}     printf( "An integer: %s (%d)\n", yytext, atoi( yytext ) );

{float}       printf( "A float: %s (%g)\n", yytext, atof( yytext ) );

{mathOp}      printf( "A math operator: %s\n", yytext );

{identifier}  printf( "An identifier: %s\n", yytext );

{assignment}  printf( "An assignment operator: %s\n", yytext );

[ \t\n]+      /* eat up whitespace */

.             printf( "Unrecognized character: %s\n", yytext );

%%

main( int argc, char ** argv )
    {
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    yylex();
    }
