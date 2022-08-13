#include <stdlib.h>
#include <string.h>

#include "mem.h"

const Except_T Mem_Failed = {"Memory operation failed"};

#define NUM_BUCKETS 2048
#define NUM_NODES 512

// When adding to the free list, more than just what's immediately
// necessary are added
#define EXTRA_BLOCK_ALLOC_SIZE 4096

static struct node {
  struct node *next_free;
  struct node *next_bucket;
  void *block_ptr;
  long block_size;
} * buckets[NUM_BUCKETS];

// This will never be used to allocate from, it's just a way to access
// the free list. Note, all its other fields are set to 0 by default
// hence head_free.block_size > block_size will never be true. New
// additions to the list will always be done after this. Allows for
// one loop in Mem_alloc b/c you can iterate to actual free nodes
// via this circular link. Otherwise, need outer loop over searches
// through noncircular list
static struct node head_free = {&head_free};

// Next node will be allocated from here
static struct node *next_node = NULL;

// Number of nodes left that can be allocated
static int num_nodes_left = 0;

// https://stackoverflow.com/a/665545
// sizeof(size_t) = sizeof(void*)
static unsigned hash(void *ptr) {
  return (((size_t)ptr) * 2654435761) % NUM_BUCKETS;
}

static struct node *allocate_node(void *block_ptr, long block_size,
                                  const char *file, int line) {
  if (num_nodes_left == 0) {
    next_node = malloc(sizeof(*next_node) * NUM_NODES);
    if (next_node == NULL) Except_raise(&Mem_Failed, file, line);
    num_nodes_left = NUM_NODES;
  }
  struct node *new_node = next_node;
  next_node++;
  new_node->block_ptr = block_ptr;
  new_node->block_size = block_size;
  new_node->next_bucket = new_node->next_free = NULL;
  num_nodes_left--;
  return new_node;
}

union align_t {
  int i;
  long l;
  long *lp;
  void *p;
  void (*fp)(void);
  float f;
  double d;
  long double ld;
};

static long align(long nbytes) {
  return ((nbytes + sizeof(union align_t) - 1) / sizeof(union align_t)) *
         sizeof(union align_t);
}

static struct node *find(void *ptr) {
  struct node *tmp;
  for (tmp = buckets[hash(ptr)]; tmp && tmp->block_ptr != ptr;
       tmp = tmp->next_bucket)
    ;
  return tmp;
}

static void free_node(struct node *node) {
  node->next_free = head_free.next_free;
  head_free.next_free = node;
}

void *Mem_alloc(long nbytes, const char *file, int line) {
  assert(nbytes > 0);
  long block_size = align(nbytes);
  struct node *tmp = head_free.next_free;
  char *block_ptr = NULL;
  int allocated = 0;
  while (!allocated) {
    /*
    This statement allows for the same address never being returned twice by
    Mem_alloc because the returned address will always be at least 1 byte away
    from pointer to beginning of free block. Let x be pointer to beginning of
    block that has been chosen to allocate. y will be the returned pointer. y is
    at least x + 1. Suppose memory pointed to by y is freed. Suppose that the
    block with y is chosen for allocation, z will be the returned pointer. z is
    at least y + 1. And so on...

    This prevents behavior:

    x = Mem_alloc(...)
    Mem_free(x)

    ---- User doesn't set x to NULL

    y = Mem_alloc(...)

    If x = y we now inadvertently have x pointing to some other data that is a
    legitimate component of our program and we may do further operations with x
    and corrupt y's data
    */
    if (tmp->block_size > block_size) {
      allocated = 1;
      tmp->block_size -= block_size;
      block_ptr = ((char *)tmp->block_ptr) + tmp->block_size;
      struct node *block_node =
          allocate_node(block_ptr, block_size, file, line);
      unsigned h = hash(block_ptr);
      block_node->next_bucket = buckets[h];
      buckets[h] = block_node;
    } else if (tmp == &head_free) {
      long new_block_size = block_size + align(EXTRA_BLOCK_ALLOC_SIZE);
      void *new_block_ptr = malloc(new_block_size);
      if (new_block_ptr == NULL) Except_raise(&Mem_Failed, file, line);
      struct node *new_block_node =
          allocate_node(new_block_ptr, new_block_size, file, line);
      new_block_node->next_free = head_free.next_free;
      head_free.next_free = new_block_node;
    }
    tmp = tmp->next_free;
  }
  return block_ptr;
}

void *Mem_calloc(long count, long nbytes, const char *file, int line) {
  assert(count > 0 && nbytes > 0);
  long alloc_size = count * nbytes;
  void *ptr = Mem_alloc(alloc_size, file, line);
  memset(ptr, '\0', alloc_size);
  return ptr;
}

void Mem_free(void *ptr, const char *file, int line) {
  assert(ptr);
  struct node *node = find(ptr);
  if (node == NULL || node->next_free) Except_raise(&Mem_Failed, file, line);
  free_node(node);
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line) {
  assert(ptr && nbytes > 0);
  struct node *node = find(ptr);
  if (node == NULL || node->next_free) Except_raise(&Mem_Failed, file, line);
  void *block_ptr = Mem_alloc(nbytes, file, line);
  memcpy(block_ptr, ptr,
         (nbytes < node->block_size) ? nbytes : node->block_size);
  free_node(node);
  return block_ptr;
}