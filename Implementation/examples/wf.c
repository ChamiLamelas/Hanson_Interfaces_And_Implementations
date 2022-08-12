#include "../libraries/table.h"
#include "../libraries/getword.h"
#include "../libraries/atom.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_WORD_SIZE 256

static void scan_file(char *fp);
static void free_count(void *key, void **value, void *cl);
static void display_counts(Table_T counts);

int main(int argc, char *argv[])
{
    if (argc == 1)
        scan_file(NULL);
    else
        for (int i = 1; i < argc; i++)
            scan_file(argv[i]);
    return 0;
}

void scan_file(char *fp)
{
    FILE *f = fp ? fopen(fp, "r") : stdin;
    if (f == NULL)
    {
        fputs(strerror(errno), stderr);
        return;
    }
    Table_T counts = Table_new(100, NULL, NULL);
    char word_buf[MAX_WORD_SIZE];
    char *word_atom;
    int *word_count;
    while (get_word(f, word_buf, NULL, MAX_WORD_SIZE, isalnum))
    {
        word_atom = (char *)Atom_string(word_buf);
        word_count = Table_get(counts, word_atom);
        if (word_count)
            (*word_count)++;
        else
        {
            word_count = malloc(sizeof(*word_count));
            assert(word_count);
            *word_count = 1;
        }
        Table_put(counts, word_atom, word_count);
    }
    if (fp)
        puts(fp);

    display_counts(counts);
    Table_map(counts, free_count, NULL);
    Table_free(&counts);
    fclose(f);
}

void free_count(void *key, void **value, void *cl)
{
    free(*value);
    *value = NULL;
}

static int compar(const void *x, const void *y)
{
    // x, y are really pointers to 2 void * next to each other
    // cast x to void ** and move by 1 to point to the second
    // pointer, then dereference that to get a void *
    // which is really an int *
    int *xvalue = *((void **)x + 1);
    int *yvalue = *((void **)y + 1);
    // Opposite of regular comparison function
    // x < y = 1 => compar -> 1
    // x > y = 1 => compar -> -1
    return (*xvalue < *yvalue) - (*xvalue > *yvalue);
}

void display_counts(Table_T counts)
{
    void **out = Table_toArray(counts, NULL);
    // qsort works on array of [char *, int *] pairs
    qsort(out, Table_length(counts), sizeof(void *) * 2, compar);
    for (int i = 0; i < Table_length(counts) * 2; i += 2)
        printf("%s:%d\n", out[i], *((int *)out[i + 1]));
    free(out);
}
