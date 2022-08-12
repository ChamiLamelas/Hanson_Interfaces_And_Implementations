#include "table.h"
#include <stdlib.h>
#include <assert.h>

#define T Table_T

static const double LOAD_FACTOR_THRESHOLD = 0.5;

struct T
{
    unsigned num_elements;
    unsigned num_buckets;
    unsigned mod_ops;
    int (*cmp)(const void *key1, const void *key2);
    unsigned (*hash)(const void *key);
    struct node
    {
        struct node *next;
        // Not marking a member as const since this node type is
        // hidden from client via opaque pointer, can't find
        // anything online about modifying const variables in struct
        // did find that this is considered bad practice:
        // https://softwareengineering.stackexchange.com/a/222461
        void *key;
        void *value;
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

static unsigned atom_hash(const void *key)
{
    return ((size_t)key) * 2654435761;
}

// Designed to act like regular cmps (0 when equal, != 0 when not equal)
static int atom_cmp(const void *key1, const void *key2)
{
    return key1 != key2;
}

// hash into table
static unsigned hash(T table, void *key)
{
    return table->hash(key) % table->num_buckets;
}

static struct node **find(T table, void *key)
{
    // Get pointer to node pointer containing key or pointer to
    // the next of the last node pointer in the bucket
    // for struct node **x, &x[h] = &(*(x+h)) = x + h
    // I just do x + h versus what's given in the text
    struct node **itr = table->buckets + hash(table, key);
    for (; *itr && table->cmp((*itr)->key, key) != 0; itr = &((*itr)->next))
        ;
    return itr;
}

// push node on top of a bucket
static void push_node(void *key, void *value, struct node **bucket)
{
    struct node *new_node;
    new_node = malloc(sizeof(*new_node));
    assert(new_node);
    new_node->key = key;
    new_node->value = value;
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

static void auto_rehash(T table)
{
    if (table->num_elements < LOAD_FACTOR_THRESHOLD * table->num_buckets)
        return;

    struct node **cpy = table->buckets;
    // update table buckets so hash works properly
    unsigned old_num_buckets = table->num_buckets;
    table->num_buckets *= 2;
    table->buckets = allocate_buckets(table->num_buckets);
    struct node *next;
    for (int i = 0; i < old_num_buckets; i++)
        for (; cpy[i]; cpy[i] = next)
        {
            // push onto the appropriate bucket and free from old buckets
            push_node(cpy[i]->key, cpy[i]->value, table->buckets + hash(table, cpy[i]->key));
            next = cpy[i]->next;
            free(cpy[i]);
        }
    free(cpy);
}

T Table_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *key))
{
    assert(hint >= 0);
    unsigned num_buckets = next_prime(hint);
    T table;
    table = malloc(sizeof(*table));
    assert(table);
    table->cmp = cmp ? cmp : atom_cmp;
    table->hash = hash ? hash : atom_hash;
    table->num_elements = 0;
    table->num_buckets = num_buckets;
    table->mod_ops = 0;
    // buckets are not allocated as part of table struct allocation
    // because buckets must be freed during rehashing
    table->buckets = allocate_buckets(num_buckets);
    return table;
}

void Table_free(T *table)
{
    assert(table && *table);
    struct node *next;
    for (int i = 0; i < (*table)->num_buckets; i++)
        for (; (*table)->buckets[i]; (*table)->buckets[i] = next)
        {
            next = (*table)->buckets[i]->next;
            free((*table)->buckets[i]);
        }
    free((*table)->buckets);
    free(*table);
    *table = NULL;
}

int Table_length(T table)
{
    assert(table);
    return table->num_elements;
}

void *Table_put(T table, void *key, void *value)
{
    assert(table);
    auto_rehash(table);
    // will store bucket to push onto
    struct node **node = find(table, key);
    void *out_value = NULL;
    // overwrite if find yielded result
    if (*node)
    {
        out_value = (*node)->value;
        (*node)->value = value;
    }
    else
    {
        // push onto bucket, note here to get the pointer
        // to the pointer that heads a bucket (so that bucket
        // head can be updated) we add to a node** to get
        // a node**
        push_node(key, value, table->buckets + hash(table, key));
        table->num_elements++;
    }
    table->mod_ops++;
    return out_value;
}

void *Table_get(T table, void *key)
{
    assert(table);
    struct node **node = find(table, key);
    return *node ? (*node)->value : NULL;
}

void *Table_remove(T table, void *key)
{
    assert(table);
    struct node **node = find(table, key);
    if (*node == NULL)
        return NULL;
    struct node *next = (*node)->next;
    void *value = (*node)->value;
    free(*node);
    *node = next;
    table->num_elements--;
    table->mod_ops++;
    return value;
}

void Table_map(T table, void apply(void *key, void **value, void *cl), void *cl)
{
    assert(table && apply);
    unsigned start_mod_ops = table->mod_ops;
    for (int i = 0; i < table->num_buckets; i++)
        for (struct node *node = table->buckets[i]; node; node = node->next)
        {
            apply(node->key, &node->value, cl);
            assert(table->mod_ops == start_mod_ops);
        }
}

void **Table_toArray(T table, void *end)
{
    assert(table);
    void **arr;
    arr = malloc(sizeof(*arr) * ((table->num_elements * 2) + 1));
    assert(arr);
    int arr_idx = 0;
    for (int i = 0; i < table->num_buckets; i++)
        for (struct node *node = table->buckets[i]; node; node = node->next)
        {
            arr[arr_idx++] = node->key;
            arr[arr_idx++] = node->value;
        }
    arr[arr_idx] = end;
    return arr;
}