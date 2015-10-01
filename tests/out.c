#include "rsl.h"
#include "out.h"
int main(void) {
	String a;
	a = String_stringlit("hello"); 
//TODO: not sure if this is supposed to work or not. Right now it doesn't
//String b = " "
	String b;
	b = String_stringlit(" "); 
	String c;
	c = String_stringlit("world"); 
	String d;
	d = (String_plus_String(a,c)); 
	String e;
	e = b; 
	Stream_print_String(stdout,(String_plus_String(a,(String_plus_String(b,c))))); 
	Stream_print_String(stdout,(String_plus_String(d,(String_plus_String(String_stringlit("|"),(String_plus_String(e,String_stringlit("|")))))))); 
	return 0;
}
