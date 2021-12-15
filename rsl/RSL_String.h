#ifndef RSL_STRING_H
#define RSL_STRING_H

#include <stddef.h>
#include "ObjectTree.h"
#include "rsl/rsl.h"

#define RSL_STRING_MAX_BUFFLEN 1024

struct String{
    char *buffer;
    size_t length;
    size_t cap;
    bool isStaticBuffer;
};


enum StringStatus
{
    StringStatusDelete=0,
    StringStatusLiteral,
    StringStatusStored,
    StringStatusReturned
};



void String_return_GCC(String *s);

void String_cleanUp_GCC(String *s);

void String_$_destructor_$_(String *s_);

String String_$_stringlit(char *strlit);

String String_$_String_$_ ();

String String_$_plus_$_String(String left, String right);

String String_$_plus_$_int(String left, int right_int);

bool String_$_isDefined_$_(String s);

// NonPrimObj * int_$_plus_$_String(int left, NonPrimObj * right_, NonPrimObj * $_mptr_in);

// NonPrimObj * String_$_plus_$_float(NonPrimObj * left_, float right, NonPrimObj * $_mptr_in);

// NonPrimObj * float_$_plus_$_String(float left, NonPrimObj * right_, NonPrimObj * $_mptr_in);

// char String_$_getObjectAtIndex_$_int(NonPrimObj * right_, int left);

// int String_$_length_$_(NonPrimObj *  string);

// String String_$_lower_$_(NonPrimObj *  right_, NonPrimObj * $_mptr_in);

// String String_$_upper_$_(NonPrimObj *  right_, NonPrimObj * $_mptr_in);

bool String_$_cmpeq_$_String(String left, String right);

#endif
