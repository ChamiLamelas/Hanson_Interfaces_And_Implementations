#include <limits.h>
#include <stdio.h>

/*
What is going on in code blocks 379 and 380? What is carry? Why are we xoring
with fill? Why propogating carry?

p. 384, assume underflow should be overflow

Why is this the overflow check for multiplication: 

assert(sx != sy || sign(z) == 0);

What restrictions does the y copy to a temporary (p 382) bypass? This means y will have a unique
address compared to x and z, but x and z could still have the same address.. 

Why do we do the memset in fromstr? 

Why do we do the memcpy in tostr? 
*/
int main(int argc, char *argv[]) {
    printf("%lu %lu\n", ~0UL, ULONG_MAX);
    return 0;
}
