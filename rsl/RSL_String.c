#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"

void String_return_GCC(String *s)
{
    s->isStored=2;
}

void String_cleanUp_GCC(String *s)
{
    if (!s->isStored==StringStatusLiteral) {
        if (!s->isStored==StringStatusReturned) {
            free(s->buffer);
        } else {
            s->isStored=StringStatusDelete;
        }
    }
}

void String_cleanUp(String *s)
{
    if (s->isStored==StringStatusDelete) {
        free(s->buffer);
    }
}

String *String_$_String_$_ ()
{
    return calloc(1, sizeof(String));
}

IDENT_RETVAR_RAW * String_$_stringlit(char *strlit)
{
    String * s = String_$_String_$_();
    s->buffer = strlit;
    s->cap = strlen(strlit);
    s->length = strlen(strlit);

    //Todod pass this from the caller
    IDENT_RETVAR_RAW * $_retvar_in = malloc(sizeof(IDENT_RETVAR_RAW));
    $_retvar_in->ctr = 1;
    $_retvar_in->ptr = 0;
    $_retvar_in->obj = s;
    $_retvar_in->objType = "String";
    return $_retvar_in;
}

String * String_$_assign_$_String(String *left, String * right)
{
    left->length = right->length + 1;
    left->buffer = malloc(left->length);
    left->isStored = StringStatusStored;
    memcpy(left->buffer, right->buffer, right->length);
    return left;
}

IDENT_RETVAR_RAW * String_$_plus_$_String(IDENT_RETVAR_RAW * left_, IDENT_RETVAR_RAW * right_, IDENT_RETVAR_RAW * $_retvar_in)
{
    String * newString;
    String * left = left_->obj;
    String * right = right_->obj;

    newString = malloc(sizeof(String));

    newString->buffer = malloc(left->length + right->length + 1);
    memcpy(newString->buffer, left->buffer, left->length);
    memcpy(newString->buffer + left->length, right->buffer, right->length);
    newString->length = left->length + right->length;
    newString->buffer[newString->length] = 0;

    $_retvar_in->ctr = 0;
    $_retvar_in->ptr = 0;
    $_retvar_in->obj = newString;
    $_retvar_in->objType = "String";
    return $_retvar_in;
}

String * String_$_plus_$_Integer(String * left, int right)
{
    String * newString;
    newString->isStored = StringStatusDelete;
    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%i", right);
    newString->buffer = malloc(left->length + right_length + 1);
    memcpy(newString->buffer, left->buffer, left->length);
    memcpy(newString->buffer + left->length, rightStr, right_length);
    newString->length = left->length + right_length;
    newString->buffer[newString->length] = 0;
    String_cleanUp(left);
    return newString;
}

String * Integer_$_plus_$_String(int left, String * right)
{
    String * newString;
    newString->isStored = StringStatusDelete;
    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%i", left);
    newString->buffer = malloc(right->length + left_length + 1);
    memcpy(newString->buffer, leftStr, left_length);
    memcpy(newString->buffer + left_length, right->buffer, right->length);
    newString->length = right->length + left_length;
    newString->buffer[newString->length] = 0;
    String_cleanUp(right);
    return newString;
}

String * String_$_plus_$_Float(String * left, float right)
{
    String * newString;
    newString->isStored = StringStatusDelete;
    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%f", right);
    newString->buffer = malloc(left->length + right_length + 1);
    memcpy(newString->buffer, left->buffer, left->length);
    memcpy(newString->buffer + left->length, rightStr, right_length);
    newString->length = left->length + right_length;
    newString->buffer[newString->length] = 0;
    String_cleanUp(left);
    return newString;
}

String * Float_$_plus_$_String(float left, String * right)
{
    String * newString;
    newString->isStored = StringStatusDelete;
    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%f", left);
    newString->buffer = malloc(right->length + left_length + 1);
    memcpy(newString->buffer + left_length, right->buffer, right->length);
    memcpy(newString->buffer, leftStr, left_length);
    newString->length = right->length + left_length;
    newString->buffer[newString->length] = 0;
    String_cleanUp(right);
    return newString;
}

char String_$_getObjectAtIndex_$_Integer(String * right, int left)
{
    return right->buffer[left];
}

int String_$_length_$_(String * right)
{
    return right->length-1;
}