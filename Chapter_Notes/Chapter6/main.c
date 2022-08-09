#include <stdio.h>
#include <stdlib.h>

/*
What is the point of a free list with the arena system? Isn't the point
of the system to maintain a larger allocated area and service user
code from that larger allocated area and then deallocate it all at the end?

Why do we want to avoid calls to malloc with a free list? Is malloc in some
way bad? This is not explained. Found this: https://stackoverflow.com/a/12825202

How are the free list chunks freed? Arena_free doesn't seem to free them, nor
does Arena_dispose?
*/

struct A
{
    char *x;
    char *y;
};

int main(int argc, char *argv[])
{
    void *vp = malloc(100);
    // This demo demonstrates how different pointers work. Most importantly,
    // it shows that we can convert from void* to char*, int*, and a struct
    // pointer. What changes with the different pointers is the result of
    // dereferencing (as discussed here: https://stackoverflow.com/a/17260931)
    // Here, we can see that with the same allocated 100 byte block, with
    // the 3 different pointer types, we get access to different amounts
    // of the allocated memory with the dereferencing (1 byte would be
    // written if we were to write to *cp, 4 bytes would be written if
    // we were to write to *ip, and 16 bytes would be written if we were
    // to write to *sp). This explains why *ptr = *arena copies over the
    // arena fields on the bottom of page 95.
    char *cp = vp;
    printf("%d\n", sizeof(*cp));
    int *ip = vp;
    printf("%d\n", sizeof(*ip));
    struct A *sp = vp;
    printf("%d\n", sizeof(*sp));

    // Cannot implicitly convert between non void pointer types
    // But you can with void *: https://stackoverflow.com/a/35410601
    char *c2p = (char *)ip;

    free(vp);
    return 0;
}
