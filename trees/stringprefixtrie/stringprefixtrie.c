/* prefix compression is do-able here, but not my primary goal. */
/* XXX: This was designed to be lowercase only. :P */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stringprefixtrie.h"

node_t *newNode(void)
{
    node_t *n = malloc(sizeof(node_t));
    memset(n, 0x00, sizeof(node_t));

    return n;
}

int search(node_t *root, const char *key, int len)
{
    node_t *node = root;
    int i;

    for (i = 0; i < len; i++) {
        int let = key[i] - 'a';
        /* That letter existed. */
        if (NULL == node->children[let]) {
            return FALSE;
        }

        /* If this is the last letter, check if this node terminates. */
        if (i == len-1) {
            if (node->last) {
                return TRUE;
            }
        }

        /* Move down the trie. */
        node = node->children[let];
    }

    return FALSE;
}

void insert(node_t *root, const char *key, int len)
{
    node_t *node = root;
    int i;

    for (i = 0; i < len; i++) {
        int let = key[i] - 'a';
        /* Check if that letter exists. */
        if (node->children[let]) {
            //
        } else {
            node->children[let] = newNode();
        }

        if (i == len-1) {
            node->last = 1; /* this node terminates. */
        }

        node = node->children[let];
    }

    return;
}

void depthFirstFree(node_t *node)
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


