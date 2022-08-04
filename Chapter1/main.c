// 1 Include section

// https://www.tutorialspoint.com/c_standard_library/stdio_h.htm
// Includes fprintf, in/out streams, file reading
#include <stdio.h>

// https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
#include <errno.h>

// https://www.tutorialspoint.com/c_standard_library/string_h.htm
// Needed for strerror (not noted in text)
#include <string.h>

// https://www.tutorialspoint.com/c_standard_library/ctype_h.htm
// Needed for examining characters
#include <ctype.h>

// 2 Prototype section
void print_words(FILE *fp);
int get_word(FILE *fp, char *buf, int size);

// 3 Data section
int lines;

// 4 Code section

int main(int argc, char *argv[])
{
    // Formatted printing to a stream (e.g. error)
    // https://www.tutorialspoint.com/c_standard_library/c_function_fprintf.htm
    fprintf(stdout, "%s: %d\n", argv[0], 1);

    // Checking the error state (and getting it as a string)
    fprintf(stderr, "%s error state: %s\n", argv[0], strerror(errno));

    // https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
    // Opening file with a mode (syntax like Python open)
    // Here fp: file pointer (not path)
    FILE *fp = fopen("test.txt", "r");

    FILE *fp2 = fopen("doesntexist.txt", "r");

    // Error occurred, check state
    if (fp2 == NULL)
    {
        fprintf(stderr, "%s error state: %s\n", "doesntexist.txt", strerror(errno));
    }

    print_words(fp);

    // Close files
    fclose(fp);
    fclose(fp2);

    // This is important in understanding C implementation of strcpy
    char null_char = '\0';
    if (!null_char)
        printf("%s\n", "null_char evaluates to 0 = false");

    return 0;
}

// prints lowercased words in file
void print_words(FILE *fp)
{
    lines = 1;
    // Small size to demonstrate how words to big are cut off
    int size = 5;
    char buf[size];
    while (get_word(fp, buf, size))
        printf("%s\n", buf);
    printf("%d lines\n", lines);
}

// Get word if one exists into buffer (return 1 to indicate
// a word was read) from an opened file
int get_word(FILE *fp, char *buf, int size)
{
    // Note local variables (such as c) are known as automatic variables as they are automatically
    // created and destroyed upon function entry and exit on the stack (K & R 1.10).
    // Read character
    int c = getc(fp);
    
    // Loop 1: Keep reading whitespace till EOF or non whitespace
    // Count newlines as they're seen
    for (; c != EOF && isspace(c); c = getc(fp))
        if (c == '\n')
            lines++;

    // Loop 2: Keep reading nonwhitespace till EOF or whitespace
    // Add to buffer as we go
    int i = 0;
    for (; c != EOF && !isspace(c); c = getc(fp), i++)
        if (i < size - 1)
            buf[i] = tolower(c);
    buf[i] = '\0';

    // ungetc puts character back into stream, why do this here? 
    // if c was a newline, it should be read back in at c 
    // initialization so it can be counted in loop 1 - this
    // is related to answer to exercise 1.1
    if (c != EOF)
        ungetc(c, fp);

    // Means we read a word
    return i > 0;
}
