
#include <stdlib.h>
#include <stdio.h>

#include "sortedlinkedlist.h"

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
void insert(int value)
{
    struct item *curr, *prev;
    struct item *n = newitem(value);

    /* add to head. */
    if (head == NULL) {
        head = n;
    } else {
        curr = head;
        prev = NULL;

        /* add to middle. */
        while (curr) {
            if (curr->value >= value) {
                prev->next = n;
                n->next = curr;
                break;
            }

            prev = curr;
            curr = curr->next;
        }

        /* add to last. */
        if (NULL == curr) {
            prev->next = n;
        }

    }

    return;
}

/* constant time pop */
struct item *pop(void)
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


