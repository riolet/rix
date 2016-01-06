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

typedef FILE *Stream;
typedef void *UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef char Char;
typedef enum { lt = -1, eq = 0, gt = 1 } Ternary;
typedef void *System;
typedef void *Generic_$$;

#define bool_$_if(A) if (A) {
#define bool_$_elif_$_bool(A) if (A) {
#define bool_$_else(A) if (A) {

#define bool_$_tf_$__$GP0_$__$GP1(T,A,B,MPTR) (T) ? (A) : (B)
#define bool_$_ft_$__$GP0_$__$GP1(T,A,B,MPTR) (T) ? (B) : (A)

#define bool_$_condreturn_$__$GP0(T,B) if (T) return (B);

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

#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (A) : ((T_==eq) ? (B) : (C)); })
#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) ({ __typeof__(T) T_=(T); (T_==lt) ? (C) : ((T_==eq) ? (B) : (A)); })


//#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (A) : ((T==eq) ? (B) : (C))
//#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C,MPTR) (T==lt) ? (C) : ((T==eq) ? (B) : (A))

int int_$_int_$_String(IDENT_MPTR_RAW * s_);


float float_$_exponent_$_int(float f, int i);

int int_$_exponent_$_int(int i1, int i2);

float int_$_exponent_$_float(int i, float f);

float float_$_exponent_$_float(float f1, float f2);

int int_$_sqrt_$_(int i);

/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream stream, IDENT_MPTR_RAW * s_);

int Stream_$_print_$_int(Stream stream, int i);

int Stream_$_print_$_float(Stream stream, float f);

int print_$_String(IDENT_MPTR_RAW * s);

int print_$_Char(char c);


int print_$_int(int i);

int print_$_float(float f);

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream stream, String * s);

int Stream_$_echo_$_int(Stream stream, int i);

int Stream_$_echo_$_float(Stream stream, float f);

int echo_$_String(String * s);

int echo_$_int(int i);

int echo_float(float f);

Ternary int_$_compare_$_int(int a, int b);

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c);

int _$$_argc;
char **_$$_argv;

IDENT_MPTR_RAW * args_$_int(int i, IDENT_MPTR_RAW * $_mptr_in);



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

void Array_$_destructor_$_ (IDENT_MPTR_RAW * a);


enum { WORD_SIZE = sizeof(uintmax_t) * sizeof(char) };

#define WORD_OFFSET(b) ((b) / WORD_SIZE)
#define BIT_OFFSET(b)  ((b) % WORD_SIZE)

#define setBit(w, i) ({w[WORD_OFFSET(i)] |= (1 << BIT_OFFSET(i));})
#define clearBit(w, i) ({w[WORD_OFFSET(i)] &= ~(1 << BIT_OFFSET(i));})
#define testBit(w,i) ({(w[WORD_OFFSET(i)] & (1 << BIT_OFFSET(i)))!=0;})

typedef struct {
    size_t size;
    size_t last;
    uintmax_t *bitArray;
    void  *data;
} StructArray;

//IDENT_MPTR_RAW * Array_$_Array_$_int(size_t size__, IDENT_MPTR_RAW * mptr, bool primYtpe, char *type);
//
//IDENT_MPTR_RAW * Array_$_putObjectAtIndex_$__$GP0_$__$GP1(IDENT_MPTR_RAW * arr, IDENT_MPTR_RAW * idx_, bool primIdx, IDENT_MPTR_RAW * elem, bool primElem, IDENT_MPTR_RAW * mptr);
//
//IDENT_MPTR_RAW * Array_$_getObjectAtIndex_$_int(IDENT_MPTR_RAW * arr, int idx, IDENT_MPTR_RAW * mptr);

void Array_$_destructor_$_ (IDENT_MPTR_RAW * a);

#define Array_$_Array_$_int(_$v$_size__, _$v$_mptr, _$v$_primYtpe, _$v$_type) ({\
    StructArray  * _$v$_arr = calloc(1,sizeof(StructArray));\
    _$v$_arr->size=_$v$_size__;\
    _$v$_arr->last=0;\
    _$v$_arr->bitArray=calloc((_$v$_size__/WORD_SIZE)+1,sizeof(uintmax_t));\
    if (!_$v$_primYtpe) {\
        _$v$_arr->data = (void *) calloc(_$v$_size__, sizeof(IDENT_MPTR_RAW));\
    } else {\
        _$v$_arr->data = (void *) calloc(_$v$_size__, sizeof(_$v$_type));\
    }\
    _$_returnAppointer(_$v$_mptr,_$v$_arr,Array_$_destructor_$_);\
    _$v$_mptr;\
})


#define Array_$_putObjectAtIndex_$__$GP0_$__$GP1(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet) ({\
    StructArray * _$v$_arrptr = (StructArray *) _$v$_arr->obj;\
    if (_$v$_idx>_$v$_arrptr->last) {\
        _$v$_arrptr->last=_$v$_idx;\
    }\
    setBit(_$v$_arrptr->bitArray,_$v$_idx);\
    if (!_$v$_primElem) {\
        IDENT_MPTR_RAW * _$v$_dataptr = (IDENT_MPTR_RAW *) _$v$_arrptr->data;\
        IDENT_MPTR_RAW_assign(&_$v$_dataptr[_$v$_idx], _$v$_elem);\
    } else {\
        /*--Todo-- This must get the right type */ int  * _$v$_dataptr = _$v$_arrptr->data;\
        _$v$_dataptr[_$v$_idx] = _$v$_elem;\
    }\
    _$v$_mptr;\
})

#define _$_return_data1(_$v$_arr__,_$v$_idx__) _$v$_arr__[_$v$_idx__]
#define _$_return_data0(_$v$_arr__,_$v$_idx__) &_$v$_arr__[_$v$_idx__]

#define Array_$_getObjectAtIndex_$_int(arr, idx, mptr, primRet, typeRet) ({ \
    StructArray * arrptr = (StructArray *) arr->obj;                        \
    typeRet *dataptr = (typeRet *) arrptr->data; \
    xcat(_$_return_data,primRet)(dataptr,idx);                              \
})
#endif
