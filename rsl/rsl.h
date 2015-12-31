#ifndef RSL_H
#define RSL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "ObjectTree.h"



typedef struct String{
    char *buffer;
    size_t length;
    size_t cap;
    bool isStaticBuffer;
} String;

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

typedef struct  {
    char *Type;
    char *data;
    size_t length;
    size_t cap;
} List;

typedef FILE *Stream;
typedef void *UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef int Integer;
typedef float Float;
typedef char Char;
typedef bool Boolean;
typedef enum { lt = -1, eq = 0, gt = 1 } Ternary;
typedef void *System;
typedef void *Generic_$$;

#define Boolean_$_if(A) if (A) {
#define Boolean_$_elif_$_Boolean(A) if (A) {
#define Boolean_$_else(A) if (A) {

#define Boolean_$_tf_$__$GP0_$__$GP1(T,A,B,MPTR) (T) ? (A) : (B)
#define Boolean_$_ft_$__$GP0_$__$GP1(T,A,B,MPTR) (T) ? (B) : (A)

#define Boolean_$_condreturn_$__$GP0(T,B) if (T) return (B);

#define if_$_Boolean(A) if (A) {
#define elif_$_Boolean(A) else if (A) {
#define else_$_() else {

#define Boolean_$_while_$_(A) while (A) {
#define while_$_Boolean(A) while (A) {

#define for_$_Integer_$_Integer(i,a,b) for (i=a;i<b;i++) {
#define for_$_Integer_$_Integer_$_Integer(i,a,b,c) for (i=a;i<b;i+=c) {

#define false_$_() false
#define true_$_() true

//#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (A) : ((T_==eq) ? (B) : (C)); })
//#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (C) : ((T_==eq) ? (B) : (A)); })

#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (A) : ((T==eq) ? (B) : (C))
#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (C) : ((T==eq) ? (B) : (A))

int Integer_$_Integer_$_String(String * s);


float Float_$_exponent_$_Integer(float f, int i);

int Integer_$_exponent_$_Integer(int i1, int i2);

float Integer_$_exponent_$_Float(int i, float f);

float Float_$_exponent_$_Float(float f1, float f2);

int Integer_$_sqrt_$_(int i);

/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream stream, IDENT_MPTR_RAW * s_);

int Stream_$_print_$_Integer(Stream stream, int i);

int Stream_$_print_$_Float(Stream stream, float f);

int print_$_String(IDENT_MPTR_RAW * s);

int print_$_Char(char c);


int print_$_Integer(int i);

int print_$_Float(float f);

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream stream, String * s);

int Stream_$_echo_$_Integer(Stream stream, int i);

int Stream_$_echo_$_Float(Stream stream, float f);

int echo_$_String(String * s);

int echo_$_Integer(int i);

int echo_Float(float f);

Ternary Integer_$_compare_$_Integer(Integer a, Integer b);

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c);

int _$$_argc;
char **_$$_argv;

IDENT_MPTR_RAW * args_$_Integer(Integer i, IDENT_MPTR_RAW $_mptr_in);



void BaseType_$_destructor (IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * BaseType_$_BaseType_$_ (IDENT_MPTR_RAW * $_mptr_in);

void _$_cleanup (IDENT_MPTR_RAW *p);

void _$_cleanup_var (IDENT_MPTR_RAW **p);

void _$_mptr_prepare(IDENT_MPTR_RAW * a, IDENT_MPTR_RAW * b);

IDENT_MPTR_RAW * IDENT_MPTR_RAW_point (IDENT_MPTR_RAW * a, IDENT_MPTR_RAW *b);

IDENT_MPTR_RAW * IDENT_MPTR_RAW_assign (IDENT_MPTR_RAW * a, IDENT_MPTR_RAW *b);

IDENT_MPTR_RAW * _$_returnAppointer (IDENT_MPTR_RAW * $_mptr_in, void * obj, void * destructor);

//List Functions
IDENT_MPTR_RAW * List_$_List_$__$GP0 (IDENT_MPTR_RAW * a, IDENT_MPTR_RAW * $_mptr_in);
#endif
