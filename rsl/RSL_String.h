#ifndef RSL_STRING_H
#define RSL_STRING_H

#include <stddef.h>
#include "ObjectTree.h"
#include "rsl/rsl.h"

#define RSL_STRING_MAX_BUFFLEN 1024

enum StringStatus
{
    StringStatusDelete=0,
    StringStatusLiteral,
    StringStatusStored,
    StringStatusReturned
};



void String_return_GCC(String *s);

void String_cleanUp_GCC(String *s);

void String_$_destructor_$_(String *s);

String *String_$_String_$_ ();

IDENT_RETVAR_RAW * String_$_stringlit(char *strlit, IDENT_RETVAR_RAW * $_retvar_in);

IDENT_RETVAR_RAW * String_$_plus_$_String(IDENT_RETVAR_RAW * left_, IDENT_RETVAR_RAW * right_, IDENT_RETVAR_RAW * $_retvar_in);

IDENT_RETVAR_RAW * String_$_plus_$_Integer(IDENT_RETVAR_RAW * left_, int right, IDENT_RETVAR_RAW * $_retvar_in);

String *Integer_$_plus_$_String(int left, String *right);

String *String_$_plus_$_Float(String *left, float right);

String *Float_$_plus_$_String(float left, String *right);

char String_$_getObjectAtIndex_$_Integer(String * right, int left);

int String_$_length_$_(IDENT_RETVAR_RAW *  string);
#endif
