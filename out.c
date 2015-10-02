#include "rsl.h"
#include "out.h"
int main(void) {
//Type inferencing
	String hello;
	hello = String_stringlit("Hello"); 
	String world;
	world = String_stringlit("World"); 
//Way better string handling
	String helloWorld;
	helloWorld = (String_plus_String(hello,(String_plus_String(String_stringlit(" "),world)))); 
//Semicolons are optional
	Stream_print_String(stdout,helloWorld); 
	int n;
	n = 5; 
//pow is now ^
	float f2;
	float f1;
	float f;
	f = (f1 = (f2 = (Float_exponent_Integer(5.5,2)))); 
//echo does not print newline
	Stream_echo_String(stdout,(String_plus_Float(String_stringlit("f2 is and "),f2))); 
	float f3;
	f3 = (Integer_exponent_Integer((((2 * 3) + 1) * 4),3)); 
//print does
	Stream_print_String(stdout,(String_plus_Float(String_stringlit(" and f3 is "),f3))); 
//No curly braces (if is not actually a keyword)
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(1 > (n / f))){
//End code block with .
		Stream_print_String(stdout,(String_plus_Float(String_stringlit("1 is more than "),(n / f))));}
//Nicer for loops, (for is not a keyword)
	int i;
	Identifier_for_Integer_Integer(i,1,(n)){
		Stream_print_Integer(stdout,i);}
//You can embed C code 
printf ("This is in C :) \n");
	return 0;
}
