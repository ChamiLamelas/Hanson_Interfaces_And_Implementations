#ifndef ATOM_INCLUDED
#define ATOM_INCLUDED

extern int Atom_length(const char *str);
extern const char *Atom_new(const char *str, int len);
extern const char *Atom_string(const char *str);
extern const char *Atom_int(long n);

// Added function that should be called at the end of any code that
// uses Atoms so that any allocated atoms can be freed
extern void Atom_free();

#endif