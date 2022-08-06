#include <stdio.h>

/*
A hash table is built on a table declared as struct binding**.
Why is it not a struct binding*? That is, an array of struct
binding values. By declaring the type as an array of pointers,
it is easy to check if a bucket in the table is empty by checking
if the pointer is NULL. Then, additions can be made as necessary.

Another interesting thing is freeing a linked list:

void List_free(List_T *list) {
    List_T tmp;
    for (; *list; *list = tmp) {
        // Does something like copying 0x111111 into
        // next from (*list)->link. It is not pointing to
        // any data that will be deallocated.
        tmp = (*list)->link;
        FREE(*list);
    }
}

On line 15 tmp makes a copy of (*list)->link which is an address
identifying where the next node is. Then, FREE will delete the
data occupied by the data pointed to by a List_T (which will
include the data pointer and next pointer ~ 16 bytes). The copy
must be done before or the next pointer (and all the pointed to
data from *list) will be lost as FREE sets it to NULL, or if using
free(), the pointed to memory will be undefined.

In the wf example, FREE() is called on the array returned by
Table_toArray after the array has been used. Note that the
array is of type void** so calling FREE() will free space
for all the void* pointers stored in the array. No data
is lost in this process as the pointers are just copies
of pointers stored in the hash table.
*/

int main(int argc, char *argv[])
{
    printf("%s", "Hello, World!");
    return 0;
}
