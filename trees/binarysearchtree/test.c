
#include <stdlib.h>
#include <stdio.h>

#include "binarysearchtree.h"

/* common, will need to avoid redefining */
#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

/* so it can be re-pointed more easily */
static node_t *root = NULL;

int main(void)
{
    int i;
    int input[] = {0, 3, 8, 6, 4, 10, 9, 15, 11};

    /* set the parent to NULL because it is. */
    root = newnode(0, NULL);

    /* skip over root. */
    for (i = 1; i < NUM_ELEMENTS(input); i++) {
        insert(root, input[i]);
    }

    depthFirstPrint(root);

    node_t *s = search(root, 4);
    printf("s->value: %d\n", s->value);

    delete(root, root, 8);

    depthFirstPrint(root);

    depthFirstFree(root);

    return 0;
}

