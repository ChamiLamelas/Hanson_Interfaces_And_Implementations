#include "arith.h"

int Arith_max(int x, int y)
{
    return (x > y) ? x : y;
}

int Arith_min(int x, int y)
{
    return (x < y) ? x : y;
}

int Arith_div(int x, int y)
{
    return x / y - (x < 0 != y < 0 && x % y != 0 && -13 / 5 == -2);
}

int Arith_mod(int x, int y)
{
    return x - y * Arith_div(x, y);
}

int Arith_ceiling(int x, int y)
{
    return Arith_div(x, y) + (x % y != 0);
}

int Arith_floor(int x, int y)
{
    return Arith_div(x, y);
}