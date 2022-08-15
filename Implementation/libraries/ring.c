#include "ring.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#define T Ring_T

struct T {
    struct node {
        struct node *prev;
        struct node *next;
        void *data;
    } * head;
    int num_elements;
};

static struct node *find(T ring, int i) {
    struct node *node = ring->head;
    if (i <= ring->num_elements / 2) {
        for (int j = 0; j < i; j++) {
            node = node->next;
        }
    } else {
        node = ring->head->prev;
        for (int j = ring->num_elements - 1; j > i; j--) {
            node = node->prev;
        }
    }
    return node;
}

static struct node *make_node(struct node *prev, struct node *next,
                              void *data) {
    struct node *node;
    node = malloc(sizeof(*node));
    assert(node);
    node->data = data;
    node->prev = prev;
    node->next = next;
    return node;
}

T Ring_new(void) {
    T r;
    r = malloc(sizeof(*r));
    assert(r);
    r->num_elements = 0;
    r->head = NULL;
    return r;
}

T Ring_ring(void *x, ...) {
    T r = Ring_new();
    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        Ring_addhi(r, x);
    }
    va_end(ap);
    return r;
}

void Ring_free(T *ring) {
    assert(ring && *ring);
    if ((*ring)->num_elements > 0) {
        (*ring)->head->prev->next = NULL;
        struct node *next;
        for (int i = 0; i < (*ring)->num_elements; i++) {
            next = (*ring)->head->next;
            free((*ring)->head);
            (*ring)->head = next;
        }
    }
    free(*ring);
    *ring = NULL;
}

int Ring_length(T ring) {
    assert(ring);
    return ring->num_elements;
}

void *Ring_get(T ring, int i) {
    assert(ring && i >= 0 && i < ring->num_elements);
    return find(ring, i)->data;
}

void *Ring_put(T ring, int i, void *x) {
    assert(ring && i >= 0 && i < ring->num_elements);
    struct node *node = find(ring, i);
    void *out = node->data;
    node->data = x;
    return out;
}

void *Ring_add(T ring, int pos, void *x) {
    assert(ring && pos >= -ring->num_elements && pos <= ring->num_elements + 1);
    struct node *new_node;
    if (ring->num_elements == 0) {
        ring->head = make_node(NULL, NULL, x);
        ring->head->prev = ring->head;
        ring->head->next = ring->head;
    } else if (pos == 1 || pos == -ring->num_elements ||
               pos == ring->num_elements + 1 || pos == 0) {
        new_node = make_node(ring->head->prev, ring->head, x);
        ring->head->prev->next = new_node;
        ring->head->prev = new_node;
        if (pos == 1 || pos == -ring->num_elements) {
            ring->head = new_node;
        }
    } else {
        pos = pos > 0 ? pos - 2 : pos + ring->num_elements - 1;
        struct node *node = find(ring, pos);
        new_node = make_node(node, node->next, x);
        node->next->prev = new_node;
        node->next = new_node;
    }
    ring->num_elements++;
    return x;
}

void *Ring_addlo(T ring, void *x) { return Ring_add(ring, 1, x); }

void *Ring_addhi(T ring, void *x) { return Ring_add(ring, 0, x); }

void *Ring_remove(T ring, int i) {
    assert(ring && i >= 0 && i < ring->num_elements);
    struct node *node = find(ring, i);
    void *out = node->data;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (i == 0) {
        ring->head = ring->head->next;
    }
    free(node);
    ring->num_elements--;
    if (ring->num_elements == 0) {
        ring->head = NULL;
    }
    return out;
}

void *Ring_remlo(T ring) { return Ring_remove(ring, 0); }

void *Ring_remhi(T ring) {
    assert(ring);  // Needed for ring->num_elements
    return Ring_remove(ring, ring->num_elements - 1);
}

void Ring_rotate(T ring, int n) {
    int an = abs(n);
    assert(ring && abs(n) <= ring->num_elements);
    if (an == 0 || an == ring->num_elements) {
        return;
    }
    // I believe this is the correct rotation described in the
    // interface. In the interface (p. 186) it says that
    // Value at index 0 goes to index 3, which means value at
    // num_elements - 3 goes to index 0, when n = 3. His
    // implementation puts node at index 3 at the head.
    ring->head = find(ring, n > 0 ? ring->num_elements - n : an);
}

#undef T