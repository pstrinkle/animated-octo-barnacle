

#ifndef _BINARYSEARCHTREE_H
#define _BINARYSEARCHTREE_H

struct node;

typedef struct node {
    struct node *left;
    struct node *right;
    struct node *parent;
    int value;
} node_t;

/* exposed so you can allocate the root for now, later, will disappear. */
node_t *newnode(int value, node_t *parent);

node_t *search(node_t *node, int value);
void insert(node_t *node, int value);
/* XXX: maybe I should use a tree context to hold things like root, etc. */
void delete(node_t *root, node_t *node, int value);
void depthFirstFree(node_t *node);
void depthFirstPrint(node_t *node);

#endif
