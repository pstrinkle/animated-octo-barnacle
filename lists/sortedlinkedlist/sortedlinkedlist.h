

#ifndef _SORTEDLINKEDLIST_H
#define _SORTEDLINKEDLIST_H

struct item;

struct item {
    struct item *next;
    /* Could be void * for anything or use an enum for variation with a flag.
     */
    int value;
};


void freelist(struct item *head);
void insert(int value);
struct item *pop(void);
void printList(struct item *head);

#endif
