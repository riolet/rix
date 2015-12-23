#include "ext/BetterString/BetterString.h"

BetterString  *  BetterString_$_BetterString_$_String(String * s)
{
    BetterString  *  result = (BetterString * ) bfromcstr(s->buffer);
    String_cleanUp(s);
    return result;
}

BetterString  *  BetterString_$_assign_$_BetterString(BetterString  *  left, BetterString  *  right)
{
    if (bassign(left, right))
        return left;
    else
        criticalError(0, "Unable to assign BetterString %s\n");
}

BetterString  *  BetterString_$_plus_$_BetterString(BetterString  *  left, BetterString  *  right)
{
    BetterString  *  retval = bstrcpy(left);
    if (bconcat(retval, right)) {
        return retval;
    } else {
        criticalError(0, "BetterString * concatenation failure");
    }
}

BetterString  *  BetterString_$_plus_$_Integer(BetterString  *  left, int right)
{
    return bformat("%s%i", left->data, right);
}

BetterString  *  Integer_$_plus_$_BetterString(int left, BetterString  *  right)
{
    return bformat("%i%s", left, right->data);
}

BetterString  *  BetterString_$_plus_$_Float(BetterString  *  left, float right)
{
    return bformat("%s%f", left->data, right);
}

BetterString  *  Float_$_plus_$_BetterString(float left, BetterString  *  right)
{
    return bformat("%f%s", left, right->data);
}

String * BetterString_$_toString_$_(BetterString  *  b)
{
    String * s = String_$_String_$_();
    s->buffer = malloc(sizeof(char)*(b->slen+1));
    memcpy(s->buffer, b->data, b->slen);
    s->cap = b->mlen;
    s->length = b->slen;
    s->isStored = StringStatusDelete;
    return s;
}

BetterString  *  BetterString_$_toLower_$_(BetterString  *  b)
{
    int result = btolower(b);
    if (result==BSTR_OK) {
        return (BetterString * ) b;
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toLower_$_");
    }
}

BetterString  *  BetterString_$_toUpper_$_(BetterString  *  b)
{
    int result = btoupper(b);
    if (result==BSTR_OK) {
        return (BetterString * ) b;
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterString_$_toUpper_$_");
    }
}

void BetterString_$_destructor_$_(BetterString  *  b)
{
    int result = bdestroy((BetterString * )b);
    if (result==BSTR_OK) {
        //Do nothing?
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterString_$_destructor_$_");
    }
}

BetterStringList * BetterString_$_split_$_Char(BetterString  *  b, char c)
{
    return bsplit(b,c);
}

BetterString  *  BetterStringList_$_getObjectAtIndex_$_Integer(BetterStringList  *  bList, Integer i)
{
 return bList->entry[i];
}

void BetterStringList_$_destructor_$_(BetterStringList * bList)
{
    int result = bstrListDestroy(bList);
    if (result==BSTR_OK) {
        //Do nothing?
    } else {
        criticalError(ERROR_RuntimeError,"Error in BetterStringList_$_destructor_$_");
    }
}

int BetterStringList_$_length_$_(BetterStringList * bList)
{
    return bList->qty;
}