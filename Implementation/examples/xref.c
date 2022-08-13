#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libraries/atom.h"
#include "../libraries/getword.h"
#include "../libraries/set.h"
#include "../libraries/table.h"

#define MAX_WORD_SIZE 256
#define DEFAULT_HINT 100

static void scan_file(char *fp, Table_T refs);
// In my implementation, my display functions also do cleanup
// (i.e. freeing all dynamically allocated memory)
static void display_and_clear_refs(Table_T *refs);

int main(int argc, char *argv[]) {
  Table_T refs = Table_new(DEFAULT_HINT, NULL, NULL);
  for (int i = 1; i < argc; i++) scan_file(argv[i], refs);
  display_and_clear_refs(&refs);
  Atom_free();
  return 0;
}

int cmp(const void *x, const void *y) {
  int xval = *((int *)x);
  int yval = *((int *)y);
  return (xval > yval) - (xval < yval);
}

unsigned hash(const void *x) { return (*((int *)x)) * 2654435761; }

void scan_file(char *fp, Table_T refs) {
  FILE *f = fopen(fp, "r");
  if (f == NULL) {
    fputs(strerror(errno), stderr);
    return;
  }

  char word_buf[MAX_WORD_SIZE];
  char *word_atom;
  char *fp_atom = (char *)Atom_string(fp);
  int curr_line = 1;
  Table_T files;
  Set_T lines;
  int *member;
  while (get_word(f, word_buf, &curr_line, MAX_WORD_SIZE, isalnum)) {
    word_atom = (char *)Atom_string(word_buf);
    files = Table_get(refs, word_atom);
    if (files == NULL) {
      files = Table_new(DEFAULT_HINT, NULL, NULL);
      Table_put(refs, word_atom, files);
    }
    lines = Table_get(files, fp_atom);
    if (lines == NULL) {
      lines = Set_new(DEFAULT_HINT, cmp, hash);
      Table_put(files, fp_atom, lines);
    }
    /*
    This check is very important. Consider that we have a word that
    that appears multiple times on the same line (say at line 6).
    The first time, we allocate a new integer to store 6. The second
    time, suppose we don't have the if guard. Another new integer is
    created to store 6 and put into the Set. The address of the new
    dynamically allocated 6 will replace the address of the previously
    dynamically allocated 6. That address will be lost creating a
    memory leak.
    */
    if (!Set_member(lines, &curr_line)) {
      member = malloc(sizeof(*member));
      *member = curr_line;
      Set_put(lines, member);
    }
  }
}

int str_compar(const void *x, const void *y) {
  return strcmp(*((void **)x), *((void **)y));
}

int int_compar(const void *x, const void *y) {
  int *xm = *((void **)x);
  int *ym = *((void **)y);
  return (*xm > *ym) - (*xm < *ym);
}

static void display_and_clear_lines(Set_T *lines) {
  void **arr = Set_toArray(*lines, NULL);
  qsort(arr, Set_length(*lines), sizeof(*arr), int_compar);
  for (int i = 0; arr[i]; i++) {
    printf(" %d", *((int *)arr[i]));
    free(arr[i]);
  }
  free(arr);
  Set_free(lines);
}

static void display_and_clear_files(Table_T *files) {
  void **arr = Table_toArray(*files, NULL);
  qsort(arr, Table_length(*files), 2 * sizeof(*arr), str_compar);
  for (int i = 0; arr[i]; i += 2) {
    printf("\t%s", arr[i]);
    display_and_clear_lines((Set_T *)(arr + i + 1));
    printf("\n");
  }
  free(arr);
  Table_free(files);
}

void display_and_clear_refs(Table_T *ref) {
  void **arr = Table_toArray(*ref, NULL);
  qsort(arr, Table_length(*ref), sizeof(*arr) * 2, str_compar);
  for (int i = 0; arr[i]; i += 2) {
    printf("%s", arr[i]);
    display_and_clear_files((Table_T *)(arr + i + 1));
  }
  free(arr);
  Table_free(ref);
}