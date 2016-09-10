
#include <stdlib.h>
#include <stdio.h>

#include "linkedlist.h"

struct item *head = NULL;

static struct item *newitem(int value)
{
    struct item *n = malloc(sizeof(struct item));
    n->next = NULL;
    n->value = value;

    return n;
}

void freelist(struct item *head)
{
    struct item *curr, *next;

    if (head == NULL) {
        return;
    }

    curr = head;
    while (curr) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    return;
}

/* if sorted insert, then O(n), otherwise constant time :) */
void push(int value)
{
    struct item *n = newitem(value);
    n->next = head; /* head is just a pointer. */
    head = n;
    return;
}

/* constant time pop */
static struct item *pop(void)
{
    if (head == NULL) {
        return NULL;
    }

    struct item *n = head;
    head = n->next;
    return n;
}

void printList(struct item *head)
{
    struct item *curr = head;
    while (curr) {
        printf("%d ", curr->value);
        curr = curr->next;
    }

    printf("\n");
}
