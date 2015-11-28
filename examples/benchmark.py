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

i = 1
while i < 2000000:
  isPrime(i)
  i += 1
