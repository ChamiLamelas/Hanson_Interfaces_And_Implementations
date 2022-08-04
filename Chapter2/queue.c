#include "queue.h"
#include <stdlib.h>

struct Queue_T
{
    int count;
    struct elem
    {
        void *data;
        struct elem *next;
    };
    struct elem *head;
    struct elem *tail;
};

Queue_T Queue_new(void)
{
    Queue_T q;
    q = malloc(sizeof(*q));
    q->count = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void Queue_enqueue(Queue_T queue, void *ep)
{
    struct elem *new_node;
    new_node = malloc(sizeof(*new_node));
    new_node->data = ep;
    new_node->next = NULL;
    if (queue->count == 0)
    {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else
    {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->count++;
}

void *Queue_dequeue(Queue_T queue)
{
    void *out = queue->head->data;
    struct elem *old_head = queue->head;
    queue->head = queue->head->next;
    free(old_head);
    if (queue->count == 1)
    {
        queue->tail = NULL;
    }
    queue->count--;
    return out;
}

void *Queue_front(Queue_T queue)
{
    return queue->head->data;
}

int Queue_size(Queue_T queue)
{
    return queue->count;
}

int Queue_empty(Queue_T queue)
{
    return queue->count == 0;
}

void Queue_free(Queue_T *queue)
{
    struct elem *n;
    for (struct elem *c = (*queue)->head; c; c = n)
    {
        n = c->next;
        free(c);
    }
    free(*queue);
    *queue = NULL;
}