#include "array.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arrayrep.h"

#define T Array_T

T Array_new(int length, int size) {
    assert(length >= 0 && size > 0);
    T arr;
    arr = malloc(sizeof(*arr));
    assert(arr);
    arr->length = length;
    arr->size = size;
    if (length > 0) {
        arr->array = calloc(length, size);
        assert(arr->array);
    } else {
        arr->array = NULL;
    }
    return arr;
}

void Array_free(T* array) {
    assert(array && *array);
    free((*array)->array);
    free(*array);
    *array = NULL;
}

int Array_length(T array) {
    assert(array);
    return array->length;
}

int Array_size(T array) {
    assert(array);
    return array->size;
}

void* Array_get(T array, int i) {
    assert(array && i >= 0 && i < array->length);
    return array->array + (i * array->size);
}

void* Array_put(T array, int i, void* elem) {
    assert(array && i >= 0 && i < array->length && elem);
    memcpy(array->array + (i * array->size), elem, array->size);
    return elem;
}

void Array_resize(T array, int length) {
    assert(array && length >= 0);
    if (array->length > 0 && length > 0) {
        array->array = realloc(array->array, length * array->size);
        assert(array->array);
        // in his implementation, he just uses RESIZE which hides a call to
        // Mem_Resize which just wraps realloc which does not guarantee
        // initialization of extra bytes to 0 (memset does that here, 
        // could also do a call to calloc)
        if (length > array->length) {
            memset(array->array + (length * array->size), 0,
                   (length - array->length) * array->size);
        }
    } else if (array->length == 0 && length > 0) {
        array->array = calloc(length, array->size);
        assert(array->array);
    } else if (array->length > 0 && length == 0) {
        free(array->array);
        array->array = NULL;
    }
    // else: array->length = 0 and length = 0 (have nothing to allocate, have
    // nothing to free)
    array->length = length;
}

T Array_copy(T array, int length) {
    assert(array && length >= 0);
    T copy;
    copy = malloc(sizeof(*copy));
    assert(copy);
    copy->size = array->size;
    copy->length = length;
    if (copy->length > 0) {
        copy->array = calloc(copy->length, copy->size);
        assert(copy->array);
    } else {
        copy->array = NULL;
    }
    if (array->length > 0 && copy->length > 0) {
        memcpy(copy->array, array->array,
               (array->length < copy->length ? array->length : copy->length) *
                   array->size);
    }
    // else: (1) array->length > 0 && copy->length == 0 (do nothing copy->array
    // already NULL), (2) array->length == 0 && copy->length > 0 (do nothing
    // copy->array has already been allocated), (3) array->length == 0 &&
    // copy->length == 0 (do nothing, copy->array already NULL)
    return copy;
}

void ArrayRep_init(T array, int length, int size, void* ary) {
    assert(array && length >= 0 && size > 0 && ary);
    if (length == 0) {
        assert(ary == NULL);
    }
    array->array = ary;
    array->length = length;
    array->size = size;
}

#undef T