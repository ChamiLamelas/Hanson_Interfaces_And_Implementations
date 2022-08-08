#include <stdio.h>

/*
Multiplicative algorithm at the top of page 303 I believe
works assuming that str holds the digits of the number
from most significant at index 0 to least significant

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
*/

int main(int argc, char *argv[])
{
    printf("%s", "Hello, World!");
    return 0;
}
