#include "stack.h"

// Use malloc, free, assert instead of improved versions (not
// implemented yet)
#include <stdlib.h>
#include <assert.h>

// T is not carried from stack.h, all instances of T in stack.h
// are replaced with Stack_T then that is copy pasted into here
#define T Stack_T

struct _Stack_T
{
    struct node
    {
        void *data;
        struct node *next;
    } * head;
    int size;
};

T Stack_new()
{
    T s;
    s = malloc(sizeof(*s));
    s->head = NULL;
    s->size = 0;
    return s;
}

int Stack_empty(T s)
{
    assert(s);
    return s->size == 0;
}

int Stack_size(T s)
{
    assert(s);
    return s->size;
}

void Stack_push(T s, void *e)
{
    assert(s);
    struct node *new_node;
    new_node = malloc(sizeof(*new_node));
    new_node->data = e;
    new_node->next = s->head;
    s->head = new_node;
    s->size++;
}

void *Stack_top(T s)
{
    assert(s && s->size > 0);
    return s->head->data;
}

void *Stack_pop(T s)
{
    assert(s && s->size > 0);
    void *out = s->head->data;
    struct node *tmp = s->head->next;
    free(s->head);
    s->head = tmp;
    s->size--;
    return out;
}

void Stack_free(T *s)
{
    assert(s && *s);
    struct node *tmp;
    while ((*s)->head)
    {
        tmp = (*s)->head->next;
        free((*s)->head);
        (*s)->head = tmp;
    }
    free(*s);
    *s = NULL;
}