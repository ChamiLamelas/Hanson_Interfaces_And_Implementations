#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arith.h"
#include "array.h"
#include "arrayrep.h"
#include "atom.h"
#include "bit.h"
#include "except.h"
#include "list.h"
#include "mem.h"
#include "ring.h"
#include "seq.h"
#include "set.h"
#include "stack.h"
#include "str.h"
#include "table.h"
#include "text.h"

static Except_T ex = {"exception"};
static Except_T ex2 = {"exception2"};

static void Arith_test(void);
static void Stack_test(void);
static void Atom_test(void);
static void Except_test(void);
static void Mem_test(void);
static void List_test(void);
static void Table_test(void);
static void Set_test(void);
static void Array_test(void);
static void Seq_test(void);
static void Ring_test(void);
static void Bit_test(void);
static void Str_test(void);
static void Text_test(void);

int main(int argc, char *argv[]) {
    // Arith_test();
    // Stack_test();
    // Atom_test();
    // Except_test();
    // Mem_test();
    // List_test();
    // Table_test();
    // Set_test();
    // Array_test();
    // Seq_test();
    // Ring_test();
    // Bit_test();
    // Str_test();
    Text_test();
    return 0;
}

void Arith_test(void) {
    assert(Arith_max(2, 2) == 2);
    assert(Arith_max(2, 3) == 3);
    assert(Arith_max(-2, -3) == -2);
    assert(Arith_min(2, 3) == 2);
    assert(Arith_min(2, 2) == 2);
    assert(Arith_min(-2, -3) == -3);
    assert(Arith_div(13, 5) == 2);
    assert(Arith_div(-13, -5) == 2);
    assert(Arith_div(15, 5) == 3);
    assert(Arith_div(-15, -5) == 3);
    assert(Arith_div(-13, 5) == -3);
    assert(Arith_div(-15, 5) == -3);
    assert(Arith_mod(13, 5) == 3);
    assert(Arith_mod(-13, -5) == -3);
    assert(Arith_mod(15, 5) == 0);
    assert(Arith_mod(-15, -5) == 0);
    assert(Arith_mod(-13, 5) == 2);
    assert(Arith_mod(-15, 5) == 0);
    assert(Arith_ceiling(13, 5) == 3);
    assert(Arith_ceiling(-13, -5) == 3);
    assert(Arith_ceiling(15, 5) == 3);
    assert(Arith_ceiling(-15, -5) == 3);
    assert(Arith_ceiling(-13, 5) == -2);
    assert(Arith_ceiling(-15, 5) == -3);
    assert(Arith_floor(13, 5) == 2);
    assert(Arith_floor(-13, -5) == 2);
    assert(Arith_floor(15, 5) == 3);
    assert(Arith_floor(-15, -5) == 3);
    assert(Arith_floor(-13, 5) == -3);
    assert(Arith_floor(-15, 5) == -3);
    puts("Arith_test finished");
}

void Stack_test(void) {
    int data[5] = {0, 1, 2, 3, 4};

    Stack_T s = Stack_new();
    assert(Stack_empty(s));

    Stack_push(s, &data[0]);
    assert(Stack_size(s) == 1);
    assert(*((int *)Stack_top(s)) == data[0]);

    Stack_push(s, &data[1]);
    assert(*((int *)Stack_top(s)) == data[1]);

    assert(*((int *)Stack_pop(s)) == data[1]);
    assert(Stack_size(s) == 1);

    for (int i = 2; i < 5; i++) {
        Stack_push(s, &data[i]);
    }
    assert(Stack_size(s) == 4);
    assert(*((int *)Stack_top(s)) == data[4]);

    Stack_free(&s);
    assert(s == NULL);
    puts("Stack_test finished");
}

void Atom_test(void) {
    const char *atom_abc = Atom_string("abc");
    const char *atom_abc2 = Atom_string("abc");
    assert(atom_abc == atom_abc2);
    const char *atom_1234 = Atom_int(1234);
    const char *atom_1234_2 = Atom_int(1234);
    assert(atom_1234 == atom_1234_2);
    assert(Atom_length(atom_abc) == 3);
    assert(Atom_length(atom_1234) == 4);
    const char *atom_long_min = Atom_int(LONG_MIN);
    const char *atom_long_min_2 = Atom_int(LONG_MIN);
    // load LONG_MIN into a string for comparison
    char check[255];
    sprintf(check, "%ld", LONG_MIN);
    assert(strcmp(check, atom_long_min) == 0);
    assert(atom_long_min == atom_long_min_2);
    const char *atom_empty = Atom_string("");
    const char *atom_empty2 = Atom_string("");
    assert(atom_empty == atom_empty2);
    puts("Atom_test finished");
}

void Raise_Ex(void) { RAISE(ex); }

void Raise_and_Except_Ex(void) {
    TRY RAISE(ex);
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    END_TRY;
}

void Except_test(void) {
    puts("-- TRY END_TRY");
    TRY puts("hello");
    END_TRY;

    puts("-- TRY EXCEPT END_TRY");
    TRY puts("hello");
    EXCEPT(ex)
    puts("Should not print");
    END_TRY;

    puts("-- TRY RAISE EXCEPT END_TRY");
    TRY RAISE(ex);
    EXCEPT(ex)
    printf("Caught exception %s\n", ex.reason);
    END_TRY;

    puts("-- TRY RAISE END_TRY");
    // TRY
    //     RAISE(ex);
    // END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT END_TRY");
    // TRY
    //     RAISE(ex);
    // EXCEPT(ex2)
    //     puts("Caught different exception");
    // END_TRY;

    puts("-- TRY RAISE FINALLY END_TRY");
    // TRY
    //     RAISE(ex);
    // FINALLY
    //     puts("Finally runs before abort");
    // END_TRY;

    puts("-- TRY RAISE EXCEPT FINALLY END_TRY");
    TRY RAISE(ex);
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    FINALLY
    puts("Finally runs after handling");
    END_TRY;

    puts("-- TRY RAISE ELSE END_TRY");
    TRY RAISE(ex);
    ELSE puts("Else");
    END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT ELSE END_TRY");
    TRY RAISE(ex2);
    EXCEPT(ex)
    puts("Caught different exception");
    ELSE puts("Caught everything");
    END_TRY;

    puts("-- TRY RAISE EXCEPT DIFFERENT ELSE FINALLY END_TRY");
    TRY RAISE(ex2);
    EXCEPT(ex)
    puts("Caught different exception");
    ELSE puts("Caught everything");
    FINALLY
    puts("Finally runs after handling");
    END_TRY;

    puts("-- CATCH RAISED EXCEPTION FROM NESTED FUNCTION");
    TRY Raise_Ex();
    EXCEPT(ex)
    printf("Exception %s\n", ex.reason);
    END_TRY;

    puts("-- CATCH RAISED EXCEPTIONS ON TWO LEVELS");
    TRY Raise_and_Except_Ex();
    RAISE(ex2);
    EXCEPT(ex2)
    printf("Exception %s\n", ex2.reason);
    END_TRY;

    puts("-- CRASH FROM NESTED FUNCTION");
    // Raise_Ex();

    puts("Except_test done");
}

void Mem_test(void) {
    void *ptr = NULL;
    ptr = Mem_alloc(1, __FILE__, __LINE__);
    assert(ptr);
    Mem_free(ptr, __FILE__, __LINE__);
    ptr = NULL;
    ptr = ALLOC(1);
    assert(ptr);
    FREE(ptr);
    assert(ptr == NULL);
    ptr = Mem_calloc(1, 1, __FILE__, __LINE__);
    assert(ptr);
    assert((*(char *)ptr) == 0);
    FREE(ptr);
    ptr = CALLOC(1, 1);
    assert(ptr);
    assert((*(char *)ptr) == 0);
    FREE(ptr);
    NEW(ptr);
    assert(ptr);
    FREE(ptr);
    NEW0(ptr);
    assert(ptr);
    assert((*(char *)ptr) == 0);
    FREE(ptr);
    // Uncomment to see these all abort via assert
    // ptr = ALLOC(-1);
    // ptr = CALLOC(-1, 1);
    // ptr = CALLOC(1, -1);
    NEW(ptr);
    // ptr = RESIZE(ptr,-1);
    FREE(ptr);
    // FREE(ptr);
    // RESIZE(ptr,1);
    puts("Mem_test done");
}

void list_apply1(void **e, void *cl) { *((int *)cl) += **((int **)e); }

void list_apply2(void **e, void *cl) { *e = cl; }

void List_test(void) {
    int data[3] = {1, 2, 3};
    int *p[3];
    for (int i = 0; i < 3; i++) p[i] = &data[i];
    int out = 0;
    int *pout = &out;
    puts("LIST(1,2,3,NULL)");
    List_T ls = List_list(p[0], p[1], p[2], NULL);
    assert(ls->data == p[0]);
    assert(ls->next->data == p[1]);
    assert(ls->next->next->data == p[2]);
    assert(ls->next->next->next == NULL);
    puts("LIST(NULL)");
    List_T ls2 = List_list(NULL);
    assert(ls2 == NULL);
    List_free(&ls2);
    puts("LIST PUSH (NULL,1)");
    List_T ls3 = List_push(NULL, p[0]);
    assert(ls3->data == p[0]);
    assert(ls3->next == NULL);
    List_free(&ls3);
    assert(ls3 == NULL);
    puts("LIST PUSH (ls,1)");
    List_T ls4 = List_push(ls, p[0]);
    assert(ls4->data == p[0]);
    assert(ls4->next->data == p[0]);
    assert(ls4->next->next->data == p[1]);
    assert(ls4->next->next->next->data == p[2]);
    assert(ls4->next->next->next->next == NULL);
    puts("LIST POP (ls4)");
    List_T ls5 = List_pop(ls4, (void **)&pout);
    assert(pout == p[0]);
    assert(ls5->data == p[0]);
    assert(ls5->next->data == p[1]);
    assert(ls5->next->next->data == p[2]);
    assert(ls5->next->next->next == NULL);
    List_free(&ls5);
    assert(ls5 == NULL);
    puts("LIST POP (NULL)");
    List_T ls6 = List_pop(NULL, (void **)&pout);
    assert(pout == p[0]);
    assert(ls6 == NULL);
    puts("LIST POP (ls4, NULL)");
    // Uncomment for crash
    // List_T ls7 = List_pop(ls4, NULL);
    puts("LIST APPEND");
    List_T ls8 = List_list(p[0], p[1], NULL);
    List_T ls9 = List_list(p[2], NULL);
    List_T ls10 = List_append(ls8, ls9);
    assert(ls10->data == p[0]);
    assert(ls10->next->data == p[1]);
    assert(ls10->next->next->data == p[2]);
    assert(ls10->next->next->next == NULL);
    List_free(&ls10);
    assert(ls10 == NULL);
    puts("LIST (,NULL)");
    List_T ls11 = List_list(p[0], p[1], NULL);
    List_T ls12 = List_append(ls11, NULL);
    assert(ls12->data == p[0]);
    assert(ls12->next->data == p[1]);
    assert(ls12->next->next == NULL);
    List_free(&ls12);
    assert(ls12 == NULL);
    puts("LIST (NULL,)");
    List_T ls13 = List_list(p[2], NULL);
    List_T ls14 = List_append(NULL, ls13);
    assert(ls14->data == p[2]);
    assert(ls14->next == NULL);
    List_free(&ls14);
    assert(ls14 == NULL);
    puts("LIST REVERSE");
    List_T ls15 = List_list(p[0], p[1], NULL);
    List_T ls16 = List_reverse(ls15);
    assert(ls16->data == p[1]);
    assert(ls16->next->data == p[0]);
    assert(ls16->next->next == NULL);
    List_free(&ls16);
    assert(ls16 == NULL);
    puts("LIST REVERSE (NULL)");
    List_T ls17 = List_reverse(NULL);
    assert(ls17 == NULL);
    puts("LIST COPY");
    List_T ls18 = List_list(p[0], p[1], NULL);
    List_T ls19 = List_copy(ls18);
    assert(ls19->data == p[0]);
    assert(ls19->next->data == p[1]);
    assert(ls19->next->next == NULL);
    assert(ls19 != ls18);
    assert(ls19->next != ls18->next);
    List_free(&ls19);
    assert(ls19 == NULL);
    puts("LIST COPY (NULL)");
    List_T ls20 = List_copy(NULL);
    assert(ls20 == NULL);
    puts("LIST COPY (1)");
    List_T ls21 = List_list(p[2], NULL);
    List_T ls22 = List_copy(ls21);
    assert(ls22->data = p[2]);
    assert(ls22->next == NULL);
    assert(ls22 != ls21);
    List_free(&ls22);
    assert(ls22 == NULL);
    puts("LIST LENGTH");
    int len = List_length(NULL);
    assert(len == 0);
    len = List_length(ls21);
    assert(len == 1);
    len = List_length(ls18);
    assert(len == 2);
    List_free(&ls21);
    List_free(&ls18);
    assert(ls21 == NULL && ls18 == NULL);
    puts("LIST APPLY 1");
    int sum = 0;
    List_T ls23 = List_list(p[0], p[1], p[2], NULL);
    List_map(ls23, list_apply1, &sum);
    List_free(&ls23);
    assert(ls23 == NULL);
    assert(sum == 6);
    puts("LIST APPLY 2");
    List_T ls24 = List_list(p[0], p[1], p[2], NULL);
    List_map(ls24, list_apply2, &sum);
    assert(ls24->data == &sum);
    assert(ls24->next->data == &sum);
    assert(ls24->next->next->data == &sum);
    List_free(&ls24);
    assert(ls24 == NULL);
    puts("LIST TOARRAY");
    List_T ls25 = List_list(p[0], p[1], p[2], NULL);
    int **arr = (int **)List_toArray(ls25, NULL);
    for (int i = 0; i < 3; i++) assert(arr[i] == p[i]);
    assert(arr[3] == NULL);
    List_free(&ls25);
    assert(ls25 == NULL);
    puts("LIST TOARRAY 1");
    List_T ls26 = List_list(p[0], NULL);
    int **arr2 = (int **)List_toArray(ls26, NULL);
    assert(arr2[0] == p[0]);
    assert(arr2[1] == NULL);
    List_free(&ls26);
    assert(ls26 == NULL);
    puts("LIST TOARRAY NULL");
    int **arr3 = (int **)List_toArray(NULL, NULL);
    assert(arr3[0] == NULL);
    puts("List_test done");
}

unsigned bad_hash(const void *key) { return 1; }

int int_cmp(const void *x, const void *y) { return *((int *)x) != *((int *)y); }

void table_apply1(void *key, void **value, void *cl) {
    *((int *)cl) += **((int **)value);
}

void table_apply2(void *key, void **value, void *cl) { *value = cl; }

void table_apply3(void *key, void **value, void *cl) {
    free(*value);
    *value = NULL;
}

void Table_test(void) {
    const char *key_data[5] = {"a", "b", "c", "d", "e"};
    int value_data[5] = {1, 2, 3, 4, 5};
    char *k[5];
    int *v[5];
    for (int i = 0; i < 5; i++) {
        k[i] = (char *)Atom_string(key_data[i]);
        v[i] = &value_data[i];
    }
    puts("TABLE EMPTY BEHAVIOR");
    Table_T t = Table_new(20, NULL, NULL);
    assert(Table_length(t) == 0);
    assert(Table_get(t, k[0]) == NULL);
    assert(Table_remove(t, k[0]) == NULL);
    assert(Table_length(t) == 0);
    puts("TABLE ADD 1");
    assert(Table_put(t, k[0], v[0]) == NULL);
    assert(Table_length(t) == 1);
    assert(Table_get(t, k[0]) == v[0]);
    puts("TABLE OVERWRITE");
    assert(Table_put(t, k[0], v[1]) == v[0]);
    assert(Table_length(t) == 1);
    assert(Table_get(t, k[0]) == v[1]);
    puts("TABLE REMOVE");
    assert(Table_remove(t, k[0]) == v[1]);
    assert(Table_length(t) == 0);
    assert(Table_get(t, k[0]) == NULL);
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE COLLISION");
    t = Table_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) {
        assert(Table_put(t, k[i], v[i]) == NULL);
        assert(Table_get(t, k[i]) == v[i]);
        assert(Table_length(t) == i + 1);
    }
    for (int i = 0; i < 3; i++) {
        assert(Table_put(t, k[i], v[3]) == v[i]);
        assert(Table_get(t, k[i]) == v[3]);
        assert(Table_length(t) == 3);
    }
    for (int i = 0; i < 3; i++) {
        assert(Table_remove(t, k[i]) == v[3]);
        assert(Table_get(t, k[i]) == NULL);
        assert(Table_length(t) == 2 - i);
    }
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE MAP");
    t = Table_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) Table_put(t, k[i], v[i]);
    int sum = 0;
    Table_map(t, table_apply1, &sum);
    assert(sum == 6);
    Table_map(t, table_apply2, v[3]);
    for (int i = 0; i < 3; i++) assert(Table_get(t, k[i]) == v[3]);
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE REHASH");
    t = Table_new(3, NULL, NULL);
    for (int i = 0; i < 5; i++) {
        assert(Table_put(t, k[i], v[i]) == NULL);
        assert(Table_get(t, k[i]) == v[i]);
        assert(Table_length(t) == i + 1);
    }
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE FREE VALUES");
    t = Table_new(20, int_cmp, bad_hash);
    int *dv[5];
    for (int i = 0; i < 5; i++) {
        dv[i] = malloc(sizeof(*dv[i]));
        *dv[i] = i;
    }
    for (int i = 0; i < 5; i++) Table_put(t, k[i], dv[i]);
    Table_map(t, table_apply3, NULL);
    assert(Table_length(t) == 5);
    for (int i = 0; i < 5; i++) assert(Table_get(t, k[i]) == NULL);
    Table_free(&t);
    assert(t == NULL);
    puts("TABLE POINTER INDEPENDENT CMP");
    t = Table_new(20, int_cmp, bad_hash);
    int x = 1;
    int *onep1 = &x;
    int y = 1;
    int *onep2 = &y;
    int z = 2;
    Table_put(t, onep1, &z);
    assert(Table_get(t, onep2));
    Table_free(&t);
    puts("TABLE CHECK BAD INPUTS");
    // Uncomment to see assertion failures
    // t = Table_new(-1, NULL, NULL);
    // t = Table_new(20, int_cmp, bad_hash);
    // Table_free(NULL);
    // Table_T t2 = NULL;
    // Table_free(&t2);
    // Table_get(NULL, k[0]);
    // Table_length(NULL);
    // Table_put(NULL, k[0], v[0]);
    // Table_remove(NULL, k[0]);
    // Table_map(NULL, NULL, NULL);
    // Table_toArray(NULL, NULL);
    puts("Table_test done");
}

void set_apply1(const void *member, void *cl) {
    *((int *)cl) += strlen(member);
}

void Set_test(void) {
    const char *member_data[5] = {"a", "b", "c", "d", "e"};
    char *m[5];
    for (int i = 0; i < 5; i++) {
        m[i] = (char *)Atom_string(member_data[i]);
    }
    puts("SET EMPTY BEHAVIOR");
    Set_T s = Set_new(20, NULL, NULL);
    assert(Set_length(s) == 0);
    assert(!Set_member(s, m[0]));
    assert(Set_remove(s, m[0]) == NULL);
    assert(Set_length(s) == 0);
    puts("SET ADD 1");
    Set_put(s, m[0]);
    assert(Set_length(s) == 1);
    assert(Set_member(s, m[0]));
    puts("SET DUPLICATE");
    Set_put(s, m[0]);
    assert(Set_length(s) == 1);
    assert(Set_member(s, m[0]));
    puts("SET REMOVE");
    assert(Set_remove(s, m[0]) == m[0]);
    assert(Set_length(s) == 0);
    assert(!Set_member(s, m[0]));
    Set_free(&s);
    assert(s == NULL);
    puts("SET COLLISION");
    s = Set_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) {
        Set_put(s, m[i]);
        assert(Set_member(s, m[i]));
        assert(Set_length(s) == i + 1);
    }
    for (int i = 0; i < 3; i++) {
        Set_put(s, m[i]);
        assert(Set_member(s, m[i]));
        assert(Set_length(s) == 3);
    }
    for (int i = 0; i < 3; i++) {
        assert(Set_remove(s, m[i]) == m[i]);
        assert(!Set_member(s, m[i]));
        assert(Set_length(s) == 2 - i);
    }
    Set_free(&s);
    assert(s == NULL);
    puts("SET MAP");
    s = Set_new(20, int_cmp, bad_hash);
    for (int i = 0; i < 3; i++) Set_put(s, m[i]);
    int sum = 0;
    Set_map(s, set_apply1, &sum);
    assert(sum == 3);
    Set_free(&s);
    assert(s == NULL);
    puts("SET REHASH");
    s = Set_new(3, NULL, NULL);
    for (int i = 0; i < 5; i++) {
        Set_put(s, m[i]);
        assert(Set_member(s, m[i]));
        assert(Set_length(s) == i + 1);
    }
    Set_free(&s);
    assert(s == NULL);
    puts("SET UNION (s, EMPTY)");
    s = Set_new(3, NULL, NULL);
    for (int i = 0; i < 3; i++) Set_put(s, m[i]);
    Set_T s2 = Set_new(20, NULL, NULL);
    Set_T o = Set_union(s, s2);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_union(s2, s);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    puts("SET UNION (s, NULL)");
    o = Set_union(s, NULL);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_union(NULL, s);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    puts("SET UNION NO OVERLAP");
    Set_put(s2, m[3]);
    Set_put(s2, m[4]);
    o = Set_union(s, s2);
    for (int i = 0; i < 5; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 5);
    Set_free(&o);
    o = Set_union(s2, s);
    for (int i = 0; i < 5; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 5);
    Set_free(&o);
    Set_free(&s2);
    puts("SET UNION OVERLAP");
    s2 = Set_new(20, NULL, NULL);
    Set_put(s2, m[2]);
    Set_put(s2, m[3]);
    o = Set_union(s, s2);
    for (int i = 0; i < 4; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 4);
    Set_free(&o);
    o = Set_union(s2, s);
    for (int i = 0; i < 4; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 4);
    Set_free(&o);
    Set_free(&s2);
    puts("SET INTER (s,EMPTY)");
    s2 = Set_new(20, NULL, NULL);
    o = Set_inter(s, s2);
    assert(Set_length(o) == 0);
    Set_free(&o);
    o = Set_inter(s2, s);
    assert(Set_length(o) == 0);
    Set_free(&o);
    puts("SET INTER (s,NULL)");
    o = Set_inter(s, NULL);
    assert(o && Set_length(o) == 0);
    Set_free(&o);
    o = Set_inter(NULL, s);
    assert(o && Set_length(o) == 0);
    Set_free(&o);
    puts("SET INTER NO OVERLAP");
    Set_put(s2, m[3]);
    Set_put(s2, m[4]);
    o = Set_inter(s, s2);
    for (int i = 0; i < 5; i++) assert(!Set_member(o, m[i]));
    assert(Set_length(o) == 0);
    Set_free(&o);
    o = Set_inter(s2, s);
    for (int i = 0; i < 5; i++) assert(!Set_member(o, m[i]));
    assert(Set_length(o) == 0);
    Set_free(&o);
    Set_free(&s2);
    puts("SET INTER OVERLAP");
    s2 = Set_new(20, NULL, NULL);
    Set_put(s2, m[2]);
    Set_put(s2, m[3]);
    o = Set_inter(s, s2);
    assert(!Set_member(o, m[0]));
    assert(!Set_member(o, m[1]));
    assert(!Set_member(o, m[3]));
    assert(Set_member(o, m[2]));
    assert(Set_length(o) == 1);
    Set_free(&o);
    o = Set_inter(s2, s);
    assert(!Set_member(o, m[0]));
    assert(!Set_member(o, m[1]));
    assert(!Set_member(o, m[3]));
    assert(Set_member(o, m[2]));
    assert(Set_length(o) == 1);
    Set_free(&o);
    Set_free(&s2);
    puts("SET MINUS (s,EMPTY)");
    s2 = Set_new(20, NULL, NULL);
    o = Set_minus(s, s2);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_minus(s2, s);
    assert(o && Set_length(o) == 0);
    Set_free(&o);
    puts("SET MINUS (s,NULL)");
    o = Set_minus(s, NULL);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_minus(NULL, s);
    assert(o && Set_length(o) == 0);
    Set_free(&o);
    puts("SET MINUS NO OVERLAP");
    Set_put(s2, m[3]);
    Set_put(s2, m[4]);
    o = Set_minus(s, s2);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    Set_free(&s2);
    puts("SET MINUS OVERLAP");
    s2 = Set_new(20, NULL, NULL);
    Set_put(s2, m[2]);
    Set_put(s2, m[3]);
    o = Set_minus(s, s2);
    for (int i = 0; i < 2; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 2);
    Set_free(&o);
    Set_free(&s2);
    puts("SET DIFF (s,EMPTY)");
    s2 = Set_new(20, NULL, NULL);
    o = Set_diff(s, s2);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_diff(s2, s);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    puts("SET DIFF (s,NULL)");
    o = Set_diff(s, NULL);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    o = Set_diff(NULL, s);
    for (int i = 0; i < 3; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    puts("SET DIFF NO OVERLAP");
    Set_put(s2, m[3]);
    Set_put(s2, m[4]);
    o = Set_diff(s, s2);
    for (int i = 0; i < 5; i++) assert(Set_member(o, m[i]));
    assert(Set_length(o) == 5);
    Set_free(&o);
    Set_free(&s2);
    puts("SET DIFF OVERLAP");
    s2 = Set_new(20, NULL, NULL);
    Set_put(s2, m[2]);
    Set_put(s2, m[3]);
    o = Set_diff(s, s2);
    assert(Set_member(o, m[0]));
    assert(Set_member(o, m[1]));
    assert(Set_member(o, m[3]));
    assert(Set_length(o) == 3);
    Set_free(&o);
    Set_free(&s2);
    puts("SET POINTER INDEPENDENT CMP");
    int x = 1;
    int y = 1;
    int *onep1 = &x;
    int *onep2 = &y;
    s2 = Set_new(20, int_cmp, bad_hash);
    Set_put(s2, onep1);
    assert(Set_member(s2, onep2));
    Set_free(&s2);
    puts("SET PUT OVERWRITE");
    s2 = Set_new(20, int_cmp, bad_hash);
    Set_put(s2, onep1);
    Set_put(s2, onep2);
    assert(Set_length(s2) == 1);
    assert(Set_remove(s2, onep2) == onep2);
    Set_free(&s2);
    puts("SET CHECK BAD INPUTS");
    // Uncomment to see assertion failures
    // s = Set_new(-1, NULL, NULL);
    // s = Set_new(20, int_cmp, bad_hash);
    // Set_free(NULL);
    // Set_T t2 = NULL;
    // Set_free(&t2);
    // Set_member(NULL, m[0]);
    // Set_member(s, NULL);
    // Set_length(NULL);
    // Set_put(NULL, m[0]);
    // Set_put(s, NULL);
    // Set_remove(NULL, m[0]);
    // Set_remove(s, NULL);
    // Set_map(NULL, NULL, NULL);
    // Set_toArray(NULL, NULL);
    // Set_T s2 = Set_new(20, NULL, NULL);
    // Set_union(NULL, NULL);
    // Set_inter(NULL, NULL);
    // Set_minus(NULL, NULL);
    // Set_diff(NULL, NULL);
    // Set_union(s, s2);
    // Set_inter(s, s2);
    // Set_minus(s, s2);
    // Set_diff(s, s2);
    puts("Set_test done");
}

void Array_test(void) {
    int d[5] = {1, 2, 3, 4, 5};
    int *e[5];
    for (int i = 0; i < 5; i++) {
        e[i] = &d[i];
    }
    puts("ARRAY EMPTY");
    Array_T a = Array_new(10, 4);
    assert(Array_length(a) == 10);
    assert(Array_size(a) == 4);
    puts("ARRAY PUT GET");
    assert(Array_put(a, 0, e[0]) == e[0]);
    assert(*(int *)Array_get(a, 0) == d[0]);
    assert(Array_put(a, Array_length(a) - 1, e[4]) == e[4]);
    assert(*(int *)Array_get(a, Array_length(a) - 1) == d[4]);
    puts("ARRAY RESIZE");
    for (int i = 1; i < 4; i++) {
        assert(Array_put(a, i, e[i]) == e[i]);
        assert(*(int *)Array_get(a, i) == d[i]);
    }
    Array_resize(a, 3);
    assert(Array_length(a) == 3);
    assert(Array_size(a) == 4);
    for (int i = 0; i < 3; i++) {
        assert(*(int *)Array_get(a, i) == d[i]);
    }
    Array_resize(a, 5);
    assert(Array_length(a) == 5);
    assert(Array_size(a) == 4);
    for (int i = 0; i < 3; i++) {
        assert(*(int *)Array_get(a, i) == d[i]);
    }
    assert(*(int *)Array_get(a, 3) == 0);
    assert(*(int *)Array_get(a, 4) == 0);
    Array_free(&a);
    puts("ARRAY COPY");
    a = Array_new(10, 4);
    for (int i = 0; i < 5; i++) {
        Array_put(a, i, e[i]);
    }
    Array_T a2 = Array_copy(a, 3);
    assert(Array_length(a2) == 3);
    assert(Array_size(a2) == 4);
    for (int i = 0; i < 3; i++) {
        assert(*(int *)Array_get(a2, i) == d[i]);
    }
    Array_put(a2, 0, e[4]);
    assert(*(int *)Array_get(a2, 0) == d[4]);
    assert(*(int *)Array_get(a, 0) == d[0]);
    Array_free(&a2);
    Array_free(&a);
    a = Array_new(3, 4);
    for (int i = 0; i < 3; i++) {
        Array_put(a, i, e[i]);
    }
    a2 = Array_copy(a, 5);
    assert(Array_length(a2) == 5);
    assert(Array_size(a2) == 4);
    for (int i = 0; i < 3; i++) {
        assert(*(int *)Array_get(a2, i) == d[i]);
    }
    assert(*(int *)Array_get(a2, 3) == 0);
    assert(*(int *)Array_get(a2, 4) == 0);
    Array_put(a2, 0, e[4]);
    assert(*(int *)Array_get(a2, 0) == d[4]);
    assert(*(int *)Array_get(a, 0) == d[0]);
    Array_free(&a);
    Array_free(&a2);
    puts("ARRAY REP INIT");
    struct Array_T sa;
    Array_T a3 = &sa;
    int l = 5;
    int s = 4;
    void *arr = malloc(l * s);
    ArrayRep_init(a3, l, s, arr);
    assert(Array_length(a3) == l);
    assert(Array_size(a3) == s);
    assert(a3->array == arr);
    puts("ARRAY CHECK BAD INPUTS");
    a = Array_new(10, 4);
    // Array_new(-1, 4);
    // Array_new(1, 0);
    // Array_free(NULL);
    Array_T t = NULL;
    // Array_free(&t);
    // Array_length(NULL);
    // Array_size(NULL);
    // Array_get(NULL, 0);
    // Array_get(a, -1);
    // Array_get(a, 10);
    // Array_put(NULL, 0, e[0]);
    // Array_put(a, -1, e[0]);
    // Array_put(a, 10, e[0]);
    // Array_put(a, 0, NULL);
    // Array_resize(NULL, 0);
    // Array_resize(a, -1);
    // Array_copy(NULL, 0);
    // Array_copy(a, -1);
    // ArrayRep_init(NULL, 0, 4, e[0]);
    // ArrayRep_init(t, -1, 4, e[0]);
    // ArrayRep_init(t, 0, 0, e[0]);
    // ArrayRep_init(t, 0, 4, NULL);
    Array_free(&a);
    puts("Array_test done");
}

void Seq_test(void) {
    char *data[5] = {"a", "b", "c", "d", "e"};
    puts("SEQ NEW");
    Seq_T s = Seq_new(10);
    assert(Seq_length(s) == 0);
    Seq_free(&s);
    puts("SEQ SEQ");
    s = Seq_seq(data[0], data[1], NULL);
    assert(Seq_length(s) == 2);
    assert(Seq_get(s, 0) == data[0]);
    assert(Seq_get(s, 1) == data[1]);
    Seq_free(&s);
    s = Seq_seq(NULL);
    assert(Seq_length(s) == 0);
    Seq_free(&s);
    puts("SEQ PUT");
    s = Seq_seq(data[0], data[1], NULL);
    assert(Seq_put(s, 0, data[2]) == data[0]);
    assert(Seq_put(s, 1, data[3]) == data[1]);
    assert(Seq_length(s) == 2);
    Seq_free(&s);
    puts("SEQ ADDLO");
    s = Seq_seq(NULL);
    assert(Seq_addlo(s, data[0]) == data[0]);
    assert(Seq_addlo(s, data[1]) == data[1]);
    assert(Seq_length(s) == 2);
    assert(Seq_get(s, 0) == data[1]);
    assert(Seq_get(s, 1) == data[0]);
    Seq_free(&s);
    s = Seq_seq(data[2], data[3], NULL);
    assert(Seq_addlo(s, data[0]) == data[0]);
    assert(Seq_addlo(s, data[1]) == data[1]);
    assert(Seq_length(s) == 4);
    assert(Seq_get(s, 0) == data[1]);
    assert(Seq_get(s, 1) == data[0]);
    assert(Seq_get(s, 2) == data[2]);
    assert(Seq_get(s, 3) == data[3]);
    Seq_free(&s);
    puts("SEQ ADDHI");
    s = Seq_seq(NULL);
    assert(Seq_addhi(s, data[0]) == data[0]);
    assert(Seq_addhi(s, data[1]) == data[1]);
    assert(Seq_length(s) == 2);
    assert(Seq_get(s, 1) == data[1]);
    assert(Seq_get(s, 0) == data[0]);
    Seq_free(&s);
    s = Seq_seq(data[2], data[3], NULL);
    assert(Seq_addhi(s, data[0]) == data[0]);
    assert(Seq_addhi(s, data[1]) == data[1]);
    assert(Seq_length(s) == 4);
    assert(Seq_get(s, 0) == data[2]);
    assert(Seq_get(s, 1) == data[3]);
    assert(Seq_get(s, 2) == data[0]);
    assert(Seq_get(s, 3) == data[1]);
    Seq_free(&s);
    puts("SEQ REMLO");
    s = Seq_seq(data[0], data[1], NULL);
    assert(Seq_remlo(s) == data[0]);
    assert(Seq_get(s, 0) == data[1]);
    assert(Seq_remlo(s) == data[1]);
    assert(Seq_length(s) == 0);
    Seq_free(&s);
    puts("SEQ REMHI");
    s = Seq_seq(data[0], data[1], NULL);
    assert(Seq_remhi(s) == data[1]);
    assert(Seq_get(s, 0) == data[0]);
    assert(Seq_remhi(s) == data[0]);
    assert(Seq_length(s) == 0);
    Seq_free(&s);
    puts("SEQ RESIZE");
    s = Seq_new(3);
    for (int i = 0; i < 4; i++) {
        Seq_addhi(s, data[i]);
    }
    assert(Seq_length(s) == 4);
    for (int i = 0; i < 4; i++) {
        assert(Seq_get(s, i) == data[i]);
    }
    Seq_free(&s);
    s = Seq_new(3);
    for (int i = 3; i >= 0; i--) {
        Seq_addlo(s, data[i]);
    }
    assert(Seq_length(s) == 4);
    for (int i = 0; i < 4; i++) {
        assert(Seq_get(s, i) == data[i]);
    }
    Seq_free(&s);
    s = Seq_new(3);
    Seq_addhi(s, data[0]);
    Seq_addhi(s, data[1]);
    Seq_addhi(s, data[2]);
    Seq_remlo(s);
    Seq_addhi(s, data[3]);
    Seq_addhi(s, data[4]);
    for (int i = 1; i < 5; i++) {
        assert(Seq_get(s, i - 1) == data[i]);
    }
    puts("SEQ CHECK BAD INPUTS");
    // Seq_new(-1);
    // Seq_free(NULL);
    Seq_T ns = NULL;
    // Seq_free(&ns);
    // Seq_length(NULL);
    // Seq_get(NULL, 0);
    // Seq_put(NULL, 0, NULL);
    // Seq_addlo(NULL, NULL);
    // Seq_addhi(NULL, NULL);
    // Seq_remlo(NULL);
    // Seq_remhi(NULL);
    Seq_T is = Seq_seq("a", "b", NULL);
    // Seq_get(is, -1);
    // Seq_get(is, 2);
    // Seq_put(is, -1, NULL);
    // Seq_put(is, 2, NULL);
    Seq_T es = Seq_seq(NULL);
    // Seq_remlo(es);
    // Seq_remhi(es);
    Seq_free(&is);
    Seq_free(&es);
    puts("Seq_test done");
}

void Ring_test(void) {
    char *data[5] = {"a", "b", "c", "d", "e"};
    puts("RING NEW");
    Ring_T r = Ring_new();
    assert(Ring_length(r) == 0);
    Ring_free(&r);
    puts("RING RING");
    r = Ring_ring(NULL);
    assert(Ring_length(r) == 0);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], NULL);
    assert(Ring_length(r) == 2);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    Ring_free(&r);
    puts("RING PUT");
    r = Ring_ring(data[0], data[1], NULL);
    assert(Ring_put(r, 0, data[2]) == data[0]);
    assert(Ring_put(r, 1, data[3]) == data[1]);
    assert(Ring_get(r, 0) == data[2]);
    assert(Ring_get(r, 1) == data[3]);
    Ring_free(&r);
    puts("RING ADDLO");
    r = Ring_ring(data[0], data[1], NULL);
    assert(Ring_addlo(r, data[2]) == data[2]);
    assert(Ring_addlo(r, data[3]) == data[3]);
    assert(Ring_get(r, 0) == data[3]);
    assert(Ring_get(r, 1) == data[2]);
    assert(Ring_get(r, 2) == data[0]);
    assert(Ring_get(r, 3) == data[1]);
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    puts("RING ADDHI");
    r = Ring_ring(data[0], data[1], NULL);
    assert(Ring_addhi(r, data[2]) == data[2]);
    assert(Ring_addhi(r, data[3]) == data[3]);
    for (int i = 0; i <= 3; i++) {
        assert(Ring_get(r, i) == data[i]);
    }
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    puts("RING REMLO");
    r = Ring_ring(data[0], data[1], data[2], data[3], NULL);
    assert(Ring_remlo(r) == data[0]);
    assert(Ring_remlo(r) == data[1]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    puts("RING REMHI");
    r = Ring_ring(data[0], data[1], data[2], data[3], NULL);
    assert(Ring_remhi(r) == data[3]);
    assert(Ring_remhi(r) == data[2]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    puts("RING ADD");
    r = Ring_ring(data[0], data[1], data[2], NULL);
    Ring_add(r, 2, data[3]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[3]);
    assert(Ring_get(r, 2) == data[1]);
    assert(Ring_get(r, 3) == data[2]);
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], NULL);
    Ring_add(r, 3, data[3]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_get(r, 2) == data[3]);
    assert(Ring_get(r, 3) == data[2]);
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], NULL);
    Ring_add(r, -2, data[3]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[3]);
    assert(Ring_get(r, 2) == data[1]);
    assert(Ring_get(r, 3) == data[2]);
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], NULL);
    Ring_add(r, -1, data[3]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_get(r, 2) == data[3]);
    assert(Ring_get(r, 3) == data[2]);
    assert(Ring_length(r) == 4);
    Ring_free(&r);
    r = Ring_ring(data[0], NULL);
    Ring_add(r, 1, data[1]);
    assert(Ring_get(r, 0) == data[1]);
    assert(Ring_get(r, 1) == data[0]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    r = Ring_ring(data[0], NULL);
    Ring_add(r, -1, data[1]);
    assert(Ring_get(r, 0) == data[1]);
    assert(Ring_get(r, 1) == data[0]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    r = Ring_ring(data[0], NULL);
    Ring_add(r, 2, data[1]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    r = Ring_ring(data[0], NULL);
    Ring_add(r, 0, data[1]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_length(r) == 2);
    Ring_free(&r);
    r = Ring_new();
    Ring_add(r, 0, data[0]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_length(r) == 1);
    Ring_free(&r);
    r = Ring_new();
    Ring_add(r, 1, data[0]);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_length(r) == 1);
    Ring_free(&r);
    puts("RING REMOVE");
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    assert(Ring_remove(r, 0) == data[0]);
    assert(Ring_length(r) == 4);
    for (int i = 0; i < 4; i++) {
        assert(Ring_get(r, i) == data[i + 1]);
    }
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    assert(Ring_remove(r, 4) == data[4]);
    assert(Ring_length(r) == 4);
    for (int i = 0; i < 4; i++) {
        assert(Ring_get(r, i) == data[i]);
    }
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    assert(Ring_remove(r, 1) == data[1]);
    assert(Ring_length(r) == 4);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[2]);
    assert(Ring_get(r, 2) == data[3]);
    assert(Ring_get(r, 3) == data[4]);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    assert(Ring_remove(r, 2) == data[2]);
    assert(Ring_length(r) == 4);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_get(r, 2) == data[3]);
    assert(Ring_get(r, 3) == data[4]);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    assert(Ring_remove(r, 3) == data[3]);
    assert(Ring_length(r) == 4);
    assert(Ring_get(r, 0) == data[0]);
    assert(Ring_get(r, 1) == data[1]);
    assert(Ring_get(r, 2) == data[2]);
    assert(Ring_get(r, 3) == data[4]);
    Ring_free(&r);
    puts("RING ROTATE");
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    Ring_rotate(r, -3);
    assert(Ring_get(r, 0) == data[3]);
    assert(Ring_get(r, 1) == data[4]);
    assert(Ring_get(r, 2) == data[0]);
    assert(Ring_get(r, 3) == data[1]);
    assert(Ring_get(r, 4) == data[2]);
    assert(Ring_length(r) == 5);
    Ring_free(&r);
    r = Ring_ring(data[0], data[1], data[2], data[3], data[4], NULL);
    Ring_rotate(r, 3);
    assert(Ring_get(r, 0) == data[2]);
    assert(Ring_get(r, 1) == data[3]);
    assert(Ring_get(r, 2) == data[4]);
    assert(Ring_get(r, 3) == data[0]);
    assert(Ring_get(r, 4) == data[1]);
    assert(Ring_length(r) == 5);
    Ring_free(&r);
    puts("RING INVALID INPUTS");
    // Ring_free(NULL);
    Ring_T r2 = NULL;
    // Ring_free(&r2);
    // Ring_length(NULL);
    // Ring_get(NULL, 0);
    r2 = Ring_ring("a", "b", NULL);
    // Ring_get(r2, -1);
    // Ring_get(r2, 2);
    // Ring_put(NULL, 0, "c");
    // Ring_put(r2, -1, "c");
    // Ring_put(r2, 2, "c");
    // Ring_add(NULL, 0, "c");
    // Ring_add(r2, -3, "c");
    // Ring_add(r2, 4, "c");
    // Ring_remove(NULL, 0);
    // Ring_remove(r2, -1);
    // Ring_remove(r2, 2);
    // Ring_rotate(NULL, 2);
    // Ring_rotate(r2, 3);
    // Ring_rotate(r2, -3);
    // Ring_addlo(NULL, "c");
    // Ring_addhi(NULL, "c");
    // Ring_remlo(NULL);
    // Ring_remhi(NULL);
    Ring_free(&r2);
    puts("Ring_test done");
}

void bit_apply(int n, int bit, void *cl) { *(int *)cl += (bit ? n : 0); }

void Bit_test(void) {
    int total = 0;
    puts("BIT NEW");
    Bit_T s = Bit_new(10);
    assert(Bit_length(s) == 10);
    assert(Bit_count(s) == 0);
    Bit_free(&s);
    puts("BIT PUT");
    s = Bit_new(10);
    assert(Bit_put(s, 0, 1) == 0);
    for (int i = 1; i < 10; i++) {
        assert(Bit_get(s, i) == 0);
    }
    assert(Bit_get(s, 0) == 1);
    assert(Bit_count(s) == 1);
    assert(Bit_put(s, 0, 0) == 1);
    assert(Bit_get(s, 0) == 0);
    assert(Bit_count(s) == 0);
    Bit_put(s, 0, 1);
    Bit_put(s, 5, 1);
    Bit_put(s, 9, 1);
    assert(Bit_get(s, 1) == 0);
    assert(Bit_get(s, 2) == 0);
    assert(Bit_get(s, 3) == 0);
    assert(Bit_get(s, 4) == 0);
    assert(Bit_get(s, 6) == 0);
    assert(Bit_get(s, 7) == 0);
    assert(Bit_get(s, 8) == 0);
    assert(Bit_count(s) == 3);
    Bit_free(&s);
    puts("BIT SET");
    s = Bit_new(42);
    Bit_set(s, 2, 2);
    assert(Bit_get(s, 2));
    assert(Bit_count(s) == 1);
    Bit_put(s, 2, 0);
    Bit_set(s, 10, 13);
    for (int i = 10; i <= 13; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 0; i < 10; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 14; i < Bit_length(s); i++) {
        assert(!Bit_get(s, i));
    }
    assert(Bit_count(s) == 4);
    Bit_set(s, 8, 31);
    for (int i = 8; i <= 15; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 0; i < 8; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 32; i < Bit_length(s); i++) {
        assert(!Bit_get(s, i));
    }
    assert(Bit_count(s) == 24);
    Bit_free(&s);
    puts("BIT CLEAR");
    s = Bit_new(42);
    Bit_set(s, 0, Bit_length(s) - 1);
    Bit_clear(s, 2, 2);
    assert(!Bit_get(s, 2));
    assert(Bit_count(s) == Bit_length(s) - 1);
    Bit_put(s, 2, 1);
    Bit_clear(s, 10, 13);
    for (int i = 10; i <= 13; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 0; i < 10; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 14; i < Bit_length(s); i++) {
        assert(Bit_get(s, i));
    }
    assert(Bit_count(s) == Bit_length(s) - 4);
    Bit_clear(s, 8, 31);
    for (int i = 8; i <= 15; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 0; i < 8; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 32; i < Bit_length(s); i++) {
        assert(Bit_get(s, i));
    }
    assert(Bit_count(s) == Bit_length(s) - 24);
    Bit_free(&s);
    puts("BIT NOT");
    s = Bit_new(42);
    Bit_not(s, 2, 2);
    assert(Bit_get(s, 2));
    Bit_not(s, 2, 2);
    assert(!Bit_get(s, 2));
    Bit_set(s, 0, Bit_length(s) - 1);
    Bit_not(s, 10, 13);
    for (int i = 10; i <= 13; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 0; i < 10; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 14; i < Bit_length(s); i++) {
        assert(Bit_get(s, i));
    }
    assert(Bit_count(s) == Bit_length(s) - 4);
    Bit_not(s, 8, 31);
    for (int i = 0; i < 8; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 32; i < Bit_length(s); i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 8; i < 10; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 10; i <= 13; i++) {
        assert(Bit_get(s, i));
    }
    for (int i = 14; i <= 31; i++) {
        assert(!Bit_get(s, i));
    }
    for (int i = 32; i < Bit_length(s); i++) {
        assert(Bit_get(s, i));
    }
    assert(Bit_count(s) == Bit_length(s) - 20);
    Bit_free(&s);
    puts("BIT LT");
    s = Bit_new(10);
    Bit_put(s, 0, 1);
    Bit_set(s, 2, 4);
    Bit_put(s, 7, 1);
    Bit_T s2 = Bit_new(10);
    Bit_put(s2, 0, 1);
    Bit_set(s2, 2, 4);
    Bit_set(s2, 7, 8);
    Bit_T s3 = Bit_new(10);
    Bit_put(s3, 0, 1);
    Bit_set(s3, 2, 4);
    Bit_put(s3, 8, 1);
    Bit_T s4 = Bit_new(10);
    Bit_put(s4, 0, 1);
    Bit_set(s4, 2, 4);
    Bit_put(s4, 7, 1);
    Bit_T s6 = Bit_new(10);
    assert(!Bit_lt(s, s));
    assert(Bit_lt(s, s2));
    assert(!Bit_lt(s, s3));
    assert(!Bit_lt(s, s4));
    assert(Bit_lt(s6, s));
    puts("BIT EQ");
    assert(Bit_eq(s, s));
    assert(!Bit_eq(s, s2));
    assert(!Bit_eq(s, s3));
    assert(Bit_eq(s, s4));
    assert(!Bit_eq(s6, s));
    puts("BIT LEQ");
    assert(Bit_leq(s, s));
    assert(Bit_leq(s, s2));
    assert(!Bit_leq(s, s3));
    assert(Bit_leq(s, s4));
    assert(Bit_leq(s6, s));
    puts("BIT MAP");
    Bit_map(s, bit_apply, &total);
    assert(total == +2 + 3 + 4 + 7);
    puts("BIT UNION");
    Bit_T out = Bit_union(s, s2);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 6);
    Bit_free(&out);
    out = Bit_union(s, s3);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 6);
    Bit_free(&out);
    out = Bit_union(s, s6);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    out = Bit_union(s, NULL);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    puts("BIT INTER");
    out = Bit_inter(s, s2);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    out = Bit_inter(s, s3);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(!Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 4);
    Bit_free(&out);
    out = Bit_inter(s, s4);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    out = Bit_inter(s, s6);
    for (int i = 0; i < 10; i++) {
        assert(!Bit_get(out, i));
    }
    assert(Bit_count(out) == 0);
    Bit_free(&out);
    out = Bit_inter(s, NULL);
    for (int i = 0; i < 10; i++) {
        assert(!Bit_get(out, i));
    }
    assert(Bit_count(out) == 0);
    Bit_free(&out);
    puts("BIT MINUS");
    out = Bit_minus(s, s2);
    for (int i = 0; i < Bit_length(out); i++) {
        assert(!Bit_get(out, i));
    }
    assert(Bit_count(out) == 0);
    Bit_free(&out);
    out = Bit_minus(s, s3);
    for (int i = 0; i < Bit_length(out); i++) {
        if (i == 7) {
            assert(Bit_get(out, i));
        } else {
            assert(!Bit_get(out, i));
        }
    }
    assert(Bit_count(out) == 1);
    Bit_free(&out);
    out = Bit_minus(s, s4);
    for (int i = 0; i < Bit_length(out); i++) {
        assert(!Bit_get(out, i));
    }
    assert(Bit_count(out) == 0);
    Bit_free(&out);
    out = Bit_minus(s, s6);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    out = Bit_minus(s, NULL);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    puts("BIT DIFF");
    out = Bit_diff(s, s2);
    for (int i = 0; i < Bit_length(out); i++) {
        if (i == 8) {
            assert(Bit_get(out, i));
        } else {
            assert(!Bit_get(out, i));
        }
    }
    assert(Bit_count(out) == 1);
    Bit_free(&out);
    out = Bit_diff(s, s3);
    for (int i = 0; i < Bit_length(out); i++) {
        if (i == 7 || i == 8) {
            assert(Bit_get(out, i));
        } else {
            assert(!Bit_get(out, i));
        }
    }
    assert(Bit_count(out) == 2);
    Bit_free(&out);
    out = Bit_diff(s, s4);
    for (int i = 0; i < Bit_length(out); i++) {
        assert(!Bit_get(out, i));
    }
    assert(Bit_count(out) == 0);
    Bit_free(&out);
    out = Bit_diff(s, s6);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    out = Bit_diff(s, NULL);
    assert(Bit_get(out, 0));
    assert(!Bit_get(out, 1));
    assert(Bit_get(out, 2));
    assert(Bit_get(out, 3));
    assert(Bit_get(out, 4));
    assert(!Bit_get(out, 5));
    assert(!Bit_get(out, 6));
    assert(Bit_get(out, 7));
    assert(!Bit_get(out, 8));
    assert(!Bit_get(out, 9));
    assert(Bit_count(out) == 5);
    Bit_free(&out);
    Bit_free(&s);
    Bit_free(&s2);
    Bit_free(&s3);
    Bit_free(&s4);
    Bit_free(&s6);
    puts("BIT INVALID INPUTS");
    // Bit_new(-1);
    // Bit_length(NULL);
    // Bit_count(NULL);
    // Bit_free(NULL);
    Bit_T ns = NULL;
    // Bit_free(&ns);
    // Bit_get(NULL, 0);
    Bit_T es = Bit_new(10);
    Bit_T es2 = Bit_new(9);
    // Bit_get(es, -1);
    // Bit_get(es, 10);
    // Bit_put(NULL, 0, 1);
    // Bit_put(es, -1, 1);
    // Bit_put(es, 10, 1);
    // Bit_put(es, 0, 2);
    // Bit_put(es, 0, -1);
    // Bit_clear(NULL, 2, 5);
    // Bit_clear(es, -1, 5);
    // Bit_clear(es, 10, 5);
    // Bit_clear(es, 2, -1);
    // Bit_clear(es, 2, 10);
    // Bit_clear(es, 3, 2);
    // Bit_set(NULL, 2, 5);
    // Bit_set(es, -1, 5);
    // Bit_set(es, 10, 5);
    // Bit_set(es, 2, -1);
    // Bit_set(es, 2, 10);
    // Bit_set(es, 3, 2);
    // Bit_not(NULL, 2, 5);
    // Bit_not(es, -1, 5);
    // Bit_not(es, 10, 5);
    // Bit_not(es, 2, -1);
    // Bit_not(es, 2, 10);
    // Bit_not(es, 3, 2);
    // Bit_lt(es, NULL);
    // Bit_lt(NULL, es);
    // Bit_lt(es, es2);
    // Bit_eq(es, NULL);
    // Bit_eq(NULL, es);
    // Bit_eq(es, es2);
    // Bit_leq(es, NULL);
    // Bit_leq(NULL, es);
    // Bit_leq(es, es2);
    // Bit_map(NULL, bit_apply, &total);
    // Bit_map(es, NULL, NULL);
    // Bit_union(NULL, NULL);
    // Bit_union(es, es2);
    // Bit_inter(NULL, NULL);
    // Bit_inter(es, es2);
    // Bit_minus(NULL, NULL);
    // Bit_minus(es, es2);
    // Bit_diff(NULL, NULL);
    // Bit_diff(es, es2);
    puts("Bit_test done");
}

static int check(char *a, char *e) {
    int c = strcmp(a, e);
    free(a);
    return c == 0;
}

void Str_test(void) {
    puts("STR SUB");
    assert(check(Str_sub("", 1, 1), ""));
    assert(check(Str_sub("abc", 2, 2), ""));
    assert(check(Str_sub("abc", 1, 3), "ab"));
    assert(check(Str_sub("abc", -3, 3), "ab"));
    assert(check(Str_sub("abc", -3, 0), "abc"));
    puts("STR DUP");
    assert(check(Str_dup("", 1, 1, 5), ""));
    assert(check(Str_dup("abc", 1, 1, 5), ""));
    assert(check(Str_dup("abc", 2, 2, 5), ""));
    assert(check(Str_dup("abc", 2, 3, 0), ""));
    assert(check(Str_dup("abc", -3, 3, 1), "ab"));
    assert(check(Str_dup("abc", -3, 0, 2), "abcabc"));
    puts("STR CAT");
    assert(check(Str_cat("abc", 1, 1, "def", 2, 2), ""));
    assert(check(Str_cat("abc", 1, 2, "def", 2, 2), "a"));
    assert(check(Str_cat("abc", 1, 1, "def", 2, 4), "ef"));
    assert(check(Str_cat("abc", -2, -1, "def", 2, 0), "bef"));
    puts("STR CATV");
    assert(check(Str_catv(NULL), ""));
    assert(check(Str_catv("abc", 1, 3, NULL), "ab"));
    assert(check(Str_catv("abc", 1, 3, "def", -3, 0, NULL), "abdef"));
    assert(check(Str_catv("abc", 1, 3, "def", -3, 0, "ghi", -3, 2, NULL),
                 "abdefg"));
    puts("STR REVERSE");
    assert(check(Str_reverse("", 1, 1), ""));
    assert(check(Str_reverse("a", 1, 2), "a"));
    assert(check(Str_reverse("a", 1, 0), "a"));
    assert(check(Str_reverse("a", -1, 0), "a"));
    assert(check(Str_reverse("abc", -3, 3), "ba"));
    assert(check(Str_reverse("abc", 2, 4), "cb"));
    puts("STR MAP");
    assert(check(Str_map("", 1, 1, "abc", "ABC"), ""));
    assert(check(Str_map("abcDEF", 2, 6, NULL, NULL), "BCDE"));
    assert(check(Str_map("abcDEF", 2, -1, NULL, NULL), "BCDE"));
    assert(check(Str_map("abcDEF", -5, -1, "ABCDEF", "abcdef"), "bcde"));
    assert(check(Str_map("abcDEF", -5, 6, NULL, NULL), "bcde"));
    puts("STR POS");
    assert(Str_pos("abc", 3) == 3);
    assert(Str_pos("abc", -1) == 3);
    puts("STR LEN");
    assert(Str_len("abc", -2, 0) == 2);
    assert(Str_len("abc", 2, 0) == 2);
    assert(Str_len("abc", 1, 3) == 2);
    puts("STR CMP");
    assert(Str_cmp("abcd", -3, -1, "abcd", 2, 0) < 0);
    assert(Str_cmp("abcd", -3, 0, "abcd", -3, 4) > 0);
    assert(Str_cmp("abcd", -3, -1, "abcd", 2, 4) == 0);
    assert(Str_cmp("abcd", -3, -1, "abce", 2, 5) < 0);
    assert(Str_cmp("abcf", -3, 0, "abce", 2, 5) > 0);
    assert(Str_cmp("abcf", -3, -1, "abce", 2, 4) == 0);
    puts("STR CHR");
    assert(Str_chr("abc", 2, 3, 'b') == 2);
    assert(Str_chr("abc", 2, 3, 'c') == 0);
    assert(Str_chr("cbc", 1, 0, 'c') == 1);
    puts("STR RCHR");
    assert(Str_rchr("abc", 2, 3, 'b') == 2);
    assert(Str_rchr("abc", 2, 3, 'c') == 0);
    assert(Str_rchr("cbc", 1, 0, 'c') == 3);
    puts("STR UPTO");
    assert(Str_upto("ace", 1, 0, "bcd") == 2);
    assert(Str_upto("ace", -3, 4, "bdf") == 0);
    assert(Str_upto("aae", -3, 3, "baf") == 1);
    puts("STR RUPTO");
    assert(Str_rupto("ace", 1, 0, "bcd") == 3);
    assert(Str_rupto("ace", -3, 4, "bdf") == 0);
    assert(Str_rupto("aae", -3, 4, "baf") == 3);
    puts("STR FIND");
    assert(Str_find("abcd", 2, 4, "bcd") == 0);
    assert(Str_find("abcd", 2, 4, "bc") == 2);
    assert(Str_find("accd", 2, 4, "c") == 2);
    assert(Str_find("abc", 2, 4, "") == 2);
    puts("STR RFIND");
    assert(Str_rfind("abcd", 2, 4, "bcd") == 0);
    assert(Str_rfind("abcd", 2, 4, "bc") == 2);
    assert(Str_rfind("accd", 2, 4, "c") == 3);
    assert(Str_rfind("abc", 2, 4, "") == 4);
    puts("STR ANY");
    assert(Str_any("abcd", 2, "abc") == 3);
    assert(Str_any("abcd", -3, "abc") == 3);
    assert(Str_any("abcd", -3, "ace") == 0);
    puts("STR MANY");
    assert(Str_many("abcd", -3, 0, "dce") == 0);
    assert(Str_many("abcd", -3, -1, "dcb") == 4);
    assert(Str_many("abeb", 2, 5, "dcb") == 3);
    puts("STR RMANY");
    assert(Str_rmany("abcd", -3, 0, "dce") == 3);
    assert(Str_rmany("abcd", -3, -1, "dcb") == 2);
    assert(Str_rmany("abeb", 1, 5, "dcb") == 4);
    puts("STR MATCH");
    assert(Str_match("abcd", 2, 4, "bcd") == 0);
    assert(Str_match("abcd", 2, 5, "bcd") == 5);
    assert(Str_match("abcb", -3, 0, "bc") == 4);
    assert(Str_match("abcb", -3, 0, "") == 2);
    puts("STR RMATCH");
    assert(Str_rmatch("abcd", 2, 4, "bcd") == 0);
    assert(Str_rmatch("abcd", 2, 5, "bcd") == 2);
    assert(Str_rmatch("abcb", -3, 0, "cb") == 3);
    assert(Str_rmatch("abcb", -3, 0, "") == 5);
    puts("STR INVALID INPUTS");
    char *si = "abcd";
    char *si2 = "defg";
    // Str_sub(NULL, 0, 1);
    // Str_sub(si, -5, 0);
    // Str_sub(si, 6, 0);
    // Str_sub(si, 0, -5);
    // Str_sub(si, 0, 6);
    // Str_dup(NULL, 0, 1, 1);
    // Str_dup(si, -5, 0, 1);
    // Str_dup(si, 6, 0, 1);
    // Str_dup(si, 0, -5, 1);
    // Str_dup(si, 0, 6, 1);
    // Str_dup(si, 0, 1, -1);
    // Str_cat(NULL, 0, 1, si2, 0, 1);
    // Str_cat(si, -5, 1, si2, 0, 1);
    // Str_cat(si, 6, 1, si2, 0, 1);
    // Str_cat(si, 1, -5, si2, 0, 1);
    // Str_cat(si, 1, 6, si2, 0, 1);
    // Str_cat(si, 1, 1, NULL, 0, 1);
    // Str_cat(si, 1, 1, si2, -5, 1);
    // Str_cat(si, 1, 1, si2, 6, 1);
    // Str_cat(si, 1, 1, si2, 1, -5);
    // Str_cat(si, 1, 1, si2, 1, 6);
    // Str_catv(si, -5, 1, si2, 0, 1, NULL);
    // Str_catv(si, 6, 1, si2, 0, 1, NULL);
    // Str_catv(si, 1, -5, si2, 0, 1, NULL);
    // Str_catv(si, 1, 6, si2, 0, 1, NULL);
    // Str_catv(si, 1, 1, si2, -5, 1, NULL);
    // Str_catv(si, 1, 1, si2, 6, 1, NULL);
    // Str_catv(si, 1, 1, si2, 1, -5, NULL);
    // Str_catv(si, 1, 1, si2, 1, 6, NULL);
    // Str_reverse(NULL, 0, 1);
    // Str_reverse(si, -5, 0);
    // Str_reverse(si, 6, 0);
    // Str_reverse(si, 0, -5);
    // Str_reverse(si, 0, 6);
    // Str_map(NULL, 0, 1, NULL, NULL);
    // Str_map(si, -5, 1, NULL, NULL);
    // Str_map(si, 6, 1, NULL, NULL);
    // Str_map(si, 1, -5, NULL, NULL);
    // Str_map(si, 1, 6, NULL, NULL);
    // Str_map(si, 1, 1, NULL, NULL);
    // Str_pos(NULL, 0);
    // Str_pos(si, -5);
    // Str_pos(si, 6);
    // Str_len(NULL, 0, 1);
    // Str_len(si, -5, 0);
    // Str_len(si, 6, 0);
    // Str_len(si, 0, -5);
    // Str_len(si, 0, 6);
    // Str_cmp(NULL, 0, 1, si2, 0, 1);
    // Str_cmp(si, -5, 1, si2, 0, 1);
    // Str_cmp(si, 6, 1, si2, 0, 1);
    // Str_cmp(si, 1, -5, si2, 0, 1);
    // Str_cmp(si, 1, 6, si2, 0, 1);
    // Str_cmp(si, 1, 1, NULL, 0, 1);
    // Str_cmp(si, 1, 1, si2, -5, 1);
    // Str_cmp(si, 1, 1, si2, 6, 1);
    // Str_cmp(si, 1, 1, si2, 1, -5);
    // Str_cmp(si, 1, 1, si2, 1, 6);
    // Str_chr(NULL, 0, 1, 97);
    // Str_chr(si, -5, 0, 97);
    // Str_chr(si, 6, 0, 97);
    // Str_chr(si, 0, -5, 97);
    // Str_chr(si, 0, 6, 97);
    // Str_rchr(NULL, 0, 1, 97);
    // Str_rchr(si, -5, 0, 97);
    // Str_rchr(si, 6, 0, 97);
    // Str_rchr(si, 0, -5, 97);
    // Str_rchr(si, 0, 6, 97);
    // Str_upto(NULL, 0, 1, si2);
    // Str_upto(si, -5, 0, si2);
    // Str_upto(si, 6, 0, si2);
    // Str_upto(si, 0, -5, si2);
    // Str_upto(si, 0, 6, si2);
    // Str_upto(si, 0, 1, NULL);
    // Str_rupto(NULL, 0, 1, si2);
    // Str_rupto(si, -5, 0, si2);
    // Str_rupto(si, 6, 0, si2);
    // Str_rupto(si, 0, -5, si2);
    // Str_rupto(si, 0, 6, si2);
    // Str_rupto(si, 0, 1, NULL);
    // Str_find(NULL, 0, 1, si2);
    // Str_find(si, -5, 0, si2);
    // Str_find(si, 6, 0, si2);
    // Str_find(si, 0, -5, si2);
    // Str_find(si, 0, 6, si2);
    // Str_find(si, 0, 1, NULL);
    // Str_rfind(NULL, 0, 1, si2);
    // Str_rfind(si, -5, 0, si2);
    // Str_rfind(si, 6, 0, si2);
    // Str_rfind(si, 0, -5, si2);
    // Str_rfind(si, 0, 6, si2);
    // Str_rfind(si, 0, 1, NULL);
    // Str_any(NULL, 0, si2);
    // Str_any(si, -5, si2);
    // Str_any(si, -6, si2);
    // Str_any(si, -6, NULL);
    // Str_any(si, 5, si2);
    // Str_many(NULL, 0, 1, si2);
    // Str_many(si, -5, 0, si2);
    // Str_many(si, 6, 0, si2);
    // Str_many(si, 0, -5, si2);
    // Str_many(si, 0, 6, si2);
    // Str_many(si, 0, 1, NULL);
    // Str_rmany(NULL, 0, 1, si2);
    // Str_rmany(si, -5, 0, si2);
    // Str_rmany(si, 6, 0, si2);
    // Str_rmany(si, 0, -5, si2);
    // Str_rmany(si, 0, 6, si2);
    // Str_rmany(si, 0, 1, NULL);
    // Str_match(NULL, 0, 1, si2);
    // Str_match(si, -5, 0, si2);
    // Str_match(si, 6, 0, si2);
    // Str_match(si, 0, -5, si2);
    // Str_match(si, 0, 6, si2);
    // Str_match(si, 0, 1, NULL);
    // Str_rmatch(NULL, 0, 1, si2);
    // Str_rmatch(si, -5, 0, si2);
    // Str_rmatch(si, 6, 0, si2);
    // Str_rmatch(si, 0, -5, si2);
    // Str_rmatch(si, 0, 6, si2);
    // Str_rmatch(si, 0, 1, NULL);
    puts("Str_test done");
}

static int text_check(Text_T a, char *e) {
    int c = strncmp(a.str, e, a.len);
    int al = a.len;
    if (c != 0) {
        puts(a.str);
    }
    Text_free(&a);
    return c == 0 && strlen(e) == al;
}

static int text_check2(Text_T a, char *e) {
    int c = strncmp(a.str, e, a.len);
    return c == 0 && strlen(e) == a.len;
}

void Text_test(void) {
    Text_T empty = {0, ""};
    Text_T abc = {3, "abc"};
    puts("TEXT SUB");
    assert(text_check2(Text_sub(empty, 1, 1), ""));
    assert(text_check2(Text_sub(abc, 2, 2), ""));
    assert(text_check2(Text_sub(abc, 1, 3), "ab"));
    assert(text_check2(Text_sub(abc, -3, 3), "ab"));
    assert(text_check2(Text_sub(abc, -3, 0), "abc"));
    puts("TEXT DUP");
    assert(text_check(Text_dup(empty, 5), ""));
    assert(text_check(Text_dup(abc, 0), ""));
    assert(text_check(Text_dup(abc, 1), "abc"));
    assert(text_check(Text_dup(abc, 2), "abcabc"));
    puts("TEXT CAT");
    Text_T def = {3, "def"};
    assert(text_check(Text_cat(empty, empty), ""));
    assert(text_check(Text_cat(abc, empty), "abc"));
    assert(text_check(Text_cat(abc, def), "abcdef"));
    puts("TEXT REVERSE");
    Text_T a = {1, "a"};
    assert(text_check(Text_reverse(empty), ""));
    assert(text_check(Text_reverse(a), "a"));
    assert(text_check(Text_reverse(abc), "cba"));
    puts("TEXT MAP");
    Text_T ABC = {3, "ABC"};
    Text_T abcDEF = {6, "abcDEF"};
    assert(text_check(Text_map(empty, &abc, &ABC), ""));
    assert(text_check(Text_map(abcDEF, NULL, NULL), "ABCDEF"));
    assert(text_check(Text_map(abcDEF, &Text_ucase, &Text_lcase), "abcdef"));
    puts("TEXT POS");
    assert(Text_pos(abc, 3) == 3);
    assert(Text_pos(abc, -1) == 3);
    puts("TEXT CMP");
    Text_T abcd = {4, "abcd"};
    assert(Text_cmp(abc, abcd) < 0);
    assert(Text_cmp(abcd, abc) > 0);
    assert(Text_cmp(abc, abc) == 0);
    puts("TEXT CHR");
    Text_T cbc = {3, "cbc"};
    assert(Text_chr(abc, 2, 3, 'b') == 2);
    assert(Text_chr(abc, 2, 3, 'c') == 0);
    assert(Text_chr(cbc, 1, 0, 'c') == 1);
    puts("TEXT RCHR");
    assert(Text_rchr(abc, 2, 3, 'b') == 2);
    assert(Text_rchr(abc, 2, 3, 'c') == 0);
    assert(Text_rchr(cbc, 1, 0, 'c') == 3);
    puts("TEXT UPTO");
    Text_T ace = {3, "ace"};
    Text_T bcd = {3, "bcd"};
    Text_T bdf = {3, "bdf"};
    Text_T aae = {3, "aae"};
    Text_T baf = {3, "baf"};
    assert(Text_upto(ace, 1, 0, bcd) == 2);
    assert(Text_upto(ace, -3, 4, bdf) == 0);
    assert(Text_upto(aae, -3, 3, baf) == 1);
    puts("TEXT RUPTO");
    assert(Text_rupto(ace, 1, 0, bcd) == 3);
    assert(Text_rupto(ace, -3, 4, bdf) == 0);
    assert(Text_rupto(aae, -3, 4, baf) == 3);
    puts("TEXT FIND");
    Text_T c = {1, "c"};
    Text_T bc = {2, "bc"};
    Text_T accd = {4, "accd"};
    assert(Text_find(abcd, 2, 4, bcd) == 0);
    assert(Text_find(abcd, 2, 4, bc) == 2);
    assert(Text_find(accd, 2, 4, c) == 2);
    assert(Text_find(abc, 2, 4, empty) == 2);
    puts("TEXT RFIND");
    assert(Text_rfind(abcd, 2, 4, bcd) == 0);
    assert(Text_rfind(abcd, 2, 4, bc) == 2);
    assert(Text_rfind(accd, 2, 4, c) == 3);
    assert(Text_rfind(abc, 2, 4, empty) == 4);
    puts("TEXT ANY");
    assert(Text_any(abcd, 2, abc) == 3);
    assert(Text_any(abcd, -3, abc) == 3);
    assert(Text_any(abcd, -3, ace) == 0);
    puts("TEXT MANY");
    Text_T dce = {3, "dce"};
    Text_T dcb = {3, "dcb"};
    Text_T abeb = {4, "abeb"};
    assert(Text_many(abcd, -3, 0, dce) == 0);
    assert(Text_many(abcd, -3, -1, dcb) == 4);
    assert(Text_many(abeb, 2, 5, dcb) == 3);
    puts("TEXT RMANY");
    assert(Text_rmany(abcd, -3, 0, dce) == 3);
    assert(Text_rmany(abcd, -3, -1, dcb) == 2);
    assert(Text_rmany(abeb, 1, 5, dcb) == 4);
    puts("TEXT MATCH");
    Text_T abcb = {4, "abcb"};
    assert(Text_match(abcd, 2, 4, bcd) == 0);
    assert(Text_match(abcd, 2, 5, bcd) == 5);
    assert(Text_match(abcb, -3, 0, bc) == 4);
    assert(Text_match(abcb, -3, 0, empty) == 2);
    puts("TEXT RMATCH");
    Text_T cb = {2, "cb"};
    assert(Text_rmatch(abcd, 2, 4, bcd) == 0);
    assert(Text_rmatch(abcd, 2, 5, bcd) == 2);
    assert(Text_rmatch(abcb, -3, 0, cb) == 3);
    assert(Text_rmatch(abcb, -3, 0, empty) == 5);
    puts("TEXT INVALID INPUTS");
    Text_T si = {4, "abcd"};
    Text_T si2 = {4, "defg"};
    // Text_sub(si, -5, 0);
    // Text_sub(si, 6, 0);
    // Text_sub(si, 0, -5);
    // Text_sub(si, 0, 6);
    // Text_dup(si, -1);
    // Text_map(si, NULL, NULL);
    // Text_pos(si, -5);
    // Text_pos(si, 6);
    // Text_chr(si, -5, 0, 97);
    // Text_chr(si, 6, 0, 97);
    // Text_chr(si, 0, -5, 97);
    // Text_chr(si, 0, 6, 97);
    // Text_rchr(si, -5, 0, 97);
    // Text_rchr(si, 6, 0, 97);
    // Text_rchr(si, 0, -5, 97);
    // Text_rchr(si, 0, 6, 97);
    // Text_upto(si, -5, 0, si2);
    // Text_upto(si, 6, 0, si2);
    // Text_upto(si, 0, -5, si2);
    // Text_upto(si, 0, 6, si2);
    // Text_rupto(si, -5, 0, si2);
    // Text_rupto(si, 6, 0, si2);
    // Text_rupto(si, 0, -5, si2);
    // Text_rupto(si, 0, 6, si2);
    // Text_find(si, -5, 0, si2);
    // Text_find(si, 6, 0, si2);
    // Text_find(si, 0, -5, si2);
    // Text_find(si, 0, 6, si2);
    // Text_rfind(si, -5, 0, si2);
    // Text_rfind(si, 6, 0, si2);
    // Text_rfind(si, 0, -5, si2);
    // Text_rfind(si, 0, 6, si2);
    // Text_any(si, -5, si2);
    // Text_any(si, -6, si2);
    // Text_any(si, 5, si2);
    // Text_many(si, -5, 0, si2);
    // Text_many(si, 6, 0, si2);
    // Text_many(si, 0, -5, si2);
    // Text_many(si, 0, 6, si2);
    // Text_rmany(si, -5, 0, si2);
    // Text_rmany(si, 6, 0, si2);
    // Text_rmany(si, 0, -5, si2);
    // Text_rmany(si, 0, 6, si2);
    // Text_match(si, -5, 0, si2);
    // Text_match(si, 6, 0, si2);
    // Text_match(si, 0, -5, si2);
    // Text_match(si, 0, 6, si2);
    // Text_rmatch(si, -5, 0, si2);
    // Text_rmatch(si, 6, 0, si2);
    // Text_rmatch(si, 0, -5, si2);
    // Text_rmatch(si, 0, 6, si2);
    puts("Text_test done");
}