#include "ext/BetterString/BetterString.h"

IDENT_RETVAR_RAW *  BetterString_$_BetterString_$_String(IDENT_RETVAR_RAW * s_, IDENT_RETVAR_RAW * $_retvar_in)
{
    String * s = s_->obj;
    BetterString *  result = (BetterString * ) bfromcstr(s->buffer);
    return _$_returnAppointer($_retvar_in, result, bdestroy);
}

//IDENT_RETVAR_RAW *  BetterString_$_plus_$_BetterString(IDENT_RETVAR_RAW *  left, IDENT_RETVAR_RAW *  right)
//{
//    IDENT_RETVAR_RAW *  retval = bstrcpy(left);
//    if (bconcat(retval, right)) {
//        return retval;
//    } else {
//        criticalError(0, "BetterString * concatenation failure");
//    }
//}
//
//IDENT_RETVAR_RAW *  BetterString_$_plus_$_Integer(IDENT_RETVAR_RAW *  left, int right)
//{
//    return bformat("%s%i", left->data, right);
//}
//
//IDENT_RETVAR_RAW *  Integer_$_plus_$_BetterString(int left, IDENT_RETVAR_RAW *  right)
//{
//    return bformat("%i%s", left, right->data);
//}
//
//IDENT_RETVAR_RAW *  BetterString_$_plus_$_Float(IDENT_RETVAR_RAW *  left, float right)
//{
//    return bformat("%s%f", left->data, right);
//}
//
//IDENT_RETVAR_RAW *  Float_$_plus_$_BetterString(float left, IDENT_RETVAR_RAW *  right)
//{
//    return bformat("%f%s", left, right->data);
//}

IDENT_RETVAR_RAW * BetterString_$_toString_$_(IDENT_RETVAR_RAW *  b_, IDENT_RETVAR_RAW * $_retvar_in)
{
    BetterString * b = b_->obj;
    String * s = String_$_String_$_();
    s->buffer = malloc(sizeof(char)*(b->slen+1));
    memcpy(s->buffer, b->data, b->slen);
    s->cap = b->mlen;
    s->length = b->slen;
    return _$_returnAppointer($_retvar_in,s,String_$_destructor_$_);
}

//IDENT_RETVAR_RAW *  BetterString_$_toLower_$_(IDENT_RETVAR_RAW *  b)
//{
//    int result = btolower(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toLower_$_");
//    }
//}
//
//IDENT_RETVAR_RAW *  BetterString_$_toUpper_$_(IDENT_RETVAR_RAW *  b)
//{
//    int result = btoupper(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toUpper_$_");
//    }
//}
//
//void BetterString_$_destructor_$_(IDENT_RETVAR_RAW *  b)
//{
//    int result = bdestroy((BetterString * )b);
//    if (result==BSTR_OK) {
//        //Do nothing?
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_destructor_$_");
//    }
//}
//
//BetterStringList * BetterString_$_split_$_Char(IDENT_RETVAR_RAW *  b, char c)
//{
//    return bsplit(b,c);
//}
//
//IDENT_RETVAR_RAW *  BetterStringList_$_getObjectAtIndex_$_Integer(BetterStringList  *  bList, Integer i)
//{
// return bList->entry[i];
//}

//void BetterStringList_$_destructor_$_(BetterStringList * bList)
//{
//    int result = bstrListDestroy(bList);
//    if (result==BSTR_OK) {
//        //Do nothing?
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterStringList_$_destructor_$_");
//    }
//}
//
//int BetterStringList_$_length_$_(BetterStringList * bList)
//{
//    return bList->qty;
//}