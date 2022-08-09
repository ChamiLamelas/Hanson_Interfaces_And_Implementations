#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "except.h"
extern const Except_T Assert_Failed;
#define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))
extern void (assert)(int e);
#endif