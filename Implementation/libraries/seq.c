#include "seq.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define T Seq_T

static const int DEFAULT_HINT = 10;

struct T {
    void **arr;
    int head;
    int capacity;
    int num_elements;
};

static void auto_resize(T s) {
    if (s->num_elements == s->capacity) {
        int new_capacity = 2 * s->capacity;
        s->arr = realloc(s->arr, new_capacity * sizeof(void *));
        // If array is full and head is greater than 0, that means we have
        // some elements that have circled around to the front
        if (s->head > 0) {
            // Since we have a new array capacity of 2x with old
            // capacity being x, we have to shift them toward the end
            int offset = s->capacity - s->head;
            memcpy(s->arr + new_capacity - offset, s->arr + s->head,
                   offset * sizeof(void *));
            s->head = new_capacity - offset;
        }
        s->capacity = new_capacity;
    }
}

T Seq_new(int hint) {
    assert(hint >= 0);
    T s;
    s = malloc(sizeof(*s));
    s->head = 0;
    s->capacity = hint > 0 ? hint : DEFAULT_HINT;
    s->num_elements = 0;
    s->arr = malloc(sizeof(*(s->arr)) * s->capacity);
    assert(s->arr);
    return s;
}

T Seq_seq(void *x, ...) {
    T s = Seq_new(0);
    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        Seq_addhi(s, x);
    }
    va_end(ap);
    return s;
}

void Seq_free(T *seq) {
    assert(seq && *seq);
    free((*seq)->arr);
    free(*seq);
    *seq = NULL;
}

int Seq_length(T seq) {
    assert(seq);
    return seq->num_elements;
}

void *Seq_get(T seq, int i) {
    assert(seq && i >= 0 && i < seq->num_elements);
    return seq->arr[(seq->head + i) % seq->capacity];
}

void *Seq_put(T seq, int i, void *x) {
    assert(seq && i >= 0 && i < seq->num_elements);
    void *out = seq->arr[(seq->head + i) % seq->capacity];
    seq->arr[(seq->head + i) % seq->capacity] = x;
    return out;
}

void *Seq_addlo(T seq, void *x) {
    assert(seq);
    auto_resize(seq);
    seq->head = seq->head > 0 ? seq->head - 1 : seq->capacity - 1;
    seq->arr[seq->head] = x;
    seq->num_elements++;
    return x;
}

void *Seq_addhi(T seq, void *x) {
    assert(seq);
    auto_resize(seq);
    seq->arr[(seq->head + seq->num_elements) % seq->capacity] = x;
    seq->num_elements++;
    return x;
}

void *Seq_remlo(T seq) {
    assert(seq && seq->num_elements > 0);
    void *out = seq->arr[seq->head];
    seq->head = (seq->head + 1) % seq->capacity;
    seq->num_elements--;
    return out;
}

void *Seq_remhi(T seq) {
    assert(seq && seq->num_elements > 0);
    void *out = seq->arr[(seq->head + seq->num_elements - 1) % seq->capacity];
    seq->num_elements--;
    return out;
}

// **********************************************************************

// ArrayList like implementation built on Array below - included to show
// difference circular buffer version is better

// #include "array.h"
// #include "arrayrep.h"

// struct T {
//     Array_T arr;
//     int num_elements;
//     int head;
// };

// static void auto_resize(T seq) {
//     if (seq->num_elements == seq->arr->length) {
//         Array_resize(seq->arr, 2 * seq->arr->length + 1);
//     }
// }

// T Seq_new(int hint) {
//     assert(hint >= 0);
//     T s;
//     s = malloc(sizeof(*s));
//     assert(s);
//     s->head = 0;
//     s->num_elements = 0;
//     s->arr = Array_new(hint, sizeof(void *));
//     return s;
// }

// T Seq_seq(void *x, ...) {
//     T s = Seq_new(DEFAULT_HINT);
//     va_list ap;
//     va_start(ap, x);
//     for (; x; x = va_arg(ap, void *)) {
//         Seq_addhi(s, x);
//     }
//     va_end(ap);
//     return s;
// }

// void Seq_free(T *seq) {
//     assert(seq && *seq);
//     Array_free(&((*seq)->arr));
//     free(*seq);
//     *seq = NULL;
// }

// int Seq_length(T seq) {
//     assert(seq);
//     return seq->num_elements;
// }

// void *Seq_get(T seq, int i) {
//     assert(seq && i >= 0 && i < seq->num_elements);
//     return *(void **)Array_get(seq->arr, i);
// }

// void *Seq_put(T seq, int i, void *x) {
//     void *out = Seq_get(seq, i);
//     Array_put(seq->arr, i, &x);
//     return out;
// }

// void *Seq_addlo(T seq, void *x) {
//     assert(seq);
//     auto_resize(seq);
//     memcpy(seq->arr->array + sizeof(x), seq->arr->array,
//            seq->num_elements * sizeof(x));
//     Array_put(seq->arr, 0, &x);
//     seq->num_elements++;
//     return x;
// }

// void *Seq_addhi(T seq, void *x) {
//     assert(seq);
//     auto_resize(seq);
//     Array_put(seq->arr, seq->num_elements, &x);
//     seq->num_elements++;
//     return x;
// }

// void *Seq_remlo(T seq) {
//     void *out = Seq_get(seq, 0);
//     seq->num_elements--;
//     memcpy(seq->arr->array, seq->arr->array + sizeof(out),
//            sizeof(void *) * seq->num_elements);
//     return out;
// }

// void *Seq_remhi(T seq) {
//     assert(seq); // needed so seq->num_elements is valid
//     void *out = Seq_get(seq, seq->num_elements - 1);
//     seq->num_elements--;
//     return out;
// }