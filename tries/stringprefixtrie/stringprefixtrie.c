/* prefix compression is do-able here, but not my primary goal. */
/* XXX: This was designed to be lowercase only. :P */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

#define TRUE 1
#define FALSE 0

#define ALPHABET_SIZE 26

struct node;

typedef struct node {
    struct node *children[ALPHABET_SIZE];
} node_t;

static node_t *root = NULL;

static node_t *
newNode(void)
{
    node_t *n = malloc(sizeof(node_t));
    memset(n, 0x00, sizeof(node_t));

    return n;
}

static int
search(const char *key, int len)
{
    return FALSE;
}

static void
insert(const char *key, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        char let = key[i] - 'a';
    }

    return;
}

static void
depthFirstFree(node_t *node)
{
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            depthFirstFree(node->children[i]);
        }
    }

    free(node);

    return;
}

int main(void)
{
    root = newNode();

    depthFirstFree(root);

    return 0;
}
