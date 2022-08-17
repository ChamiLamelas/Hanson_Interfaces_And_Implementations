#include "text.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bit.h"

#define T Text_T

#define NUM_CHARS 256

unsigned char all[NUM_CHARS] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
    90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255};

const T Text_cset = {NUM_CHARS, all};
const T Text_ascii = {128, all};
const T Text_ucase = {26, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
const T Text_lcase = {26, "abcdefghijklmnopqrstuvwxyz"};
const T Text_digits = {10, "0123456789"};
const T Text_null = {0, ""};

static unsigned char map[NUM_CHARS] = {0};
static int map_set = 0;

// Copied from str.c
static inline void check_position(int i, int len) {
    assert(i <= len + 1 && i >= -len);
}

static inline int convert_positive(int i, int len) {
    return i <= 0 ? i + len + 1 : i;
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

static inline int min(int a, int b) { return a < b ? a : b; }

// New helpers

T Text_box(const char *str, int len) {
    assert(str);
    T t;
    t.str = str;
    t.len = len;
    return t;
}

T Text_sub(T s, int i, int j) {
    order_positions(&i, &j, s.len);
    T out;
    out.str = s.str + i - 1;
    out.len = j - i;
    return out;
}

int Text_pos(T s, int i) {
    check_position(i, s.len);
    return convert_positive(i, s.len);
}

int Text_cmp(T s1, T s2) {
    int c = strncmp(s1.str, s2.str, min(s1.len, s2.len));
    return c == 0 ? s1.len - s2.len : c;
}

int Text_chr(T s, int i, int j, int c) {
    order_positions(&i, &j, s.len);
    for (int k = i; k < j; k++) {
        if (s.str[k - 1] == c) {
            return k;
        }
    }
    return 0;
}

int Text_rchr(T s, int i, int j, int c) {
    order_positions(&i, &j, s.len);
    for (int k = j - 1; k >= i; k--) {
        if (s.str[k - 1] == c) {
            return k;
        }
    }
    return 0;
}

// Similar 1/set.len runtime improvement to Str_upto
int Text_upto(T s, int i, int j, T set) {
    order_positions(&i, &j, s.len);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set.len; k++) {
        Bit_put(bs, set.str[k], 1);
    }
    for (int k = i; k < j; k++) {
        if (Bit_get(bs, s.str[k - 1])) {
            return k;
        }
    }
    Bit_free(&bs);
    return 0;
}

int Text_rupto(T s, int i, int j, T set) {
    order_positions(&i, &j, s.len);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set.len; k++) {
        Bit_put(bs, set.str[k], 1);
    }
    for (int k = j - 1; k >= i; k--) {
        if (Bit_get(bs, s.str[k - 1])) {
            return k + 1;
        }
    }
    Bit_free(&bs);
    return 0;
}

int Text_any(T s, int i, T set) {
    check_position(i, s.len);
    i = convert_positive(i, s.len);
    assert(i <= s.len);
    return strchr(set.str, s.str[i - 1]) ? i + 1 : 0;
}

int Text_many(T s, int i, int j, T set) {
    order_positions(&i, &j, s.len);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set.len; k++) {
        Bit_put(bs, set.str[k], 1);
    }
    int k;
    for (k = i; k < j && Bit_get(bs, s.str[k - 1]); k++)
        ;
    Bit_free(&bs);
    return k == i ? 0 : k;
}

int Text_rmany(T s, int i, int j, T set) {
    order_positions(&i, &j, s.len);
    Bit_T bs = Bit_new(256);
    for (int k = 0; k < set.len; k++) {
        Bit_put(bs, set.str[k], 1);
    }
    int k;
    for (k = j - 1; k >= i && Bit_get(bs, s.str[k - 1]); k--)
        ;
    Bit_free(&bs);
    return k == j - 1 ? 0 : k + 1;
}

int Text_find(T s, int i, int j, T str) {
    order_positions(&i, &j, s.len);
    if (str.len == 0) {
        return i;
    }
    for (int k = i; k < j - str.len + 1; k++) {
        if (strncmp(s.str + k - 1, str.str, str.len) == 0) {
            return k;
        }
    }
    return 0;
}

int Text_rfind(T s, int i, int j, T str) {
    order_positions(&i, &j, s.len);
    if (str.len == 0) {
        return j;
    }
    for (int k = j - str.len; k >= i; k--) {
        if (strncmp(s.str + k - 1, str.str, str.len) == 0) {
            return k;
        }
    }
    return 0;
}

int Text_match(T s, int i, int j, T str) {
    order_positions(&i, &j, s.len);
    if (str.len == 0) {
        return i;
    }
    return ((j - i >= str.len) && strncmp(s.str + i - 1, str.str, str.len) == 0)
               ? i + str.len
               : 0;
}

int Text_rmatch(T s, int i, int j, T str) {
    order_positions(&i, &j, s.len);
    if (str.len == 0) {
        return j;
    }
    return ((j - i >= str.len) &&
            strncmp(s.str + j - str.len - 1, str.str, str.len) == 0)
               ? j - str.len
               : 0;
}

char *Text_get(char *str, int size, T s) {
    if (str) {
        assert(size >= s.len + 1);
    } else {
        str = malloc(s.len + 1);
    }
    memcpy(str, s.str, s.len);
    str[s.len] = '\0';
    return str;
}

T Text_put(const char *str) {
    T s;
    s.len = strlen(str);
    s.str = malloc(s.len);
    memcpy((char*)s.str, str, s.len);
    return s;
}

T Text_cat(T s1, T s2) {
    T s;
    s.len = s1.len + s2.len;
    s.str = malloc(s.len);
    memcpy((char*)s.str, s1.str, s1.len);
    memcpy((char*)s.str + s1.len, s2.str, s2.len);
    return s;
}

T Text_dup(T s, int n) {
    assert(n>=0);
    T out;
    out.len = n * s.len;
    out.str = malloc(out.len);
    for (int i = 0; i < n; i++) {
        memcpy((char*)out.str + i * s.len, s.str, s.len);
    }
    return out;
}

T Text_reverse(T s) {
    T out;
    out.len = s.len;
    out.str = malloc(out.len);
    char *p = (char *)(out.str + out.len - 1);
    for (int i = 0; i < s.len; i++) {
        *p-- = s.str[i];
    }
    return out;
}

T Text_map(T s, const T *from, const T *to) {
    T out;
    out.len = s.len;
    out.str = malloc(out.len);
    char *p = (char *)out.str;
    if (!map_set) {
        assert(from && to);
        map_set = 1;
    }
    if (from && to) {
        assert(from->len == to->len);
        for (int i = 0; i < NUM_CHARS; i++) {
            map[i] = i;
        }
        for (int i = 0; i < from->len; i++) {
            map[from->str[i]] = to->str[i];
        }
    }
    for (int i = 0; i < s.len; i++) {
        *p++ = map[s.str[i]];
    }
    return out;
}

void Text_free(T *s) {
    free((char*)s->str);
    s->str = NULL;
    s->len = 0;
}

#undef T