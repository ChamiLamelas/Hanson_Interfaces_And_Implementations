#include "getword.h"

int get_word(FILE *f, char *word_buf, int *curr_line_ptr, int word_buf_size,
             int inword(int c)) {
  int c = getc(f);

  for (; c != EOF && !inword(c); c = getc(f))
    if (curr_line_ptr && c == '\n') (*curr_line_ptr)++;

  int buf_idx = 0;
  for (; c != EOF && inword(c); c = getc(f))
    if (buf_idx < word_buf_size - 1) word_buf[buf_idx++] = c;
  word_buf[buf_idx] = '\0';

  if (buf_idx > 0) {
    ungetc(c, f);
    return 1;
  }
  return 0;
}