#!/usr/bin/env python
from math import sqrt

def isPrime(n):
    if (n < 2): return False
    elif (n == 2): return True
    elif (n % 2 == 0): return False

    upper_limit = int(sqrt(n))
    i = 3
    while (i <= upper_limit):
        if (n % i == 0): return False
        i += 2

    return True

lim = 2000000
no_primes = 0
n = 0

while (n <= lim):
    if (isPrime(n)): no_primes += 1
    n += 1

print ("Number of primes in the interval [0,%d]: %d" % (lim, no_primes))
