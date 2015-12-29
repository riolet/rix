#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"

void String_$_destructor_$_(IDENT_MPTR_RAW *s_)
{
    String * s = (String *) s_->obj;
    if (!s->isStaticBuffer) {
        free(s->buffer);
    }
    free(s);
}

IDENT_MPTR_RAW * String_$_String_$_ (IDENT_MPTR_RAW * $_mptr_in)
{
    String * s = calloc(1, sizeof(String));
    return  _$_returnAppointer($_mptr_in,s,String_$_destructor_$_);
}

IDENT_MPTR_RAW * String_$_stringlit(char *strlit, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("String_$_stringlit %s %s\n",strlit,$_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                                    xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;

    s->buffer = strlit;
    s->cap = strlen(strlit);
    s->length = strlen(strlit);
    s->isStaticBuffer = true;
    debugPrintf("String_$_stringlit %s -> %s\n",$_mptr_in->debugName,s->buffer);

    //Todod pass this from the caller
    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}

String * String_$_assign_$_String(String *left, String * right)
{
    left->length = right->length;
    left->buffer = malloc(left->length);
    left->isStaticBuffer = false;
    left->cap=right->cap;
    memcpy(left->buffer, right->buffer, right->length);
    return left;
}

IDENT_MPTR_RAW * String_$_plus_$_String(IDENT_MPTR_RAW * left_, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("String_$_plus_$_String %s %s -> %s\n",left_->debugName,right_->debugName, $_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                                    xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;
    String * left = left_->obj;
    String * right = right_->obj;

    s->buffer = malloc(left->length + right->length + 1);
    memcpy(s->buffer, left->buffer, left->length);
    memcpy(s->buffer + left->length, right->buffer, right->length);
    s->length = left->length + right->length;
    s->buffer[s->length] = 0;
    s->isStaticBuffer = false;

    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}

IDENT_MPTR_RAW * String_$_plus_$_Integer(IDENT_MPTR_RAW * left_, int right, IDENT_MPTR_RAW * $_mptr_in)
{

    debugPrintf("String_$_plus_$_Integer %s -> %s\n",left_->debugName, $_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                                    xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;
    String * left = left_->obj;

    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%i", right);

    s->buffer = malloc(left->length + right_length + 1);
    memcpy(s->buffer, left->buffer, left->length);
    memcpy(s->buffer + left->length, rightStr, right_length);
    s->length = left->length + right_length;
    s->buffer[s->length] = 0;
    s->isStaticBuffer = false;

    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}

IDENT_MPTR_RAW * Integer_$_plus_$_String(int left, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("Integer_$_plus_$_String %s -> %s\n",right_->debugName, $_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                                    xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;
    String * right = right_->obj;

    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%i", left);

    s->buffer = malloc(right->length + left_length + 1);
    memcpy(s->buffer, leftStr, left_length);
    memcpy(s->buffer + left_length, right->buffer, right->length);
    s->length = right->length + left_length;
    s->buffer[s->length] = 0;
    s->isStaticBuffer = false;

    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}

String * String_$_plus_$_Float(String * left, float right)
{
    String * newString;
    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%f", right);
    newString->buffer = malloc(left->length + right_length + 1);
    memcpy(newString->buffer, left->buffer, left->length);
    memcpy(newString->buffer + left->length, rightStr, right_length);
    newString->length = left->length + right_length;
    newString->buffer[newString->length] = 0;
    return newString;
}

String * Float_$_plus_$_String(float left, String * right)
{
    String * newString;
    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%f", left);
    newString->buffer = malloc(right->length + left_length + 1);
    memcpy(newString->buffer + left_length, right->buffer, right->length);
    memcpy(newString->buffer, leftStr, left_length);
    newString->length = right->length + left_length;
    newString->buffer[newString->length] = 0;
    return newString;
}

char String_$_getObjectAtIndex_$_Integer(IDENT_MPTR_RAW * right_, int left)
{
    String * right = (String *) right_->obj;
    return right->buffer[left];
}

int String_$_length_$_(IDENT_MPTR_RAW *  s_)
{
    String * s = s_ ->obj;
    return s->length;
}