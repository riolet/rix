#ifndef RSL_STRING_H
#define RSL_STRING_H

#define RSL_STRING_MAX_BUFFLEN 1024
#include <stddef.h>
enum StringStatus
{
    StringStatusDelete=0,
    StringStatusLiteral,
    StringStatusStored,
    StringStatusReturned
};

typedef struct {
    char *buffer;
    size_t length;
    size_t cap;

    int isStored;
} String;

void String_return_GCC(String *s);

void String_cleanUp_GCC(String *s);

void String_cleanUp(String *s);

String String_$_String_$_ ();

String String_$_stringlit(char *strlit);

String String_$_assign_$_String(String *left, String right);

String String_$_plus_$_String(String left, String right);

String String_$_plus_$_Integer(String left, int right);

String Integer_$_plus_$_String(int left, String right);

String String_$_plus_$_Float(String left, float right);

String Float_$_plus_$_String(float left, String right);

char String_$_getObjectAtIndex_$_Integer(String right, int left);

int String_$_length_$_(String right);
#endif
