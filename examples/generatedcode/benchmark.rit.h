typedef union _$__$_Last _$_Last;
union _$__$_Last {
    BaseType *pBaseType;
    Boolean pBoolean;
    Ternary pTernary;
    Stream pStream;
    String pString;
    Number *pNumber;
    Integer pInteger;
    Float pFloat;
    System pSystem;
};
Boolean isPrime_$_Integer(Integer n)
{
    Integer upperLimit;
    Integer i;
    _$_Last _$_prev;
    _$_prev.pBoolean = (n < 2);
    if (_$_prev.pBoolean) {;
        return false;
    }
    _$_prev.pBoolean = (n == 2);
    if (_$_prev.pBoolean) {;
        return true;
    }
    _$_prev.pBoolean = (n % 2 == 0);
    if (_$_prev.pBoolean) {;
        return false;
    }
    upperLimit = (Integer_$_sqrt(n)) + 1;
    for_$_Integer_$_Integer_$_Integer(i, 3, upperLimit, 2);
    _$_Last _$_prev;
    _$_prev.pBoolean = (n % i == 0);
    if (_$_prev.pBoolean) {;
        return false;
    }
}

return true;
}
