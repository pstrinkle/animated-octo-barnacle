

#include <stdlib.h>
#include <stdio.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

struct node;

typedef struct node {
    struct node *left;
    struct node *right;
    struct node *parent;
    int value;
} node_t;

/* so it can be re-pointed more easily */
static node_t *root = NULL;

static node_t *newnode(int value, node_t *parent)
{
    node_t *t = malloc(sizeof(node_t));
    //NULLCHECK
    t->value = value;
    t->left = NULL;
    t->right = NULL;

    /* adding simplification of parent pointer; in the python it required
     * passing around an extra parameter, but I decided to do the C differently
     */
    t->parent = parent;
    return t;
}

static node_t *duplicatenode(node_t *node) {
    node_t *t = newnode(node->value, node->parent);
    t->left = node->left;
    t->right = node->right;

    return t;
}

static node_t *search(node_t *node, int value)
{
    if (node == NULL) {
        return node;
    }

    if (node->value == value) {
        return node;
    }
    if (node->value < value) {
        return search(node->right, value);
    } else {
        return search(node->left, value);
    }
}

static void insert(node_t *node, int value)
{
    /* prevent duplicates in this version. */
    if (node->value == value) {
        return;
    }

    if (node->value > value) {
        if (node->left == NULL) {
            node->left = newnode(value, node);
        } else {
            insert(node->left, value);
        }

        return;
    } else {
        if (node->right == NULL) {
            node->right = newnode(value, node);
        } else {
            insert(node->right, value);
        }

        return;
    }
}

static node_t *findmin(node_t *node) {
    if (node->left == NULL) {
        return node;
    }
    return findmin(node->left);
}

static node_t *findmax(node_t *node) {
    if (node->right == NULL) {
        return node;
    }
    return findmax(node->right);
}

static void delete(node_t *node, int value)
{
    node_t *replacement, *parent;
    int foundLeft = 0;

    if (node == NULL) {
        return;
    }

    if (node->value != value) {
        if (node->value > value) {
            return delete(node->left, value);
        } else {
            return delete(node->right, value);
        }
    }

    /* if we're here, then it's a match. */

    if (node->parent == NULL) {
        parent = root;
    } else {
        parent = node->parent;
    }

    /* we're a boring leaf. */
    if (node->left == NULL && node->right == NULL) {
        if (parent->left == node) {
            parent->left = NULL;
        } else {
            parent->right = NULL;
        }
        free(node);
        return;
    }

    /* we're not a boring leaf! */
    if (node->left == NULL) {
        /* ok, so there is nothing to the left, so we need to find the minimum
         * value to the right.
         */
        replacement = findmin(node->right);

    } else {
        /* ok, so there isn nothing to the right, so we need to find the
         * maximum value to the left.
         */
        replacement = findmax(node->left);
        foundLeft = 1;
        /* just so I know whether the replacement was found to the right or
         * left -- could just reject if node->left/right was NULL but I feel
         * this is cleaner.
         */
    }

    /* I'm this node's immediate parent; which simplifies things. */
    if (node == replacement->parent) {
        if (parent->left == node) {
            parent->left = replacement;
        } else {
            parent->right = replacement;
        }

        /* if it's being moved up-right or up-left it needs to continue the
         * chain.
         */
        if (foundLeft) {
            replacement->right = node->right;
        } else {
            replacement->left = node->left;
        }

        free(node);
        return;
    }

    /* if we got here then the replacement wasn't immediately our child, so
     * this is the more normal case.
     */
    node_t *copy = duplicatenode(replacement);

    delete(node, replacement->value);

    /* fully transplant it; by calling delete here, there are some convenient
     * things we can know based on the possible graphs that lead here, :D */
    copy->left = node->left;
    copy->right = node->right;

    if (parent->left == node) {
        parent->left = copy;
    } else {
        parent->right = copy;
    }

    free(node);
    return;
}

static void depthFirstFree(node_t *node)
{
    if (node == NULL) {
        return;
    }

    if (node->left != NULL) {
        depthFirstFree(node->left);
    }
    if (node->right != NULL) {
        depthFirstFree(node->right);
    }

    free(node);
    return;
}

static void depthFirstPrint(node_t *node)
{
    if (node == NULL) {
        return;
    }

    printf("node: %d ", node->value);
    if (node->left) {
        printf("%d.left: %d ", node->value, node->left->value);
    } else {
        printf("%d.left: None ", node->value);
    }

    if (node->right) {
        printf("%d.right: %d ", node->value, node->right->value);
    } else {
        printf("%d.right: None ", node->value);
    }

    printf("\n");

    depthFirstPrint(node->left);
    depthFirstPrint(node->right);
}

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

    delete(root, 8);

    depthFirstPrint(root);

    depthFirstFree(root);

    return 0;
}
