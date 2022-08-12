#include <stdio.h>

// Why does cmpint take (int**) hidden in void*?
// Why even make cmpint and intcmp? Nothing online for this
// In multiple places constness is just cast away with (void *), can't
// find anything online about this regarding legality/if this is good practice
// Why choose the following hint in Set_minus?
// T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
// It seems to me in Set_minus, we are doing t-s (not s-t like the text says).
// What if t and s are disjoint and s has a smaller cardinality than t?
// Then, t-s = t. Why choose s->size as the hint? Choosing t->size seems
// more reasonable, it will only be an overestimate.

int check(void *b)
{
    return b != NULL;
}

int func()
{
    return 0;
}

int *nfunc() {
    return NULL;
}

int main(int argc, char *argv[])
{
    int y = 1;
    void *x = (int *)&y;
    printf("%p %p\n", x, &x);

    printf("%d %d\n", check(NULL), check(x));

    printf("%d %d\n", !func(), !nfunc());

    printf("%s", "Hello, World!");
    return 0;
}
