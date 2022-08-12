#ifndef GETWORD_INCLUDED
#define GETWORD_INCLUDED

#include <stdio.h>

extern int get_word(FILE *f, char *word_buf, int *curr_line_ptr, int word_buf_size, int inword(int c));

#endif