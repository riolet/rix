#ifndef RSL_STRING_H
#define RSL_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RSL_STRING_MAX_BUFFLEN 1024

typedef struct {
    char *buffer;
    size_t length;
    size_t cap;

    int isStored;
    int isLiteral;
} String;

void String_cleanUp_GCC(String *s)
{
    //printf("\nGCC Cleaning up String %s refCount %d\n", s->buffer, s->isStored);
    if (!s->isLiteral) {
        //printf("\nFreeing %s refCount %d\n", s->buffer, s->isStored);
        free(s->buffer);
    } else {
        //printf("\nString %s is a literal\n", s->buffer);
    }
}

void String_cleanUp(String *s)
{
    //printf("\nCleaning up String %s refCount %d\n", s->buffer, s->isStored);
    if (!s->isLiteral) {
        if (s->isStored == 0) {
            //printf("\nFreeing %s refCount %d\n", s->buffer, s->isStored);
            free(s->buffer);
        } else {

        }
    } else {
            //printf("\nString %s is a literal\n", s->buffer);
    }
}

String String_$_stringlit(char *strlit)
{
    String s;
    s.buffer = strlit;
    s.cap = s.length = strlen(strlit);
    s.isStored = 0;
    s.isLiteral = 1;
    return s;
}

String String_$_assign_$_String(String *left, String right)
{
    left->length = right.length + 1;
    left->buffer = malloc(left->length);
    left->isStored = 1;
    left->isLiteral = 0;
    memcpy(left->buffer, right.buffer, right.length);
    //printf("Assiging\n");
    //printf("Left length %d buffer %s\n",left->length,left->buffer);
    return *left;
}

String String_$_plus_$_String(String left, String right)
{
    String newString;
    newString.isStored = 0;
    newString.isLiteral = 0;
    newString.buffer = malloc(left.length + right.length + 1);
    //printf("Left length %d buffer %s\n",left.length,left.buffer);
    memcpy(newString.buffer, left.buffer, left.length);
    memcpy(newString.buffer + left.length, right.buffer, right.length);
    newString.length = left.length + right.length;
    newString.buffer[newString.length] = 0;
    String_cleanUp(&left);
    String_cleanUp(&right);
    return newString;
}

String String_$_plus_$_Integer(String left, int right)
{
    String newString;
    newString.isStored = 0;
    newString.isLiteral = 0;
    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%i", right);
    newString.buffer = malloc(left.length + right_length + 1);
    memcpy(newString.buffer, left.buffer, left.length);
    memcpy(newString.buffer + left.length, rightStr, right_length);
    newString.length = left.length + right_length;
    newString.buffer[newString.length] = 0;
    return newString;
}

String Integer_$_plus_$_String(int left, String right)
{
    String newString;
    newString.isStored = 1;
    newString.isLiteral = 0;
    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%i", left);
    newString.buffer = malloc(right.length + left_length + 1);
    memcpy(newString.buffer, leftStr, left_length);
    memcpy(newString.buffer + left_length, right.buffer, right.length);
    newString.length = right.length + left_length;
    newString.buffer[newString.length] = 0;
    return newString;
}

String String_$_plus_$_Float(String left, float right)
{
    String newString;
    newString.isStored = 1;
    newString.isLiteral = 0;
    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%f", right);
    newString.buffer = malloc(left.length + right_length + 1);
    memcpy(newString.buffer, left.buffer, left.length);
    memcpy(newString.buffer + left.length, rightStr, right_length);
    newString.length = left.length + right_length;
    newString.buffer[newString.length] = 0;
    return newString;
}

String Float_$_plus_$_String(float left, String right)
{
    String newString;
    newString.isStored = 1;
    newString.isLiteral = 0;
    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%f", left);
    newString.buffer = malloc(right.length + left_length + 1);
    memcpy(newString.buffer + left_length, right.buffer, right.length);
    memcpy(newString.buffer, leftStr, left_length);
    newString.length = right.length + left_length;
    newString.buffer[newString.length] = 0;
    return newString;
}

#endif
