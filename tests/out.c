#include "rsl.h"
#include "out.h"
int main(void) {
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(1 > 2)){
		Stream_print_String(stdout,String_stringlit("1 > 2 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("1 > 2 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(2 > 1)){
		Stream_print_String(stdout,String_stringlit("2 > 1 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("2 > 1 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(1 < 2)){
		Stream_print_String(stdout,String_stringlit("1 < 2 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("1 < 2 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(2 < 1)){
		Stream_print_String(stdout,String_stringlit("2 < 1 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("2 < 1 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(3 == 2)){
		Stream_print_String(stdout,String_stringlit("3 == 2 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("3 == 2 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(3 == 3)){
		Stream_print_String(stdout,String_stringlit("3 == 3 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("3 == 3 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(1.0 > 2.0)){
		Stream_print_String(stdout,String_stringlit("1.0 > 2.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("1.0 > 2.0 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(2.0 > 1.0)){
		Stream_print_String(stdout,String_stringlit("2.0 > 1.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("2.0 > 1.0 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(1.0 < 2.0)){
		Stream_print_String(stdout,String_stringlit("1.0 < 2.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("1.0 < 2.0 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(2.0 < 1.0)){
		Stream_print_String(stdout,String_stringlit("2.0 < 1.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("2.0 < 1.0 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(3.0 == 2.0)){
		Stream_print_String(stdout,String_stringlit("3.0 == 2.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("3.0 == 2.0 = False")); 
	}
	UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(3.0 == 3.0)){
		Stream_print_String(stdout,String_stringlit("3.0 == 3.0 = True")); 
	}
	UNKNOWNTYPE_else(UNKNOWNOBJECT){
		Stream_print_String(stdout,String_stringlit("3.0 == 3.0 = False")); 
	}
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1<>2 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,2)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1<>1 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,1)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1<>0 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,0)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1.0<>2.0 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,2)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1.0<>1.0 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,1)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	Stream_print_String(stdout,(String_plus_String(String_stringlit("1.0<>0.0 = "),(Ternary_pick_String_String_String((Integer_compare_Integer(1,0)),String_stringlit("Greater"),(String_stringlit("Lesser")),(String_stringlit("Equal"))))))); 
	return 0;
}
