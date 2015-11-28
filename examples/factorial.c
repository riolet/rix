#include "rsl.h"
#include "factorial.h"
int main(void) {
	int n;
	n = 5; 
	int result;
	result = 1; 
	int i;
	Identifier_for_Integer_Integer(i,1,(n + 1)){
			result = (result * i);}
	Stream_print_Integer(stdout,result); 
	return 0;
}
