#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"

int main(int argc, char *argv[]) {
  puts("ALLOC FREE");
  void *x = ALLOC(10);
  FREE(x);
  puts("ALLOC Mem_free Mem_free");
  void *y = ALLOC(10);
  Mem_free(y, __FILE__, __LINE__);
  // Uncomment to test for crash
  // Mem_free(y, __FILE__, __LINE__);
  puts("Mem_free non alloc ptr");
  int i = 1;
  int *z = &i;
  // Mem_free(z, __FILE__, __LINE__);
  puts("CALLOC FREE");
  void *a = CALLOC(10, 4);
  FREE(a);
  puts("RESIZE ALLOC\'D");
  void *b = ALLOC(10);
  RESIZE(b, 12);
  FREE(b);
  puts("RESIZE CALLOC\'D");
  void *c = CALLOC(10, 4);
  RESIZE(c, 42);
  FREE(c);
  puts("RESIZE non alloc ptr");
  // RESIZE(z, 12);
  puts("Mem_free RESIZE");
  void *d = ALLOC(10);
  Mem_free(d, __FILE__, __LINE__);
  // RESIZE(d, 12);
  puts("Mem chk test done");
  return 0;
}