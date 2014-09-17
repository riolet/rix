#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFLEN 512
typedef struct {
    char *buff;
    size_t length;
    size_t cap;
} String;

#define Boolean_if(B) if (B)
#define Boolean_while(B) while (B)
#define ident_for_Integer_Integer(i,a,b) for (i=a;i<b;i++)

float Float_raisedto_Integer (float f, int i) {
    return pow(f,i);
}

float Integer_raisedto_Integer (int f, int i) {
    return pow(f,i);
}

String String_plus_String (String left, String right) {
    String newString;
    newString.buff=malloc(left.length+right.length+1);
    memcpy(newString.buff,left.buff,left.length);
    memcpy(newString.buff+left.length,right.buff,right.length);
    newString.length=left.length+right.length;
    newString.buff[newString.length]=0;
    return newString;
}

String String_plus_Float (String left, float right) {
    String newString;
    char rightStr[BUFFLEN];
    int right_length=snprintf(rightStr,BUFFLEN,"%f",right);
    newString.buff=malloc(left.length+right_length+1);
    memcpy(newString.buff,left.buff,left.length);
    memcpy(newString.buff+left.length,rightStr,right_length);
    newString.length=left.length+right_length;
    newString.buff[newString.length]=0;
    return newString;
}

String stringlit_plus_Float (const char * left, float right) {
    String newString;
    char rightStr[BUFFLEN];
    int right_length=snprintf(rightStr,BUFFLEN,"%s%f",left,right);
    newString.buff=malloc(right_length);
    memcpy(newString.buff,rightStr,right_length);
    newString.length=right_length;
    newString.buff[newString.length]=0;
    return newString;
}

String String_plus_stringlit (String left, const char * right) {
    String newString;
    int rightLen=strlen(right);
    newString.buff=malloc(left.length+rightLen+1);
    memcpy(newString.buff,left.buff,left.length);
    memcpy(newString.buff+left.length,right,rightLen);
    newString.length=left.length+rightLen;
    newString.buff[newString.length]=0;
    return newString;
}

String stringlit_plus_String (const char * left, String right) {
    String newString;
    int leftLen=strlen(left);
    newString.buff=malloc(leftLen+right.length+1);
    memcpy(newString.buff,left,leftLen);
    memcpy(newString.buff+leftLen,right.buff,right.length);
    newString.length=leftLen+right.length;
    newString.buff[newString.length]=0;
    return newString;
}

Stream_print_stringlit(FILE *stream,char *s) {
    return fprintf(stream,"%s\n",s);
}
Stream_print_String(FILE *stream,String s) {
    return fwrite(s.buff,sizeof(char),s.length,stream)+fputc('\n',stream);
}

Stream_print_Integer(FILE *stream, int i) {
    return fprintf(stream,"%d\n",i);
}

Stream_print_Float(FILE *stream, float f) {
    return fprintf(stream,"%f\n",f);
}

/* New Line */
int Stream_echo_stringlit(FILE *stream,char *s) {
    return fprintf(stream,"%s",s);
}
int Stream_echo_String(FILE *stream,String s) {
    return fwrite(s.buff,sizeof(char),s.length,stream);
}

int Stream_echo_Integer(FILE *stream, int i) {
    return fprintf(stream,"%d",i);
}

int Stream_echo_Float(FILE *stream, float f) {
    return fprintf(stream,"%f",f);
}
