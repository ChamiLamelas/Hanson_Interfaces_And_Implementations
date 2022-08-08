#include <stdio.h>
#include "test.h"
#include "lib.h"
#include "assert.h"
#include "queue.h"

/*
C requirement:

(x/y)*y + x%y = x   [1]

Take x = -1, y = N

[1] => x%y = x - (x/y)*y

-1/N can evaluate to 0 or -1

-1 % N = {
    -1 - 0N = -1        if (-1/N) evaluates to 0
    -1 - (-1)N = N-1    if (-1/N) evaluates to -1
}

Hence we get two possible results.

Consider 13/5, we know this is 2. This can be thought of
as getting the largest integer smaller than the real result.
For x/y where sgn(x)=sgn(y) following this will lead to results
in the direction of 0 and -infinity. However, with negative
numbers, when we do -13/5, we could say its -3 if we choose
it to the largest integer smaller than the real result or 0
if we choose the integer closest to -13/5 coming from 0. 
Hence we have the two types of truncation leading to x/y
having values that are 1 apart.

https://stackoverflow.com/questions/3602827/what-is-the-behavior-of-integer-division
*/
int main(int argc, char *argv[])
{
    printf("%d %d\n", y, f(1));

    // We cannot access members of LIB_T (thus creating encapsulation).
    // This is known as an opaque pointer as the actual type it points
    // too is unknown to this file (because it is unknown in lib.h which
    // is what is included). The reason we cannot access the members
    // is because a dereference operation triggers a compilation error
    // saying we cannot instantiate a pointer and try to dereference an
    // incomplete type (again what we see from lib.h). See the paragraph
    // under Object Life Cycle here:
    // https://interrupt.memfault.com/blog/opaque-pointers#the-opaque-pointer-pattern
    // Since lib.h has no information on the struct LIB_T refers to,
    // they cannot be instantiated as variables because compiler does
    // not know how much space to allocate. Hence, to create these
    // structs so LIB_T can refer to them, this must be done dynamically.
    // https://stackoverflow.com/a/60006579
    // See here how incompleteness is used to create encapsulation:
    // https://en.wikipedia.org/wiki/Opaque_data_type

    LIB_T e;
    // e->x;

    // Assert keyword provided in assert.h allows for checking
    // boolean value (failure to pass leads to program termination)
    int a = 1;
    int *ap = &a;
    assert(ap);
    int *bp = NULL;
    // assert(bp);

    // It is not clear to me how the Stack_free function sets the argument
    // of type T = struct T* to NULL. Stack_free takes T* which is a pointer
    // to a pointer which seems right, but then FREE is called on *(T*) which
    // is just a pointer. Inside FREE (looking at mem.h there is a macro
    // https://github.com/drh/cii/blob/master/include/mem.h) which seems to just
    // set the pointer to 0 = NULL.

    // Something interesting comes up looking at NEW defined here:
    // https://github.com/drh/cii/blob/master/include/mem.h
    // Looking at the implementation of Stack_new we see that
    // NEW is called on a T variable that is not initialized to
    // anything which is essentially an uninitialized pointer.
    // Thus, malloc is called on sizeof() a dereference
    // of an uninitialized pointer like below. Why can you
    // dereference a pointer here that is uninitialized?
    // It's because sizeof doesn't actually evaluate the operand,
    // it works off the type that it would be. See the following
    // related articles:
    // https://stackoverflow.com/questions/21996765/is-deferencing-an-uninitialized-pointer-in-sizeof-legal
    // https://stackoverflow.com/questions/8225776/why-does-sizeofx-not-increment-x
    int *c;
    printf("%d %d\n", sizeof(c), sizeof(*c));

    int e1 = 3;
    int e2 = 4;
    Queue_T q = Queue_new();
    Queue_enqueue(q, &e1);
    Queue_enqueue(q, &e2);
    printf("size(q) = %d\n", Queue_size(q));
    int *out = Queue_dequeue(q);
    printf("out == &e1 = %d size(q) = %d\n", out == &e1, Queue_size(q));
    out = Queue_front(q);
    printf("out == &e2 = %d\n", out == &e2);
    Queue_dequeue(q);
    printf("empty(q) = %d\n", Queue_empty(q));
    Queue_free(&q);
    printf("q == NULL = %d\n", q == NULL);

    printf("%s\n", "done");
    return 0;
}
