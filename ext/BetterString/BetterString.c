#include "ext/BetterString/BetterString.h"

BetterString BetterString_$_BetterString_$_String(String s)
{
    return bfromcstr(s.buffer);
}

BetterString BetterString_$_stringlit(char *strlit)
{
    return bfromcstr(strlit);
}

BetterString BetterString_$_assign_$_BetterString(BetterString left, BetterString right)
{
    if (bassign(left, right))
        return left;
    else
        criticalError(0, "Unable to assign BetterString %s\n");
}

BetterString BetterString_$_plus_$_BetterString(BetterString left, BetterString right)
{
    BetterString retval = bstrcpy(left);
    if (bconcat(retval, right)) {
        return retval;
    } else {
        criticalError(0, "BetterString concatenation failure");
    }
}

BetterString BetterString_$_plus_$_Integer(BetterString left, int right)
{
    return bformat("%s%i", left->data, right);
}

BetterString Integer_$_plus_$_BetterString(int left, BetterString right)
{
    return bformat("%i%s", left, right->data);
}

BetterString BetterString_$_plus_$_Float(BetterString left, float right)
{
    return bformat("%s%f", left->data, right);
}

BetterString Float_$_plus_$_BetterString(float left, BetterString right)
{
    return bformat("%f%s", left, right->data);
}

String BetterString_$_toString(BetterString b)
{
    String s;
    s.buffer = b->data;
    s.cap = b->mlen;
    s.length = b->slen;
    return s;
}
