#include "str.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "bit.h"

#define NUM_CHARS 256

static unsigned char map[NUM_CHARS] = {0};
static int map_set = 0;

static inline void check_position(int i, int len) {
    assert(i <= len + 1 && i >= -len);
}

static inline int min(int a, int b) { return a < b ? a : b; }

// https://stackoverflow.com/questions/1932311/when-to-use-inline-function-and-when-not-to-use-it
static inline int convert_positive(int i, int len) {
    return i <= 0 ? i + len + 1 : i;
}

static char *alloc_str(int len) {
    char *s = malloc(len + 1);
    s[len] = '\0';
    return s;
}

static void order_positions(int *i, int *j, int len) {
    check_position(*i, len);
    check_position(*j, len);
    *i = convert_positive(*i, len);
    *j = convert_positive(*j, len);
    if (*i > *j) {
        int tmp = *i;
        *i = *j;
        *j = tmp;
    }
}

char *Str_sub(const char *s, int i, int j) {
    assert(s);
    order_positions(&i, &j, strlen(s));
    int out_len = j - i;
    char *out = alloc_str(out_len);
    memcpy(out, s + i - 1, out_len);
    return out;
}

char *Str_dup(const char *s, int i, int j, int n) {
    assert(s && n >= 0);
    order_positions(&i, &j, strlen(s));
    int out_len = (j - i) * n;
    char *out = alloc_str(out_len);
    for (int k = 0; k < n; k++) {
        memcpy(out + k * (j - i), s + i - 1, j - i);
    }
    return out;
}

char *Str_cat(const char *s1, int i1, int j1, const char *s2, int i2, int j2) {
    assert(s1 && s2);
    order_positions(&i1, &j1, strlen(s1));
    order_positions(&i2, &j2, strlen(s2));
    int out_len = j1 - i1 + j2 - i2;
    char *out = alloc_str(out_len);
    memcpy(out, s1 + i1 - 1, j1 - i1);
    memcpy(out + j1 - i1, s2 + i2 - 1, j2 - i2);
    return out;
}

char *Str_catv(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    int out_len = 0;
    int i;
    int j;
    int len;
    const char *curr = s;
    for (; curr; curr = va_arg(ap, const char *)) {
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        len = strlen(curr);
        order_positions(&i, &j, len);
        out_len += j - i;
    }
    va_end(ap);
    char *out = alloc_str(out_len);
    int curr_len = 0;
    va_start(ap, s);
    for (; s; s = va_arg(ap, const char *)) {
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        len = strlen(s);
        order_positions(&i, &j, len);
        memcpy(out + curr_len, s + i - 1, j - i);
        curr_len += j - i;
    }
    va_end(ap);
    return out;
}

char *Str_reverse(const char *s, int i, int j) {
    assert(s);
    order_positions(&i, &j, strlen(s));
    int out_len = j - i;
    char *out = alloc_str(out_len);
    for (int k = 0; k < out_len; k++) {
        out[k] = s[j - k - 2];
    }
    return out;
}

char *Str_map(const char *s, int i, int j, const char *from, const char *to) {
    assert(s);
    if (!map_set) {
        assert(from && to);
        map_set = 1;
    }
    if (from && to) {
        int map_len = strlen(from);
        assert(map_len == strlen(to));
        for (int k = 0; k < NUM_CHARS; k++) {
            map[k] = k;
        }
        for (int k = 0; k < map_len; k++) {
            map[from[k]] = to[k];
        }
    }
    order_positions(&i, &j, strlen(s));
    int out_len = j - i;
    char *out = alloc_str(out_len);
    for (int k = 0; k < out_len; k++) {
        out[k] = map[s[i + k - 1]];
    }
    return out;
}

int Str_pos(const char *s, int i) {
    assert(s);
    int len = strlen(s);
    check_position(i, len);
    return convert_positive(i, len);
}

int Str_len(const char *s, int i, int j) {
    assert(s);
    order_positions(&i, &j, strlen(s));
    return j - i;
}

int Str_cmp(const char *s1, int i1, int j1, const char *s2, int i2, int j2) {
    assert(s1 && s2);
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    order_positions(&i1, &j1, len1);
    order_positions(&i2, &j2, len2);
    int c = strncmp(s1 + i1 - 1, s2 + i2 - 1, min(j1 - i1, j2 - i2));
    return c == 0 ? (j1 - i1) - (j2 - i2) : c;
}

int Str_chr(const char *s, int i, int j, int c) {
    assert(s);
    int len = strlen(s);
    order_positions(&i, &j, len);
    for (int k = i; k < j; k++) {
        if (s[k - 1] == c) {
            return k;
        }
    }
    return 0;
}

int Str_rchr(const char *s, int i, int j, int c) {
    assert(s);
    int len = strlen(s);
    order_positions(&i, &j, len);
    for (int k = j - 1; k >= i; k--) {
        if (s[k - 1] == c) {
            return k;
        }
    }
    return 0;
}

// Improvement over provided solution. Provided solution
// runs in O((j-i) * l) time where l = strlen(set).
// This solution runs in O(j-i) time with only on the
// order 256 additional bits allocated.
int Str_upto(const char *s, int i, int j, const char *set) {
    assert(s && set);
    int len = strlen(s);
    order_positions(&i, &j, len);
    Bit_T bs = Bit_new(256);
    int set_len = strlen(set);
    for (int k = 0; k < set_len; k++) {
        Bit_put(bs, set[k], 1);
    }
    for (int k = i; k < j; k++) {
        if (Bit_get(bs, s[k - 1])) {
            return k;
        }
    }
    Bit_free(&bs);
    return 0;
}

// Similar 1/strlen(set) runtime improvement to Str_upto
int Str_rupto(const char *s, int i, int j, const char *set) {
    assert(s && set);
    int len = strlen(s);
    order_positions(&i, &j, len);
    Bit_T bs = Bit_new(256);
    int set_len = strlen(set);
    for (int k = 0; k < set_len; k++) {
        Bit_put(bs, set[k], 1);
    }
    for (int k = j - 1; k >= i; k--) {
        if (Bit_get(bs, s[k - 1])) {
            return k + 1;
        }
    }
    Bit_free(&bs);
    return 0;
}

int Str_find(const char *s, int i, int j, const char *str) {
    assert(s && str);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int str_len = strlen(str);
    if (str_len == 0) {
        return i;
    }
    for (int k = i; k < j - str_len + 1; k++) {
        if (strncmp(s + k - 1, str, str_len) == 0) {
            return k;
        }
    }
    return 0;
}

int Str_rfind(const char *s, int i, int j, const char *str) {
    assert(s && str);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int str_len = strlen(str);
    if (str_len == 0) {
        return j;
    }
    for (int k = j - str_len; k >= i; k--) {
        if (strncmp(s + k - 1, str, str_len) == 0) {
            return k;
        }
    }
    return 0;
}

int Str_any(const char *s, int i, const char *set) {
    assert(s && set);
    int len = strlen(s);
    check_position(i, len);
    i = convert_positive(i, len);
    assert(i <= len);
    return strchr(set, s[i - 1]) ? i + 1 : 0;
}

// Similar 1/strlen(set) runtime improvement to Str_upto
int Str_many(const char *s, int i, int j, const char *set) {
    assert(s && set);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int set_len = strlen(set);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set_len; k++) {
        Bit_put(bs, set[k], 1);
    }
    int k;
    for (k = i; k < j && Bit_get(bs, s[k - 1]); k++)
        ;
    Bit_free(&bs);
    return k == i ? 0 : k;
}

// Similar 1/strlen(set) runtime improvement to Str_upto
int Str_rmany(const char *s, int i, int j, const char *set) {
    assert(s && set);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int set_len = strlen(set);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set_len; k++) {
        Bit_put(bs, set[k], 1);
    }
    int k;
    for (k = j - 1; k >= i && Bit_get(bs, s[k - 1]); k--)
        ;
    Bit_free(&bs);
    return k == j - 1 ? 0 : k + 1;
}

int Str_match(const char *s, int i, int j, const char *str) {
    assert(s && str);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int str_len = strlen(str);
    if (str_len == 0) {
        return i;
    }
    return ((j - i >= str_len) && strncmp(s + i - 1, str, str_len) == 0)
               ? i + str_len
               : 0;
}

int Str_rmatch(const char *s, int i, int j, const char *str) {
    assert(s && str);
    int len = strlen(s);
    order_positions(&i, &j, len);
    int str_len = strlen(str);
    if (str_len == 0) {
        return j;
    }
    return ((j - i >= str_len) &&
            strncmp(s + j - str_len - 1, str, str_len) == 0)
               ? j - str_len
               : 0;
}