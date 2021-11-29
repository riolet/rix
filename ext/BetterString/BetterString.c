#include "ext/BetterString/BetterString.h"
#include "rsl/RSL_Vector.h"

IDENT_MPTR_RAW * BetterString_$_BetterString_$_BetterString_$_  (BetterString * b, IDENT_MPTR_RAW * $_mptr_in)
{
    return _$_returnAppointer($_mptr_in,b,BetterString_$_destructor_$_);
}


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

    _$_TEMP_OBJ($_mptr_temp);
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

IDENT_MPTR_RAW * BetterString_$_split_$_char(IDENT_MPTR_RAW *  b_, char c, IDENT_MPTR_RAW * $_mptr_in)
{
    _$_VARIABLE(_$_temp_vector);
    _$_VARIABLE(_$_temp_vector_item);
    _$_VARIABLE(_$_temp_betterstring);
    const_bstring b = b_->obj;
    struct bstrList * result = bsplit(b,c);

    //printf("Parts %d ",result->qty);
    Vector_$_Vector_$_int(result->qty, _$_temp_vector, false, BetterString);
    /*
    Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet)    */
    int i;

    for (i=0;i<result->qty;i++) {
        BetterString_$_BetterString_$_BetterString_$_(bstrcpy(result->entry[i]),_$_temp_betterstring);
        Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$_temp_vector, i, true,
                                                            _$_temp_betterstring,
                                                            false, _$_temp_vector_item, false, _$_mptr);
    }
    _$_mptr_prepare(_$_temp_vector,$_mptr_in);
    bstrListDestroy(result);
    return $_mptr_in;
}

IDENT_MPTR_RAW * BetterString_$_split_$_String(IDENT_MPTR_RAW *  b_, IDENT_MPTR_RAW *  s_, IDENT_MPTR_RAW * $_mptr_in)
{
    const_bstring b = b_->obj;
    String * s = s_->obj;
    bstring sBstring = bfromcstr(s->buffer);
    struct bstrList * result = bsplitstr(b,sBstring);

    _$_VARIABLE(_$_temp_vector);
    _$_VARIABLE(_$_temp_vector_item);
    _$_VARIABLE(_$_temp_betterstring);

    //printf("Parts %d ",result->qty);
    Vector_$_Vector_$_int(result->qty, _$_temp_vector, false, BetterString);
    /*
    Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet)    */
    int i;

    for (i=0;i<result->qty;i++) {
        BetterString_$_BetterString_$_BetterString_$_(bstrcpy(result->entry[i]),_$_temp_betterstring);
        Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$_temp_vector, i, true,
                                                            _$_temp_betterstring,
                                                            false, _$_temp_vector_item, false, _$_mptr);
    }
    _$_mptr_prepare(_$_temp_vector,$_mptr_in);
    bdestroy(sBstring);
    bstrListDestroy(result);
    return $_mptr_in;
}

IDENT_MPTR_RAW * BetterString_$_splits_$_String(IDENT_MPTR_RAW *  b_, IDENT_MPTR_RAW *  s_, IDENT_MPTR_RAW * $_mptr_in)
{
    const_bstring b = b_->obj;
    String * s = s_->obj;
    bstring sBstring = bfromcstr(s->buffer);
    struct bstrList * result = bsplits(b,sBstring);

    _$_VARIABLE(_$_temp_vector);
    _$_VARIABLE(_$_temp_vector_item);
    _$_VARIABLE(_$_temp_betterstring);

    //printf("Parts %d ",result->qty);
    Vector_$_Vector_$_int(result->qty, _$_temp_vector, false, BetterString);
    /*
    Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$v$_arr, _$v$_idx, _$v$_primIdx, _$v$_elem, _$v$_primElem,\
                                                    _$v$_mptr, _$v$_primRet, _$v$_typeRet)    */
    int i;

    for (i=0;i<result->qty;i++) {
        BetterString_$_BetterString_$_BetterString_$_(bstrcpy(result->entry[i]),_$_temp_betterstring);
        Vector_$_putObjectAtIndex_$_Generic_$$_$_Generic_$$(_$_temp_vector, i, true,
                                                            _$_temp_betterstring,
                                                            false, _$_temp_vector_item, false, _$_mptr);
    }
    _$_mptr_prepare(_$_temp_vector,$_mptr_in);
    bdestroy(sBstring);
    bstrListDestroy(result);
    return $_mptr_in;
}