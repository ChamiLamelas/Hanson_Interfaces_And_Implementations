#ifndef MEM_INCLUDED
#define MEM_INCLUDED

#include "except.h"

extern const Except_T Mem_Failed;

extern void *Mem_alloc(long nbytes, const char *file, int line);
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);
extern void Mem_free(void *ptr, const char *file, int line);
extern void *Mem_resize(void *ptr, long nbytes, const char *file, int line);

#define ALLOC(n) Mem_alloc((n), __FILE__, __LINE__)
#define CALLOC(c, n) Mem_calloc((c), (n), __FILE__, __LINE__)
#define NEW(p) ((p) = ALLOC((long)sizeof(*(p))))
#define NEW0(p) ((p) = CALLOC(1, (long)sizeof(*(p))))
#define FREE(p) ((p) = (Mem_free((p), __FILE__, __LINE__), NULL))
#define RESIZE(p, n) ((p) = Mem_resize((p), (n), __FILE__, __LINE__))

#endif