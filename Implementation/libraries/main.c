#include "arith.h"
#include <assert.h>
#include <stdio.h>

void Arith_test();

int main(int argc, char *argv[])
{
    Arith_test();
    return 0;
}

void Arith_test()
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