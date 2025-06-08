// prime.c
#include <math.h>
#include "prime.h"

/*
 * Return whether x is prime or not
 *
 * Returns:
 *    1  - prime
 *    0  - not prime
 *    -1 - undefined (i.e. x < 2)
 */

int isPrime(const int x)
{
    // return -1 if less than 2 :: undefined
    if (x < 2) return -1;

    // isolates 3, of which should return 1 :: prime
    if (x < 4) return 1;

    // if 2 cleanly divides x, return 0 :: not prime
    if ((x % 2) == 0) return 0;

    // start i at 3, while i is less than the sqrt of x rounded to the 
    // largest integer <=x, increment i by 2
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {

        // if x modulo i results in 0, return 0 :: not prime
        if ((x % i) == 0) {
            return 0;
        }
    }
    // otherwise return 1 :: prime
    return 1;
}
/*
 * Return the next prime after x, or x if x is prime
 */
int nextPrime(int x)
{
    // increment x until it returns 1 :: prime
    while (isPrime(x) != 1) {
        x++;
    }
    return x;
}


