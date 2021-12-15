#ifndef RSL_H
#define RSL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct String String;
typedef struct Stream Stream;

typedef struct {
    int object_$_id;
} BaseType;

typedef struct _NonPrimObj NonPrimObj;

struct _NonPrimObj {
    void *obj;
    void (*destructor)(void *);
};

typedef void *UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
//typedef char Char;
typedef enum { lt = -1, eq = 0, gt = 1 } Ternary;
typedef void *System;
typedef void *Generic_$$;

#define bool_$_if(A) if (A) {
#define bool_$_elif_$_bool(A) if (A) {
#define bool_$_else(A) if (A) {

// #define bool_$_tf_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (A) : (B)
// #define bool_$_ft_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (B) : (A)

#define bool_$_tf_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR) (T) ? (A) : (B)
#define bool_$_ft_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR) (T) ? (B) : (A)


#define bool_$_condreturn_$_Generic_$$_$_Generic_$$(T,B) if (T) return (B);

#define if_$_bool(A) if (A) {
#define elif_$_bool(A) else if (A) {
#define else_$_() else {

#define bool_$_while_$_(A) while (A) {
#define while_$_bool(A) while (A) {

#define for_$_int_$_int(i,a,b) for (i=a;i<b;i++) {
#define for_$_int_$_int_$_int(i,a,b,c) for (i=a;i<b;i+=c) {
#define for_$_Generic_$$_$_Generic_$$_$_Generic_$$(i,a,b,c) for (i=a;i<b;i+=c) {
    
#define downfor_$_int_$_int(i,a,b) for (i=a;i>=b;i--) {
#define downfor_$_int_$_int_$_int(i,a,b,c) for (i=a;i>=b;i-=c) {


#define false_$_() false
#define true_$_() true

#define Ternary_$_leg_$_Generic_$$_$_Generic_$$_$_Generic_$$(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (A) : ((T_==eq) ? (B) : (C)); })
#define Ternary_$_gel_$_Generic_$$_$_Generic_$$_$_Generic_$$(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (C) : ((T_==eq) ? (B) : (A)); })


//#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (A) : ((T==eq) ? (B) : (C))
//#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (C) : ((T==eq) ? (B) : (A))

int int_$_int_$_String(String s);


double double_$_exponent_$_int(double f, int i);

int int_$_exponent_$_int(int i1, int i2);

double int_$_exponent_$_double(int i, double f);

double double_$_exponent_$_double(double f1, double f2);

int int_$_sqrt_$_(int i);

int print_$_String(String s);

int print_$_Char(char c);


int print_$_int(int i);

int print_$_double(double f);

int echo_$_String(String s);

int echo_$_int(int i);

int echo_double(double f);

Ternary int_$_compare_$_int(int a, int b);

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c);

int _$$_argc;
char **_$$_argv;

String args_$_int(int i);

int args_$_();

void BaseType_$_destructor (NonPrimObj   $_mptr_in);

NonPrimObj   BaseType_$_BaseType_$_ (NonPrimObj   $_mptr_in);

void _$_cleanup (NonPrimObj  p);

void _$_cleanup_var (NonPrimObj  *p);

void _$_object_ownership_transfer(NonPrimObj   a, NonPrimObj   b);

NonPrimObj   NonPrimObj_point (NonPrimObj   a, NonPrimObj  b);

NonPrimObj   NonPrimObj_assign_with_alloc (NonPrimObj  *a_ptr, NonPrimObj  b,char *debugName);

NonPrimObj   NonPrimObj_assign (NonPrimObj   a, NonPrimObj  b);

NonPrimObj   _$_returnAppointer (NonPrimObj   $_mptr_in, void * obj, void * destructor);

void NonPrimObj_initialize(NonPrimObj  mptr, char *name);

char *snprintfauto(const char *format, ...);
void _$_cleanup_object(NonPrimObj  p);

#define import_$_String(x) true

#endif