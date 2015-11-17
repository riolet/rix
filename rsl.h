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

typedef void BaseType;
typedef FILE * Stream;
typedef void * UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef int Integer;
typedef float Float;
typedef enum { false, true } Boolean;
typedef enum { lt=-1, eq=0, gt=1 } Ternary;

#define if_Boolean(A) A; if (_$_prev_$_Boolean) {
#define elif_Boolean_Boolean(A, B) (!A && B); if (_$_prev_$_Boolean) {
#define else_Boolean(A) !A; if (_$_prev_$_Boolean) {
#define while_Boolean(A) while (A) {
#define for_Integer_Integer(i,a,b) for (i=a;i<b;i++) {

float exponent_Float_Integer (float f, int i) {
    return pow(f,i);
}

int exponent_Integer_Integer (int f, int i) {
    return (int)pow(f,i);
}

float exponent_Integer_Float (int f, float i) {
    return pow(f,i);
}

float exponent_Float_Float (float f, float i) {
    return pow(f,i);
}

String String_stringlit ( char * strlit) {
    String s;
    s.buffer = strlit;
    s.cap = s.length = strlen(strlit);
    return s;
}
String assign_String_String (String left, String right) {
    left.buffer=malloc(right.length+1);
    memcpy(left.buffer,right.buffer,right.length);
    return left;
}

String plus_String_String (String left, String right) {
    String newString;
    newString.buffer=malloc(left.length+right.length+1);
    memcpy(newString.buffer,left.buffer,left.length);
    memcpy(newString.buffer+left.length,right.buffer,right.length);
    newString.length=left.length+right.length;
    newString.buffer[newString.length]=0;
    return newString;
}

String plus_String_Integer (String left, int right) {
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

String plus_Integer_String (int left, String right) {
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

String plus_String_Float (String left, float right) {
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

String plus_Float_String (float left, String right) {
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
int print_Stream_String(Stream stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream)+fputc('\n',stream);
}

int print_Stream_Integer(Stream stream, int i) {
    return fprintf(stream,"%d\n",i);
}

int print_Stream_Float(Stream stream, float f) {
    return fprintf(stream,"%f\n",f);
}

int print_String(String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stdout)+fputc('\n',stdout);
}

int print_Integer(int i) {
    return fprintf(stdout,"%d\n",i);
}

int print_Float(float f) {
    return fprintf(stdout,"%f\n",f);
}

/* Echo omits the newline char */
int echo_Stream_String(Stream stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream);
}

int echo_Stream_Integer(Stream stream, int i) {
    return fprintf(stream,"%d",i);
}

int echo_Stream_Float(Stream stream, float f) {
    return fprintf(stream,"%f",f);
}

int echo_String(String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stdout);
}

int echo_Integer(int i) {
    return fprintf(stdout,"%d",i);
}

int echo_Float(float f) {
    return fprintf(stdout,"%f",f);
}

Ternary compare_Integer_Integer (Integer a, Integer b) {
    if (a>b) {
        return lt;
    } else if (a==b) {
        return eq;
    } else {
        return gt;
    }
}
String pick_Ternary_String_String_String (Ternary ternary, String a, String b, String c) {
    if (ternary==lt) {
        return a;
    } else if (ternary==eq) {
        return b;
    } else if (ternary==gt) {
        return c;
    }
}
