
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "btree.h"

/******************************************************************************
 * Globals
 *****************************************************************************/

extern int counter;
extern block_t *blockStorage[128];

/******************************************************************************
 * Macros
 *****************************************************************************/

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

/******************************************************************************
 * Debug Macros
 *****************************************************************************/

#define VERIFY_BLOCK(ID, VALUES) \
    test_verifyBlock((ID), (VALUES), NUM_ELEMENTS((VALUES)))

#define VERIFY_BLOCK2(ID, ...) \
    do { \
        int X[] = {__VA_ARGS__}; \
        test_verifyBlock((ID), X, NUM_ELEMENTS(X)); \
    } while (0)

#define VERIFY_DELETE(ROOT, INPUT, X) \
    test_verifyDeletion((ROOT), (INPUT), NUM_ELEMENTS((INPUT)), (X));

/******************************************************************************
 * Test code start
 *****************************************************************************/

typedef void (*test_ptr_t)(void);

static block_t *test_buildTree(block_t *root, int *input, int count);
/* deleted is the value you expect to be missing, not the index in input */
static void test_verifyDeletion(block_t *root, int *input, int count, int deleted);
static void test_verifyTree(block_t *root, int *input, int count);
static int test_verifyBlock(int block, int *values, int count);

static block_t *test_buildTree(block_t *root, int *input, int count)
{
    int i;

    root = newBlock();
    assert(root);

    for (i = 0; i < count; i++) {
        insert(root, input[i]);
    }

    test_verifyTree(root, input, count);

    return root;
}

static void test_verifyDeletion(block_t *root, int *input, int count, int deleted)
{
    int i;
    block_t *f;

    for (i = 0; i < count; i++) {
        f = search(root, input[i]);
        if (input[i] == deleted) {
            assert(NULL == f);
        } else {
            assert(NULL != f);
        }
    }
}

static void test_verifyTree(block_t *root, int *input, int count)
{
    int i;
    block_t *f;

    for (i = 0; i < count; i++) {
        f = search(root, input[i]);
        assert(NULL != f);
    }
}

static int test_verifyBlock(int block, int *values, int count)
{
    int i, ret;
    block_t *blk = blockStorage[block];

    if (blk->used != count) {
        ret = 0;
        goto check;
    }

    for (i = 0; i < blk->used; i++) {
        if (blk->keys[i].key != values[i]) {
            ret = 0;
            goto check;
        }
    }

    /* It matched keys.  we don't need to deliberately check pointers yet. */
    ret = 1;

check:
    assert(ret == 1);
    return ret;
}

/*
 * Create the following tree.
 *
 * Inserting 9-2, then insert 1 to create the other tree.
 *
 *     |6|                   |6|
 *    /       \             /         \
 *   |4|      |8|     =>   |2|4|      |8|
 *  /   \     /  \        /   \  \    /  \
 * |2|3| |5| |7| |9|     |1| |3| |5| |7| |9|
 *
 */
static void test_insertDescending1(void)
{
    int input[] = {9, 8, 7, 6, 5, 4, 3, 2};
    block_t *root = NULL;

    printf("testing insert descending\n");

    {
        int i;

        root = newBlock();
        assert(root);

        for (i = 0; i < NUM_ELEMENTS(input); i++) {
            insert(root, input[i]);
            fprintf(stderr, "\nafter: %d", input[i]);
            printTree("\n", root);
        }
    }

    //root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    //insert(root, 1);

    depthFirstFree(root);
}

/*
 * Boring building a tree by monotonically increasing keys, which always
 * appends to the far right of the tree.
 */
static void test_insertBalance(void)
{
    int i;
    int input[50];
    block_t *root = NULL;

    printf("testing insert with double split\n");

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        input[i] = i + 1;
    }

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printf("\n\n");
    depthFirstPrint(root);

    depthFirstFree(root);

    return;
}

/*
 * This is leaf case: boring.
 *
 *     |2|             |2|
 *    /   \      =>   /   \
 *   |1|  |3|4|      |1|  |4|
 *
 * Deleting 3.  The node isn't left empty, so meh.
 *
 * To create this, we did insert: 1-4
 */
static void test_deleteLeafFirstSimple(void)
{
    int input[] = {1, 2, 3, 4};
    block_t *root = NULL;

    printf("testing delete leaf basic\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 3);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 3);

        VERIFY_BLOCK2(0, 2);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 4);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf case: boring.
 *
 *     |2|             |2|
 *    /   \      =>   /   \
 *   |1|  |3|4|      |1|  |3|
 *
 * Deleting 4.  The node isn't left empty, so meh.
 *
 * To create this, we did insert: 1-4
 */
static void test_deleteLeafEndSimple(void)
{
    int input[] = {1, 2, 3, 4};
    block_t *root = NULL;

    printf("testing delete leaf basic\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 4);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 4);

        VERIFY_BLOCK2(0, 2);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 2.
 *
 *      |4|                     |4|
 *    /      \                /      \
 *   |2|     |6|       =>    |2|     |7|
 *  /  \    /   \           /  \    /   \
 * |1| |3| |5|  |7|8|      |1| |3| |6|  |8|
 *
 * Deleting 5, we rotate left because the right sibling has sufficient keys.
 *
 * To create this, we did insert: 1-8.
 */
static void test_deleteCase2(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8};
    block_t *root = NULL;

    printf("testing delete leaf case 2\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 5);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 5);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 6);
        VERIFY_BLOCK2(4, 8);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 7);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 3.
 *
 *      |4|                   |4|
 *    /      \              /      \
 *   |2|     |8|     =>    |2|     |6|
 *  /  \    /    \        /  \    /   \
 * |1| |3| |5|6| |9|     |1| |3| |5| |8|
 *
 * Deleting 9, we rotate right because the left sibling has sufficient keys.
 *
 * To create this, we did insert: 1-5, 8, 9, 6.
 */
static void test_deleteCase3(void)
{
    int input[] = {1, 2, 3, 4, 5, 8, 9, 6};
    block_t *root = NULL;

    printf("testing delete leaf case 3\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 9);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 9);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 8);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 6);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 4a.
 *
 * In this case, the parent has more than one key, but all of the siblings have
 * one key each.
 *
 *      |4|                       |4|
 *    /       \                 /     \
 *   |2|      |6|8|      =>    |2|     |6|
 *  /  \     /  \  \          /  \    /  \
 * |1| |3| |5| |7| |9|       |1| |3| |5| |7|8|
 *
 * In this case, what happens when 9 is deleted?
 *
 * To create this, we did insert: 1-9
 */
static void test_deleteCase4a(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    block_t *root = NULL;

    printf("testing delete leaf case 4a\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 9);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 9);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 7, 8);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 6);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 4b.
 *
 * In this case, the parent has more than one key, but all of the siblings have
 * one key each.
 *
 *      |4|                       |4|
 *    /       \                 /     \
 *   |2|      |6|8|      =>    |2|     |8|
 *  /  \     /  \  \          /  \    /   \
 * |1| |3| |5| |7| |9|       |1| |3| |5|6| |9|
 *
 * In this case, what happens when 7 is deleted?
 *
 * To create this, we did insert: 1-9
 */
static void test_deleteCase4b(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    block_t *root = NULL;

    printf("testing delete leaf case 4b\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 7);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 7);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5, 6);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 8);
        VERIFY_BLOCK2(7, 9);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 4c.
 *
 * In this case, the parent has more than one key, but all of the siblings have
 * one key each.
 *
 *      |4|                       |4|
 *    /       \                 /     \
 *   |2|      |6|8|      =>    |2|     |8|
 *  /  \     /  \  \          /  \    /   \
 * |1| |3| |5| |7| |9|       |1| |3| |6|7| |9|
 *
 * In this case, what happens when 5 is deleted?
 *
 * To create this, we did insert: 1-9
 */
static void test_deleteCase4c(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    block_t *root = NULL;

    printf("testing delete leaf case 4c\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 5);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 5);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(4, 6, 7);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 8);
        VERIFY_BLOCK2(7, 9);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 5a.
 *
 *      |4|                        |4|
 *    /      \                   /      \
 *   |2|     |6|8|        =>    |2|     |6|9|
 *  /  \    /  \  \             /  \    /   \   \
 * |1| |3| |5| |7| |9|10|      |1| |3| |5|  |8| |10|
 *
 * Deleting 7, we rotate left because the right sibling has sufficient keys.
 *
 * To create this, we did insert: 1-10.
 */
static void test_deleteCase5a(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    block_t *root = NULL;

    printf("testing delete leaf case 5a\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 7);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 7);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 8);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 6, 9);
        VERIFY_BLOCK2(7, 10);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 5b.
 *
 *      |4|                        |4|
 *    /      \                   /      \
 *   |2|     |6|9|        =>    |2|     |7|9|
 *  /  \    /  \     \          /  \    /   \   \
 * |1| |3| |5| |7|8| |11|      |1| |3| |6|  |8| |11|
 *
 * Deleting 5, we rotate left because the right sibling has sufficient keys.
 *
 * To create this, we did insert: 1-7, 9, 11, 8.
 */
static void test_deleteCase5b(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 9, 11, 8};
    block_t *root = NULL;

    printf("testing delete leaf case 5b\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 5);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 5);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 6);
        VERIFY_BLOCK2(4, 8);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 7, 9);
        VERIFY_BLOCK2(7, 11);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 6.
 *
 *      |4|                        |4|
 *    /      \                   /      \
 *   |2|     |6|9|        =>    |2|     |6|8|
 *  /  \    /  \     \         /  \    /   \   \
 * |1| |3| |5| |7|8| |10|     |1| |3| |5|  |7| |9|
 *
 * Deleting 10, we rotate right because the left sibling has sufficient keys.
 *
 * To create this, we did insert: 1-7, 10, 9, 8
 */
static void test_deleteCase6(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 10, 9, 8};
    block_t *root = NULL;

    printf("testing delete leaf case 6\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 10);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 10);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 7);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 6, 8);
        VERIFY_BLOCK2(7, 9);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 7.
 *
 *      |4|                           |4|
 *    /      \                      /      \
 *   |2|     |15|25|         =>    |2|     |14|25|
 *  /  \    /      \    \         /  \    /   \    \
 * |1| |3| |10|14| |20| |30|     |1| |3| |10| |15| |30|
 *
 * Deleting 20, we rotate right because the left sibling has sufficient keys.
 *
 * To create this, we did insert: 1-4, 10, 15, 20, 25, 30, 14
 */
static void test_deleteCase7(void)
{
    int input[] = {1, 2, 3, 4, 10, 15, 20, 25, 30, 14};
    block_t *root = NULL;

    printf("testing delete leaf case 7\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 20);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 20);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 10);
        VERIFY_BLOCK2(4, 15);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 14, 25);
        VERIFY_BLOCK2(7, 30);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 8.
 *
 *      |4|                              |4|
 *    /      \                         /      \
 *   |2|     |15|25|            =>    |2|     |14|25|
 *  /  \    /     \    \             /  \    /   \    \
 * |1| |3| |10|14| |20| |30|31|     |1| |3| |10| |15| |30|31|
 *
 * Deleting 20, we rotate right because the left sibling has sufficient keys.
 *
 * To create this, we did insert: 1-4, 10, 15, 20, 25, 30, 14, 31
 */
static void test_deleteCase8(void)
{
    int input[] = {1, 2, 3, 4, 10, 15, 20, 25, 30, 14, 31};
    block_t *root = NULL;

    printf("testing delete leaf case 8\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);

    delete(root, 20);

    printTree("\npost-delete:\n\n", root);

    // test it!
    {
        VERIFY_DELETE(root, input, 20);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 10);
        VERIFY_BLOCK2(4, 15);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 14, 25);
        VERIFY_BLOCK2(7, 30, 31);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 10a.
 *
 *       |4|8|                            |4|
 *     /    \      \                    /     \
 *   |2|     |6|    |10|        =>     |2|     |6|8|
 *  /  \    /   \   /   \             /  \    /  \  \
 * |1| |3| |5| |7| |9| |11|          |1| |3| |5| |7| |9|10|
 *
 * Delete 11.
 *
 * Deleting 11, pushes 10 into the block with 9, which leaves the previous
 * block empty.  We then push 8 into 6, and make them buddies.
 *
 * To create this, we did insert: 1-11
 */
static void test_deleteCase10a(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    block_t *root = NULL;

    printf("testing delete leaf case 10a\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);
    printf("\n");

    delete(root, 11);

    printTree("\npost-delete:\n\n", root);
    printf("\n");

    // test it!
    {
        VERIFY_DELETE(root, input, 11);

        VERIFY_BLOCK2(0, 4);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 7);
        VERIFY_BLOCK2(5, 2);
        VERIFY_BLOCK2(6, 6, 8);
        VERIFY_BLOCK2(7, 9, 10);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 10b.
 *
 *       |4|8|                            |8|
 *     /    \       \                   /          \
 *   |2|     |6|     |10|        =>    |2|4|        |10|
 *  /  \    /   \    /   \            /  \  \       /  \
 * |1| |3| |5| |7| |9| |11|         |1| |3| |5|6|  |9| |11|
 *
 * Delete 7.
 *
 * To create this, we did insert: 1-11
 */
static void test_deleteCase10b(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    block_t *root = NULL;

    printf("testing delete leaf case 10b\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);
    printf("\n");

    delete(root, 7);

    printTree("\npost-delete:\n\n", root);
    printf("\n");

    // test it!
    {
        VERIFY_DELETE(root, input, 7);

        VERIFY_BLOCK2(0, 8);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5, 6);
        VERIFY_BLOCK2(5, 2, 4);
        VERIFY_BLOCK2(7, 9);
        VERIFY_BLOCK2(8, 11);
        VERIFY_BLOCK2(9, 10);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 10c.
 *
 *       |4|8|                            |8|
 *     /    \       \                   /         \
 *   |2|     |6|     |10|        =>    |4|6|       |10|
 *  /  \    /   \    /   \            /   \  \     /  \
 * |1| |3| |5| |7| |9| |11|         |2|3| |5| |7| |9| |11|
 *
 * Delete 1.
 *
 * This is interesting because the code was written to handle right-side
 * deletions for the most part, and this is a left side. :D
 *
 * To create this, we did insert: 1-11
 */
static void test_deleteCase10c(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    block_t *root = NULL;

    printf("testing delete leaf case 10c\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);
    printf("\n");

    delete(root, 1);

    printTree("\npost-delete:\n\n", root);
    printf("\n");

    // test it!
    {
        VERIFY_DELETE(root, input, 1);

        VERIFY_BLOCK2(0, 8);
        VERIFY_BLOCK2(2, 2, 3);
        VERIFY_BLOCK2(3, 5);
        VERIFY_BLOCK2(4, 7);
        VERIFY_BLOCK2(6, 4, 6);
        VERIFY_BLOCK2(7, 9);
        VERIFY_BLOCK2(8, 11);
        VERIFY_BLOCK2(9, 10);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 10d.
 *
 *       |4|8|                            |4|
 *     /    \       \                   /       \
 *   |2|     |6|     |10|        =>    |2|      |8|10|
 *  /  \    /   \    /   \            /   \    /   \  \
 * |1| |3| |5| |7| |9| |11|          |1| |3| |6|7| |9| |11|
 *
 * Delete 5.
 *
 * This is interesting because the code was written to handle right-side
 * deletions for the most part, and this is a left side. :D
 *
 * To create this, we did insert: 1-11
 */
static void test_deleteCase10d(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    block_t *root = NULL;

    printf("testing delete leaf case 10d\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);
    printf("\n");

    delete(root, 5);

    printTree("\npost-delete:\n\n", root);
    printf("\n");

    // test it!
    {
        VERIFY_DELETE(root, input, 5);

        VERIFY_BLOCK2(0, 8);
        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(4, 6, 7);
        VERIFY_BLOCK2(5, 2, 4);
        VERIFY_BLOCK2(7, 9);
        VERIFY_BLOCK2(8, 11);
        VERIFY_BLOCK2(9, 10);
    }

    depthFirstFree(root);

    return;
}

/*
 * This is leaf delete case 11a.
 *
 *      |4|
 *     /    \
 *   |2|     |6|     =>   |2|4|
 *  /  \    /   \        /  \  \
 * |1| |3| |5| |7|      |1| |3| |5|6|
 *
 * Delete 7.
 *
 * This is interesting because the code was written to handle right-side
 * deletions for the most part, and this is a left side. :D
 *
 * To create this, we did insert: 1-11
 */
static void test_deleteCase11a(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7};
    block_t *root = NULL;

    printf("testing delete leaf case 11a\n");

    root = test_buildTree(root, input, NUM_ELEMENTS(input));

    printTree("\nfully-built:\n\n", root);
    printf("\n");

    delete(root, 7);

    printTree("\npost-delete:\n\n", root);
    printf("\n");

    // test it!
    {
        VERIFY_DELETE(root, input, 7);

        VERIFY_BLOCK2(1, 1);
        VERIFY_BLOCK2(2, 3);
        VERIFY_BLOCK2(3, 5, 6);
        VERIFY_BLOCK2(0, 2, 4); /* copied back into root block. */
    }

    depthFirstFree(root);

    return;
}

int main(void)
{
    int i;

    /*
     * Prevent innocuous code changes from breaking code that made reasonable
     * assumptions.
     */
    assert(80 == sizeof(block_t));
    assert(16 == offsetof(block_t, keys));

    test_ptr_t tests[] = {
            test_insertBalance,
            test_deleteLeafFirstSimple,
            test_deleteLeafEndSimple,
            test_deleteCase2,
            test_deleteCase3,
            test_deleteCase4a,
            test_deleteCase4b,
            test_deleteCase4c,
            test_deleteCase5a,
            test_deleteCase5b,
            test_deleteCase6,
            test_deleteCase7,
            test_deleteCase8,
            test_deleteCase10a,
            test_deleteCase10b,
            test_deleteCase10c,
            test_deleteCase10d,
            test_deleteCase11a,
            test_insertDescending1,
    };

    for (i = 0; i < NUM_ELEMENTS(tests); i++) {
        counter = 0; /* to make it easier on humans. */
        memset(blockStorage, 0x00, sizeof(blockStorage));
        printf("-----------------------------------------------------------\n");
        tests[i]();
    }

    return 0;
}
