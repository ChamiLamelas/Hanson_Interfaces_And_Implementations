#include "except.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

Except_Frame *stack = NULL;

void Except_raise(const Except_T *e, const char *file, int line)
{
    assert(e);
    if (stack == NULL)
    {
        if (e->reason)
            fprintf(stderr, "Exception: %s", e->reason);
        if (file != NULL && line > 0)
            fprintf(stderr, " at %s:%d", file, line);
        fprintf(stderr, "... aborting\n");
        // https://stackoverflow.com/questions/18317564/c-c-flush-output-before-abnormal-termination
        fflush(stderr);
        abort();
    }
    Except_Frame *tmp = stack;
    stack = stack->prev;
    tmp->exception = e;
    tmp->file = file;
    tmp->line = line;
    longjmp(tmp->env, unhandled);
}
