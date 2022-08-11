#include "arith.h"
#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include "atom.h"
#include <limits.h>
#include <string.h>
#include "except.h"
#include "mem.h"
#include "list.h"

static Except_T ex = {"exception"};
static Except_T ex2 = {"exception2"};

static void Arith_test(void);
static void Stack_test(void);
static void Atom_test(void);
static void Except_test(void);
static void Mem_test(void);
static void List_test(void);

int main(int argc, char *argv[])
{
    // Arith_test();
    // Stack_test();
    // Atom_test();
    // Except_test();
    // Mem_test();
    List_test();
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
    assert((*(char *)ptr) == 0);
    FREE(ptr);
    ptr = CALLOC(1, 1);
    assert(ptr);
    assert((*(char *)ptr) == 0);
    FREE(ptr);
    NEW(ptr);
    assert(ptr);
    FREE(ptr);
    NEW0(ptr);
    assert(ptr);
    assert((*(char *)ptr) == 0);
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

void apply1(void **e, void *cl)
{
    *((int *)cl) += **((int **)e);
}

void apply2(void **e, void *cl)
{
    *e = cl;
}

void List_test()
{
    int data[3] = {1, 2, 3};
    int *p[3];
    for (int i = 0; i < 3; i++)
        p[i] = &data[i];
    int out = 0;
    int *pout = &out;
    puts("LIST(1,2,3,NULL)");
    List_T ls = List_list(p[0], p[1], p[2], NULL);
    assert(ls->data == p[0]);
    assert(ls->next->data == p[1]);
    assert(ls->next->next->data == p[2]);
    assert(ls->next->next->next == NULL);
    puts("LIST(NULL)");
    List_T ls2 = List_list(NULL);
    assert(ls2 == NULL);
    puts("LIST PUSH (NULL,1)");
    List_T ls3 = List_push(NULL, p[0]);
    assert(ls3->data == p[0]);
    assert(ls3->next == NULL);
    puts("LIST PUSH (ls,1)");
    List_T ls4 = List_push(ls, p[0]);
    assert(ls4->data == p[0]);
    assert(ls4->next->data == p[0]);
    assert(ls4->next->next->data == p[1]);
    assert(ls4->next->next->next->data == p[2]);
    assert(ls4->next->next->next->next == NULL);
    puts("LIST POP (ls4)");
    List_T ls5 = List_pop(ls4, (void **)&pout);
    assert(pout == p[0]);
    assert(ls5->data == p[0]);
    assert(ls5->next->data == p[1]);
    assert(ls5->next->next->data == p[2]);
    assert(ls5->next->next->next == NULL);
    puts("LIST POP (NULL)");
    List_T ls6 = List_pop(NULL, (void **)&pout);
    assert(pout == p[0]);
    assert(ls6 == NULL);
    puts("LIST POP (ls4, NULL)");
    // Uncomment for crash
    // List_T ls7 = List_pop(ls4, NULL);
    puts("LIST APPEND");
    List_T ls8 = List_list(p[0], p[1], NULL);
    List_T ls9 = List_list(p[2], NULL);
    List_T ls10 = List_append(ls8, ls9);
    assert(ls10->data == p[0]);
    assert(ls10->next->data == p[1]);
    assert(ls10->next->next->data == p[2]);
    assert(ls10->next->next->next == NULL);
    puts("LIST (,NULL)");
    List_T ls11 = List_list(p[0], p[1], NULL);
    List_T ls12 = List_append(ls11, NULL);
    assert(ls12->data == p[0]);
    assert(ls12->next->data == p[1]);
    assert(ls12->next->next == NULL);
    puts("LIST (NULL,)");
    List_T ls13 = List_list(p[2], NULL);
    List_T ls14 = List_append(NULL, ls13);
    assert(ls14->data == p[2]);
    assert(ls14->next == NULL);
    puts("LIST REVERSE");
    List_T ls15 = List_list(p[0], p[1], NULL);
    List_T ls16 = List_reverse(ls15);
    assert(ls16->data == p[1]);
    assert(ls16->next->data == p[0]);
    assert(ls16->next->next == NULL);
    puts("LIST REVERSE (NULL)");
    List_T ls17 = List_reverse(NULL);
    assert(ls17 == NULL);
    puts("LIST COPY");
    List_T ls18 = List_list(p[0], p[1], NULL);
    List_T ls19 = List_copy(ls18);
    assert(ls19->data == p[0]);
    assert(ls19->next->data == p[1]);
    assert(ls19->next->next == NULL);
    assert(ls19 != ls18);
    assert(ls19->next != ls18->next);
    puts("LIST COPY (NULL)");
    List_T ls20 = List_copy(NULL);
    assert(ls20 == NULL);
    puts("LIST COPY (1)");
    List_T ls21 = List_list(p[2], NULL);
    List_T ls22 = List_copy(ls21);
    assert(ls22->data = p[2]);
    assert(ls22->next == NULL);
    assert(ls22 != ls21);
    puts("LIST LENGTH");
    int len = List_length(NULL);
    assert(len == 0);
    len = List_length(ls21);
    assert(len == 1);
    len = List_length(ls18);
    assert(len == 2);
    puts("LIST APPLY 1");
    int sum = 0;
    List_T ls23 = List_list(p[0], p[1], p[2], NULL);
    List_map(ls23, apply1, &sum);
    assert(sum == 6);
    puts("LIST APPLY 2");
    List_T ls24 = List_list(p[0], p[1], p[2], NULL);
    List_map(ls24, apply2, &sum);
    assert(ls24->data == &sum);
    assert(ls24->next->data == &sum);
    assert(ls24->next->next->data == &sum);
    puts("LIST TOARRAY");
    List_T ls25 = List_list(p[0], p[1], p[2], NULL);
    int **arr = (int **)List_toArray(ls25, NULL);
    for (int i = 0; i < 3; i++)
        assert(arr[i] == p[i]);
    assert(arr[3] == NULL);
    puts("LIST TOARRAY 1");
    List_T ls26 = List_list(p[0], NULL);
    int **arr2 = (int **)List_toArray(ls26, NULL);
    assert(arr2[0] == p[0]);
    assert(arr2[1] == NULL);
    puts("LIST TOARRAY NULL");
    int **arr3 = (int **)List_toArray(NULL, NULL);
    assert(arr3[0] == NULL);
    puts("List_test done");
}