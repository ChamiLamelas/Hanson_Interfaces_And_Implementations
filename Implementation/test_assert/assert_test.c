#include "except.h"
#include "assert.h"


int main(int argc, char*argv[]) {
    assert(1);
    TRY
        assert(0);
    EXCEPT (Assert_Failed)
        puts("Assertion failed + caught");
    END_TRY;
    assert(0);
    return 0;
}