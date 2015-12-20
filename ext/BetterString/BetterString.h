#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef struct tagbstring BetterString;

//BetterString BetterString_$_BetterString_$_String(String s);

BetterString * BetterString_$_BetterString_$_String(String s);

BetterString * BetterString_$_assign_$_BetterString(BetterString * left, BetterString * right);

BetterString * BetterString_$_plus_$_BetterString(BetterString * left, BetterString * right);

BetterString * BetterString_$_plus_$_Integer(BetterString * left, int right);

BetterString * BetterString_$_plus_$_Float(BetterString * left, float right);

String BetterString_$_toString_$_(BetterString * b);

BetterString * BetterString_$_toLower_$_(BetterString * b);

BetterString * BetterString_$_toUpper_$_(BetterString * b);

void BetterString_$_destructor_$_(BetterString * b);
#endif
