/*
 * Just examples of walking a graph with different approaches to
 * depth-first search.  I'm using a binary search tree as the thing to walk.
 */

#include <stdlib.h>
#include <stdio.h>

#include "binarysearchtree.h"

static void preorderPrint(struct node *curr)
{
    if (!curr) {
        return;
    }

    /* pre-order visits the root, then left of the root, then right. */
    printf("%d\n", curr->value);
    if (curr->left) {
        preorderPrint(curr->left);
    }
    if (curr->right) {
        preorderPrint(curr->right);
    }
}

static void inorderPrint(struct node *curr)
{
    if (!curr) {
        return;
    }

    /* visits left of the root, then root, then right of the root. */
    if (curr->left) {
        inorderPrint(curr->left);
    }
    printf("%d\n", curr->value);
    if (curr->right) {
        inorderPrint(curr->right);
    }
}

static void postorderPrint(struct node *curr)
{
    if (!curr) {
        return;
    }

    /* post-order visits the left, then right, then root. */
    if (curr->left) {
        postorderPrint(curr->left);
    }
    if (curr->right) {
        postorderPrint(curr->right);
    }
    printf("%d\n", curr->value);
}


int main(void)
{
    return 0;
}
