/*
Note on sizeof with and without () : https://stackoverflow.com/a/28075117

In code chunk 255:

    map is built by initializing it so that map[c] is equal to c; that is, each
    character is mapped to itself.

Why? In case from and to don't specify mappings for all characters, we want
to still index map (easier loop) so we say if a character has no mapping,
it maps to itself.

Str_pos:

i could be positive or negative position. This function returns the equivalent
nonnegative position if its negative. The positive position left of index i is
i + 1, the positive position right of index is i + 2.

What's confusing about this implementation is that for a positive position,
this function converts i to i - 1 then returns i + 1 which is just the
original i, so I think all that's necessary is of a check if the position
is valid.

Also, in the description of this function, he says:

    A positive position can always be converted to an index by subtracting one,
so Str_pos is often used when an index is needed.

Take s = "Interface". 10 is a positive position. 10 - 1 is not a valid index,
indices only go from 0...8. Str_pos(s, 10) does return 10. Str_any seems to
pay attention to this edge case with the return if safeguard.

Str_find:

Note we go till i+len <= j because we don't count finds of strings that include
the jth character (basically we are looking at i,j range exclusive on the right)

Str_many + Str_rmany: I would use a return if then a while, not a do-while
nested in an if - confusing without understanding the interface requirements.

Overall with this library I am unsure of the benefit of the position over
index numbering system. The negatives help (as in Python), but why not mimic
the Python negative indexing which I find more intuitive? The specific
positioning requirements of the interface make it tricky to understand why
certain functions are implemented the way they are. Example: Str_match and
Str_rmatch are most easily understood with examples (like looking at behavior
on first and last 3 chars in "Interface").
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i)-1)
int Str_pos(const char *s, int i) {
    int len;
    assert(s);
    len = strlen(s);
    i = idx(i, len);
    assert(i >= 0 && i <= len);
    return i + 1;
}

int f() {
    puts("Running f");
    return 10;
}

// Can split string literal over multiple lines -they are just
// automatically concatenated together up to the semi-colon
// see: https://stackoverflow.com/a/24967488
char set[] =
    "0123456789_"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main(int argc, char *argv[]) {
    puts(set);

    // Sets a char array to all 0s (nothing printed)
    char a[256] = {0};
    puts(a);

    // Can also do fast initialization to all 0s with integer array
    int b[256] = {0};
    for (int i = 0; i < 256; i++) {
        printf("%d\n", b[i]);
    }

    printf("%d\n", Str_pos("Interface", 10));

    // Compare first n characters
    // https://www.tutorialspoint.com/c_standard_library/c_function_strncmp.htm
    char *bstr = "abz";
    char *cstr = "acy";
    printf("%d %d\n", strncmp(bstr, cstr, 1), strncmp(bstr, cstr, 2));

    for (int i = 0; i < f(); i++) {
        printf("%d\n", i);
    }

    puts("----");
    int fo = f();
    for (int i = 0; i < fo; i++) {
        printf("%d\n", i);
    }

    return 0;
}
