
#include <stdlib.h>
#include <stdio.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

struct item;

struct item {
    struct item *next;
    /* Could be void * for anything or use an enum for variation with a flag.
     */
    int value;
};

struct item *head = NULL;

static struct item *newitem(int value)
{
    struct item *n = malloc(sizeof(struct item));
    n->next = NULL;
    n->value = value;

    return n;
}

static void freelist(struct item *head)
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
static void insert(int value)
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
static struct item *pop(void)
{
    if (head == NULL) {
        return NULL;
    }

    struct item *n = head;
    head = n->next;
    return n;
}

static void printList(struct item *head)
{
    struct item *curr = head;
    while (curr) {
        printf("%d ", curr->value);
        curr = curr->next;
    }

    printf("\n");
}

int main(void)
{
    int i;
    int input[] = {0, 3, 8, 6, 4, 10, 9, 15, 11};

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", input[i]);
    }
    printf("\n");

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        insert(input[i]);
    }

    printList(head);

    freelist(head);

    return 0;
}
