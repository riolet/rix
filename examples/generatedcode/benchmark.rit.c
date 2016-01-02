#include "rsl.h"
#include "benchmark.rit.h"
int main(int _$$_argc_, char **_$$_argv_)
{
    _$$_argc = _$$_argc_;
    _$$_argv = _$$_argv_;
    int lim;
    int numPrimes;
    int n;
    int i;
    lim = 2000000;
    numPrimes = 0;
    n = 0;
    for_$_int_$_int(i, 1, lim);
    _$_Last _$_prev;
    _$_prev.pbool = (isPrime_$_int(i));
    if (_$_prev.pbool) {;
        numPrimes = (numPrimes + 1);
    }
}

print_$_String(String_$_plus_$_String
               (String_$_stringlit("Number of primes in the interval [0,"),
                int_$_plus_$_String(lim,
                                        String_$_plus_$_int(String_$_stringlit("]: "),
                                                                numPrimes))));
return 0;
}
