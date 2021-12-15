#ifndef RSL_EXT_BETTERSTRING_H
#define RSL_EXT_BETTERSTRING_H

#include "errors.h"
#include "rsl/rsl.h"
#include "rsl/RSL_String.h"
#include "ext/BetterString/bstrlib.h"

typedef struct tagbstring BetterString;
typedef struct bstrList BetterStringList;

NonPrimObj * BetterString_$_BetterString_$_BetterString_$_  (BetterString * b, NonPrimObj * $_mptr_in);
NonPrimObj * BetterString_$_BetterString_$_ (NonPrimObj * $_mptr_in);
NonPrimObj *  BetterString_$_BetterString_$_String(NonPrimObj * s_, NonPrimObj * $_mptr_in);
NonPrimObj * BetterString_$_toString_$_(NonPrimObj *  b, NonPrimObj * $_mptr_in);
void BetterString_$_destructor_$_(NonPrimObj *  b_);

NonPrimObj * BetterString_$_split_$_char(NonPrimObj *  b, char c, NonPrimObj * $_mptr_in);
NonPrimObj * BetterString_$_split_$_String(NonPrimObj *  b_, NonPrimObj *  s_, NonPrimObj * $_mptr_in);
NonPrimObj * BetterString_$_splits_$_String(NonPrimObj *  b_, NonPrimObj *  s_, NonPrimObj * $_mptr_in);
NonPrimObj * BetterStringList_$_getObjectAtIndex_$_int(NonPrimObj *  bList_, int i, NonPrimObj * $_mptr_in);
void BetterStringList_$_destructor_$_(NonPrimObj * bList_);


#endif
