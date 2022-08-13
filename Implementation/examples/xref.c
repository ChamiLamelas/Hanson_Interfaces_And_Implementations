#include "../libraries/set.h"
#include "../libraries/table.h"
#include "../libraries/getword.h"
#include "../libraries/atom.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD_SIZE 256
#define DEFAULT_HINT 100

static void scan_file(char *fp, Table_T refs);
static void display_and_clear(Table_T *refs);

int main(int argc, char *argv[])
{
    Table_T refs = Table_new(DEFAULT_HINT, NULL, NULL);
    for (int i = 1; i < argc; i++)
        scan_file(argv[i], refs);
    display_and_clear(&refs);
    Atom_free();
    return 0;
}

int cmp(const void *x, const void *y)
{
    int xval = *((int *)x);
    int yval = *((int *)y);
    return (xval > yval) - (xval < yval);
}

unsigned hash(const void *x)
{
    return (*((int *)x)) * 2654435761;
}

void scan_file(char *fp, Table_T refs)
{
    FILE *f = fopen(fp, "r");
    if (f == NULL)
    {
        fputs(strerror(errno), stderr);
        return;
    }

    char word_buf[MAX_WORD_SIZE];
    char *word_atom;
    char *fp_atom;
    int curr_line = 1;
    Table_T files;
    Set_T lines;
    int *member;
    while (get_word(f, word_buf, &curr_line, MAX_WORD_SIZE, isalnum))
    {
        word_atom = (char *)Atom_string(word_buf);
        files = Table_get(refs, word_atom);
        if (files == NULL)
        {
            files = Table_new(DEFAULT_HINT, NULL, NULL);
            Table_put(refs, word_atom, files);
        }
        fp_atom = (char *)Atom_string(fp);
        lines = Table_get(files, fp_atom);
        if (lines == NULL)
        {
            lines = Set_new(DEFAULT_HINT, cmp, hash);
            Table_put(files, fp_atom, lines);
        }
        member = malloc(sizeof(*member));
        *member = curr_line;
        Set_put(lines, member);
    }
}

int file_compar(const void *x, const void *y)
{
    char *file1 = *((void **)x);
    char *file2 = *((void **)y);
    return strcmp(file1, file2);
}

int line_compar(const void *x, const void *y)
{
    int *xm = *((void **)x);
    int *ym = *((void **)y);
    return (*xm > *ym) - (*xm < *ym);
}

static void display_and_clear_lines(Set_T lines)
{
    void **arr = Set_toArray(lines, NULL);
    qsort(arr, Set_length(lines), sizeof(*arr), line_compar);
    for (int i = 0; arr[i]; i++)
    {
        printf(" %d", *((int *)arr[i]));
        free(arr[i]);
    }
    free(arr);
}

static void refs_apply(void *word, void **files, void *cl)
{
    printf("%s", word);
    void **arr = Table_toArray(*files, NULL);
    qsort(arr, Table_length(*files), 2 * sizeof(*arr), file_compar);
    for (int i = 0; arr[i]; i += 2)
    {
        printf("\t%s:", arr[i]);
        display_and_clear_lines(arr[i + 1]);
        printf("\n");
    }
    Table_free((Table_T *)files);
    free(arr);
}

void display_and_clear(Table_T *refs)
{
    Table_map(*refs, refs_apply, NULL);
    Table_free(refs);
}