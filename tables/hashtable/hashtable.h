

#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#define SMALL_TABLE 8

/* m (table size) == 2**r; maybe make variable */
#define TABLE_POWER 16

struct hashtable; /* forward declare. */

typedef int (*hash_t)(struct hashtable *t, int k);

typedef struct list {
    struct list *next;
    int value;
} list_t;

struct hashtable {
    int n; /* number of filled slots. */
    int m; /* size of table. */
    int g; /* n that triggers growth. */
    int s; /* n that triggers shrink. */
    list_t **table; /* the table. */
    hash_t hash; /* the method. */
};

void buildhashtable(struct hashtable *table, int m);
int search(struct hashtable *table, int value);
void insert(struct hashtable *table, int value);
void delete(struct hashtable *table, int value);
void freetable(struct hashtable *table);

#endif
