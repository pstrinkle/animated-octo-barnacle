
#ifndef _STRINGPREFIXTRIE_H
#define _STRINGPREFIXTRIE_H

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

node_t *newNode(void);
int search(node_t *root, const char *key, int len);
void insert(node_t *root, const char *key, int len);
void depthFirstFree(node_t *node);

#endif
