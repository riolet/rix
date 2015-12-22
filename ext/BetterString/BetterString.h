#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef bstring BetterString;
typedef struct bstrList * BetterStringList;

//BetterString BetterString_$_BetterString_$_String(String s);

BetterString BetterString_$_BetterString_$_String(String s);

BetterString BetterString_$_assign_$_BetterString(BetterString left, BetterString right);

BetterString BetterString_$_plus_$_BetterString(BetterString left, BetterString right);

BetterString BetterString_$_plus_$_Integer(BetterString left, int right);

BetterString BetterString_$_plus_$_Float(BetterString left, float right);

String BetterString_$_toString_$_(BetterString b);

BetterString BetterString_$_toLower_$_(BetterString b);

BetterString BetterString_$_toUpper_$_(BetterString b);

void BetterString_$_destructor_$_(BetterString b);

//Better String List Functions
BetterStringList BetterString_$_split_$_Char(BetterString b,char c);

BetterString BetterStringList_$_getObjectAtIndex_$_Integer(BetterStringList bList, Integer i);

void BetterStringList_$_destructor_$_(BetterStringList bList);
#endif
