#include "rsl.h"
#include "example.h"
int main(void) {
	String hello;
	hello = String_stringlit("Hello"); 
	String world;
	world = String_stringlit("World"); 
	String helloWorld;
	helloWorld = (String_plus_String(hello,(String_plus_String(String_stringlit(" "),world)))); 
	Stream_print_String(stdout,helloWorld); 
	int num;
	num = 5; 
	float f2;
	float f1;
	float f;
	f = (f1 = (f2 = (Float_exponent_Integer(5.5,2)))); 
	Stream_echo_String(stdout,(String_plus_Float(String_stringlit("f2 is and "),f2))); 
	int f3;
	f3 = (Integer_exponent_Integer((((2 * 3) + 1) * 4),3)); 
	Stream_print_String(stdout,(String_plus_Integer(String_stringlit(" and f3 is "),f3))); 
	Stream_print_String(stdout,(String_plus_String((String_plus_Integer(String_stringlit("Factorial of "),num)),(String_plus_Integer(String_stringlit(" is "),(UNKNOWNTYPE_factorial_Integer(UNKNOWNOBJECT,num))))))); 
	Stream_print_String(stdout,(String_plus_String((String_plus_Integer(String_stringlit("Sum of first "),num)),(String_plus_Integer(String_stringlit(" integers is "),(UNKNOWNTYPE_sum_Integer(UNKNOWNOBJECT,num))))))); 
	return 0;
}
