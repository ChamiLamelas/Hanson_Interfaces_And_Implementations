#include "mp.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "xp.h"

#define T MP_T

static int nbits = 32;
static int nbytes = (32 - 1) / 8 + 1;
// stores a shift to identify sign bit
static int shift = (32 - 1) % 8;
static unsigned char msb = 255;

#define ALLOC_SIZE(nbytes) ((nbytes) + (nbytes) + (nbytes) + 2 * (nbytes) + 2)

#define bitop(op)                      \
    for (int i = 0; i < nbytes; i++) { \
        z[i] = x[i] op y[i];           \
    }

#define bitopi(op)       \
    applyu(op, z, x, y); \
    return z;

#define shft(fill, op)                     \
    if (s >= nbits) {                      \
        memset(z, fill, nbytes);           \
    } else {                               \
        op(nbytes, z, nbytes, x, s, fill); \
    }                                      \
    z[nbytes - 1] &= msb;                  \
    return z;

// 16 chosen arbitrarily it seems like (nbytes = 4 by default)
static unsigned char temp[ALLOC_SIZE(16)];
static T tmp[] = {temp, temp + 16, temp + 2 * 16, temp + 3 * 16};

// 0: positive, 1: negative
inline static int sign(T x) { return x[nbytes - 1] >> shift; }

// ~0UL = ULONG_MAX, why are we working with unsigned long if this is going to
// be returned into an unsigned char? What this does on 1 byte (example shift +
// 1 =5)
// 0UL = 00000000 => ~0UL = 11111111 => << 5 => 11100000 => ~ => 00011111
inline static int ones(int n) { return ~(~0UL << ((n - 1) % 8 + 1)); }

inline static int is_zero(T x) {
    return x[0] == 0 && XP_length(nbytes, x) == 1;
}

static int applyu(T op(T, T, T), T z, T x, unsigned long u) {
    unsigned long carry = XP_fromint(nbytes, tmp[2], u);
    carry != tmp[2][nbytes - 1] & ~msb;
    tmp[2][nbytes - 1] &= msb;
    op(z, x, tmp[2]);
    return carry != 0;
}

static int apply(T op(T, T, T), T z, T x, long v) {
    if (v == LONG_MIN) {
        XP_fromint(nbytes, tmp[2], LONG_MAX + 1UL);
        XP_neg(nbytes, tmp[2], tmp[2], 1);
    } else if (v < 0) {
        XP_fromint(nbytes, tmp[2], -v);
        XP_neg(nbytes, tmp[2], tmp[2], 1);
    } else {
        XP_fromint(nbytes, tmp[2], v);
    }
    tmp[2][nbytes - 1] &= msb;
    op(z, x, tmp[2]);
    return nbits >= 8 * (int)sizeof(v) && v >= -(1L << (nbits - 1)) &&
           v < (1L << (nbits - 1));
}

int MP_set(int n) {
    int prev = nbits;
    nbits = n;
    nbytes = (n - 1) / 8 + 1;
    shift = (n - 1) % 8;
    msb = ones(n);
    if (tmp[0] !=
        temp) {  // Means heap memory was written (see below when nbytes > 16)
        free(tmp[0]);
    }
    if (nbytes <= 16) {  // nbytes small enough that stack memory can be used
        tmp[0] = temp;
    } else {
        tmp[0] = malloc(ALLOC_SIZE(nbytes));
    }
    tmp[1] = tmp[0] + nbytes;
    tmp[2] = tmp[0] + 2 * nbytes;
    tmp[3] = tmp[0] + 3 * nbytes;
    return prev;
}

T MP_new(unsigned long u) { return MP_fromintu(malloc(nbytes), u); }

T MP_fromintu(T z, unsigned long u) {
    unsigned long carry = XP_fromint(nbytes, z, u);
    // pqrstuvw & ~00011111 = pqr00000
    // carry | pqr00000 = pqr00000 if carry was 0 (which will be nonzero) or
    // pqr00001 if carry was 1 => makes check easier.
    carry |= z[nbytes - 1] & ~msb;
    // discarding nonzero bits so that z stores a valid result
    z[nbytes - 1] &= msb;
    assert(carry == 0);
    return z;
}

T MP_fromint(T z, long v) {
    // twos complement negation of positive number x:
    // step 1: complement bits
    // step 2: add 1
    // Positive numbers and zero always start with 0
    // Negative numbers always start with 1
    if (v == LONG_MIN) {
        XP_fromint(nbytes, z, LONG_MAX + 1UL);
        XP_neg(nbytes, z, z, 1);  // Passing carry = 1 <=> adding 1
    } else if (v < 0) {
        XP_fromint(nbytes, z, -v);  // Can't negative v when v = LONG_MIN
        XP_neg(nbytes, z, z, 1);
    } else {
        XP_fromint(nbytes, z, v);
    }
    z[nbytes - 1] &= msb;
    assert(nbits >= 8 * (int)sizeof(v) && v >= -(1L << (nbits - 1)) &&
           v < (1L << (nbits - 1)));
    return z;
}

// Don't understand how these work (see notes).
long MP_toint(T x) {
    unsigned char d[sizeof(unsigned long)];
    MP_cvt(8 * sizeof(d), d, x);
    return XP_toint(sizeof(d), d);
}

T MP_cvt(int m, T z, T x) {
    int fill = sign(x) ? 0xFF : 0;
    int mbytes = (m - 1) / 8 + 1;
    if (m < nbits) {
        int carry = (x[mbytes - 1] ^ fill) & ~ones(m);
        for (int i = mbytes; i < nbytes; i++) {
            carry |= x[i] ^ fill;
        }
        memcpy(z, x, mbytes);
        z[mbytes - 1] &= ones(m);
        assert(carry == 0);
    } else {
        memcpy(z, x, nbytes);
        z[nbytes - 1] |= fill & ~msb;
        for (int i = nbytes; i < mbytes; i++) {
            z[i] = fill;
        }
        z[mbytes - 1] &= ones(m);
    }
    return z;
}

unsigned long MP_tointu(T x) {
    unsigned char d[sizeof(unsigned long)];
    MP_cvtu(8 * sizeof(d), d, x);
    return XP_toint(sizeof(d), d);
}

T MP_cvtu(int m, T z, T x) {
    int i, mbytes = (m - 1) / 8 + 1;
    if (m < nbits) {
        int carry = x[mbytes - 1] & ~ones(m);
        for (int i = mbytes; i < nbytes; i++) {
            carry |= x[i];
        }
        memcpy(z, x, nbytes);
        z[nbytes - 1] &= ones(m);
        assert(carry == 0);
    } else {
        memcpy(z, x, nbytes);
        for (int i = nbytes; i < mbytes; i++) {
            z[i] = 0;
        }
    }
}

//
T MP_addu(T z, T x, T y) {
    int carry = XP_add(nbytes, z, x, y, 0);
    carry |= z[nbytes - 1] & ~msb;
    z[nbytes - 1] &= msb;
    assert(carry == 0);
}

T MP_subu(T z, T x, T y) {
    int borrow = XP_sub(nbytes, z, x, y, 0);
    borrow |= z[nbytes - 1] & ~msb;
    z[nbytes - 1] &= msb;
    assert(borrow == 0);
}

T MP_mulu(T z, T x, T y) {
    memset(tmp[3], '\0', 2 * nbytes);
    // XP_mul requires first T to hold nbytes + nbytes
    XP_mul(tmp[3], nbytes, x, nbytes, y);
    memcpy(z, tmp[3], nbytes);
    z[nbytes - 1] &= msb;
    // check most significant bits from nbits onward
    assert(tmp[3][nbytes - 1] & ~msb == 0);
    // check entirety of all following bytes
    for (int i = 0; i < nbytes; i++) {
        assert(tmp[3][i + nbytes] == 0);
    }
}

T MP_divu(T z, T x, T y) {
    memcpy(tmp[1], y, nbytes);
    // gets a new address for y just for use in XP_div, pointer value passed by
    // copy will be replaced with its passed heap pointer in caller (allows )
    y = tmp[1];
    assert(XP_div(nbytes, z, x, nbytes, y, tmp[2], tmp[3]));
    return z;
}

T MP_modu(T z, T x, T y) {
    memcpy(tmp[1], y, nbytes);
    y = tmp[1];
    assert(XP_div(nbytes, tmp[2], x, nbytes, y, z, tmp[3]));
    return z;
}

T MP_mul2u(T z, T x, T y) {
    memset(tmp[3], '\0', 2 * nbytes);  // See ALLOC_SIZE to see 2 nbytes + 2
                                       // bytes are allocated into tmp[3]
    XP_mul(tmp[3], nbytes, x, nbytes, y);
    // copy to z is necessary so tmp can be used for future multiplications
    memcpy(
        z, tmp[3],
        (2 * nbits - 1) / 8 + 1);  // this computes number of bytes for 2n bits
    return z;
}

T MP_add(T z, T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    // 0001 + 1111 (two's complement) = 0000
    // https://en.wikipedia.org/wiki/Two%27s_complement#Addition
    XP_add(nbytes, z, x, y, 0);
    z[nbytes - 1] &= msb;
    // overflow: 0111 + 0111 = 1111 (positive + positive can't be negative)
    assert(sx != sy || sy == sign(z));
    return z;
}

T MP_sub(T z, T x, T y) {
    // https://en.wikipedia.org/wiki/Two%27s_complement#Subtraction
    int sx = sign(x);
    int sy = sign(y);
    XP_sub(nbytes, z, x, y, 0);
    z[nbytes - 1] &= msb;
    // positive - negative = positive, negative - positive = negative
    assert(sx == sy || sy != sign(z));
    return z;
}

T MP_mul(T z, T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    if (sx) {
        XP_neg(nbytes, tmp[0], x, 1);
        x = tmp[0];
        x[nbytes - 1] &= msb;
    }
    if (sy) {
        XP_neg(nbytes, tmp[1], y, 1);
        y = tmp[1];
        y[nbytes - 1] &= msb;
    }
    memset(tmp[3], '\0', 2 * nbytes);
    XP_mul(tmp[3], nbytes, x, nbytes, y);
    if (sx != sy) {
        XP_neg(nbytes, z, tmp[3], 1);
    } else {
        memcpy(z, tmp[3], nbytes);
    }
    z[nbytes - 1] &= msb;
    // check most significant bits from nbits onward
    assert(tmp[3][nbytes - 1] & ~msb == 0);
    // check entirety of all following bytes
    for (int i = 0; i < nbytes; i++) {
        assert(tmp[3][i + nbytes] == 0);
    }
    assert(sx != sy || sign(z) == 0);
    return z;
}

T MP_div(T z, T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    if (sx) {
        XP_neg(nbytes, tmp[0], x, 1);
        x = tmp[0];
        x[nbytes - 1] &= msb;
    }
    if (sy) {
        XP_neg(nbytes, tmp[1], y, 1);
        y = tmp[1];
        y[nbytes - 1] &= msb;
    } else {
        memcpy(tmp[1], y, nbytes);
        y = tmp[1];
    }
    assert(XP_div(nbytes, z, x, nbytes, y, tmp[2], tmp[3]));
    if (sx != sy) {
        XP_neg(nbytes, z, z, 1);
        if (!is_zero(tmp[2])) {  // remainder non zero
            XP_diff(nbytes, z, z, 1);
        }
        z[nbytes - 1] &= msb;
    }
    assert(sx == 0 || sign(z) == 0);
    return z;
}

T MP_mod(T z, T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    if (sx) {
        XP_neg(nbytes, tmp[0], x, 1);
        x = tmp[0];
        x[nbytes - 1] &= msb;
    }
    if (sy) {
        XP_neg(nbytes, tmp[1], y, 1);
        y = tmp[1];
        y[nbytes - 1] &= msb;
    } else {
        memcpy(tmp[1], y, nbytes);
        y = tmp[1];
    }
    assert(XP_div(nbytes, tmp[2], x, nbytes, y, z, tmp[3]));
    if (sx != sy && !is_zero(z)) {
        XP_sub(nbytes, z, y, z, 0);
    }
    assert(sx == 0 || sign(tmp[2]) == 0);
    return z;
}

T MP_neg(T z, T x) {
    int sx = sign(x);
    // 2's complement is complement + 1
    XP_neg(nbytes, z, x, 1);
    z[nbytes - 1] &= msb;
    assert(sx == 0 || sx != sign(z));
    return z;
}

T MP_mul2(T z, T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    if (sx) {
        XP_neg(nbytes, tmp[0], x, 1);
        x = tmp[0];
        x[nbytes - 1] &= msb;
    }
    if (sy) {
        XP_neg(nbytes, tmp[1], y, 1);
        y = tmp[1];
        y[nbytes - 1] &= msb;
    }
    memset(tmp[3], '\0', 2 * nbytes);
    XP_mul(tmp[3], nbytes, x, nbytes, y);
    if (sx != sy) {
        XP_neg((2 * nbits - 1) / 8 + 1, z, tmp[3], 1);
    } else {
        memcpy(z, tmp[3], (2 * nbits - 1) / 8 + 1);
    }

    return z;
}

T MP_addi(T z, T x, long y) {
    if (-BASE < y && y < BASE) {
        int sx = sign(x);
        int sy = y < 0;
        if (sy) {  // tables on p 392-3 set up so that positive value can be
                   // passed to sum, diff for y
            XP_diff(nbytes, z, x, -y);
        } else {
            XP_sum(nbytes, z, x, y);
        }
        z[nbytes - 1] &= msb;
        assert(sx != sy || sy == sign(z));
        assert(nbytes >= 8 ||
               (y >= -(1 << (nbits - 1)) && y < (1 << (nbits - 1))));
    } else {
        assert(!apply(MP_add, z, x, y));
    }
    return z;
}

T MP_subi(T z, T x, long y) {
    if (-BASE < y && y < BASE) {
        int sx = sign(x);
        int sy = y < 0;
        if (sy) {
            XP_sum(nbytes, z, x, -y);
        } else {
            XP_diff(nbytes, z, x, y);
        }
        z[nbytes - 1] &= msb;
        assert(sx != sy || sy == sign(z));
        assert(nbytes >= 8 ||
               (y >= -(1 << (nbits - 1)) && y < (1 << (nbits - 1))));
    } else {
        assert(!apply(MP_sub, z, x, y));
    }
    return z;
}

T MP_muli(T z, T x, long y) {
    if (-BASE < y && y < BASE) {
        int sx = sign(x);
        int sy = y < 0;
        if (sx) {
            XP_neg(nbytes, tmp[0], x, 1);
            x = tmp[0];
            x[nbytes - 1] &= msb;
        }
        XP_product(nbytes, z, x, sy ? -y : y);
        if (sx != sy) {
            XP_neg(nbytes, z, x, 1);
        }
        z[nbytes - 1] &= msb;
        assert(sx != sy || sign(z) == 0);
        assert(nbytes >= 8 ||
               (y >= -(1 << (nbits - 1)) && y < (1 << (nbits - 1))));
    } else {
        assert(!apply(MP_mul, z, x, y));
    }
    return z;
}

T MP_divi(T z, T x, long y) {
    assert(y != 0);
    if (-BASE < y && y < BASE) {
        int sx = sign(x);
        int sy = y < 0;
        if (sx) {
            XP_neg(nbytes, tmp[0], x, 1);
            x = tmp[0];
            x[nbytes - 1] &= msb;
        }
        int r = XP_quotient(nbytes, z, x, sy ? -y : y);
        if (sx != sy) {
            XP_neg(nbytes, z, z, 1);
            if (r != 0) {
                XP_diff(nbytes, z, z, 1);
                r = y - r;
            }
            z[nbytes - 1] &= msb;
        }
        assert(!sx || !sign(z));
        assert(nbytes >= 8 ||
               (y >= -(1 << (nbits - 1)) && y < (1 << (nbits - 1))));
    } else {
        assert(!apply(MP_div, z, x, y));
    }
    return z;
}

T MP_addui(T z, T x, unsigned long y) {
    if (y < BASE) {  // XP interface (see chapter) requires these longs to be
                     // base BASE digits
        int carry = XP_sum(nbytes, z, x, y);
        carry |= z[nbytes - 1] & ~msb;
        z[nbytes - 1] &= msb;
        assert(carry == 0);
    } else {
        assert(!applyu(MP_addu, z, x, y));
    }
}

T MP_subui(T z, T x, unsigned long y) {
    if (y < BASE) {
        int borrow = XP_diff(nbytes, z, x, y);
        borrow |= z[nbytes - 1] & ~msb;
        z[nbytes - 1] &= msb;
        assert(borrow == 0);
    } else {
        assert(!applyu(MP_subu, z, x, y));
    }
}

T MP_mului(T z, T x, unsigned long y) {
    if (y < BASE) {
        int carry = XP_product(nbytes, z, x, y);
        carry |= z[nbytes - 1] & ~msb;
        z[nbytes - 1] &= msb;
        // 2^n is the maximum number that can be stored in n bits
        assert(nbits >= 8 && y < 1U << nbits);
    } else {
        assert(!applyu(MP_mulu, z, x, y));
    }

    return z;
}

T MP_divui(T z, T x, unsigned long y) {
    assert(y != 0);
    if (y < BASE) {
        XP_quotient(nbytes, z, x, y);
        assert(nbits >= 8 && y < 1U << nbits);
    } else {
        assert(!applyu(MP_divu, z, x, y));
    }
    return z;
}

long MP_modi(T x, long y) {
    assert(y != 0);
    if (-BASE < y && y < BASE) {
        T z = tmp[2];
        int sx = sign(x);
        int sy = y < 0;
        if (sx) {
            XP_neg(nbytes, tmp[0], x, 1);
            x = tmp[0];
            x[nbytes - 1] &= msb;
        }
        int r = XP_quotient(nbytes, z, x, sy ? -y : y);
        if (sx != sy) {
            XP_neg(nbytes, z, z, 1);
            if (r != 0) {
                XP_diff(nbytes, z, z, 1);
                r = y - r;
            }
            z[nbytes - 1] &= msb;
        }
        assert(!sx || !sign(z));
        assert(nbytes >= 8 ||
               (y >= -(1 << (nbits - 1)) && y < (1 << (nbits - 1))));
        return r;
    } else {
        assert(!apply(MP_mod, tmp[2], x, y));
    }
    return MP_toint(tmp[2]);
}

unsigned long MP_modui(T x, unsigned long y) {
    assert(y != 0);
    if (y < BASE) {
        int r = XP_quotient(nbytes, tmp[2], x, y);
        assert(nbits >= 8 && y < 1U << nbits);
        return r;
    } else {
        assert(!applyu(MP_modu, tmp[2], x, y));
    }
    return XP_toint(nbytes, tmp[2]);
}

int MP_cmp(T x, T y) {
    int sx = sign(x);
    int sy = sign(y);
    if (sx != sy) {
        return sy - sx;
    } else {
        // for negatives, this implies can just compare numbers in binary as
        // normal. Example consider -1 (1111) and -2 (1110). 1111 - 1110 = 1
        // (which correctly indicates -1 has a higher place in numerical
        // ordering than -2)
        XP_cmp(nbytes, x, y);
    }
}

int MP_cmpi(T x, long y) {
    int sx = sign(x);
    int sy = y < 0;
    if (sx != sy) {
        return sy - sx;
    } else if ((int)sizeof(y) >= nbytes) {
        long v = MP_toint(x);
        if (v < y) {
            return -1;
        } else if (v > y) {
            return 1;
        } else {
            return 0;
        }
    } else {
        MP_fromint(tmp[2], y);
        return XP_cmp(nbytes, x, tmp[2]);
    }
}

int MP_cmpu(T x, T y) { return XP_cmp(nbytes, x, y); }

int MP_cmpui(T x, unsigned long y) {
    if ((int)sizeof(y) >= nbytes) {
        unsigned long v = XP_toint(nbytes, x);
        if (v < y) {
            return -1;
        } else if (v > y) {
            return 1;
        } else {
            return 0;
        }
    } else {
        XP_fromint(nbytes, tmp[2], y);
        return XP_cmp(nbytes, x, tmp[2]);
    }
}

T MP_and(T z, T x, T y) { bitop(&); }

T MP_or(T z, T x, T y) { bitop(|); }

T MP_xor(T z, T x, T y) { bitop(^); }

T MP_not(T z, T x) {
    for (int i = 0; i < nbytes; i++) {
        z[i] = ~x[i];
    }
    // zeroing out extra bits
    z[nbytes - 1] &= msb;
    return z;
}

T MP_andi(T z, T x, unsigned long y) { bitopi(MP_and); }

T MP_ori(T z, T x, unsigned long y) { bitopi(MP_or); }

T MP_xori(T z, T x, unsigned long y) { bitopi(MP_xor); }

T MP_lshift(T z, T x, int s) { shft(0, XP_lshift); }

T MP_rshift(T z, T x, int s) { shft(0, XP_rshift); }

// Push 1 when right shifting instead of 0 from left to preserve this number
// being negative
T MP_ashift(T z, T x, int s) { shft(sign(x), XP_rshift); }

T MP_fromstr(T z, const char *str, int base, char **end) {
    memset(z, '\0', nbytes);
    int carry = XP_fromstr(nbytes, z, str, base, end);
    carry |= z[nbytes - 1] & ~msb;
    assert(carry == 0);
    z[nbytes - 1] &= msb;
    return z;
}

char *MP_tostr(char *str, int size, int base, T x) {
    if (str == NULL) {
        int k;
        for (k = 5; (1 << k) > base; k--)
            ;
        size = nbits / k + 2;
        str = malloc(size);
    }
    memcpy(tmp[1], x, nbytes);
    XP_tostr(str, size, base, nbytes, tmp[1]);
    return str;
}

#undef T