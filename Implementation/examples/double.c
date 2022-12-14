/*
Output for: ./double test2.txt

test2.txt:3: a
test2.txt:7: c

Line 1: improvement over provided implementation where words are considered
non space characters. Here, words are considered alphanumeric characters, so
[a a.] is considered an adjacent instance of a.

Line 2: this matches implementation, if a line ends with a word that starts the
next line, then that is counted as an adjacent instance.
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "../libraries/getword.h"

#define MAX_WORD_SIZE 256

static void scan_file(char *fp);

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
        scan_file(argv[i]);
    if (argc == 1)
        scan_file(NULL);
    return 0;
}

void scan_file(char *fp)
{
    FILE *f = (fp == NULL) ? stdin : fopen(fp, "r");
    if (f == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
    }

    int curr_line = 1;
    char curr_buf[MAX_WORD_SIZE];
    char prev_buf[MAX_WORD_SIZE];

    while (get_word(f, curr_buf, &curr_line, MAX_WORD_SIZE, isalnum))
    {
        if (curr_line > 1 && strcmp(curr_buf, prev_buf) == 0)
            if (fp == NULL)
                printf("%d: %s\n", curr_line, curr_buf);
            else
                printf("%s:%d: %s\n", fp, curr_line, curr_buf);
        strcpy(prev_buf, curr_buf);
    }

    fclose(f);
}
