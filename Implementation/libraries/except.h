/*
Different implementation of TRY-EXCEPT-ELSE-FINALLY-END_TRY
I only try to pop exception stack in END_TRY as opposed to in
every EXCEPT, FINALLY, ELSE, and END_TRY as in his. END_TRY
will always be run so might as well do the pop there. I also 
removed the finalized exception state.

If we have 

TRY
    S1
EXCEPT(e1)
    S2
ELSE
    S3
FINALLY
    S4
END_TRY;

This translates to

do {
    Except_Frame frame; 
    frame.prev = stack; 
    stack = &frame; 
    volatile int state; 
    state = setjmp(frame.env); 
    if (state == start) {
        S1
    } else if (&(e) == frame.exception) {
        state = handled; 
        S2
    } else {
        state = handled;
        S3
    } {
        S4
    } if (state == start) 
        stack = stack->prev; 
    else if (state == unhandled) 
        Except_raise(frame.exception, frame.file, frame.line);
} while(0);
*/

#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include <setjmp.h>
#include <assert.h>
#include <stdio.h>

#define T Except_T
typedef struct T
{
    const char *reason;
} T;

typedef struct _Except_Frame Except_Frame;
struct _Except_Frame
{
    Except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const T *exception;
};

enum Except_code {
    start = 0, unhandled, handled
};

extern Except_Frame *stack;

#define TRY do { \
    Except_Frame frame; \
    frame.prev = stack; \
    stack = &frame; \
    volatile int state; \
    state = setjmp(frame.env); \
    if (state == start) {

#define EXCEPT(e)\
    } else if (&(e) == frame.exception) {\
        state = handled;

#define ELSE\
    } else {\
        state = handled;

#define FINALLY\
    } {

#define END_TRY\
    } if (state == start) \
        stack = stack->prev; \
    else if (state == unhandled) \
        RERAISE; \
} while(0)

#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RERAISE Except_raise(frame.exception, frame.file, frame.line)

/*
Nested handling won't work here, suppose we had function: 

void f(void) {
    TRY
        TRY
            RETURN
        ELSE
            puts("hello");
        END_TRY
    ELSE 
        puts("test");
    END_TRY
}

This would pop only one exception frame off the stack (not 2).
But we could nest function calls (as in main tests).
*/
#define RETURN switch(stack = stack->prev, 0) default: return

extern void Except_raise(const T *e, const char *file, int line);

#undef T
#endif