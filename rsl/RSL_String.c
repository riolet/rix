#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"

void String_$_destructor_$_(String *s)
{
    // debugPrintf("Cleaning up String %s\n",_$_mptr_in->debugName);
    // String *s = (String *) _$_mptr_in->obj;
    // if (!s->isStaticBuffer) {
    //     debugPrintf("String %s contains %s\n",_$_mptr_in->debugName, s->buffer);
    //     free(s->buffer); 
    // } else {
    //     debugPrintf("Not cleaning StaticString %s containing %s\n",_$_mptr_in->debugName, s->buffer);
    // }
    // free(s);
    // debugPrintf("Cleaned up String %s",_$_mptr_in->debugName);
}

String String_$_String_$_ (String s)
{
    String self;
    self.buffer=0;
    return self;
}

String String_$_stringlit(char *strlit)
{    
    String s;
    s.buffer = strlit;
    s.cap = strlen(strlit);
    s.length = strlen(strlit);
    s.isStaticBuffer = true;
    return s;
}

String String_$_plus_$_String(String left, String right)
{
    String  s;
    s.buffer = malloc(left.length + right.length + 1);
    memcpy(s.buffer, left.buffer, left.length);
    memcpy(s.buffer + left.length, right.buffer, right.length);
    s.length = left.length + right.length;
    s.buffer[s.length] = 0;
    s.isStaticBuffer = false;
    return s;
}

String String_$_plus_$_int(String left, int right_int)
{

    String s;

    char rightStr[RSL_STRING_MAX_BUFFLEN];
    int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%i", right_int);

    s.buffer = malloc(left.length + right_length + 1);
    memcpy(s.buffer, left.buffer, left.length);
    memcpy(s.buffer + left.length, rightStr, right_length);
    s.length = left.length + right_length;
    s.buffer[s.length] = 0;
    s.isStaticBuffer = false;
    return s;
}

bool String_$_isDefined_$_(String s) {
    return (s.buffer!=0);
}

// NonPrimObj * int_$_plus_$_String(int left, NonPrimObj * right_, NonPrimObj * $_mptr_in)
// {
//     debugPrintf("int_$_plus_$_String %s -> %s\n",right_->debugName, $_mptr_in->debugName);
//     _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
//                                                                                                     xstr($_mptr_temp));
//     NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);
//     String * s = s_->obj;
//     String * right = right_->obj;

//     char leftStr[RSL_STRING_MAX_BUFFLEN];
//     int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%i", left);

//     s->buffer = malloc(right->length + left_length + 1);
//     memcpy(s->buffer, leftStr, left_length);
//     memcpy(s->buffer + left_length, right->buffer, right->length);
//     s->length = right->length + left_length;
//     s->buffer[s->length] = 0;
//     s->isStaticBuffer = false;

//     _$_object_ownership_transfer(&$_mptr_temp,$_mptr_in);
//     return $_mptr_in;
// }

// NonPrimObj * String_$_plus_$_float(NonPrimObj * left_, float right, NonPrimObj * $_mptr_in)
// {

//     debugPrintf("String_$_plus_$_int %s -> %s\n",left_->debugName, $_mptr_in->debugName);
//     _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
//                                                                                               xstr($_mptr_temp));
//     NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);
//     String * s = s_->obj;
//     String * left = left_->obj;

//     char rightStr[RSL_STRING_MAX_BUFFLEN];
//     int right_length = snprintf(rightStr, RSL_STRING_MAX_BUFFLEN, "%f", right);

//     s->buffer = malloc(left->length + right_length + 1);
//     memcpy(s->buffer, left->buffer, left->length);
//     memcpy(s->buffer + left->length, rightStr, right_length);
//     s->length = left->length + right_length;
//     s->buffer[s->length] = 0;
//     s->isStaticBuffer = false;

//     _$_object_ownership_transfer(&$_mptr_temp,$_mptr_in);
//     return $_mptr_in;
// }

// NonPrimObj * float_$_plus_$_String(float left, NonPrimObj * right_, NonPrimObj * $_mptr_in)
// {
//     debugPrintf("int_$_plus_$_String %s -> %s\n",right_->debugName, $_mptr_in->debugName);
//     _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp; IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp,
//                                                                                               xstr($_mptr_temp));
//     NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);
//     String * s = s_->obj;
//     String * right = right_->obj;

//     char leftStr[RSL_STRING_MAX_BUFFLEN];
//     int left_length = snprintf(leftStr, RSL_STRING_MAX_BUFFLEN, "%f", left);

//     s->buffer = malloc(right->length + left_length + 1);
//     memcpy(s->buffer, leftStr, left_length);
//     memcpy(s->buffer + left_length, right->buffer, right->length);
//     s->length = right->length + left_length;
//     s->buffer[s->length] = 0;
//     s->isStaticBuffer = false;

//     _$_object_ownership_transfer(&$_mptr_temp,$_mptr_in);
//     return $_mptr_in;
// }
// char String_$_getObjectAtIndex_$_int(NonPrimObj * right_, int left)
// {
//     String * right = (String *) right_->obj;
//     return right->buffer[left];
// }

// int String_$_length_$_(NonPrimObj *  s_)
// {
//     String * s = s_ ->obj;
//     return s->length;
// }

// NonPrimObj * String_$_lower_$_(NonPrimObj *  right_, NonPrimObj * $_mptr_in)
// {
//     debugPrintf("String_$_lower %s -> %s\n",right_->debugName, $_mptr_in->debugName);
//     _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp;
//     IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp, xstr($_mptr_temp));
//     NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);
//     String * s = s_->obj;
//     String * right = right_->obj;

//     s->buffer = malloc(right->length + 1);


//     int i;
//     for(i = 0; right->buffer[i]; i++){
//       s->buffer[i] = tolower(right->buffer[i]);
//     }

//     s->length = right->length;
//     s->buffer[s->length] = 0;
//     s->isStaticBuffer = false;

//     _$_object_ownership_transfer(&$_mptr_temp,$_mptr_in);
//     return $_mptr_in;
// }

// NonPrimObj * String_$_upper_$_(NonPrimObj *  right_, NonPrimObj * $_mptr_in)
// {
//     debugPrintf("String_$_lower %s -> %s\n",right_->debugName, $_mptr_in->debugName);
//     _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp;
//     IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp, xstr($_mptr_temp));
//     NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);
//     String * s = s_->obj;
//     String * right = right_->obj;

//     s->buffer = malloc(right->length + 1);

//     int i;
//     for(i = 0; right->buffer[i]; i++){
//       s->buffer[i] = toupper(right->buffer[i]);
//     }

//     s->length = right->length;
//     s->buffer[s->length] = 0;
//     s->isStaticBuffer = false;

//     _$_object_ownership_transfer(&$_mptr_temp,$_mptr_in);
//     return $_mptr_in;
// }

bool String_$_cmpeq_$_String(String left, String right) {
    if (!(left.buffer&&right.buffer)) {
      return false;
    } 
    if (strcmp(left.buffer,right.buffer)) {
        return false;
    } else {
        return true;
    }
}