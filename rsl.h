#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFLEN 512
typedef struct {
    char *buffer;
    size_t length;
    size_t cap;
} String;

typedef struct {
  int object_$_id;
} BaseType;

typedef struct {
  BaseType parent;
} Number;

typedef FILE * Stream;
typedef void * UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef int Integer;
typedef float Float;
typedef enum { false, true } Boolean;
typedef enum { lt=-1, eq=0, gt=1 } Ternary;
typedef void * System;

#define Boolean_$_if(A) if (A) {
#define Boolean_$_elif_$_Boolean(A) if (A) {
#define Boolean_$_else(A) if (A) {

#define if_$_Boolean(A) if (A) {
#define elif_$_Boolean(A) else if (A) {
#define else_$_() else {

#define Boolean_$_while_$_(A) while (A) {
#define while_$_Boolean(A) while (A) {

#define for_$_Integer_$_Integer(i,a,b) for (i=a;i<b;i++) {
#define for_$_Integer_$_Integer_$_Integer(i,a,b,c) for (i=a;i<b;i+=c) {

#define false_$_() false
#define true_$_() true
#define Boolean_$_condreturn_$__$GP0(A,B) (A) ? return B;
#define Ternary_$_leg_$__$GP0_$__$GP1_$__$GP2(T,A,B,C) (T==lt) ? A : ((T==eq) ? B : C)
#define Ternary_$_gel_$__$GP0_$__$GP1_$__$GP2(T,A,B,C) (T==lt) ? C : ((T==eq) ? B : A)


int Integer_$_Integer_$_String (String s) {
    return atoi(s.buffer);
}

float Float_$_exponent_$_Integer (float f, int i) {
    return pow(f,i);
}

int Integer_$_exponent_$_Integer (int i1, int i2) {
    return (int)pow(i1,i2);
}

float Integer_$_exponent_$_Float (int i, float f) {
    return pow(i,f);
}

float Float_$_exponent_$_Float (float f1, float f2) {
    return pow(f1,f2);
}

int Integer_$_sqrt_$_ (int i) {
    return (int) sqrt(i);
}

String String_$_stringlit ( char * strlit) {
    String s;
    s.buffer = strlit;
    s.cap = s.length = strlen(strlit);
    return s;
}
String String_$_assign_$_String (String left, String right) {
    left.buffer=malloc(right.length+1);
    memcpy(left.buffer,right.buffer,right.length);
    return left;
}

String String_$_plus_$_String (String left, String right) {
    String newString;
    newString.buffer=malloc(left.length+right.length+1);
    memcpy(newString.buffer,left.buffer,left.length);
    memcpy(newString.buffer+left.length,right.buffer,right.length);
    newString.length=left.length+right.length;
    newString.buffer[newString.length]=0;
    return newString;
}

String String_$_plus_$_Integer (String left, int right) {
    String newString;
    char rightStr[BUFFLEN];
    int right_length=snprintf(rightStr,BUFFLEN,"%i",right);
    newString.buffer=malloc(left.length+right_length+1);
    memcpy(newString.buffer,left.buffer,left.length);
    memcpy(newString.buffer+left.length,rightStr,right_length);
    newString.length=left.length+right_length;
    newString.buffer[newString.length]=0;
    return newString;
}

String Integer_$_plus_$_String (int left, String right) {
    String newString;
    char leftStr[BUFFLEN];
    int left_length=snprintf(leftStr,BUFFLEN,"%i",left);
    newString.buffer=malloc(right.length+left_length+1);
    memcpy(newString.buffer,leftStr,left_length);
    memcpy(newString.buffer+left_length,right.buffer,right.length);
    newString.length=right.length+left_length;
    newString.buffer[newString.length]=0;
    return newString;
}

String String_$_plus_$_Float (String left, float right) {
    String newString;
    char rightStr[BUFFLEN];
    int right_length=snprintf(rightStr,BUFFLEN,"%f",right);
    newString.buffer=malloc(left.length+right_length+1);
    memcpy(newString.buffer,left.buffer,left.length);
    memcpy(newString.buffer+left.length,rightStr,right_length);
    newString.length=left.length+right_length;
    newString.buffer[newString.length]=0;
    return newString;
}

String Float_$_plus_$_String (float left, String right) {
    String newString;
    char leftStr[BUFFLEN];
    int left_length=snprintf(leftStr,BUFFLEN,"%f",left);
    newString.buffer=malloc(right.length+left_length+1);
    memcpy(newString.buffer+left_length,right.buffer,right.length);
    memcpy(newString.buffer,leftStr,left_length);

    newString.length=right.length+left_length;
    newString.buffer[newString.length]=0;
    return newString;
}



/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream)+fputc('\n',stream);
}

int Stream_$_print_$_Integer(Stream stream, int i) {
    return fprintf(stream,"%d\n",i);
}

int Stream_$_print_$_Float(Stream stream, float f) {
    return fprintf(stream,"%f\n",f);
}

int print_$_String(String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stdout)+fputc('\n',stdout);
}

int print_$_Integer(int i) {
    return fprintf(stdout,"%d\n",i);
}

int print_$_Float(float f) {
    return fprintf(stdout,"%f\n",f);
}

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream);
}

int Stream_$_echo_$_Integer(Stream stream, int i) {
    return fprintf(stream,"%d",i);
}

int Stream_$_echo_$_Float(Stream stream, float f) {
    return fprintf(stream,"%f",f);
}

int echo_$_String(String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stdout);
}

int echo_$_Integer(int i) {
    return fprintf(stdout,"%d",i);
}

int echo_Float(float f) {
    return fprintf(stdout,"%f",f);
}

Ternary Integer_$_compare_$_Integer (Integer a, Integer b) {
    if (a>b) {
        return lt;
    } else if (a==b) {
        return eq;
    } else {
        return gt;
    }
}
String Ternary_$_pick_$_String_$_String_$_String (Ternary ternary, String a, String b, String c) {
    if (ternary==lt) {
        return a;
    } else if (ternary==eq) {
        return b;
    } else if (ternary==gt) {
        return c;
    }
}

int _$$_argc;
char **_$$_argv;

 String args_$_Integer (Integer i) {
    return String_$_stringlit(_$$_argv[i]);
}
