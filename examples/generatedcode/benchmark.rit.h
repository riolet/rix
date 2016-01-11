typedef union _$__$_Last _$_Last;
union _$__$_Last {
    BaseType *pBaseType;
    bool pbool;
    Ternary pTernary;
    Stream pStream;
    String pString;
    Number *pNumber;
    int pint;
    float pfloat;
    System pSystem;
};
bool isPrime_$_int(int n)
{
    int upperLimit;
    int i;
    _$_Last _$_prev;
    _$_prev.pbool = (n < 2);
    if (_$_prev.pbool) {;
        return false;
    }
    _$_prev.pbool = (n == 2);
    if (_$_prev.pbool) {;
        return true;
    }
    _$_prev.pbool = (n % 2 == 0);
    if (_$_prev.pbool) {;
        return false;
    }
    upperLimit = (int_$_sqrt(n)) + 1;
    for_$_int_$_int_$_int(i, 3, upperLimit, 2);
    _$_Last _$_prev;
    _$_prev.pbool = (n % i == 0);
    if (_$_prev.pbool) {;
        return false;
    }
}

return true;
}
