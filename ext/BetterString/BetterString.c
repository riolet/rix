#include "ext/BetterString/BetterString.h"
#include "rsl/RSL_Vector.h"

NonPrimObj * BetterString_$_BetterString_$_BetterString_$_  (BetterString * b, NonPrimObj * $_mptr_in)
{
    return _$_returnAppointer($_mptr_in,b,BetterString_$_destructor_$_);
}


NonPrimObj * BetterString_$_BetterString_$_ (NonPrimObj * $_mptr_in)
{
    BetterString * b = calloc(1, sizeof(BetterString));
    return _$_returnAppointer($_mptr_in,b,BetterString_$_destructor_$_);
}

NonPrimObj *  BetterString_$_BetterString_$_String(NonPrimObj * s_, NonPrimObj * $_mptr_in)
{
    String * s = s_->obj;
    BetterString *  result = (BetterString * ) bfromcstr(s->buffer);
    debugPrintf("Creating %s from %s\n",$_mptr_in->debugName, s->buffer);
    return _$_returnAppointer($_mptr_in, result, BetterString_$_destructor_$_);
}

//NonPrimObj *  BetterString_$_plus_$_BetterString(NonPrimObj *  left, NonPrimObj *  right)
//{
//    NonPrimObj *  retval = bstrcpy(left);
//    if (bconcat(retval, right)) {
//        return retval;
//    } else {
//        criticalError(0, "BetterString * concatenation failure");
//    }
//}
//
//NonPrimObj *  BetterString_$_plus_$_int(NonPrimObj *  left, int right)
//{
//    return bformat("%s%i", left->data, right);
//}
//
//NonPrimObj *  int_$_plus_$_BetterString(int left, NonPrimObj *  right)
//{
//    return bformat("%i%s", left, right->data);
//}
//
//NonPrimObj *  BetterString_$_plus_$_float(NonPrimObj *  left, float right)
//{
//    return bformat("%s%f", left->data, right);
//}
//
//NonPrimObj *  float_$_plus_$_BetterString(float left, NonPrimObj *  right)
//{
//    return bformat("%f%s", left, right->data);
//}



NonPrimObj * BetterString_$_toString_$_(NonPrimObj *  b_, NonPrimObj * $_mptr_in)
{
    debugPrintf("Creating string  from %s and storing in %s\n",b_->debugName,$_mptr_in->debugName);
    BetterString * b = b_->obj;

    _$_mptr __attribute__ ((__cleanup__(_$_cleanup))) $_mptr_temp;
    IDENT_MPTR_INITIALIZE_RAW (&$_mptr_temp, xstr($_mptr_temp));

    NonPrimObj * s_ = String_$_String_$_(&$_mptr_temp);

    String *s = (String *) s_->obj;
    s->buffer = malloc(sizeof(char)*(b->slen+1));
    memcpy(s->buffer, b->data, b->slen);
    s->cap = b->mlen;
    s->length = b->slen;
    _$_object_ownership_transfer(&$_mptr_temp, $_mptr_in);
    return $_mptr_in;
}

//NonPrimObj *  BetterString_$_toLower_$_(NonPrimObj *  b)
//{
//    int result = btolower(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toLower_$_");
//    }
//}
//
//NonPrimObj *  BetterString_$_toUpper_$_(NonPrimObj *  b)
//{
//    int result = btoupper(b);
//    if (result==BSTR_OK) {
//        return (BetterString * ) b;
//    } else {
//        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toUpper_$_");
//    }
//}
//
void BetterString_$_destructor_$_(NonPrimObj *  b_)
{
    BetterString * b = (BetterString *) b_->obj;
    int result = bdestroy((BetterString * )b);
    if (result==BSTR_OK) {
        //Do nothing?
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterString_$_destructor_$_");
    }
}

NonPrimObj * BetterString_$_split_$_char(NonPrimObj *  b_, char c, NonPrimObj * $_mptr_in)
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
    _$_object_ownership_transfer(_$_temp_vector,$_mptr_in);
    bstrListDestroy(result);
    return $_mptr_in;
}

NonPrimObj * BetterString_$_split_$_String(NonPrimObj *  b_, NonPrimObj *  s_, NonPrimObj * $_mptr_in)
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
    _$_object_ownership_transfer(_$_temp_vector,$_mptr_in);
    bdestroy(sBstring);
    bstrListDestroy(result);
    return $_mptr_in;
}

NonPrimObj * BetterString_$_splits_$_String(NonPrimObj *  b_, NonPrimObj *  s_, NonPrimObj * $_mptr_in)
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
    _$_object_ownership_transfer(_$_temp_vector,$_mptr_in);
    bdestroy(sBstring);
    bstrListDestroy(result);
    return $_mptr_in;
}