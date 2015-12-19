#include "rsl.h"
#include "benchmark.rit.h"
int main(int _$$_argc_, char **_$$_argv_)
{
    _$$_argc = _$$_argc_;
    _$$_argv = _$$_argv_;
    Integer lim;
    Integer numPrimes;
    Integer n;
    Integer i;
    lim = 2000000;
    numPrimes = 0;
    n = 0;
    for_$_Integer_$_Integer(i, 1, lim);
    _$_Last _$_prev;
    _$_prev.pBoolean = (isPrime_$_Integer(i));
    if (_$_prev.pBoolean) {;
        numPrimes = (numPrimes + 1);
    }
}

print_$_String(String_$_plus_$_String
               (String_$_stringlit("Number of primes in the interval [0,"),
                Integer_$_plus_$_String(lim,
                                        String_$_plus_$_Integer(String_$_stringlit("]: "),
                                                                numPrimes))));
return 0;
}
