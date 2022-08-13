#include "list.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define T List_T

static T make_node(void *data, T next) {
  T node;
  node = malloc(sizeof(*node));
  assert(node);
  node->data = data;
  node->next = next;
  return node;
}

/*
This implementation uses lead (represented by elements of varargs) and lag
(itr) to add elements.
*/

// T List_list(void *x, ...)
// {
//     T head = NULL;
//     va_list ap;
//     va_start(ap, x);
//     void *e = x;
//     if (e != NULL)
//     {
//         head = make_node(e, NULL);
//         T itr = head;
//         while (e = va_arg(ap, void *))
//         {
//             itr->next = make_node(e, NULL);
//             itr = itr->next;
//         }
//     }
//     va_end(ap);
//     return head;
// }

/*
This implementation uses pointer to pointer list iteration (which is a common
C practice for iterating over lists according to the reading). This allows
you to avoid explicitly stating cases regarding x = NULL.

How it works:

Start with this setup (if variable has address as data, it means
its a pointer and dereferencing it allows read write access to
the variable at that address)

Address     Variable        Data
x1000       list            NULL
x1001       pp              x1000

Iteration 1 (after first line of for body)

Address     Variable        Data
x1000       list            x1002
x1001       pp              x1000
x1002       list->data      x
x1003       list->next      NULL

Iteration 2 (after second line of for body)

Address     Variable        Data
x1000       list            x1002
x1001       pp              x1003
x1002       list->data      x
x1003       list->next      NULL

Here is the key difference, we can move up the pointer-to-pointer
pp such that it now can write into the next fields of the pointer
we have just created (i.e. now if we write to (*pp) we will be
writing to x1003 which is list->next)

This is opposed to keeping pointer to previous node to write to
the next field.
*/
T List_list(void *x, ...) {
  va_list ap;
  va_start(ap, x);
  T list = NULL;
  T *pp = &list;
  for (void *e = x; e; e = va_arg(ap, void *)) {
    (*pp) = make_node(e, NULL);
    pp = &((*pp)->next);
  }
  va_end(ap);
  return list;
}

T List_push(T list, void *x) { return make_node(x, list); }

T List_pop(T list, void **x) {
  if (list == NULL) return list;
  assert(x);
  *x = list->data;
  T new_list = list->next;
  free(list);
  return new_list;
}

// lead and lag implementation
// T List_append(T list, T tail)
// {
//     if (list == NULL)
//         return tail;
//     else if (tail == NULL)
//         return list;
//     T itr;
//     for (itr = list; itr->next; itr = itr->next)
//         ;
//     itr->next = tail;
//     return list;
// }

/*
Pointer to pointer implementation.

Tracing on:

list -> A -> B ->
tail -> C -> D ->

Start:

Address     Variable            Data
x1000       list                x1003
x1001       tail                x1200 (irrelevant here)
x1002       pp                  x1000
x1003       list->data          A
x1004       list->next          x1005
x1005       list->next->data    B
x1006       list->next->next    NULL

After end of first loop iteration

Address     Variable            Data
x1000       list                x1003
x1001       tail                x1200 (irrelevant here)
x1002       pp                  x1004
x1003       list->data          A
x1004       list->next          x1005
x1005       list->next->data    B
x1006       list->next->next    NULL

After end of second loop iteration

Address     Variable            Data
x1000       list                x1003
x1001       tail                x1200 (irrelevant here)
x1002       pp                  x1006
x1003       list->data          A
x1004       list->next          x1005
x1005       list->next->data    B
x1006       list->next->next    NULL

pp now if dereferenced edits list->next->next which can be set to tail
*/
T List_append(T list, T tail) {
  // added optimization, if there isn't actually anything to attach
  // don't bother iterating over list
  if (tail) {
    T *pp = &list;
    for (; *pp; pp = &((*pp)->next))
      ;
    *pp = tail;
  }
  return list;
}

T List_reverse(T list) {
  T next;
  T prev = NULL;
  while (list) {
    next = list->next;
    list->next = prev;
    prev = list;
    list = next;
  }
  return prev;
}

// Lead lag implementation
// T List_copy(T list)
// {
//     if (list == NULL)
//         return NULL;

//     T new_list = make_node(list->data, NULL);
//     T itr = new_list;
//     for (list = list->next; list; list = list->next, itr = itr->next)
//         itr->next = make_node(list->data, NULL);
//     return new_list;
// }

/*
pointer-to-pointer implementation

Start:

Address     Variable            Data
x1000       list                x1001
x1001       list->data          A
x1002       list->next          x1003
x1003       list->next->data    B
x1004       list->next->next    NULL
x1005       new_list            NULL
x1006       pp                  x1005

After 1st line of 1st iteration

Address     Variable            Data
x1000       list                x1001
x1001       list->data          A
x1002       list->next          x1003
x1003       list->next->data    B
x1004       list->next->next    NULL
x1005       new_list            x1007
x1006       pp                  x1005
x1007       new_list->data      A
x1008       new_list->next      NULL

After 2nd line of 1st iteration

Address     Variable            Data
x1000       list                x1001
x1001       list->data          A
x1002       list->next          x1003
x1003       list->next->data    B
x1004       list->next->next    NULL
x1005       new_list            x1007
x1006       pp                  x1008
x1007       new_list->data      A
x1008       new_list->next      NULL

After completion of 1st iteration body

Address     Variable            Data
x1000       list                x1003
x1001       list->data          A
x1002       list->next          x1003
x1003       list->next->data    B
x1004       list->next->next    NULL
x1005       new_list            x1007
x1006       pp                  x1008
x1007       new_list->data      A
x1008       new_list->next      NULL

At this point, (*pp) will read write into new_list->next which
means we can add another node copying from list

After 1st line of 2nd iteration

Address     Variable                Data
x1000       list                    x1003
x1001       list->data              A
x1002       list->next              x1003
x1003       list->next->data        B
x1004       list->next->next        NULL
x1005       new_list                x1007
x1006       pp                      x1008
x1007       new_list->data          A
x1008       new_list->next          x1009
x1009       new_list->next->data    B
x1010       new_list->next->next    NULL

After 2nd line of 2nd iteration

Address     Variable                Data
x1000       list                    x1003
x1001       list->data              A
x1002       list->next              x1003
x1003       list->next->data        B
x1004       list->next->next        NULL
x1005       new_list                x1007
x1006       pp                      x1010
x1007       new_list->data          A
x1008       new_list->next          x1009
x1009       new_list->next->data    B
x1010       new_list->next->next    NULL

After completion of 2nd iteration

Address     Variable                Data
x1000       list                    NULL
x1001       list->data              A
x1002       list->next              x1003
x1003       list->next->data        B
x1004       list->next->next        NULL
x1005       new_list                x1007
x1006       pp                      x1010
x1007       new_list->data          A
x1008       new_list->next          x1009
x1009       new_list->next->data    B
x1010       new_list->next->next    NULL
*/
T List_copy(T list) {
  T new_list = NULL;
  T *pp = &new_list;
  for (; list; list = list->next) {
    *pp = make_node(list->data, NULL);
    pp = &((*pp)->next);
  }
  return new_list;
}

int List_length(T list) {
  int len = 0;
  for (; list; list = list->next, len++)
    ;
  return len;
}

void List_free(T *list) {
  assert(list);
  T next;
  for (; *list; *list = next) {
    next = (*list)->next;
    free(*list);
  }
  *list = NULL;
}

void List_map(T list, void apply(void **x, void *cl), void *cl) {
  assert(apply);
  for (; list; list = list->next) apply(&list->data, cl);
}

void **List_toArray(T list, void *end) {
  int len = List_length(list) + 1;
  void **arr;
  arr = malloc(len * sizeof(*arr));
  assert(arr);
  for (int i = 0; i < len - 1; i++, list = list->next) arr[i] = list->data;
  arr[len - 1] = end;
  return arr;
}

#undef T