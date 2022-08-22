/*
He seems to use putc when writing to a file stream, what is the
difference between putc and fputc? Both seem to have same
return type and parameters:
https://www.tutorialspoint.com/c_standard_library/c_function_putc.htm
https://www.tutorialspoint.com/c_standard_library/c_function_fputc.htm

fputc is apparently prefered: https://stackoverflow.com/questions/14008907/fputc-vs-putc-in-c

It seems that the reason flags is 
*/

#include <stdio.h>

// Valid to have function take a parameter array that specifies
// its length - this is done in chapter 14 for various ararys
// Some discourage this: https://stackoverflow.com/a/5187301
void func_w_arr(int a[1])
{
    printf("%d\n", a[0]);
}

int my_put(int c, void *cl)
{
    return c;
}

// Function can take another function as a paramer by not declaring
// it to be a function pointer, see: https://stackoverflow.com/a/6893288
// However, it seems common to declare a typedef with a function pointer
// in the definition, see: https://stackoverflow.com/a/840504
void func_w_func(int put(int c, void *cl))
{
    printf("%d\n", put(100, NULL));
}

int main(int argc, char *argv[])
{
    int b[1] = {1};
    func_w_arr(b);
    func_w_func(my_put);
    return 0;
}
