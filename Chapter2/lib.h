#ifndef LIB_INCLUDED
#define LIB_INCLUDED

// Can typedef with an incomplete (undefined in this file) type. 
// Furthermore, can have type name same as struct (see Hanson 2.3).
// In theory, when working with structs (as some sort of data structure)
// we would have functions that operate on pointers of the structure
// so we can modify the contents, hence the defined type is a pointer type.

// In the book Hanson makes a remark on how const LIB_T (the defined type) 
// actually makes the pointer const, this is backed up here:
// https://stackoverflow.com/a/35352325

// Some more info on typedef:
// https://stackoverflow.com/a/41752767
// https://stackoverflow.com/a/41751649
typedef struct LIB_T * LIB_T;

#endif