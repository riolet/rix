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
    printf("String_from_stringlit %d good\n",tempStr);
    return tempStr;
}

String * String_assign_String (String * left, String * right) {
    printf("Right %d\n",right);
    left = right;
}

String * String_plus_String (String * left, String * right) {
    printf("Left %d Right %d\n",left,right);
    String * tempStr;
    tempStr=malloc(sizeof(String));
    tempStr->buff=malloc(sizeof(char)*(left->length+right->length));
    memcpy(tempStr->buff,left,left->length);
    memcpy(tempStr->buff+left->length,right,right->length);
    tempStr->length=left->length+right->length;
    tempStr->cap=left->length+right->length;
    printf("String_plus_String good\n");
    return tempStr;
}

Stream_print_String(FILE *stream,String *s) {
    fwrite(s->buff,1,s->length,stream);
}

Stream_print_Float(FILE *stream, float f) {
    fprintf(stream,"%f",f);
}
