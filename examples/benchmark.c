#include <math.h>

int isPrime(int n) {
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

int main() {
    int i;
    for (i = 1; i < 2000000; i++) {
        isPrime(i);
    }
    return 0;
}
