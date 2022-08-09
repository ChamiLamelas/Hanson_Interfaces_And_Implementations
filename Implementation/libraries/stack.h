// Improved version that includes top, size

#ifndef STACK_INCLUDED
#define STACK_INCLUDED

typedef struct _Stack_T *Stack_T;
#define T Stack_T

extern T Stack_new(void);
extern int Stack_empty(T s);
extern int Stack_size(T s);
extern void Stack_push(T s, void *e);
extern void *Stack_top(T s);
extern void *Stack_pop(T s);
extern void Stack_free(T *s);

#endif