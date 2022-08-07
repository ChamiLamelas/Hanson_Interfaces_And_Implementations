#include <stdio.h>
#include <stdarg.h>

/*
In sections 7 and 11, he says that the 2nd, 3rd, and further arguments
passed to ... in var args functions must be cast to void * if they're not
char * b/c this doesn't happen implictly.  The following code seems to
indicate otherwise, because varg_fn seems to be able to take a bunch
of different types just fine. I was under the assumption that void *
was the C solution for a generic type, as discussed here:

https://stackoverflow.com/questions/1736833/void-pointers-difference-between-c-and-c

Unsure how SeqFree is implemented. Why do you use Array_free on a Sequence object?
How does this also free the length, head values in a Seq_T? I thought a more
reasonable implementaiton would be:

void Seq_free(T *seq) {
    // assertion checks...
    // (*seq->array) is a struct Array_T, Array_free takes a struct Array_T **
    Array_free(&&((*seq)->array));
    FREE(*seq)
}

Note that for Sequence and Ring, the ith value is returned by get as opposed
to a pointer to that value as in Array. Not sure why Array is implemented as
such, perhaps that dynamic array is implemented such that the elements are
specified by size not by value.
*/

void print_str(void *a)
{
    puts(a);
}

void varg_fn(void *a, ...)
{
    va_list ap;
    va_start(ap, a);
    void *tmp;
    for (; a; a = va_arg(ap, void *))
    {
        tmp = a;
    }

    va_end(ap);
}

struct e
{
    int a;
    int b;
};

int main(int argc, char *argv[])
{
    char *c = "abc";
    int i = 1;
    int *ip = &i;
    struct e x;
    x.a = 1;
    x.b = 2;
    struct e *xp = &x;

    print_str(c);
    // This compiles and runs for c, i, ip, x as well
    varg_fn(c, xp, NULL);

    return 0;
}
