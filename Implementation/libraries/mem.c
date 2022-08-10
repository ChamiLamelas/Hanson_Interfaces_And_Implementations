#include "mem.h"
#include <assert.h>
#include <stdlib.h>

const Except_T Mem_Failed = {"Memory operation failed"};

void *Mem_alloc(long nbytes, const char *file, int line)
{
    assert(nbytes > 0);
    void *p = malloc(nbytes);
    if (p == NULL)
        Except_raise(&Mem_Failed, file, line);
    return p;
}

void *Mem_calloc(long count, long nbytes, const char *file, int line)
{
    assert(nbytes > 0 && count > 0);
    void *p = calloc(count, nbytes);
    if (p == NULL)
        Except_raise(&Mem_Failed, file, line);
    return p;
}

// Different implementation, freeing NULL pointer triggers abort guaranteed
void Mem_free(void *ptr, const char *file, int line)
{
    assert(ptr);
    free(ptr);
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line)
{
    assert(ptr && nbytes >= 0);
    void *new_ptr = realloc(ptr, nbytes);
    if (new_ptr == NULL)
        Except_raise(&Mem_Failed, file, line);
    return new_ptr;
}