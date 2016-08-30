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

/* a trie node for this somewhat inefficient trie. */
typedef struct node {
    /* the children pointers */
    struct node *children[ALPHABET_SIZE];
    /* Is this an end node? Consider: 'con', 'consider' you need to track that
     * 'con' is in the trie as well as 'consider'
     */
    int last;
} node_t;

static node_t *
newNode(void)
{
    node_t *n = malloc(sizeof(node_t));
    memset(n, 0x00, sizeof(node_t));

    return n;
}

static int
search(node_t *root, const char *key, int len)
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

static void
insert(node_t *root, const char *key, int len)
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
    int i;
    node_t *root = NULL;
    root = newNode();

    const char *words[] = {"asdf", "asde", "as", "tea", "ted", "ten", "qwerty",
            "qwe", "what", "how"};

    for (i = 0; i < NUM_ELEMENTS(words); i++) {
        printf("%s\n", words[i]);
        insert(root, words[i], strlen(words[i]));
        assert(TRUE == search(root, words[i], strlen(words[i])));
    }

    //const char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    //insert(root, alphabet, strlen(alphabet));
    //assert(TRUE == search(root, alphabet, strlen(alphabet)));

    depthFirstFree(root);

    return 0;
}
