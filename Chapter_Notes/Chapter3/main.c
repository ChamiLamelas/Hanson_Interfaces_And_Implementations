#include <stdio.h>
#include <limits.h>
#include <string.h>

// The text includes this struct definition of atom. 3 things to note:
// 1. This is marked as static to indicate it is only used in this source
// file - it is not exposed elsewhere (See Kernigan and Ritchie 4.7)
// 2. Inside Atom_new, in the call to ALLOC (defined here
// https://github.com/drh/cii/blob/master/include/mem.h) which is just
// essentially a call to malloc, space for sizeof(struct atom)+ len + 1
// bytes is requested. Why does this work? malloc(b) will allocate b
// contiguous bytes. sizeof(link) + sizeof(len) + sizeof(str) = 24
// for the 3 pointers. malloc(sizeof(struct atom) + len + 1) will
// allocate 24 bytes followed by len + 1 bytes. The actual string
// can then be stored in those following bytes. This trick of asking
// for extra space then storing array elements in it works only
// if our array structure is the last member (see here
// https://stackoverflow.com/a/7436199)
// 3. p->str is set to be at p + 1. I assume this is the case because
// adding 1 to a pointer p to a struct atom will move a pointer by
// a struct atom in size. There isn't another struct atom but it moves
// the pointe rin position to write at the len + 1 spots allocated
// in the above malloc

static struct atom {
    struct atom *link;
    int len;
    char *str;
};

int main(int argc, char *argv[])
{
    // Misleading, sizeof matches array length only because char is 1 byte
    char a[43];
    printf("%d\n", sizeof(a));

    // Here sizeof is array length x 4 because int is 4 bytes
    int b[43];
    printf("%d\n", sizeof(b));

    // This illustrates the difference between the two copy functions
    // strcpy works with null terminated strings and stops the copy
    // while memcpy just copies the bytes regardless of null terminator.

    char c[3] = {'a', '\0', 'b'};
    char str_dest[3] = {'x', 'x', 'x'};
    char mem_dest[3] = {'x', 'x', 'x'};

    memcpy(mem_dest, c, 3);
    printf("mem: %c %c %c\n", mem_dest[0], mem_dest[1], mem_dest[2]);

    strcpy(str_dest, c);
    printf("str: %c %c %c\n", str_dest[0], str_dest[1], str_dest[2]);

    printf("%s\n", "Hello, World!");
    return 0;
}
