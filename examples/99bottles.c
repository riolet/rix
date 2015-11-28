#include "rsl.h"
#include "99bottles.h"
int main(void) {
	int b;
	String ob;
	String line;
	line = (String_plus_String((UNKNOWNTYPE_bottles_Integer(UNKNOWNOBJECT,(b = 99))),(ob = String_stringlit(" of beer")))); 
	UNKNOWNTYPE_while_Boolean(UNKNOWNOBJECT,(b > 0)){
		String w;
		Stream_print_String(stdout,(String_plus_String(line,(String_plus_String(ob,(String_plus_String((w = String_stringlit(" on the wall\n")),(String_plus_String(line,(String_plus_String(ob,String_stringlit("\nTake one down and pass it around")))))))))))); 
						Stream_print_String(stdout,(String_plus_String((line = (UNKNOWNTYPE_bottles_Integer(UNKNOWNOBJECT,(b = (b - 1))))),(String_plus_String(ob,w))))); 
}
	return 0;
}
