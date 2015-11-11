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

typedef void * UNKNOWNTYPE;
UNKNOWNTYPE UNKNOWNOBJECT;
typedef int Integer;
typedef float Float;
typedef enum { false, true } Boolean;
typedef enum { lt=-1, eq=0, gt=1 } Ternary;

#define UNKNOWNTYPE_if_Boolean(A,B) if (B)
#define UNKNOWNTYPE_elif_Boolean(A,B) else if (B)
#define UNKNOWNTYPE_else(A) else
#define UNKNOWNTYPE_while_Boolean(A,B) while (B)
#define Identifier_for_Integer_Integer(i,a,b) for (i=a;i<b;i++)

float Float_exponent_Integer (float f, int i) {
    return pow(f,i);
}

int Integer_exponent_Integer (int f, int i) {
    return (int)pow(f,i);
}

float Integer_exponent_Float (int f, float i) {
    return pow(f,i);
}

float Float_exponent_Float (float f, float i) {
    return pow(f,i);
}

String String_stringlit (const char * strlit) {
    String s;
    s.buffer = strlit;
    s.cap = s.length = strlen(strlit);
    return s;
}
String String_assign_String (String left, String right) {
    left.buffer=malloc(right.length+1);
    memcpy(left.buffer,right.buffer,right.length);
    return left;
}

String String_plus_String (String left, String right) {
    String newString;
    newString.buffer=malloc(left.length+right.length+1);
    memcpy(newString.buffer,left.buffer,left.length);
    memcpy(newString.buffer+left.length,right.buffer,right.length);
    newString.length=left.length+right.length;
    newString.buffer[newString.length]=0;
    return newString;
}

String String_plus_Integer (String left, int right) {
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

String Integer_plus_String (int left, String right) {
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

String String_plus_Float (String left, float right) {
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

String Float_plus_String (float left, String right) {
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


Stream_print_String(FILE *stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream)+fputc('\n',stream);
}

Stream_print_Integer(FILE *stream, int i) {
    return fprintf(stream,"%d\n",i);
}

Stream_print_Float(FILE *stream, float f) {
    return fprintf(stream,"%f\n",f);
}

int print_Integer(int i) {
    return fprintf(stdout,"%d\n",i);
}

int print_Float(float f) {
    return fprintf(stdout,"%f\n",f);
}

/* New Line */
int Stream_echo_stringlit(FILE *stream,char *s) {
    return fprintf(stream,"%s",s);
}
int Stream_echo_String(FILE *stream,String s) {
    return fwrite(s.buffer,sizeof(char),s.length,stream);
}

int Stream_echo_Integer(FILE *stream, int i) {
    return fprintf(stream,"%d",i);
}

int Stream_echo_Float(FILE *stream, float f) {
    return fprintf(stream,"%f",f);
}

Ternary Integer_compare_Integer (Integer a, Integer b) {
    if (a>b) {
        return lt;
    } else if (a==b) {
        return eq;
    } else {
        return gt;
    }
}
String Ternary_pick_String_String_String (Ternary ternary, String a, String b, String c) {
    if (ternary==lt) {
        return a;
    } else if (ternary==eq) {
        return b;
    } else if (ternary==gt) {
        return c;
    }
}
