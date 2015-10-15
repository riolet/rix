Integer UNKNOWNTYPE_factorial_Integer(UNKNOWNTYPE self, Integer n)
{
		int result;
		result = 1; 
		int i;
		Identifier_for_Integer_Integer(i,1,(n + 1)){
						result = (result * i); 
		}
return (result);	}
Integer UNKNOWNTYPE_sum_Integer(UNKNOWNTYPE self, Integer n)
{
		UNKNOWNTYPE_if_Boolean(UNKNOWNOBJECT,(n == 0)){
return (n);		}
return ((n + (UNKNOWNTYPE_sum_Integer(UNKNOWNOBJECT,(n - 1)))));	}
