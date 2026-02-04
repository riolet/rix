#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Forward declarations

// String helper
char* str_concat(const char* a, const char* b) {
    char* result = malloc(strlen(a) + strlen(b) + 1);
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char* int_to_str(int n) {
    char* result = malloc(32);
    sprintf(result, "%d", n);
    return result;
}

// Struct definitions
// Function prototypes
bool isPrime(int n);

bool isPrime(int n) {
    if ((n < 2)) {
        return false;
    }
    if ((n == 2)) {
        return true;
    }
    if (((n % 2) == 0)) {
        return false;
    }
    int upperLimit = (int)sqrt((double)n);
    int i = 3;
    while ((i <= upperLimit)) {
        if (((n % i) == 0)) {
            return false;
        }
        i = (i + 2);
    }
    return true;
}

int main(int argc, char** argv) {
    int limit = 2000000;
    int noPrimes = 0;
    int n = 0;
    while ((n <= limit)) {
        if (isPrime(n)) {
            noPrimes = (noPrimes + 1);
        }
        n = (n + 1);
    }
    printf("%s\n", str_concat(str_concat(str_concat("pi(", int_to_str(limit)), ") = "), int_to_str(noPrimes)));
    return 0;
}