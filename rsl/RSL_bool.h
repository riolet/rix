#define bool_$_if(A) if (A) {
#define bool_$_elif_$_bool(A) if (A) {
#define bool_$_else(A) if (A) {

// #define bool_$_tf_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (A) : (B)
// #define bool_$_ft_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (B) : (A)

#define bool_$_tf_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim) (T) ? (A) : (B)
#define bool_$_ft_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim) (T) ? (B) : (A)


#define bool_$_condreturn_$_Generic_$$_$_Generic_$$(T,B) if (T) return (B);

#define if_$_bool(A) if (A) {
#define elif_$_bool(A) else if (A) {
#define else_$_() else {

#define bool_$_while_$_(A) while (A) {