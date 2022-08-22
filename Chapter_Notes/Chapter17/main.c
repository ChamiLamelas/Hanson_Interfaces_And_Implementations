#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
Multiplicative algorithm at the top of page 303 I believe
works assuming that str holds the digits of the number
from most significant at index 0 to least significant. This
contradicts what was stated at the beginning of the section that
least significant digits are put first.

x   = d0 + bd1 + b^2d2 + ....
    = d0 + b(d1 + b(d2 + ....))

Algorithm:

z = 0
z = 0*b + dn-1 = dn-1
z = (dn-1)*b + dn-2
...

Most significant digit is multiplied the most with b (which
is what we want).

I thought XP library is built on base = 2^8, what is the point
of the string conversion functions? Unrelated?

The functions: 

right_shift_demo();
left_shift_demo();
and_demo();

are an unsuccessful attempt to compare division and shifting,
turns out everything seems to be 0s. This would make sense if 
the compiler optimizes and chooses the faster of each pair: 

/ (2^x)     >> x
* (2^x)     << x
% 2         & 1

See: https://stackoverflow.com/a/6357114

Why is x mod (ULONG_MAX + 1) = 8 * sizeof(unsigned long) least 
significant bits of x

Don't see why x[i] needs to be cast to unsigned char if
it's just a bit flip (range to 00000000 to 11111111)

P 309 - assume this should be z = xy not z + xy

For XP_mul, it has at most n + m digits in result (can
have n + m - 1, take 123 x 45). Partial products have
increasing numbers of digits (not n). But, there are m
products.

No explanation for recursive algorithm.

What is q->digits? 

Why do we need a leading 0 for long division? Have we not
guaranteed that the length of y in digits cannot exceed that
of x

Why can we estimate qk from division of 3 digit prefix of
rem and 2 digit prefix of y?

Example: 0476123 / 159

047 / 15 = 3 

But, 159 * 3 = 477 (so it should be 2 as first digit in quotient)

Logical vs. Arithmetic right shift: 
https://stackoverflow.com/questions/44694957/the-difference-between-logical-shift-right-arithmetic-shift-right-and-rotate-r
*/

typedef unsigned long T;
const T START = ULONG_MAX;
#define NUM_POWERS 7
unsigned powers[NUM_POWERS] = {1, 2, 3, 4, 5, 6, 7};
#define NUM_RANDS 100000
const unsigned MULTIPLIER = 1000;

static void right_shift_demo() {
    T itr;
    time_t t;
    for (int i = 0; i < NUM_POWERS; i++) {
        itr = START;
        t = time(NULL);
        for (int j = 0; j < MULTIPLIER; j++) {
            while (itr > 0) {
                itr /= pow(2, powers[i]);
            }
        }

        t = time(NULL) - t;
        printf("/ %u %ld s\n", powers[i], t);
    }
    for (int i = 0; i < NUM_POWERS; i++) {
        itr = START;
        t = time(NULL);
        for (int j = 0; j < MULTIPLIER; j++) {
            while (itr > 0) {
                itr >>= powers[i];
            }
        }
        t = time(NULL) - t;
        printf(">> %u %ld s\n", powers[i], t);
    }
}

static void left_shift_demo() {
    T itr;
    time_t t;
    unsigned p;
    for (int i = 0; i < NUM_POWERS; i++) {
        itr = 1;
        p = pow(2, powers[i]);
        t = time(NULL);
        for (int j = 0; j < MULTIPLIER; j++) {
            while (itr < START / p) {
                itr *= p;
            }
        }
        t = time(NULL) - t;
        printf("* %u %ld s\n", powers[i], t);
    }
    for (int i = 0; i < NUM_POWERS; i++) {
        itr = 1;
        p = pow(2, powers[i]);
        t = time(NULL);
        for (int j = 0; j < MULTIPLIER; j++) {
            while (itr < START / p) {
                itr <<= powers[i];
            }
        }
        t = time(NULL) - t;
        printf("<< %u %ld s\n", powers[i], t);
    }
}

static void and_demo() {
    int rands[NUM_RANDS];
    srand(1);
    for (int i = 0; i < NUM_RANDS; i++) {
        rands[i] = rand();
    }

    time_t t;
    int out;
    t = time(NULL);
    for (int j = 0; j < MULTIPLIER; j++) {
        for (int i = 0; i < NUM_RANDS; i++) {
            out = rands[i] % 2;
        }
    }
    t = time(NULL) - t;
    printf("%% %ld s\n", t);
    t = time(NULL);
    for (int j = 0; j < MULTIPLIER; j++) {
        for (int i = 0; i < NUM_RANDS; i++) {
            out = rands[i] & 1;
        }
    }
    t = time(NULL) - t;
    printf("& %ld s\n", t);
}

int main(int argc, char *argv[]) {
    
    return 0;
}
