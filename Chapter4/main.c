#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>


/*
A couple things to note about this macro: 
1. To separate a macro over multiple lines, end each line with \
2. It is customary with macros to surround instances of the parameter in parenthesis
   in case the macro is applied to some expression. 
3. (a,b,c) is valid in C. It will evaluate a, then b, then c but only return c. Thus,
   I believe that 0 is included at the end of the function body (which is a print,
   abort, then a 0) in order that something can be returned. abort() doesn't return
   anything so we can't end with that (compile error).

Note also the meaning of e1 || e2 being used here and the reason for the void cast
provided in the reading.

See also: 
https://stackoverflow.com/questions/8896281/why-am-i-getting-void-value-not-ignored-as-it-ought-to-be
https://stackoverflow.com/questions/54142/how-does-the-comma-operator-work
https://stackoverflow.com/questions/39628799/void-cast-macro-explaination

See also on why we need do-while(0) even when code blocks with {} exist
in C:
https://stackoverflow.com/questions/154136/why-use-apparently-meaningless-do-while-and-if-else-statements-in-macros
*/
#define assert(e) ((void)((e) ||                                            \
                          (fprintf(stderr, "%s:%d: Assertion failed: %s\n", \
                                   __FILE__, (int)__LINE__, #e),            \
                           abort(), 0)))

/*
User errors: errors based on user entry, should be handled and not trigger a crash.
Runtime errors: program bugs (which is why they are enforced via asserts in libraries)
                and should trigger a crash.
Exceptions: unexpected errors typically related to resources (e.g. running out of
            memory, file error, etc.), possibly recoverable.
*/

/*
What is setjmp and longjmp? longjmp triggers goto like behavior and setjmp
sets the place where the jump goes. The jmp_buf stores the environment for
the jump to occur (connecting the two jumps). setjmp on initial call is
guaranteed to return 0. Hence, when calling longjmp one should pass a
nonzero value as that is what will be returned by setjmp on the jump to it.
That can be used to report the error and terminate execution.

Demo 1: https://en.wikipedia.org/wiki/Setjmp.h
Demo 2: https://www.tutorialspoint.com/c_standard_library/c_function_longjmp.htm

volatile keyboard specifies that a variable may be changed outside of
the code (i.e. dependent on implementation or something else). Hence it is
used for variables declared in between setjmp, longjmp:
https://stackoverflow.com/questions/246127/why-is-volatile-needed-in-c
*/

static jmp_buf buf1;
static jmp_buf buf2;

void second()
{
    printf("second\n");
    // Trigger the jump with a nonzero value
    longjmp(buf1, 1);
}

void first()
{
    second();
    printf("first\n");
}

void jmp_demo1()
{
    // Sets the jump point for the associated with environment buffer.
    int val = setjmp(buf1);

    // Observe here that this prints 0 then 1.
    printf("val = %d\n", val);

    // val starts 0, so first() runs but then we return here
    // after jump
    if (val)
        printf("Exception thrown\n");
    else
        first();
}

void jmp_function()
{
    longjmp(buf2, 1);
}

void jmp_demo2()
{
    int val = setjmp(buf2);
    printf("%d\n", val);
    if (val != 0)
    {
        printf("Returned from a longjmp() with value = %d\n", val);
        exit(0); // Immediately terminates program
    }
    jmp_function();
}

struct T
{
    int a;
    int b;
};

// Enums: default start from 0, can also specify enumeration start
enum
{
    x,
    y,
    z
};

enum
{
    a = 1,
    b,
    c
};

int main(int argc, char *argv[])
{
    // jmp_demo1();
    // jmp_demo2();

    // Can get current file and line using preprocessor macros
    printf("%s %d\n", __FILE__, __LINE__);

    // Can initialize a struct with literals like this:
    struct T t = {1, 2};
    printf("%d %d\n", t.a, t.b);

    // Can also partially initialize structs
    struct T t2 = {1};
    printf("%d %d\n", t2.a, t2.b);

    printf("%d %d %d\n", x, y, z);
    printf("%d %d %d\n", a, b, c);

    return 0;
}
