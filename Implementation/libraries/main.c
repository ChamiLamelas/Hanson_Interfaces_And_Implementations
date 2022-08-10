#include "arith.h"
#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include "atom.h"
#include <limits.h>
#include <string.h>
#include "except.h"
#include "mem.h"

static Except_T ex = {"exception"};
static Except_T ex2 = {"exception2"};

static void Arith_test(void);
static void Stack_test(void);
static void Atom_test(void);
static void Except_test(void);
static void Mem_test(void);

int main(int argc, char *argv[])
{
    // Arith_test();
    // Stack_test();
    // Atom_test();
    // Except_test();
    Mem_test();
    return 0;
}

void Arith_test(void)
{
    assert(Arith_max(2, 2) == 2);
    assert(Arith_max(2, 3) == 3);
    assert(Arith_max(-2, -3) == -2);
    assert(Arith_min(2, 3) == 2);
    assert(Arith_min(2, 2) == 2);
    assert(Arith_min(-2, -3) == -3);
    assert(Arith_div(13, 5) == 2);
    assert(Arith_div(-13, -5) == 2);
    assert(Arith_div(15, 5) == 3);
    assert(Arith_div(-15, -5) == 3);
    assert(Arith_div(-13, 5) == -3);
    assert(Arith_div(-15, 5) == -3);
    assert(Arith_mod(13, 5) == 3);
    assert(Arith_mod(-13, -5) == -3);
    assert(Arith_mod(15, 5) == 0);
    assert(Arith_mod(-15, -5) == 0);
    assert(Arith_mod(-13, 5) == 2);
    assert(Arith_mod(-15, 5) == 0);
    assert(Arith_ceiling(13, 5) == 3);
    assert(Arith_ceiling(-13, -5) == 3);
    assert(Arith_ceiling(15, 5) == 3);
    assert(Arith_ceiling(-15, -5) == 3);
    assert(Arith_ceiling(-13, 5) == -2);
    assert(Arith_ceiling(-15, 5) == -3);
    assert(Arith_floor(13, 5) == 2);
    assert(Arith_floor(-13, -5) == 2);
    assert(Arith_floor(15, 5) == 3);
    assert(Arith_floor(-15, -5) == 3);
    assert(Arith_floor(-13, 5) == -3);
    assert(Arith_floor(-15, 5) == -3);
    puts("Arith_test finished");
}

void Stack_test(void)
{
    int data[5] = {0, 1, 2, 3, 4};

    Stack_T s = Stack_new();
    assert(Stack_empty(s));

    Stack_push(s, &data[0]);
    assert(Stack_size(s) == 1);
    assert(*((int *)Stack_top(s)) == data[0]);

    Stack_push(s, &data[1]);
    assert(*((int *)Stack_top(s)) == data[1]);

    assert(*((int *)Stack_pop(s)) == data[1]);
    assert(Stack_size(s) == 1);

    for (int i = 2; i < 5; i++)
    {
        Stack_push(s, &data[i]);
    }
    assert(Stack_size(s) == 4);
    assert(*((int *)Stack_top(s)) == data[4]);

    Stack_free(&s);
    assert(s == NULL);
    puts("Stack_test finished");
}

void Atom_test(void)
{
    const char *atom_abc = Atom_string("abc");
    const char *atom_abc2 = Atom_string("abc");
    assert(atom_abc == atom_abc2);
    const char *atom_1234 = Atom_int(1234);
    const char *atom_1234_2 = Atom_int(1234);
    assert(atom_1234 == atom_1234_2);
    assert(Atom_length(atom_abc) == 3);
    assert(Atom_length(atom_1234) == 4);
    const char *atom_long_min = Atom_int(LONG_MIN);
    const char *atom_long_min_2 = Atom_int(LONG_MIN);
    // load LONG_MIN into a string for comparison
    char check[255];
    sprintf(check, "%ld", LONG_MIN);
    assert(strcmp(check, atom_long_min) == 0);
    assert(atom_long_min == atom_long_min_2);
    const char *atom_empty = Atom_string("");
    const char *atom_empty2 = Atom_string("");
    assert(atom_empty == atom_empty2);
    puts("Atom_test finished");
}

void Raise_Ex(void)
{
    RAISE(ex);
}

void Raise_and_Except_Ex(void)
{
    TRY
        RAISE(ex);
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    END_TRY;
}

void Except_test()
{
    puts("-- TRY END_TRY");
    TRY
        puts("hello");
    END_TRY;

    puts("-- TRY EXCEPT END_TRY");
    TRY
        puts("hello");
    EXCEPT(ex)
    puts("Should not print");
    END_TRY;

    puts("-- TRY RAISE EXCEPT END_TRY");
    TRY
        RAISE(ex);
    EXCEPT(ex)
    printf("Caught exception %s\n", ex.reason);
    END_TRY;

    puts("-- TRY RAISE END_TRY");
    // TRY
    //     RAISE(ex);
    // END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT END_TRY");
    // TRY
    //     RAISE(ex);
    // EXCEPT(ex2)
    //     puts("Caught different exception");
    // END_TRY;

    puts("-- TRY RAISE FINALLY END_TRY");
    // TRY
    //     RAISE(ex);
    // FINALLY
    //     puts("Finally runs before abort");
    // END_TRY;

    puts("-- TRY RAISE EXCEPT FINALLY END_TRY");
    TRY
        RAISE(ex);
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    FINALLY
    puts("Finally runs after handling");
    END_TRY;

    puts("-- TRY RAISE ELSE END_TRY");
    TRY
        RAISE(ex);
    ELSE
        puts("Else");
    END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT ELSE END_TRY");
    TRY
        RAISE(ex2);
    EXCEPT(ex)
    puts("Caught different exception");
    ELSE
        puts("Caught everything");
    END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT ELSE FINALLY END_TRY");
    TRY
        RAISE(ex2);
    EXCEPT(ex)
    puts("Caught different exception");
    ELSE
        puts("Caught everything");
    FINALLY
    puts("Finally runs after handling");
    END_TRY;

    puts("-- CATCH RAISED EXCEPTION FROM NESTED FUNCTION");
    TRY
    Raise_Ex();
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    END_TRY;

    puts("-- CATCH RAISED EXCEPTIONS ON TWO LEVELS");
    TRY
    Raise_and_Except_Ex();
    RAISE(ex2);
    EXCEPT(ex2)
    printf("Exception %s\n", ex2.reason);
    END_TRY;

    puts("-- CRASH FROM NESTED FUNCTION");
    // Raise_Ex();

    puts("Except_test done");
}

void Mem_test()
{
    void *ptr = NULL;
    ptr = Mem_alloc(1, __FILE__, __LINE__);
    assert(ptr);
    Mem_free(ptr, __FILE__, __LINE__);
    ptr = NULL;
    ptr = ALLOC(1);
    assert(ptr);
    FREE(ptr);
    assert(ptr == NULL);
    ptr = Mem_calloc(1, 1, __FILE__, __LINE__);
    assert(ptr);
    assert((*(char *)ptr)==0);
    FREE(ptr);
    ptr = CALLOC(1, 1);
    assert(ptr);
    assert((*(char *)ptr)==0);
    FREE(ptr);
    NEW(ptr);
    assert(ptr);
    FREE(ptr);
    NEW0(ptr);
    assert(ptr);
    assert((*(char *)ptr)==0);
    FREE(ptr);
    // Uncomment to see these all abort via assert
    // ptr = ALLOC(-1);
    // ptr = CALLOC(-1, 1);
    // ptr = CALLOC(1, -1);
    NEW(ptr);
    // ptr = RESIZE(ptr,-1);
    FREE(ptr);
    // FREE(ptr);
    // RESIZE(ptr,1);
    puts("Mem_test done");
}