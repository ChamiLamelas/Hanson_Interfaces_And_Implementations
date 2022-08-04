#ifndef TEST_H
#define TEST_H

// What is the extern keyword? Allows for declarations of variables
// in header files without definition (which can be left to implementation).
// Function declarations in header file (and other files) are implicitly 
// defined extern, but adding the extern can make it more clear.
// https://stackoverflow.com/a/3367584
// See The C Book (Banahan et. al.) 4.4
extern int y;

extern int f(int x);

#endif