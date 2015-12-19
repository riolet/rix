#ifndef RSL_H
#define RSL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rsl/RSL_String.h"

typedef struct {
    int object_$_id;
} BaseType;

typedef struct {
    BaseType parent;
} Number;

typedef FILE *Stream;
typedef void *UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef int Integer;
typedef float Float;
typedef enum { false, true } Boolean;
typedef enum { lt = -1, eq = 0, gt = 1 } Ternary;
typedef void *System;
typedef void *Generic_$$;

#define Boolean_$_if(A) if (A) {
#define Boolean_$_elif_$_Boolean(A) if (A) {
#define Boolean_$_else(A) if (A) {

#define Boolean_$_tf_$__$GP0_$__$GP1(T,A,B) (T) ? (A) : (B);
#define Boolean_$_ft_$__$GP0_$__$GP1(T,A,B) (T) ? (B) : (A);

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

#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C) (T==lt) ? (A) : ((T==eq) ? (B) : (C))
#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C) (T==lt) ? (C) : ((T==eq) ? (B) : (A))

int Integer_$_Integer_$_String(String s)
{
    return atoi(s.buffer);
}

float Float_$_exponent_$_Integer(float f, int i)
{
    return pow(f, i);
}

int Integer_$_exponent_$_Integer(int i1, int i2)
{
    return (int)pow(i1, i2);
}

float Integer_$_exponent_$_Float(int i, float f)
{
    return pow(i, f);
}

float Float_$_exponent_$_Float(float f1, float f2)
{
    return pow(f1, f2);
}

int Integer_$_sqrt_$_(int i)
{
    return (int)sqrt(i);
}

/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream stream, String s)
{
    int result = fwrite(s.buffer, sizeof(char), s.length, stream) + fputc('\n', stream);
    String_cleanUp(&s);
    return result;
}

int Stream_$_print_$_Integer(Stream stream, int i)
{
    return fprintf(stream, "%d\n", i);
}

int Stream_$_print_$_Float(Stream stream, float f)
{
    return fprintf(stream, "%f\n", f);
}

int print_$_String(String s)
{
    return Stream_$_print_$_String(stdout, s);
}

int print_$_Integer(int i)
{
    return fprintf(stdout, "%d\n", i);
}

int print_$_Float(float f)
{
    return fprintf(stdout, "%f\n", f);
}

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream stream, String s)
{
    return fwrite(s.buffer, sizeof(char), s.length, stream);
}

int Stream_$_echo_$_Integer(Stream stream, int i)
{
    return fprintf(stream, "%d", i);
}

int Stream_$_echo_$_Float(Stream stream, float f)
{
    return fprintf(stream, "%f", f);
}

int echo_$_String(String s)
{
    return fwrite(s.buffer, sizeof(char), s.length, stdout);
}

int echo_$_Integer(int i)
{
    return fprintf(stdout, "%d", i);
}

int echo_Float(float f)
{
    return fprintf(stdout, "%f", f);
}

Ternary Integer_$_compare_$_Integer(Integer a, Integer b)
{
    if (a > b) {
        return lt;
    } else if (a == b) {
        return eq;
    } else {
        return gt;
    }
}

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c)
{
    if (ternary == lt) {
        return a;
    } else if (ternary == eq) {
        return b;
    } else if (ternary == gt) {
        return c;
    }
}

int _$$_argc;
char **_$$_argv;

String args_$_Integer(Integer i)
{
    return String_$_stringlit(_$$_argv[i]);
}

#endif
