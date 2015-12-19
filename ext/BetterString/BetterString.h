#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef bstring BetterString;

//BetterString BetterString_$_BetterString_$_String(String s);

BetterString BetterString_$_stringlit(char *strlit);

BetterString BetterString_$_assign_$_BetterString(BetterString left, BetterString right);

BetterString BetterString_$_plus_$_BetterString(BetterString left, BetterString right);

BetterString BetterString_$_plus_$_Integer(BetterString left, int right);

BetterString BetterString_$_plus_$_Float(BetterString left, float right);

String BetterString_$_toString_$_BetterString(BetterString b);
#endif
