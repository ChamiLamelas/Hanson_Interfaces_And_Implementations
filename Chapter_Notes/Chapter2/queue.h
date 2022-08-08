#ifndef QUEUE_INCLUDED
#define QUEUE_INCLUDED

typedef struct Queue_T *Queue_T;

extern Queue_T Queue_new(void);
extern void Queue_enqueue(Queue_T queue, void *ep);
extern void *Queue_dequeue(Queue_T queue);
extern void *Queue_front(Queue_T queue);
extern int Queue_size(Queue_T queue);
extern int Queue_empty(Queue_T queue);
extern void Queue_free(Queue_T *queue);

#endif