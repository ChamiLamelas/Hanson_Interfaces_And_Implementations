#include "arith.h"
#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include "atom.h"
#include <limits.h>
#include <string.h>

static void Arith_test(void);
static void Stack_test(void);
static void Atom_test(void);

int main(int argc, char *argv[])
{
    // Arith_test();
    // Stack_test();
    Atom_test();
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