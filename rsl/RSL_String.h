#ifndef RSL_STRING_H
#define RSL_STRING_H

#include <stddef.h>
#include "ObjectTree.h"
#include "rsl/rsl.h"

#define RSL_STRING_MAX_BUFFLEN 1024

struct String{
    IDENT_MPTR_RAW * $super;
    BaseType * $super_;
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

void String_$_destructor_$_(IDENT_MPTR_RAW *s_);

IDENT_MPTR_RAW * String_$_stringlit(char *strlit, IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * String_$_String_$_ (IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * String_$_plus_$_String(IDENT_MPTR_RAW * left_, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * String_$_plus_$_int(IDENT_MPTR_RAW * left_, int right, IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * int_$_plus_$_String(int left, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * String_$_plus_$_float(IDENT_MPTR_RAW * left_, float right, IDENT_MPTR_RAW * $_mptr_in);

IDENT_MPTR_RAW * float_$_plus_$_String(float left, IDENT_MPTR_RAW * right_, IDENT_MPTR_RAW * $_mptr_in);

char String_$_getObjectAtIndex_$_int(IDENT_MPTR_RAW * right_, int left);

int String_$_length_$_(IDENT_MPTR_RAW *  string);
#endif
