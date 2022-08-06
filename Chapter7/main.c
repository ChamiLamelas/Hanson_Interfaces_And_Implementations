#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>'

double scale_sum(int num, double factor, ...)
{
    // Object used for iterating over variable length arguments
    va_list ap;
    // Starts iteration, provide it with the last named argument
    // note that its type has no impact on the following arguments
    va_start(ap, factor);

    // Calculate sum of num arguments
    int total = 0;
    for (int i = 0; i < num; i++)
    {
        total += va_arg(ap, int);
    }

    // End necessary to end variable argument reading before return
    va_end(ap);
    return total * factor;
}

// comp must take const void *, const int *
// causes compilation error
int comp(const void *a, const void *b)
{
    // https://en.wikipedia.org/wiki/Qsort
    int x = *(int *)a;
    int y = *(int *)b;
    return (x > y) - (x < y);
}

int main(int argc, char *argv[])
{
    printf("%f\n", scale_sum(3, 0.5, 2, 3, 4));

    int a[5] = {5, 4, 3, 2, 1};
    qsort(a, 5, sizeof(a[0]), comp);
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", a[i]);
    }

    return 0;
}
