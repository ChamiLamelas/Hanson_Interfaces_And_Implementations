/*
Why is the remainder being defined as x - y * q implemented by taking
the remainder to be |y| - (|x| % |y|) when sgn(x) != sgn(y) and |x| % |y| != 0

In the calculator implementation, why again do we fill a character buffer
potentially going beyond the size and only after do we check if we passed it. It
seems like an error could be raised in the initial filling (accessing heap
memory beyond bounds).
*/

#include <limits.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Shows overflow
    printf("%ld\n", -LONG_MIN);
    return 0;
}
