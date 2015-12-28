#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef struct tagbstring BetterString;
typedef struct bstrList BetterStringList;

IDENT_RETVAR_RAW *  BetterString_$_BetterString_$_String(IDENT_RETVAR_RAW * s_, IDENT_RETVAR_RAW * $_retvar_in);
IDENT_RETVAR_RAW * BetterString_$_toString_$_(IDENT_RETVAR_RAW *  b, IDENT_RETVAR_RAW * $_retvar_in);
void BetterString_$_destructor_$_(IDENT_RETVAR_RAW *  b_);

IDENT_RETVAR_RAW * BetterString_$_split_$_Char(IDENT_RETVAR_RAW *  b, char c, IDENT_RETVAR_RAW * $_retvar_in);
IDENT_RETVAR_RAW * BetterString_$_split_$_String(IDENT_RETVAR_RAW *  b_, IDENT_RETVAR_RAW *  s_, IDENT_RETVAR_RAW * $_retvar_in);
IDENT_RETVAR_RAW * BetterStringList_$_getObjectAtIndex_$_Integer(IDENT_RETVAR_RAW *  bList_, Integer i, IDENT_RETVAR_RAW * $_retvar_in);
void BetterStringList_$_destructor_$_(IDENT_RETVAR_RAW * bList_);


#endif
