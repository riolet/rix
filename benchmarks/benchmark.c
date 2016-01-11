#include <stdio.h>
#include <math.h>

int isPrime(int n)
{
    if (n < 2)
        return 0;
    else if (n == 2)
        return 1;
    else if (n % 2 == 0)
        return 0;

    int upperLimit = sqrt(n);
    int i = 3;
    while (i <= upperLimit) {
        if (n % i == 0)
            return 0;
        i += 2;
    }
    return 1;
}

int main()
{
    int noPrimes = 0, limit = 2000000, n;
    for (n = 0; n <= limit; n++) {
        if (isPrime(n))
            noPrimes++;
    }
    printf("pi(%d) = %d\n", limit, noPrimes);
    return 0;
}
