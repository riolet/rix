#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"

void String_$_destructor_$_(IDENT_MPTR_RAW * _$_mptr_in)
{

    String *s = (String *) _$_mptr_in->obj;

    if (!s->isStaticBuffer) {
        free(s->buffer);
    }
    _$_cleanup(s->$super);
    free(s->$super);
    free(s);
}

IDENT_MPTR_RAW * String_$_String_$_ (IDENT_MPTR_RAW * _$_mptr_in)
{
//    String * s = calloc(1, sizeof(String));
//    return  _$_returnAppointer($_mptr_in,s,String_$_destructor_$_);

    _$_HEAP_VARIABLE(_$_mptr_super);
    String * self_ = calloc(1, sizeof(String));
    _$_mptr * self = _$_returnAppointer(_$_mptr_in,self_,String_$_destructor_$_);
    self_->$super= BaseType_$_BaseType_$_(_$_mptr_super);
    self_->$super_= self_->$super->obj;
    return self;
}

IDENT_MPTR_RAW * String_$_stringlit(char *strlit, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("String_$_stringlit %s %s\n",strlit,$_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp;
    IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp, xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;

    s->buffer = strlit;
    s->cap = strlen(strlit);
    s->length = strlen(strlit);
    s->isStaticBuffer = true;
    debugPrintf("String_$_stringlit %s -> %s\n",$_mptr_temp.debugName,s->buffer);

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

IDENT_MPTR_RAW * String_$_plus_$_int(IDENT_MPTR_RAW * left_, int right, IDENT_MPTR_RAW * $_mptr_in)
{

    debugPrintf("String_$_plus_$_int %s -> %s\n",left_->debugName, $_mptr_in->debugName);
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

IDENT_MPTR_RAW * int_$_plus_$_String(int left, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("int_$_plus_$_String %s -> %s\n",right_->debugName, $_mptr_in->debugName);
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

IDENT_MPTR_RAW * String_$_plus_$_float(IDENT_MPTR_RAW * left_, float right, IDENT_MPTR_RAW * $_mptr_in)
{

    debugPrintf("String_$_plus_$_int %s -> %s\n",left_->debugName, $_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                              xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;
    String * left = left_->obj;

    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%f", right);

    s->buffer = malloc(left->length + right_length + 1);
    memcpy(s->buffer, left->buffer, left->length);
    memcpy(s->buffer + left->length, rightStr, right_length);
    s->length = left->length + right_length;
    s->buffer[s->length] = 0;
    s->isStaticBuffer = false;

    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}

IDENT_MPTR_RAW * float_$_plus_$_String(float left, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("int_$_plus_$_String %s -> %s\n",right_->debugName, $_mptr_in->debugName);
    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
                                                                                              xstr($_mptr_temp));
    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);
    String * s = s_->obj;
    String * right = right_->obj;

    char leftStr[RSL_STRING_MAX_BUFFLEN];
    int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%f", left);

    s->buffer = malloc(right->length + left_length + 1);
    memcpy(s->buffer, leftStr, left_length);
    memcpy(s->buffer + left_length, right->buffer, right->length);
    s->length = right->length + left_length;
    s->buffer[s->length] = 0;
    s->isStaticBuffer = false;

    _$_mptr_prepare(&$_mptr_temp,$_mptr_in);
    return $_mptr_in;
}
char String_$_getObjectAtIndex_$_int(IDENT_MPTR_RAW * right_, int left)
{
    String * right = (String *) right_->obj;
    return right->buffer[left];
}

int String_$_length_$_(IDENT_MPTR_RAW *  s_)
{
    String * s = s_ ->obj;
    return s->length;
}