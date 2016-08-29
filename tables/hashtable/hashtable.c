/* 
 * Deletion constant, when to shrink the table must be less than when
 * we grow, preferrably shrink when 75% empty, grow when 50% full (revisit).
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

struct hashtable; /* forward declare. */

typedef int (*hash_t)(struct hashtable *t, int k);

typedef struct list {
    struct list *next;
    int value;
} list_t;

struct hashtable {
    int n; /* number of filled slots. */
    int m; /* size of table. */
    list_t **table; /* the table. */
    hash_t hash; /* the method. */
};

/* division */
static int
division(struct hashtable *table, int key)
{
    /* 
     * Doesn't work well if both are even, and we can do better
     */
    return key % table->m;
}

/* m (table size) == 2**r; maybe make variable */
#define TABLE_POWER 16

#if 0 /* revisit when done with design. */
#define ARCH_BITS (sizeof(int) * 8)
#define POWER (1 << ARCH_BITS)

/* random integer within w bits, selected ahead of time, must be odd */
static int a = 0;

static void mulsetup() {
    a = random();
}

/* table size is power of 2, 2**r */
static int
multiplication(struct hashtable *table, int key) 
{
    int i = (a * key);
    int v = (i % POWER) >> (ARCH_BITS - TABLE_POWER);
    return v;
}
#endif

static void
buildhashtable(struct hashtable *table)
{
    int size = sizeof(list_t *) * table->m;

    table->table = malloc(size);
    assert(table->table != NULL);

    memset(table->table, 0x00, size);

    return;
}

/* Is the key in the table? */
static int
search(struct hashtable *table, int value)
{
    int found = 0;
    int slot = table->hash(table, value);
    list_t *curr = table->table[slot];

    while (curr && !found) {
        if (curr->value == value) {
            found++;
        } else {
            curr = curr->next;
        } /* could improve by sorted linkedlist, but meh, these should be shallow. */
    }

    return found;
}

static void
insert(struct hashtable *table, int value)
{
    list_t *prev = NULL;

    int found = search(table, value);
    if (found) {
        return;
    }

    int slot = table->hash(table, value);
    list_t *curr = table->table[slot];

    while (curr) {
        prev = curr;
        curr = curr->next;
    }

    /* curr is NULL, prev points to the previous entry. */
    curr = malloc(sizeof(list_t));
    curr->value = value;
    curr->next = NULL;

    if (prev == NULL) {
        /* 0th entry in the slot */
        table->table[slot] = curr;
    } else {
        prev->next = curr;
    }

    table->n++;

    return;
}

static void
freetable(struct hashtable *table)
{
    int i;
    list_t *curr, *next;

    for (i = 0; i < table->m; i++) {
        curr = table->table[i];
        while (curr) {
            next = curr->next;
            free(curr);
            curr = next;
        }
    }

    free(table->table);
}

int main(void)
{
    int i;
    int input[] = {1, 4, 6, 100, 1000, 234, 12312, 1435, 166, 132409, 111111, 12, 0, 1};

    struct hashtable ht = {
        .n = 0,
        .m = (1 << TABLE_POWER),
        .table = NULL,
        .hash = division
    };

    buildhashtable(&ht);

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        insert(&ht, input[i]);
        assert(search(&ht, input[i]) == 1);
    }

    /* minus 1 because 1 is duplicated. */
    assert(ht.n == NUM_ELEMENTS(input)-1);

    freetable(&ht);

    return 0;
}
