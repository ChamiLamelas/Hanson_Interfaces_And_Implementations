#include "set.h"
#include <stdlib.h>
#include <assert.h>

#define T Set_T

static const double LOAD_FACTOR_THRESHOLD = 0.5;

static const int EMPTY_SET_HINT = 100;

struct T
{
    unsigned num_elements;
    unsigned num_buckets;
    unsigned mod_ops;
    int (*cmp)(const void *member1, const void *member2);
    unsigned (*hash)(const void *member);
    struct node
    {
        struct node *next;
        void *member;
    } * *buckets;
};

// Checks if n is prime
static int is_prime(int n)
{
    // Optimization: 2 is the only prime even number
    if (n % 2 == 0)
        return n == 2;
    // Only need to go to halfway point over odd numbers up to n
    // anything about halfway point yields quotient of 1
    for (int i = 3; i < (n / 2) + 1; i += 2)
        if (n % i == 0)
            return 0;
    return 1;
}

// Finds the smallest prime number > n
static unsigned next_prime(int n)
{
    n += (n % 2 == 0);
    for (; !is_prime(n); n += 2)
        ;
    return n;
}

static unsigned atom_hash(const void *member)
{
    return ((size_t)member) * 2654435761;
}

// Designed to act like regular cmps (0 when equal, != 0 when not equal)
static int atom_cmp(const void *member1, const void *member2)
{
    return member1 != member2;
}

// hash into set
static unsigned hash(T set, void *member)
{
    return set->hash(member) % set->num_buckets;
}

static struct node **find(T set, void *member)
{
    // Get pointer to node pointer containing member or pointer to
    // the next of the last node pointer in the bucket
    // for struct node **x, &x[h] = &(*(x+h)) = x + h
    // I just do x + h versus what's given in the text
    struct node **itr = set->buckets + hash(set, member);
    for (; *itr && set->cmp((*itr)->member, member) != 0; itr = &((*itr)->next))
        ;
    return itr;
}

// push node on top of a bucket
static void push_node(void *member, struct node **bucket)
{
    struct node *new_node;
    new_node = malloc(sizeof(*new_node));
    assert(new_node);
    new_node->member = member;
    new_node->next = *bucket;
    *bucket = new_node;
}

// allocates buckets and inits to NULL
static struct node **allocate_buckets(int num_buckets)
{
    struct node **buckets;
    buckets = malloc(sizeof(buckets[0]) * num_buckets);
    assert(buckets);
    for (int i = 0; i < num_buckets; i++)
        buckets[i] = NULL;
    return buckets;
}

static void auto_rehash(T set)
{
    if (set->num_elements < LOAD_FACTOR_THRESHOLD * set->num_buckets)
        return;

    struct node **cpy = set->buckets;
    // update set buckets so hash works properly
    unsigned old_num_buckets = set->num_buckets;
    set->num_buckets *= 2;
    set->buckets = allocate_buckets(set->num_buckets);
    struct node *next;
    for (int i = 0; i < old_num_buckets; i++)
        for (; cpy[i]; cpy[i] = next)
        {
            // push onto the appropriate bucket and free from old buckets
            push_node(cpy[i]->member, set->buckets + hash(set, cpy[i]->member));
            next = cpy[i]->next;
            free(cpy[i]);
        }
    free(cpy);
}

static void check_sets_for_set_ops(T s, T t)
{
    assert(s || t);
    if (s && t)
        assert(s->cmp == t->cmp && s->hash == t->hash);
}

T Set_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *x))
{
    assert(hint >= 0);
    unsigned num_buckets = next_prime(hint);
    T set;
    set = malloc(sizeof(*set));
    assert(set);
    set->cmp = cmp ? cmp : atom_cmp;
    set->hash = hash ? hash : atom_hash;
    set->num_elements = 0;
    set->num_buckets = num_buckets;
    set->mod_ops = 0;
    // buckets are not allocated as part of set struct allocation
    // because buckets must be freed during rehashing
    set->buckets = allocate_buckets(num_buckets);
    return set;
}

void Set_free(T *set)
{
    assert(set && *set);
    struct node *next;
    for (int i = 0; i < (*set)->num_buckets; i++)
        for (; (*set)->buckets[i]; (*set)->buckets[i] = next)
        {
            next = (*set)->buckets[i]->next;
            free((*set)->buckets[i]);
        }
    free((*set)->buckets);
    free(*set);
    *set = NULL;
}

int Set_length(T set)
{
    assert(set);
    return set->num_elements;
}

int Set_member(T set, void *member)
{
    assert(set && member);
    return *find(set, member) != NULL;
}

void Set_put(T set, void *member)
{
    assert(set && member);
    auto_rehash(set);
    struct node **node = find(set, member);
    if (*node)
        (*node)->member = member;
    else
    {
        // push onto bucket, note here to get the pointer
        // to the pointer that heads a bucket (so that bucket
        // head can be updated) we add to a node** to get
        // a node**
        push_node(member, set->buckets + hash(set, member));
        set->num_elements++;
    }
    set->mod_ops++;
}

void *Set_remove(T set, void *member)
{
    assert(set && member);
    struct node **node = find(set, member);
    if (*node == NULL)
        return NULL;
    struct node *next = (*node)->next;
    void *out = (*node)->member;
    free(*node);
    *node = next;
    set->num_elements--;
    set->mod_ops++;
    return out;
}

void Set_map(T set, void apply(const void *member, void *cl), void *cl)
{
    assert(set && apply);
    unsigned start_mod_ops = set->mod_ops;
    for (int i = 0; i < set->num_buckets; i++)
        for (struct node *node = set->buckets[i]; node; node = node->next)
        {
            apply(node->member, cl);
            assert(set->mod_ops == start_mod_ops);
        }
}

void **Set_toArray(T set, void *end)
{
    assert(set);
    void **arr;
    arr = malloc(sizeof(*arr) * (set->num_elements + 1));
    assert(arr);
    int arr_idx = 0;
    for (int i = 0; i < set->num_buckets; i++)
        for (struct node *node = set->buckets[i]; node; node = node->next)
            arr[arr_idx++] = node->member;
    arr[arr_idx] = end;
    return arr;
}

T Set_union(T s, T t)
{
    check_sets_for_set_ops(s, t);
    T out = Set_new((s ? s->num_elements : 0) + (t ? t->num_elements : 0), s ? s->cmp : t->cmp, s ? s->hash : t->hash);
    struct node *itr;
    if (s)
    {
        for (int i = 0; i < s->num_buckets; i++)
            for (itr = s->buckets[i]; itr; itr = itr->next)
                push_node(itr->member, out->buckets + hash(out, itr->member));
        out->num_elements = s->num_elements;
    }
    if (t)
        for (int i = 0; i < t->num_buckets; i++)
            for (itr = t->buckets[i]; itr; itr = itr->next)
                Set_put(out, itr->member);
    return out;
}

T Set_inter(T s, T t)
{
    check_sets_for_set_ops(s, t);
    if (s == NULL || t == NULL)
        return Set_new(EMPTY_SET_HINT, s ? s->cmp : t->cmp, s ? s->hash : t->hash);
    T out = Set_new(s->num_elements < t->num_elements ? s->num_elements : t->num_elements, s->cmp, s->hash);
    struct node *itr;
    for (int i = 0; i < s->num_buckets; i++)
        for (itr = s->buckets[i]; itr; itr = itr->next)
            if (Set_member(t, itr->member))
            {
                push_node(itr->member, out->buckets + hash(out, itr->member));
                out->num_elements++;
            }
    return out;
}

T Set_minus(T s, T t)
{
    check_sets_for_set_ops(s, t);
    if (s == NULL)
        return Set_new(EMPTY_SET_HINT, t->cmp, t->hash);
    T out = Set_new(s->num_elements, s->cmp, s->hash);
    struct node *itr;
    for (int i = 0; i < s->num_buckets; i++)
        for (itr = s->buckets[i]; itr; itr = itr->next)
            if (t == NULL || Set_member(t, itr->member) == 0)
            {
                push_node(itr->member, out->buckets + hash(out, itr->member));
                out->num_elements++;
            }

    return out;
}

T Set_diff(T s, T t)
{
    check_sets_for_set_ops(s, t);
    T out = Set_new((s ? s->num_elements : 0) + (t ? t->num_elements : 0), s ? s->cmp : t->cmp, s ? s->hash : t->hash);
    struct node *itr;
    if (s)
        for (int i = 0; i < s->num_buckets; i++)
            for (itr = s->buckets[i]; itr; itr = itr->next)
                if (t == NULL || Set_member(t, itr->member) == 0)
                {
                    push_node(itr->member, out->buckets + hash(out, itr->member));
                    out->num_elements++;
                }

    if (t)
        for (int i = 0; i < t->num_buckets; i++)
            for (itr = t->buckets[i]; itr; itr = itr->next)
                if (s == NULL || Set_member(s, itr->member) == 0)
                {
                    push_node(itr->member, out->buckets + hash(out, itr->member));
                    out->num_elements++;
                }

    return out;
}
