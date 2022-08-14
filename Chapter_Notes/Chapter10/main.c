#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int *arr;
    arr = malloc(sizeof(*arr));
    *arr = 1;
    arr = realloc(arr, 3 * sizeof(*arr));
    printf("%d %d %d\n", arr[0], arr[1], arr[2]);
    memset(((char *)arr) + sizeof(*arr), 0, 2 * sizeof(*arr));
    printf("%d %d %d\n", arr[0], arr[1], arr[2]);
    return 0;
}
