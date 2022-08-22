#include "xp.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

#define T XP_T

unsigned long XP_fromint(int n, T z, unsigned long u) {
    for (int i = 0; i < n; i++) {
        z[i] = u % BASE;
        u /= BASE;
    }
    return u;
}

unsigned long XP_toint(int n, T x) {
    unsigned long out = 0;
    unsigned stop = (unsigned)sizeof(unsigned long);
    if (stop > n) {
        stop = n;
    }
    for (int i = 0; i < stop; i++) {
        out = out * BASE + x[i];
    }
    return out;
}

int XP_length(int n, T x) {
    while (n > 1 && x[n - 1] == 0) {
        n--;
    }
    return n;
}

int XP_add(int n, T z, T x, T y, int carry) {
    for (int i = 0; i < n; i++) {
        carry += x[i] + y[i];
        z[i] = carry % BASE;
        carry /= BASE;
    }
    return carry;
}

int XP_sub(int n, T z, T x, T y, int borrow) {
    int d;
    for (int i = 0; i < n; i++) {
        d = (x[i] + BASE) - borrow - y[i];
        z[i] = d % BASE;
        borrow = 1 - d / BASE;
    }
    return borrow;
}

int XP_sum(int n, T z, T x, int y) {
    for (int i = 0; i < n; i++) {
        y += x[i];
        z[i] = y % BASE;
        y /= BASE;
    }
    return y;
}

int XP_diff(int n, T z, T x, int y) {
    // In XP_sub replace burrow with y and take T y = 0...0
    int d;
    for (int i = 0; i < n; i++) {
        d = (x[i] + BASE) - y;
        z[i] = d % BASE;
        y = 1 - d / BASE;
    }
    return y;
}

int XP_neg(int n, T z, T x, int carry) {
    for (int i = 0; i < n; i++) {
        carry += ~x[i];
        z[i] = carry % BASE;
        carry /= BASE;
    }
    return carry;
}

int XP_mul(T z, int n, T x, int m, T y) {
    int j, carryout = 0;
    for (int i = 0; i < n; i++) {
        unsigned carry = 0;
        // i,j = 0 only written into once (makes sense since
        // all following rows will be 0), i+j = 1 written twice
        // first time from x[1] * y[0] next time x[0] * y[1],
        // and so on...
        for (j = 0; j < m; j++) {
            carry += x[i] * y[j] + z[i + j];
            z[i + j] = carry % BASE;
            carry /= BASE;
        }

        for (; j < n + m - i; j++) {
            carry += z[i + j];
            z[i + j] = carry % BASE;
            carry /= BASE;
        }
        carryout |= carry;
    }
    return carryout;
}

int XP_product(int n, T z, T x, int y) {
    unsigned carry = 0;
    for (int i = 0; i < n; i++) {
        carry += x[i] * y;
        z[i] = carry % BASE;
        carry /= BASE;
    }
    return carry;
}

int XP_quotient(int n, T z, T x, int y) {
    unsigned carry = 0;
    // will fill less significant bits with 0s
    // in cases like 7000 / 7
    for (int i = n - 1; i >= 0; i--) {
        carry = carry * BASE + x[i];
        z[i] = carry / y;
        carry %= y;
    }
    return carry;
}

// q has n digits, r has m digits (consider 100 / 85 => remainder
// 15 (2 digits), 7000 / 7 => quotient 1000 (4 digits))
int XP_div(int n, T q, T x, int m, T y, T r, T tmp) {
    int len_x = XP_length(n, x);
    int len_y = XP_length(m, y);

    if (m == 1) {
        if (y[0] == 0) {
            return 0;
        }
        r[0] = XP_quotient(n, q, x, y[0]);
        memset(r + 1, '\0', m - 1);
    } else if (len_x < len_y) {
        // https://stackoverflow.com/a/23811204 on setting \0 and 0
        // copying x as the remainder and leaving quotient be 0
        memset(q, '\0', n);
        memcpy(r, x, len_x);
        memset(r + len_x, '\0', m - len_x);
    } else {  // long division
        T rem = tmp;
        T dq = tmp + len_x + 1;
        // removed assertion b/c assertion criterion are guaranteed
        // by if, else-if. take 0615367 / 296 this has k = len_x - len_y
        // + 1 = 6 - 3 + 1 = 4 divisions to be done, which is the number of
        // digits in the quotient

        // initializing rem with most significant 0, assumed rem
        // has space for len_x + 1 digits
        memcpy(rem, x, len_x);
        rem[len_x] = 0;
        for (int k = len_x - len_y; k >= 0; k--) {
            // how to find qk? akin to how do we find that we have 2
            // as first digit in quotient of 0615367 / 296
            // slow strategy tries 9 * 296, 8 * 296, ... 1 * 296
            // until one is found <= 615 (this is base 10 for example)
            int km = k + len_y;
            unsigned long y2 = y[len_y - 1] * BASE + y[len_y - 2];
            unsigned long r3 =
                rem[km] * (BASE * BASE) + rem[km - 1] * BASE + rem[km - 2];
            int qk = r3 / y2;
            if (qk >= BASE) {
                qk = BASE - 1;
            }
            dq[len_y] = XP_product(len_y, dq, y, qk);
            int i;
            for (i = len_y; i > 0; i--) {
                if (rem[i + k] != dq[i]) {
                    break;
                }
            }
            if (rem[i + k] < dq[k]) {
                dq[len_y] = XP_product(m, dq, y, --qk);
            }
            q[k] = qk;
            // why is this assertion needed?
            assert(0 <= k && k <= k + m);
            int borrow = XP_sub(m + 1, rem + k, rem + k, dq, 0);
            // Removed assertion because assertion being != 0 implies
            // that dq is > the first m + 1 digits of rem, but we choose
            // rem to be < than the first m + 1 digits (why do we need this?)
        }

        // number of nonzero digits in remainder is equivalent to number
        // of nonzero digits in y
        memcpy(r, rem, len_y);
        int i;
        for (i = len_x - len_y + 1; i < n; i++) {
            q[i] = 0;
        }
        for (i = len_y; i < m; i++) {
            r[i] = 0;
        }
    }

    return 1;
}

XP_cmp(int n, T x, T y) {
    for (int i = n - 1; i >= 0; i--) {
        if (x[i] != y[i]) {
            return x[i] - y[i];
        }
    }
    return 0;
}

static int get_numeric(char c, int base) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'a' + base - 10) {
        return c - 'a' + 10;
    }
    return -1;
}

int XP_fromstr(int n, T z, const char *str, int base, char **end) {
    assert(str);
    assert(base >= 2 && base <= 36);
    const char *itr = str;
    while (*itr && isspace(*itr)) {
        itr++;
    }
    int carry;
    int numeric;
    for (; *itr; itr++) {
        // converts "B" and "b" -> 11, "3" -> 3, etc.
        numeric = get_numeric(tolower(*itr), base);
        if (numeric == -1) {
            // Always returns 0 in the event of a character that
            // is not in the base is encountered (not surrounded
            // with outer if).
            if (end) {
                *end = itr;
            }
            return 0;
        }

        // "123", going L->R start with 1, then multiply by 10
        // Get 10, then add 2. Go right, multiply 12 by 10 and add
        // 3, in general: z = z * base + d
        carry = XP_product(n, z, z, base);
        if (carry) {
            break;
        }
        XP_sum(n, z, z, numeric);
    }
    return carry;
}

static char get_digit(int num) {
    if (num >= 0 && num <= 9) {
        return '0' + num;
    } else {
        return 'a' + num - 10;
    }
}

char *XP_tostr(char *str, int size, int base, int n, T x) {
    // Don't try to overfill like in implementation
    // just fill from reverse starting with least significant
    // digit
    for (int i = size - 2; i >= 0; i--) {
        str[i] = get_digit(XP_quotient(n, x, x, base));
        // Also did not do the n decrement loop, the loop
        // means that XP_quotient will be done on fewer
        // digits (note that XP_quotient still works with
        // leading 0s)
    }
    // if we ended with some data still left in x, that means
    // we couldn't fit in size, can't just do x[0] != 0
    // b/c we don't have that n decrement loop
    for (int i = 0; i < n; i++) {
        assert(x[i] == 0);
    }
    str[size - 1] = '\0';
    return str;
}

void XP_lshift(int n, T z, int m, T x, int s, int fill) {
    fill = fill ? 255 : 0;
    int j = n - 1;

    // n > m => start copying from x at m - 1, i will
    // be a lag pointer to j lead (if n = m + 1, and we
    // are shifting a few bytes some of the most significant
    // bytes from x will be lost)
    // n <= m => start copying from n - s/8 + 1 which is
    // the most significant x byte we will be able to copy
    // over into shifted T
    int i = n > m ? m - 1 : n - s / 8 - 1;

    // m + s/8 is first byte that will be set to 0
    for (; j >= m + s / 8; j--) {
        z[j] = 0;
    }

    // Copy rest of x
    for (; i >= 0; i--, j--) {
        z[j] = x[i];
    }

    // Fill in 0s after shift
    for (; j >= 0; j--) {
        z[j] = fill;
    }

    s %= 8;
    if (s > 0) {
        // 1 << s = 2^s
        XP_product(n, z, z, 1 << s);
        // Only apply fill to 8-s most significant bits
        // For b=0,1 b|1 = 1 and b|0 = b
        // After multiplication, least significant byte
        // will have its s least significant bits be 0
        // and the 8-s most significant bits be 0s and 1s
        // Apply |= fill>>(8-s) will combine 0 via |
        // with the 8-s most significant bits (keeping
        // them) and then will combine fill via | with
        // the s least significant bits which will just
        // be fill | 0 = fill
        z[0] |= fill >> (8 - s);
    }
}

void XP_rshift(int n, T z, int m, T x, int s, int fill) {
    fill = fill ? 255 : 0;
    int j = 0;
    /*
    s/8 is first digit to be copied from x. Why? Remember that x
    is stored least -> most significant bytes. When we right shift
    least significant bytes are removed. If z cannot fit the shift, 
    j < n protects against overfilling.
    */
    for (int i = s / 8; i < m && j < n; i++, j++) {
        z[j] = x[i];
    }
    // Fill in most significant bytes that were vacated with fill
    // as indicated
    for (; j < n; j++) {
        z[j] = fill;
    }
    s %= 8;
    if (s > 0) {
        // Division by 2^s will yield s 0s in the most significant bits
        // in the most significant byte (in z[n-1])
        XP_quotient(n, z, z, 1 << s);
        // fill << (8-s) will yield s fills as the most significant
        // bits and 0 for the other 8-s less significant bits.
        // Combining via | with the most significant byte will replace
        // the s most significant bits with fill (because they are 0s
        // and fill | 0 = fill) and will leave the remaining 8-s less
        // significant bits in z[n-1] the same (b/c b | 0 = b).
        z[n - 1] |= fill << (8 - s);
    }
}