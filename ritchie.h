#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buff;
    size_t length;
    size_t cap;
} String;

String * String_from_stringlit (const char * strlit) {
    String * tempStr;
    tempStr=malloc(sizeof(String));
    tempStr->length=strlen(strlit);
    tempStr->buff=malloc(tempStr->length*sizeof(char));
    memcpy(tempStr->buff,strlit,tempStr->length);
    tempStr->cap=tempStr->length;
    return tempStr;
}

String * String_plus_String (String * left, String * right) {
    String * tempStr=malloc(sizeof(String));
    tempStr->length=left->length+right->length;
    tempStr->buff=malloc(sizeof(char)*(tempStr->length));
    memcpy(tempStr->buff,left->buff,left->length);
    memcpy((tempStr->buff)+left->length,right->buff,right->length);
    tempStr->cap=left->length+right->length;
    return tempStr;
}

Stream_print_String(FILE *stream,String *s) {
    fwrite(s->buff,sizeof(char),s->length,stream);
}

Stream_print_Float(FILE *stream, float f) {
    fprintf(stream,"%f",f);
}
