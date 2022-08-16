#include "bit.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T Bit_T

typedef unsigned long word;
typedef unsigned char byte;

static const int counts[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
static const byte one_bit_masks[8] = {1, 2, 4, 8, 16, 32, 64, 128};

// most_significant_one_masks[i] gives the mask that when combined
// with a byte via | sets bits to 1 going in the direction of more
// significant bits from bit i. ~most_significant_one_masks[i]
// gives the most significant zero mask
static const byte most_significant_one_masks[8] = {
    255,  // 11111111 ~ => 00000000
    254,  // 11111110 ~ => 00000001
    252,  // 11111100 ~ => 00000011
    248,  // 11111000 ~ => 00000111
    240,  // 11110000 ~ => 00001111
    224,  // 11100000 ~ => 00011111
    192,  // 11000000 ~ => 00111111
    128   // 10000000 ~ => 01111111
};

// least_significant_one_masks[i] gives the mask that when combined
// with a byte via | sets bits to 1 going in the direction of less
// significant bits from bit i. ~least_significant_one_masks[i]
// gives the least significant zero mask. masks for middle ranges
// can be formed via &, e.g. most_significant_one_masks[2] &
// least_significant_one_masks[5] yields a mask with 1s from 2 to 5
static const byte least_significant_one_masks[8] = {
    1,    // 00000001 ~ => 11111110
    3,    // 00000011 ~ => 11111100
    7,    // 00000111 ~ => 11111000
    15,   // 00001111 ~ => 11110000
    31,   // 00011111 ~ => 11100000
    63,   // 00111111 ~ => 11000000
    127,  // 01111111 ~ => 10000000
    255   // 11111111 ~ => 00000000
};

struct T {
    word *words;
    byte *bytes;
    int length;
    int num_words;
    int num_bytes;
};

static int multiple(int x, int y) { return (x + y - 1) / y; }

static int get_bit_value(T set, int n) {
    return (set->bytes[n / 8] >> (n % 8)) % 2;
}

static T copy(T set) {
    T cpy = Bit_new(set->length);
    memcpy(cpy->words, set->words, set->num_bytes);
    return cpy;
}

static void check_for_setops(T s, T t) {
    assert(s || t);
    if (s && t) {
        assert(s->length == t->length);
    }
}

T Bit_new(int length) {
    assert(length >= 0);
    T s;
    s = malloc(sizeof(*s));
    assert(s);
    s->length = length;
    if (length > 0) {
        s->num_bytes = multiple(length, 8);
        s->num_words = multiple(length, 8 * sizeof(word));
        s->words = calloc(s->num_words, sizeof(word));
        s->bytes = (byte *)s->words;
    } else {
        s->num_bytes = s->num_words = 0;
        s->words = NULL;
        s->bytes = NULL;
    }
    return s;
}

int Bit_length(T set) {
    assert(set);
    return set->length;
}

int Bit_count(T set) {
    assert(set);
    int count = 0;
    for (int i = 0; i < set->num_bytes; i++) {
        count += counts[set->bytes[i] >> 4] + counts[set->bytes[i] % 16];
    }
    return count;
}

void Bit_free(T *set) {
    assert(set && *set);
    free((*set)->words);
    free(*set);
    *set = NULL;
}

int Bit_get(T set, int n) {
    assert(set && n >= 0 && n < set->length);
    return get_bit_value(set, n);
}

int Bit_put(T set, int n, int bit) {
    assert(set && n >= 0 && n < set->length && (bit == 0 || bit == 1));
    int old = get_bit_value(set, n);
    if (bit == 1) {  // RHS creates something like 00100000 for n % 8 = 5
        set->bytes[n / 8] |= 1 << (n % 8);
    } else {  // RHS creates something like 11011111 for n % 8 = 5
        set->bytes[n / 8] &= ~(1 << (n % 8));
    }
    return old;
}

void Bit_clear(T set, int lo, int hi) {
    assert(set && lo <= hi && lo >= 0 && lo < set->length && hi >= 0 &&
           hi < set->length);
    if (lo / 8 == hi / 8) {
        set->bytes[lo / 8] &= ~(most_significant_one_masks[lo % 8] &
                                least_significant_one_masks[hi % 8]);
    } else {
        set->bytes[lo / 8] &= ~most_significant_one_masks[lo % 8];
        for (int i = (lo / 8) + 1; i <= (hi / 8) - 1; i++) {
            set->bytes[i] = 0;
        }
        set->bytes[hi / 8] &= ~least_significant_one_masks[hi % 8];
    }
}

void Bit_set(T set, int lo, int hi) {
    assert(set && lo <= hi && lo >= 0 && lo < set->length && hi >= 0 &&
           hi <= set->length);
    if (lo / 8 == hi / 8) {
        set->bytes[lo / 8] |= most_significant_one_masks[lo % 8] &
                              least_significant_one_masks[hi % 8];
    } else {
        set->bytes[lo / 8] |= most_significant_one_masks[lo % 8];
        for (int i = (lo / 8) + 1; i <= (hi / 8) - 1; i++) {
            set->bytes[i] = 255;
        }
        set->bytes[hi / 8] |= least_significant_one_masks[hi % 8];
    }
}

void Bit_not(T set, int lo, int hi) {
    assert(set && lo <= hi && lo >= 0 && lo < set->length && hi >= 0 &&
           hi < set->length);
    if (lo / 8 == hi / 8) {
        set->bytes[lo / 8] ^= most_significant_one_masks[lo % 8] &
                              least_significant_one_masks[hi % 8];
    } else {
        set->bytes[lo / 8] ^= most_significant_one_masks[lo % 8];
        for (int i = (lo / 8) + 1; i <= (hi / 8) - 1; i++) {
            set->bytes[i] ^= 255;
        }
        set->bytes[hi / 8] ^= least_significant_one_masks[hi % 8];
    }
}

int Bit_lt(T s, T t) {
    assert(s && t && s->length == t->length);
    int found_lt = 0;
    for (int i = 0; i < s->num_words; i++) {
        // The parenthesis around this & is essential
        if ((s->words[i] & ~t->words[i]) != 0) {
            return 0;
        }
        if (!found_lt) {
            found_lt = t->words[i] != s->words[i];
        }
    }
    return found_lt;
}

int Bit_eq(T s, T t) {
    assert(s && t && s->length == t->length);
    for (int i = 0; i < s->num_words; i++) {
        if (s->words[i] != t->words[i]) {
            return 0;
        }
    }
    return 1;
}

int Bit_leq(T s, T t) {
    assert(s && t && s->length == t->length);
    for (int i = 0; i < s->num_words; i++) {
        if ((s->words[i] & ~t->words[i]) != 0) {
            return 0;
        }
    }
    return 1;
}

void Bit_map(T set, void apply(int n, int bit, void *cl), void *cl) {
    assert(set && apply);
    for (int i = 0; i < set->length; i++) {
        apply(i, get_bit_value(set, i), cl);
    }
}

T Bit_union(T s, T t) {
    check_for_setops(s, t);
    if (s == NULL) {
        return copy(t);
    } else if (t == NULL) {
        return copy(s);
    }
    T out = Bit_new(s->length);
    for (int i = 0; i < out->num_words; i++) {
        out->words[i] = s->words[i] | t->words[i];
    }
    return out;
}

T Bit_inter(T s, T t) {
    check_for_setops(s, t);
    if (s == NULL) {
        return Bit_new(t->length);
    } else if (t == NULL) {
        return Bit_new(s->length);
    }
    T out = Bit_new(s->length);
    for (int i = 0; i < out->num_words; i++) {
        out->words[i] = s->words[i] & t->words[i];
    }
    return out;
}

T Bit_minus(T s, T t) {
    check_for_setops(s, t);
    if (s == NULL) {
        return Bit_new(t->length);
    } else if (t == NULL) {
        return copy(s);
    }
    T out = Bit_new(s->length);
    for (int i = 0; i < out->num_words; i++) {
        // ~t turns excess bits into 1s, but since it is &
        // with excess bits of s which are 0s, the extra
        // bits in out remain 0 meaning Bit_count is unaffected
        out->words[i] = s->words[i] & ~t->words[i];
    }
    return out;
}

T Bit_diff(T s, T t) {
    check_for_setops(s, t);
    if (s == NULL) {
        return copy(t);
    } else if (t == NULL) {
        return copy(s);
    }
    T out = Bit_new(s->length);
    for (int i = 0; i < out->num_words; i++) {
        out->words[i] = s->words[i] ^ t->words[i];
    }
    return out;
}

#undef T