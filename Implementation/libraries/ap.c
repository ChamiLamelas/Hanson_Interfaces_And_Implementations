#include "ap.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "xp.h"

#define T AP_T

struct T {
    int sign;     // 1 or -1
    int ndigits;  // Number digits in use (AP_Ts are normalized meaning that
                  // they don't have leading 0s)
    int size;     // Number digits pointed to by digits
    XP_T digits;
};

static T mk(int size) {
    T z = calloc(1, sizeof(*z) + size);
    assert(z);
    // What is the size assertion for, why is it after the CALLOC?
    z->sign = 1;
    z->size = size;
    z->ndigits = 1;
    z->digits = (XP_T)(z + 1);
    return z;
}

inline static void normalize(T z) {
    z->ndigits = XP_length(z->size, z->digits);
}

static T set(T z, long n) {
    if (n == LONG_MIN) {  // necessary b/c n < 0 block below wouldn't work since
                          // -LONG_MIN overflows to -LONG_MIN
        XP_fromint(z->size, z->digits, LONG_MAX + 1UL);
        z->sign = -1;
    } else if (n < 0) {
        XP_fromint(z->size, z->digits, -n);
        z->sign = -1;
    } else {
        XP_fromint(z->size, z->digits, n);
    }
    normalize(z);
    return z;
}

inline static int is_zero(T x) { return x->digits[0] == 0 && x->ndigits == 1; }

inline static int is_abs_one(T x) {
    return x->digits[0] == 1 && x->ndigits == 1;
}

// pre-condition (see AP_add, AP_sub) x could be larger or smaller than y
static void abs_add(T z, T x, T y) {
    int n = y->ndigits;
    if (x->ndigits < n) {
        // allowed since a + b = b + a
        abs_add(z, y, x);
    } else if (x->ndigits > n) {
        // Add along digits that match like if we're doing 175 + 35, this would
        // be the 75 + 35 part
        int carry = XP_add(n, z->digits, x->digits, y->ndigits, 0);
        // Then we do from the digits beyond the matching number of digits (1 +
        // carry from 75 + 35), put carry from this into the last slot of
        // z->digits
        z->digits[z->size - 1] = XP_sum(x->ndigits - n, z + n, x + n, carry);
    } else {  // z->size = n + 1
        z->digits[n] = XP_add(n, z->digits, x->digits, y->digits, 0);
    }
    normalize(z);
}

inline static int max_digits(T x, T y) {
    return x->ndigits > y->ndigits ? x->ndigits : y->ndigits;
}

inline static int abs_cmp(T x, T y) {
    return x->ndigits == y->ndigits ? XP_cmp(x->ndigits, x->digits, y->digits)
                                    : x->ndigits - y->ndigits;
}

// pre-condition (see AP_add, AP_sub): y never exceeds x
static void abs_sub(T z, T x, T y) {
    int n = y->ndigits;
    int borrow = XP_sub(n, z->digits, x->digits, y->digits, 0);

    // subtraction of borrow from more significant digits of x
    if (x->ndigits > n) {
        // If x->ndigits > n, since |x| > |y| always via pre-condition,
        // borrow will be at most 1 (see XP_sub implementation), then
        // result of XP_diff is guaranteed to be 0. If x has more digits
        // than y, that means it has at least a 1 at x + n. Any subtraction
        // from the more significant digits from x + n onwards will yield a
        // borrow of 0.
        XP_diff(x->ndigits - n, z + n, x + n, borrow);
    }
    // else x->ndigits == n, in this case (since the sgn(x) == sgn(y)) the
    // borrow will be 0 (I assume this is why there is an assertion). See
    // example: 10 - 99 = -89, -10 - (-99) = 89
    normalize(z);
}

static T abs_mul_mod(T x, T y, T p) {
    T xy = AP_mul(x, y);
    T z = AP_mod(xy, p);
    free(&xy);
    return z;
}

// ceil(a/b)
inline static int quotient_ceil(int a, int b) { return (a + b - 1) / b; }

inline static int is_digit(char c, int base) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c < 'a' + base - 10);
}

void AP_free(T *z) {
    free(*z);
    *z = NULL;
}

T AP_neg(T x) {
    T z = mk(x->ndigits);
    memcpy(z->digits, x->digits, x->ndigits);
    z->ndigits = x->ndigits;
    if (!is_zero(x)) {
        z->sign = -x->sign;
    }
    return z;
}

T AP_mul(T x, T y) {
    T z = mk(x->ndigits + y->ndigits);
    XP_mul(z->digits, x->ndigits, x, y->ndigits, y);
    normalize(z);
    if (!is_zero(x) && !is_zero(y) && x->sign != y->sign) {
        z->sign = -1;
    }
    return z;
}

T AP_add(T x, T y) {
    T z;
    if (x->sign == y->sign) {
        z = mk(max_digits(x, y) + 1);
        abs_add(z, x, y);
        z->sign = x->sign;
    } else if (abs_cmp(x, y) > 0) {  // |x| > |y| => x has more digits than y
        // When adding numbers of different signs, cannot yield more digits than
        // the larger magnitude number
        z = mk(x->ndigits);
        // and x >= 0, y < 0 => |x|-|y|
        // and x < 0, y >= 0 => -(|x|-|y|)
        abs_sub(z, x, y);
        // Above can be condensed into sgn(RHS) = sgn(x)
        // Note, don't do zero check of z because abs_cmp(x,y) would be 0 if
        // |x|=|y|
        z->sign = x->sign;
    } else {  // abs_cmp(x,y) <= 0 => |x| <= |y| => y has more digits than x
        // When adding numbers of different signs, cannot yield more digits than
        // the larger magnitude number
        z = mk(y->ndigits);
        // and x >= 0, y < 0 => -(|y|-|x|)
        // and x < 0, y >= 0 => |y|-|x|
        abs_sub(z, y, x);
        // 0 check necessary b/c abs_cmp(x,y) could be 0
        if (!is_zero(z)) {
            // Above can be condensed into sgn(RHS) = -sgn(x)
            z->sign = -x->sign;
        }
    }
    return z;
}

T AP_sub(T x, T y) {
    // Similar derivation from table as AP_add
    T z;
    if (x->sign != y->sign) {
        z = mk(max_digits(x, y) + 1);
        // e.g. -6 - 5 = -11 and 5 - -6 = 11
        abs_add(z, x, y);
        z->sign = x->sign;
    } else if (abs_cmp(x, y) > 0) {
        // When subtracting numbers of the same sign, can't exceed number of
        // digits of larger magnitude number
        z = mk(x->ndigits);
        abs_sub(z, x, y);
        z->sign = x->sign;
    } else {
        // When subtracting numbers of the same sign, can't exceed number of
        // digits of larger magnitude number
        z = mk(y->ndigits);
        abs_sub(z, y, x);
        // 0 check necessary b/c abs_cmp(x,y) could be 0
        if (!is_zero(z)) {
            z->sign = -x->sign;
        }
    }
    return z;
}

T AP_div(T x, T y) {
    assert(!is_zero(y));

    // Quotient can have at most as many digits as dividend
    T q = mk(x->ndigits);
    // Reminder can have at most as many digits as divisor
    T r = mk(y->ndigits);

    // Temporary required - see XP_div in ch 17
    XP_T tmp = malloc(x->ndigits + y->ndigits + 2);
    XP_div(x->ndigits, q->digits, x->digits, y->ndigits, y->digits, r->digits,
           tmp);
    free(tmp);
    tmp = NULL;

    normalize(q);
    normalize(r);

    if (x->sign != y->sign && !is_zero(x)) {
        q->sign = -1;
    }

    // Here, since division via XP_div is done on positive x, y remainder r is x
    // mod y
    if (x->sign != y->sign && !is_zero(r)) {
        // Why do we know returned carry is 0? Only way for carry to be 1 is if
        // quotient |x|/|y| was already the maximum value for the number of
        // digits in |x|. The only way this is possible is if |x| is that
        // maximum value and |y| = 1. This would not have passed above if guard
        // because remainder of max value for number of digits of x divided by 1
        // is 0.
        XP_sum(q->sign, q->digits, q->digits, 1);
        normalize(q);
    }

    AP_free(&r);
    return q;
}

T AP_mod(T x, T y) {
    assert(!is_zero(y));

    // Quotient can have at most as many digits as dividend
    T q = mk(x->ndigits);
    // Reminder can have at most as many digits as divisor
    T r = mk(y->ndigits);

    // Temporary required - see XP_div in ch 17
    XP_T tmp = malloc(x->ndigits + y->ndigits + 2);
    XP_div(x->ndigits, q->digits, x->digits, y->ndigits, y->digits, r->digits,
           tmp);
    free(tmp);
    tmp = NULL;

    normalize(q);
    normalize(r);

    if (x->sign != y->sign && !is_zero(x)) {
        q->sign = -1;
    }

    if (x->sign != y->sign && !is_zero(r)) {
        // r = |y| - (|x| % |y|) = |y| - r (r is result of XP_div division
        // between positive |x| and |y|). Why is the borrow never going to be 1?
        // Because a % b will always be at most b - 1
        XP_sub(r->size, r->digits, y->digits, r->digits, 0);
        normalize(r);
    }

    AP_free(&q);
    return r;
}

T AP_pow(T x, T y, T p) {
    if (is_zero(x)) {
        return AP_new(0);
    } else if (is_zero(y)) {
        return AP_new(1);
    } else if (is_abs_one(x)) {
        return AP_new(y->digits[0] % 2 == 0 ? 1 : x->sign);
    }

    T z;

    if (p) {
        // Note x^(2k+1) = xx^(2k) = x(x^k)^2 can alternatively be calculated as
        // x^(2k+1) = (x^((2k+1)/2))^2 (x) = (x^k)^2 (x) because (2k+1)/2 = k
        if (isone(y)) {
            // Will be freed because this is the t that can be returned by
            // AP_pow below is freed
            z = AP_addi(x, 0);
        } else {
            T y2 = AP_rshift(y, 1);  // division by 2 (since y is positive)
            T t = AP_pow(x, y2, NULL);
            z = AP_mul(t, t);
            // free allocated Ts
            AP_free(&y2);
            AP_free(&t);
            if (y->digits[0] % 2 == 1) {
                // Saves old z in t so it can be freed (z allocated by AP_mul)
                z = AP_mul(x, t = z);
                AP_free(&t);
            }
        }
    } else {
        if (isone(y)) {  // base case to satisfy (xy)%p = ((x%p)(y%p))%p
            z = AP_mod(x, p);
        } else {
            T y2 = AP_rshift(y, 1);  // division by 2 (since y is positive)
            T t = AP_pow(x, y2, NULL);
            z = abs_mul_mod(t, t,
                            p);  // t (since it comes from AP_pow) will be
                                 // modded by p (from base case, this setting of
                                 // z, and the setting of z in the odd case)
            AP_free(&y2);
            AP_free(&t);
            if (y->digits[0] % 2 == 1) {
                // Must do mod p of x when doing x * (x^k)^2
                z = abs_mul_mod(y2 = AP_mod(x, p), t = z, p);
                AP_free(&t);
                AP_free(&y2);
            }
        }
    }

    return z;
}

int AP_cmp(T x, T y) {
    // x < 0 => return -1, y < 0 => x >= 0 => return 1
    // Returns == sgn(x)
    if (x->sign != y->sign) {
        return x->sign;
    }
    // x, y < 0 return -1 if |x| > |y|
    else if (x->sign == -1) {
        return -abs_cmp(x, y);
    }
    // x, y >= 0 return -1 if |x| < |y|
    else {
        return abs_cmp(x, y);
    }
}

T AP_addi(T x, long y) {
    // https://stackoverflow.com/a/8239384
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    return AP_add(x, set(&t, y));
}

T AP_subi(T x, long y) {
    // https://stackoverflow.com/a/8239384
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    return AP_sub(x, set(&t, y));
}

T AP_muli(T x, long y) {
    // https://stackoverflow.com/a/8239384
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    return AP_mul(x, set(&t, y));
}

T AP_divi(T x, long y) {
    // https://stackoverflow.com/a/8239384
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    return AP_div(x, set(&t, y));
}

int AP_cmpi(T x, long y) {
    // https://stackoverflow.com/a/8239384
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    return AP_cmp(x, set(&t, y));
}

long AP_modi(T x, long y) {
    unsigned char d[sizeof(unsigned long)];
    struct T t;
    t.size = sizeof d;
    t.digits = d;
    t.sign = 1;
    T r = AP_mod(x, set(&t, y));
    long rem = XP_toint(r->ndigits, r->digits);
    free(&r);
    return rem;
}

T AP_lshift(T x, int s) {
    // Why does this create ceil(s/8) additional bytes? E.g. 1 byte number, left
    // shift 9 bits results in 3 = 1 + 2 = 1 + ceil(9/8) bytes
    T z = mk(x->ndigits + quotient_ceil(s, 8));
    XP_lshift(z->size, z->digits, x->size, x, s, 0);
    z->sign = x->sign;
    normalize(z);
    return z;
}

T AP_rshift(T x, int s) {
    // Why does this create floor(s/8) less bytes? 8 bit number, right shift 3
    // bits, have 1 = 1 - 0 bytes. Note floor(s/8) = s//8 (integer division).
    if (s >= 8 * x->ndigits) {
        return AP_new(0);
    }
    T z = mk(x->ndigits - s / 8);
    XP_rshift(z->size, z->digits, x->size, x, s, 0);
    normalize(z);
    // check if right shift <=> division yielded a 0 (not needed with left shift
    // <=> multiplication )
    z->sign = iszero(z) ? 1 : x->sign;
    return z;
}

T AP_fromstr(const char *str, int base, char **end) {
    const char *p = str;
    char *endp;
    while (*p && isspace(*p)) {
        p++;
    }
    char sign = '\0';
    if (*p == '+' || *p == '-') {
        sign = *p++;
    }
    // bits in z = n * log_2(base) * 8
    // e.g. n digit number in base 16 has n * log_2(16) * 8 = 32n bits
    const char *start;
    int k, n = 0;
    for (; *p == '0' && p[1] == '0'; p++)
        ;
    start = p;
    // Count number of digits
    for (; is_digit(tolower(*p), base); p++) {
        n++;
    }
    // Identify number of bits necessary to represent a number in base 10
    for (k = 1; (1 << k) < base; k++)
        ;
    T z = mk(quotient_ceil(k * n, 8));
    p = start;
    XP_fromstr(z->size, z->digits, p, base, &endp);
    normalize(z);
    if (endp == p) {
        endp = (char *)str;
        z = AP_new(0);
    } else {
        z->sign = is_zero(z) || sign != '-' ? 1 : -1;
    }
    if (end) {
        *end = (char *)endp;
    }
    return z;
}

long AP_toint(T x) {
    unsigned long u = XP_toint(x->ndigits, x->digits) % (LONG_MAX + 1UL);
    return x->sign * (long)u;
}

char *AP_tostr(char *str, int size, int base, T x) {
    if (str == NULL) {
        int k;
        for (k = 5; (1 << k) > base; k--)
            ;
        size = quotient_ceil(8 * x->ndigits, k) + 1;
        if (x->sign == -1) {
            size++;
        }
        str = malloc(size);
    }
    // Need to make a copy so that the data in x is not destroyed, not sure why
    // because in XP_tostr (which is what's used), the input number is
    // destroyed.
    XP_T q = malloc(x->ndigits);
    memcpy(q, x->digits, x->ndigits);
    if (x->sign == -1) {
        str[0] = '-';
        XP_tostr(str + 1, size - 1, base, x->ndigits, q);
    } else {
        XP_tostr(str, size, base, x->ndigits, q);
    }
    free(q);
    return str;
}

#undef T