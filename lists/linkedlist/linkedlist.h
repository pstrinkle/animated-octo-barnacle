
#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

struct item;

struct item {
    struct item *next;
    /* Could be void * for anything or use an enum for variation with a flag.
     */
    int value;
};

void freelist(struct item *head);
void push(int value);
void printList(struct item *head);

#endif
