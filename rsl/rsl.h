#ifndef RSL_H
#define RSL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "ObjectTree.h"



typedef struct String String;
typedef struct Stream Stream;

typedef struct {
    int object_$_id;
} BaseType;

typedef struct {
    BaseType parent;
} Number;

typedef struct IDENT_MPTR_RAW IDENT_MPTR_RAW;


struct IDENT_MPTR_RAW {
    int ctr;
    IDENT_MPTR_RAW *ptr;
    void *obj;
    void (*destructor)(void *);
    char *debugName;
};

typedef IDENT_MPTR_RAW * IDENT_MPTR_RAW_PTR;

typedef struct  {
    char *Type;
    char *data;
    size_t length;
    size_t cap;
} List;

typedef void *UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef char Char;
typedef enum { lt = -1, eq = 0, gt = 1 } Ternary;
typedef void *System;
typedef void *Generic_$$;

#define bool_$_if(A) if (A) {
#define bool_$_elif_$_bool(A) if (A) {
#define bool_$_else(A) if (A) {

#define bool_$_tf_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (A) : (B)
#define bool_$_ft_$_Generic_$$_$_Generic_$$(T,A,isAPrim, B,isBPrim,MPTR,isReturnPrim,returnType) (T) ? (B) : (A)

#define bool_$_condreturn_$_Generic_$$_$_Generic_$$(T,B) if (T) return (B);

#define if_$_bool(A) if (A) {
#define elif_$_bool(A) else if (A) {
#define else_$_() else {

#define bool_$_while_$_(A) while (A) {
#define while_$_bool(A) while (A) {

#define for_$_int_$_int(i,a,b) for (i=a;i<b;i++) {
#define for_$_int_$_int_$_int(i,a,b,c) for (i=a;i<b;i+=c) {

#define downfor_$_int_$_int(i,a,b) for (i=a;i>=b;i--) {
#define downfor_$_int_$_int_$_int(i,a,b,c) for (i=a;i>=b;i-=c) {


#define false_$_() false
#define true_$_() true

#define Ternary_$_leg_$_Generic_$$_$_Generic_$$_$_Generic_$$(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (A) : ((T_==eq) ? (B) : (C)); })
#define Ternary_$_gel_$_Generic_$$_$_Generic_$$_$_Generic_$$(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (C) : ((T_==eq) ? (B) : (A)); })


//#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (A) : ((T==eq) ? (B) : (C))
//#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (C) : ((T==eq) ? (B) : (A))

int int_$_int_$_String(IDENT_MPTR_RAW * s_);


float float_$_exponent_$_int(float f, int i);

int int_$_exponent_$_int(int i1, int i2);

float int_$_exponent_$_float(int i, float f);

float float_$_exponent_$_float(float f1, float f2);

int int_$_sqrt_$_(int i);

int print_$_String(IDENT_MPTR_RAW * s);

int print_$_Char(char c);


int print_$_int(int i);

int print_$_float(float f);

int echo_$_String(IDENT_MPTR_RAW * s);

int echo_$_int(int i);

int echo_float(float f);

Ternary int_$_compare_$_int(int a, int b);

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c);

int _$$_argc;
char **_$$_argv;

IDENT_MPTR_RAW * args_$_int(int i, IDENT_MPTR_RAW * $_mptr_in);

int args_$_();

void BaseType_$_destructor (IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * BaseType_$_BaseType_$_ (IDENT_MPTR_RAW * $_mptr_in);

void _$_cleanup (IDENT_MPTR_RAW *p);

void _$_cleanup_var (IDENT_MPTR_RAW **p);

void _$_mptr_prepare(IDENT_MPTR_RAW * a, IDENT_MPTR_RAW * b);

IDENT_MPTR_RAW * IDENT_MPTR_RAW_point (IDENT_MPTR_RAW * a, IDENT_MPTR_RAW *b);

IDENT_MPTR_RAW * IDENT_MPTR_RAW_assign (IDENT_MPTR_RAW * a, IDENT_MPTR_RAW *b);

IDENT_MPTR_RAW * _$_returnAppointer (IDENT_MPTR_RAW * $_mptr_in, void * obj, void * destructor);

#endif