/*
16 possible nibbles = 2^4 = 16 possible configurations of 4 bits.
That is, each nibble holds 0...15. Derivation of counts array done
using following table.

Base 2      Count of 1s         Base 10
0000        0                   0
0001        1                   1
0010        1                   2
0100        1                   4
1000        1                   8
0011        2                   3
0110        2                   6
1100        2                   12
0101        2                   5
1010        2                   10
1001        2                   9
1110        3                   14
1101        3                   13
1011        3                   11
0111        3                   7
1111        4                   15

For counts[i], look up i in Base 10 column and put count of 1s.

counts = {
    0,1,1,2,1,2,2,3,1,2,2,3,2,2,3,2,3,3,4
}

As in the PDF.

Byte splitting:

c&0xF

Let c have binary representation pqrstuvw

0xF = 15, or in binary: 00001111

  00001111
& pqrstuvw

= 0000tuvw

c >> 4

pqrstuvw >> 4 = 0000pqrs

Hence we end up with pqrs and tuvw which then in base 10 can be used
to index counts constructed as above.

Furthermore, in nbytes and nwords, he computes ceil(a/b) as
((a+b-1)&(~(b-1)))/b

I was not able to find anywhere online why you would do this and not
use a provided utility. Furthermore, I did find that this is sufficient:

ceil(a/b) = (a+b-1)//b where // denotes integer division.

Why the extra bitwise ops?

Proof of above:

Let a, b in 0,1,...,255
Suppose a = ub + v
Where u is 0,1,2,... and v in 0,1,...,b-1

Case 1: v = 0

a = ub
ceil(a/b) = ceil(ub/b) = ceil(u) = u
(a+b-1)//b = (ub+b-1)//b = u because ub + b - 1 is guaranteed < (u+1)b

Case 2: v = 1,...,b-1

a = ub + v
ceil(a/b) = ceil((ub+v)/b) = u + 1
(a+b-1)//b = (ub+v+b-1)//b = u + 1 because v+b-1 is guaranteed to be at
least b but less than 2b

To understand chunk 205, simple example:

2 bytes arranged like this.

Byte 0: Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
Byte 1: Bit15   Bit14   Bit13   Bit12   Bit11   Bit10   Bit9    Bit8

Let n = 10

10/8 = 1, 10 % 8 = 2

Right shift Byte 1 twice, get:

0   0   Bit15   Bit14   Bit13   Bit12   Bit11   Bit10

Let x have binary representation pqrstuvw

x & 1:

    pqrstuvw
&   00000001

    0000000w

Hence we get Bit 10 from chunk 205.

Explanation of Function 203 (Bit_put):

1 << 2 = 00000001 << 2 = 00000100

x | (1<<2) -> Let x have binary representation pqrstuvw

    pqrstuvw
|   00000100

    pqrst1vw

x & ~(1<<2) -> Let x have binary representation pqrstuvw

    pqrstuvw
& ~(00000100)

    pqrstuvw
&   11111011

    pqrst0vw

Most significant masks, taken from output of below code:

Let x have binary representation pqrstuvw

Mask                    Mask | x

0xFF = 255 = 11111111   11111111
0xFE = 254 = 11111110   1111111w
0xFC = 252 = 11111100   111111vw
0xF8 = 248 = 11111000   11111uvw
0xF0 = 240 = 11110000   1111tuvw
0xE0 = 224 = 11100000   111stuvw
0xC0 = 192 = 11000000   11rstuvw
0x80 = 128 = 10000000   1qrstuvw

Does this table make sense? consider lo = 3, then lo % 8 = 3 which means if we have x = pqrstuvw,
we want pqrst to be replaced with 1. Mask 3: 0xF8 = 248 = 11111000 will do that.

Least significant masks, taken from output of below code:

Let x have binary representation pqrstuvw

Mask                    Mask | x

0x1 = 1     = 00000001  pqrstuv1
0x3 = 3     = 00000011  pqrstu11
0x7 = 7     = 00000111  pqrst111
0xF = 15    = 00001111  pqrs1111
0x1F = 31   = 00011111  pqr11111
0x3F = 63   = 00111111  pq111111
0x7F = 127  = 01111111  p1111111
0xFF = 255  = 11111111  11111111

Does this table make sense? consider hi = 54. hi % 8 = 6. If x = pqrstuvw, we want least
significant bits 0-6 (in this case bits 48-54) to be set to 1. Mask 6: 0x7F = 127  = 01111111
does just that.

Note these tables require 0xFF entries because bytes lo/8 and hi/8 are handled separately
regardless of whether or not they are also entirely being set to 1s.

Explanation for chunk 208: 

How to get mask for lo%8 to hi%8 when these bit positions fall in the same byte? 

Get the mask as if we are doing the most significant bits in the byte starting with lo%8

Something like 1......10...

Get the mask as if we are doing the least significant bits in the byte going up to hi%8

Something like 0..01......1

How do we get mask that is like 0..01..10...? 

We do & on the previous two masks. This resulting mask when combined with stored bits using
| will yield 1s in the desired positions.

Explanation for Bit_clear:

Let x be a number, and m a mask. Recall from function 203 that:

x | m -> 1 for all bits where m has 1, and the bit from w where m has 0
x & (~m) -> 0 for all bits where m has 1, and the bit from w where m has 0

Instead of setting whole bytes to 0xFF, we set whole bytes to 0.

Explanation of Bit_not: 

Let b be in {0,1}. Then: 

b ^ 1 = 1 if b = 0 (only 1 operand is true)
b ^ 1 = 0 if b = 1 (both operands can't be true)

Hence, we can again reuse the masks from before by combining stored bits with the masks
using ^ instead of | and &(~ ) as before. Where there are 1s in the masks, we will be flipping
bits instead of setting to 1s or 0s. Like with Bit_set we use ^ combined with 0xFF = 11111111 
for flipping whole bytes.

Set Comparison Operations

What is the member u.words? Assume this is a typo.
Why does he iterate backwards in Set_eq, Set_leq? 

<= : subset of
* : intersect
0 : nullset

S <= T <=> S * Tc

Pf: S has no elements that are not in T
    => All S elements in T

Why can this be applied iteratively for this? Iteratively checking if intersect complement
is satisfied is basically doing this: 

Check if 0...7 subset of S is a subset of the 0...7 subset of T
Check if 8...15 subset of S is a subset of the 8...15 subset of T
... 

Etc.

Why in Bit_union does he copy(T) twice? Why not make a temporary variable. 
*/

#include <stdio.h>

#define BPW (8 * sizeof(unsigned long))
#define nwords(len) ((((len) + BPW - 1) & (~(BPW - 1))) / BPW)

unsigned char msbmask[] = {
    0xFF, 0xFE, 0xFC, 0xF8,
    0xF0, 0xE0, 0xC0, 0x80};

unsigned char lsbmask[] = {
    0x01, 0x03, 0x07, 0x0F,
    0x1F, 0x3F, 0x7F, 0xFF};

int main(int argc, char *argv[])
{
    int length1 = 60;
    int length2 = 64;

    printf("%d %d %d\n", BPW, nwords(length1), nwords(length2));

    int a = BPW - 1;
    printf("%d\n", ~a);

    //
    int c = 64;
    printf("%d %d\n", c & 0xF, c >> 4);

    for (int i = 0; i < 8; i++)
        printf("0x%X = %d\n", msbmask[i], msbmask[i]);

    for (int i = 0; i < 8; i++)
        printf("0x%X = %d\n", lsbmask[i], lsbmask[i]);

    return 0;
}
