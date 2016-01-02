#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef struct tagbstring BetterString;
typedef struct bstrList BetterStringList;

IDENT_MPTR_RAW *  BetterString_$_BetterString_$_String(IDENT_MPTR_RAW * s_, IDENT_MPTR_RAW * $_mptr_in);
IDENT_MPTR_RAW * BetterString_$_toString_$_(IDENT_MPTR_RAW *  b, IDENT_MPTR_RAW * $_mptr_in);
void BetterString_$_destructor_$_(IDENT_MPTR_RAW *  b_);

IDENT_MPTR_RAW * BetterString_$_split_$_Char(IDENT_MPTR_RAW *  b, char c, IDENT_MPTR_RAW * $_mptr_in);
IDENT_MPTR_RAW * BetterString_$_split_$_String(IDENT_MPTR_RAW *  b_, IDENT_MPTR_RAW *  s_, IDENT_MPTR_RAW * $_mptr_in);
IDENT_MPTR_RAW * BetterStringList_$_getObjectAtIndex_$_int(IDENT_MPTR_RAW *  bList_, int i, IDENT_MPTR_RAW * $_mptr_in);
void BetterStringList_$_destructor_$_(IDENT_MPTR_RAW * bList_);


#endif
