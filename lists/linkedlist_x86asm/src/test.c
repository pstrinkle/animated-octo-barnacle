/*
 ============================================================================
 Name        : testing.c
 Author      : Patrick Trinkle
 Version     :
 Copyright   : Apache 2.0
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define SEVEN 7

struct item {
    struct item *next;
    int value;
};

/* external. */
int returnseven();
struct item *findvalue(struct item *head, int value);
struct item *allocate(int value);

#if 0
/* exposed. */
struct item *allocate(int value) {
    struct item *n = malloc(sizeof(struct item));
    n->next = NULL;
    n->value = value;

    return n;
}
#endif

/* here. (not a sorted list) */
static void prepend(struct item **head, int value);
static void dump(struct item *head);

int main(void) {
    int v;
    struct item *head = NULL;

    assert(sizeof(struct item) == 8);
    assert(sizeof(size_t) == 4);

    v = returnseven();
	if (SEVEN != v) {
	    fprintf(stderr, "Unexpected output received: %d\n", v);
	}

	prepend(&head, 1);
	prepend(&head, 2);
	prepend(&head, 3);

	dump(head);

	/* can my code find it? */
	struct item *it = findvalue(head, 2);
	assert(NULL != it && 2 == it->value);

	it = findvalue(head, 1);
	assert(NULL != it && 1 == it->value);

    it = findvalue(head, 3);
    assert(NULL != it && 3 == it->value);

    assert(NULL == findvalue(head, 4));

	return 0;
}

static void dump(struct item *head) {
    struct item *curr = head;
    while (NULL != curr) {
        fprintf(stderr, "value: %d\n", curr->value);
        curr = curr->next;
    }

    return;
}


static void
prepend(struct item **head, int value) {
    struct item *ins = allocate(value);

    /* if head is NULL, the list is empty. */
    if (NULL == *head) {
        *head = ins;
    } else {
        ins->next = *head;
        *head = ins;
    }
}
