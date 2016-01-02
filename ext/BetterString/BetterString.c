#include "ext/BetterString/BetterString.h"

IDENT_MPTR_RAW * BetterString_$_BetterString_$_ (IDENT_MPTR_RAW * $_mptr_in)
{
    BetterString * b = calloc(1, sizeof(BetterString));
    return _$_returnAppointer($_mptr_in,b,BetterString_$_destructor_$_);
}

IDENT_MPTR_RAW *  BetterString_$_BetterString_$_String(IDENT_MPTR_RAW * s_, IDENT_MPTR_RAW * $_mptr_in)
{
    String * s = s_->obj;
    BetterString *  result = (BetterString * ) bfromcstr(s->buffer);
    debugPrintf("Creating %s from %s\n",$_mptr_in->debugName, s->buffer);
    return _$_returnAppointer($_mptr_in, result, BetterString_$_destructor_$_);
}

//IDENT_MPTR_RAW *  BetterString_$_plus_$_BetterString(IDENT_MPTR_RAW *  left, IDENT_MPTR_RAW *  right)
//{
//    IDENT_MPTR_RAW *  retval = bstrcpy(left);
//    if (bconcat(retval, right)) {
//        return retval;
//    } else {
//        criticalError(0, "BetterString * concatenation failure");
//    }
//}
//
//IDENT_MPTR_RAW *  BetterString_$_plus_$_int(IDENT_MPTR_RAW *  left, int right)
//{
//    return bformat("%s%i", left->data, right);
//}
//
//IDENT_MPTR_RAW *  int_$_plus_$_BetterString(int left, IDENT_MPTR_RAW *  right)
//{
//    return bformat("%i%s", left, right->data);
//}
//
//IDENT_MPTR_RAW *  BetterString_$_plus_$_float(IDENT_MPTR_RAW *  left, float right)
//{
//    return bformat("%s%f", left->data, right);
//}
//
//IDENT_MPTR_RAW *  float_$_plus_$_BetterString(float left, IDENT_MPTR_RAW *  right)
//{
//    return bformat("%f%s", left, right->data);
//}



IDENT_MPTR_RAW * BetterString_$_toString_$_(IDENT_MPTR_RAW *  b_, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("Creating string  from %s and storing in %s\n",b_->debugName,$_mptr_in->debugName);
    BetterString * b = b_->obj;

    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp;
    IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp, xstr($_mptr_temp));

    IDENT_MPTR_RAW * s_ = String_$_String_$_(&$_mptr_temp);

    String *s = (String *) s_->obj;
    s->buffer = malloc(sizeof(char)*(b->slen+1));
    memcpy(s->buffer, b->data, b->slen);
    s->cap = b->mlen;
    s->length = b->slen;
    _$_mptr_prepare(&$_mptr_temp, $_mptr_in);
    return $_mptr_in;
}

//IDENT_MPTR_RAW *  BetterString_$_toLower_$_(IDENT_MPTR_RAW *  b)
//{
//    int result = btolower(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toLower_$_");
//    }
//}
//
//IDENT_MPTR_RAW *  BetterString_$_toUpper_$_(IDENT_MPTR_RAW *  b)
//{
//    int result = btoupper(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toUpper_$_");
//    }
//}
//
void BetterString_$_destructor_$_(IDENT_MPTR_RAW *  b_)
{
    BetterString * b = (BetterString *) b_->obj;
    int result = bdestroy((BetterString * )b);
    if (result==BSTR_OK) {
        //Do nothing?
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterString_$_destructor_$_");
    }
}

IDENT_MPTR_RAW * BetterString_$_split_$_Char(IDENT_MPTR_RAW *  b_, char c, IDENT_MPTR_RAW * $_mptr_in)
{
    const_bstring b = b_->obj;
    return _$_returnAppointer($_mptr_in,bsplit(b,c),BetterStringList_$_destructor_$_);
}

IDENT_MPTR_RAW * BetterString_$_split_$_String(IDENT_MPTR_RAW *  b_, IDENT_MPTR_RAW *  s_, IDENT_MPTR_RAW * $_mptr_in)
{
    const_bstring b = b_->obj;
    String * s = s_->obj;
    bstring sBstring = bfromcstr(s->buffer);
    struct bstrList * result = bsplitstr(b,sBstring);
    bdestroy(sBstring);
    return _$_returnAppointer($_mptr_in,result,BetterStringList_$_destructor_$_);
}

IDENT_MPTR_RAW * BetterStringList_$_getObjectAtIndex_$_int(IDENT_MPTR_RAW *  bList_, int i, IDENT_MPTR_RAW * $_mptr_in)
{
    debugPrintf("BetterStringList_$_getObjectAtIndex_$_int %i\n",i);

    struct bstrList * bList = bList_->obj;
    return _$_returnAppointer($_mptr_in, bList->entry[i], BetterString_$_destructor_$_);
}

void BetterStringList_$_destructor_$_(IDENT_MPTR_RAW * bList_)
{
    BetterStringList * bList = (BetterStringList *) bList_->obj;
    free (bList->entry);
    free (bList);
}

int BetterStringList_$_length_$_(IDENT_MPTR_RAW *  bList_)
{
    struct bstrList * bList = bList_->obj;
    return bList->qty;
}