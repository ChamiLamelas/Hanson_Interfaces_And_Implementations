#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
In 5.3, he mentions that one wants to protect against
allocating to the same address multiple times. I am unsure
why this should not be allowed except for this case.

Suppose we have:

int *p;
p = malloc(sizeof(*p));
...
free(p);
...
int *q;
q = malloc(sizeof(*q));

Suppose at this point (by chance) p == q. This means we have
p (which appeared until the second malloc to be a dangling
pointer) inadvertently pointing to some other day which
could lead to further bugs. This is the first common
memory management error given in section 5.

Why does ((x + y - 1)//y) * y round up to the smallest multiple
of y that is larger than x?

Let x = uy + v where 0 <= v < y and u is an integer.
Get: ((uy + v + y - 1)//y) * y
// is integer division so this yields:
(u+1) * y

This is indeed the smallest multiple of y that is larger than x.
*/

#define NULL_SET(p) ((p) = NULL)

union Data
{
    int i;
    float f;
    char c[20];
};

int main(int argc, char *argv[])
{
    int *a;
    int *b;

    // Two different allocation strategies, calloc initializes
    // to 0s (can specify an array size too)
    a = malloc(sizeof(*a));
    b = calloc(10, sizeof(*b));

    printf("%d\n", *a);

    for (int i = 0; i < 10; i++)
    {
        printf("%d ", *(b + i));
    }
    printf("\n");

    // free is used with both malloc and calloc
    free(a);
    free(b);

    // There is also realloc which tries to expand
    // previously dynamically allocated memory
    // It is guaranteed to free old memory
    // if it cannot expand from that contiguously
    // https://stackoverflow.com/a/46461293
    // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm

    int *c;
    c = calloc(10, sizeof(*c));

    // Special formatter for pointer types, prints are included
    // to show if a new location is made
    printf("%p\n", c);

    c = realloc(c, 12);

    printf("%p\n", c);

    // Observe that realloc doesn't keep 0s from calloc
    for (int i = 0; i < 12; i++)
    {
        printf("%d ", *(c + i));
    }
    printf("\n");

    free(c);

    // This caused some confusion earlier, with a macro, since it replaces
    // the line of text, we can actually set a pointer to be null (hence
    // its use in the improved mem library)
    NULL_SET(a);
    NULL_SET(b);
    NULL_SET(c);
    printf("%p %p %p\n", a, b, c);

    // Can set any member of a union (which allows for multiple types
    // to be stored via 1 variable). However, only the most recently
    // set field is the one that retains its set value.
    union Data d;
    d.i = 1;
    d.f = 2.2;

    printf("%d %f\n", d.i, d.f);
    printf("%d\n", sizeof(d));

    // Can't modify the contents of a string literal, so you can't
    // use memset or even do something like *str = 'a'
    char *str = "hello world";

    // Real example of memset - function that modifies some part
    // of a string that can be modified (also puts is useful
    // as puts(s) = printf("%s\n",s )).
    char str2[12];
    strcpy(str2, "hello world");
    puts(str2);
    memset(str2, '*', 5);
    puts(str2);

    return 0;
}
