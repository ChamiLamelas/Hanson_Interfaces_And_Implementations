#include <stdio.h>

int x = 0;

// Can return the result of an assignment
int func()
{
    return x = 1;
}

int main(int argc, char *argv[])
{
    // Can chain assignments
    int a;
    int b;
    int c;
    a = b = c = 2;

    printf("%d %d %d %d\n", func(), a, b, c);
    

    return 0;
}
