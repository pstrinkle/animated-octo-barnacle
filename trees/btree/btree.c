/*
 * Basic implementation of a B-tree.
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************
 * Macros
 *****************************************************************************/
#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

/*
 * I'm just going to implement this to store up to three keys per block.
 */
#define NUM_KEYS 3

/******************************************************************************
 * Objects
 *****************************************************************************/
struct block;

typedef struct key {
    struct block *ptr; /* child block. */
    int key; /* the value. */
} key_t;

typedef struct block {
    int id; /* for humans. */
    int used;
    struct block *parent; /* back-link to make splitting and so on easier */
    key_t keys[NUM_KEYS+1]; /* so that there's a trailing pointer. */
} block_t;

/******************************************************************************
 * Program globals.
 *****************************************************************************/
static int counter = 0;

/******************************************************************************
 * Test code start
 *****************************************************************************/

/*
 * Test inserting nodes into the tree that trigger splits, and
 * multiple-recursive splits.
 */
static void test_insertBalance(void);
/*
 * Test building a simple tree, and deleting a key from a leaf that doesn't
 * leave it empty.
 */
static void test_deleteSimple(void);

/******************************************************************************
 * Implementation start
 *****************************************************************************/

/* create a new empty block. */
static block_t *newBlock(void);

/* insert a promoted key into the block. */
static void blockInsert(block_t *insert, key_t *promoted, block_t *newblk);
/* split a normal block. */
static void blockSplit(block_t *blk);
/* split the root block */
static void rootSplit(block_t *root);
/* insert the value into the tree starting at the specified block. */
static void insert(block_t *root, int value);
/* print one block. */
static void blockPrint(block_t *blk);
/* print each block in a dfs pattern. */
static void depthFirstPrint(block_t *blk);

static block_t *newBlock(void)
{
    block_t *b = malloc(sizeof(block_t));
    memset(b, 0x00, sizeof(block_t));
    b->id = counter++;

    return b;
}

/**
 * @return NULL if not found
 * @return block ptr if found.
 */
static block_t *search(block_t *blk, int value)
{
    block_t *where;
    key_t *curr;
    int i;
    int found = 0;

    where = blk;
    while (where && !found) {
        for (i = 0; i < where->used; i++) {
            curr = &(where->keys[i]);
            /* If it's less, we try going down the pointer. */
            if (value < curr->key) {
                if (curr->ptr) {
                    where = curr->ptr;
                    break;
                } else {
                    where = NULL;
                    break;
                }
            } else if (value == curr->key) {
                found = 1;
                break;
            } else {
                /* moving on? */

                if (i == (where->used-1)) {
                    /* We're greater than the last one. */
                    curr += 1;
                    if (curr->ptr) {
                        where = curr->ptr;
                        break;
                    } else {
                        /*
                         * If there's nothing beyond it, then it isn't there.
                         */
                        where = NULL;
                        break;
                    }
                }
            }
        }
    }

    if (!found) {
        return NULL;
    }

    return where;
}

static void blockInsert(block_t *insert, key_t *promoted, block_t *newblk)
{
    printf("blockInsert to blk: %d, key: %d, newblk: %d\n",
            insert->id,
            promoted->key,
            newblk->id);

    insert->keys[insert->used].key = promoted->key; /* i think this is good.. */

    insert->keys[insert->used+1].ptr = newblk;
    insert->used++;

    if (NUM_KEYS == insert->used) {
        printf("need to split %d\n", insert->id);

        if (NULL == insert->parent) {
            printf("need to split root variant\n");
            return rootSplit(insert);
        } else {
            printf("need to call normal block split\n");
            return blockSplit(insert);
        }
    }

    /*
     * This could need to be split; if so, I believe the promoted item changes.
     */

    return;
}

static void blockSplit(block_t *blk)
{
    /* We've received blk, which is full, and we need to split it. */

    int i;
    printf("blockSplit on %d\n", blk->id);
    blockPrint(blk);

    /*
     * XXX: This code is for splitting a leaf at present but I can likely make
     * it more generic by some more bookkeeping. :D
     */

    int middleIndex = NUM_KEYS / 2;
    key_t *middle = &(blk->keys[middleIndex]); /* this guy will be promoted. */

    /* can make generic. */
    key_t middleSave;
    middleSave.ptr = middle->ptr;
    key_t promote;
    promote.key = middle->key;

    /* these will be copied into a new block. */
    key_t *rightStart = middle + 1;
    key_t *end = &(blk->keys[blk->used+1]);
    size_t rightSize = (size_t)end - (size_t)rightStart;

    block_t *newRight = newBlock();
    key_t *newRightStart = &(newRight->keys[0]);

    memcpy(newRightStart, rightStart, rightSize);
    newRight->used = (rightSize / sizeof(key_t)) - 1; /* to correct for far reach. */
    newRight->parent = blk->parent;

    /* bookkeeping. */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (newRight->keys[i].ptr) {
            newRight->keys[i].ptr->parent = newRight;
        }
    }

    memset(middle, 0x00, rightSize + sizeof(key_t)); /* clear them off. */
    blk->used = middleIndex; /* used is index+1 */
    if (middleSave.ptr) {
        blk->keys[blk->used].ptr = middleSave.ptr;
    }

    {
        printf("blockSplit fin: \n");
        printf("left: %d (parent: %d) ", blk->id, blk->parent->id);
        blockPrint(blk);
        printf("right: %d (parent: %d) ", newRight->id, newRight->parent->id);
        blockPrint(newRight);
    }

    return blockInsert(blk->parent, &promote, newRight);
}

/*
 * Splitting the root block is presently done as a special edge case.
 */
static void rootSplit(block_t *root)
{
    int i;
    /* find middle key in block. */
    int middleIndex = NUM_KEYS / 2;

    printf("rootSplit...\n");
    blockPrint(root);

    /* pointer to the middle key, the one that will remain. */
    key_t *middle = &(root->keys[middleIndex]);

    /* start at the beginning of the keys. */
    key_t *leftStart = &(root->keys[0]);
    key_t *leftEnd = middle;
    size_t leftSize = (size_t)leftEnd - (size_t)leftStart;

    /* starts immediately following the middle. */
    key_t *rightStart = middle + 1;
    key_t *end = &(root->keys[root->used+1]); /* to include last ptr regardless */
    size_t rightSize = (size_t)end - (size_t)rightStart;

    block_t *newLeft = newBlock();
    block_t *newRight = newBlock();

    key_t *newLeftStart = &(newLeft->keys[0]);
    key_t *newRightStart = &(newRight->keys[0]);

    /*
     * int specific, but could be generalized and stored on the stack
     * (or heap).
     */
    int middleValue = middle->key;

    int leftUsed = leftSize / sizeof(key_t);
    /* rightUsed-1, because end includes the next entry. */
    int rightUsed = (rightSize / sizeof(key_t)) - 1;

    /* Set up new left. */
    memcpy(newLeftStart, leftStart, leftSize);
    newLeft->used = leftUsed;
    newLeft->parent = root;
    if (middle->ptr) {
        newLeft->keys[newLeft->used].ptr = middle->ptr;
    }

    /* bookkeeping. */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (newLeft->keys[i].ptr) {
            newLeft->keys[i].ptr->parent = newLeft;
        }
    }

    /* Set up new right. */
    memcpy(newRightStart, rightStart, rightSize);
    newRight->used = rightUsed;
    newRight->parent = root;

    /* bookkeeping. */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (newRight->keys[i].ptr) {
            newRight->keys[i].ptr->parent = newRight;
        }
    }

    /* Rebuild root block. */
    memset(root, 0x00, sizeof(block_t));
    root->used = 1;
    root->keys[0].key = middleValue;
    root->keys[0].ptr = newLeft;
    root->keys[1].ptr = newRight;

    return;
}

static void deleteLeaf(block_t *where, key_t *curr)
{
    /* we know it's a leaf, so we delete it, then, we need to check if that was
     * the last item in the block.
     */

    /*
     * A leaf cannot have all three, A, B, and C, because it would be split
     * if the block holds maximum 3.  So, this can be expanded generically
     * easily (and is done so below).
     *
     * -----------------
     * | A | B | C | X |
     * -----------------
     *
     * If curr is C, then we want to copy X over it, which we know will be
     * empty.  If curr is B, or A, we want to copy from the next to the end
     * over it.
     *
     * curr cannot be X. :D  Which is a handy thing to know.
     */
    key_t *start = curr + 1;
    block_t *end = where + 1; /* points immediately after X's end. */
    size_t len = (size_t)end - (size_t)start;
    memcpy(curr, start, len);
    where->used--;

    if (where->used) {
        return; /* yay, bail! */
    }

    /* if this is root. */
    if (!where->parent) {
        fprintf(stderr, "root node is empty!\n");
        return;
    }

    fprintf(stderr, "the leaf block is now empty!\n");

    /* Ok, so we now have an empty block, so we need to recursively try to
     * merge and re-balance the tree.
     *
     * This could just mean a pull or a rotate depending, and that might be the
     * end of it.
     */

    /* Check the parent, if they're greater than floor(NUM_KEYS/2), then we can
     * safely pull them down.  Otherwise, we need to rotate; could still leave
     * issues that need to be balanced.
     */

    /* I enumerated 8 different interesting cases for basic leaf deletion,
     * however there are more cases if recursion is required.
     */

    return;
}

static void delete(block_t *root, int value)
{
    int i;
    key_t *curr;
    int leaf = 0;

    /* 1. Find the block. */
    block_t *where = search(root, value);

    if (!where) {
        return;
    }

    /* 2. Find the key, check if it's a leaf. */
    for (i = 0; i < where->used; i++) {
        curr = &(where->keys[i]);

        if (value == curr->key) {
            if (!curr->ptr) {
                leaf = 1;
            }
            break;
        }
    }

    if (leaf) {
        /* 3a. Handle leaf deletion. */
        deleteLeaf(where, curr);
    } else {
        /* 3b. Handle parent deletion. */
        fprintf(stderr, "this node has children!\n");
        //deleteInternal();
    }
}

static void insert(block_t *root, int value)
{
    block_t *where;
    key_t *curr;
    int i;
    int found = 0;

    printf("\nentered insert: %d\n", value);

    /* first dude ever; could be called if all deleted. */
    if (0 == root->used) {
        root->used++;
        root->keys[0].key = value;

        printf("placed 0th root entry\n");
        return;
    }

    /* could do this recursively, but meh. */
    where = root;
    while (!found) {
        for (i = 0; i < where->used; i++) {
            curr = &(where->keys[i]);

            /* If the value is less, we follow the link. */
            if (value < curr->key) {
                if (curr->ptr) {
                    where = curr->ptr;
                } else {
                    found = 1; /* we're at the lowest block. */
                }
                break;
            } else {
                /*
                 * We're greater than the key; check if we're after the last
                 * one.
                 */
                if (i == (where->used-1)) {
                    /* we're at the last one, and we're greater, use pointer
                     * to the right.
                     */
                    curr += 1;
                    if (curr->ptr) {
                        where = curr->ptr;
                    } else {
                        found = 1;
                    }
                    break;
                }
            }
        }
    }

    printf("found block: %d\n", where->id);

    /*
     * let's check this block to see if it goes there.
     *
     * we know that all the slots aren't in use yet or it would already be
     * split, so we can guarantee placement, then split.
     */

    /*
     * Really don't need to find this again; except later i may split up
     * finding the block and finding where it goes.
     */
    for (i = 0; i < where->used; i++) {
        curr = &(where->keys[i]);

        if (value < curr->key) {
            /* it goes here. */
            /* we need to shift everything to the right by one. */

            /* point immediately after the last memory slot. */
            key_t *end = &(where->keys[where->used]);
            size_t len = (size_t)end - (size_t)curr;

            (void)memmove(curr + 1, curr, len);
            curr->key = value;
            where->used++;
            break;
        } else {
            /* we're greater */
            if (i == (where->used-1)) {
                /* we're at the last slot in use; so we can just set after. */
                where->keys[where->used].key = value;
                where->used++;
                break;
            }
        }
    }

    /* ok, now do we need to split where? */
    if (NUM_KEYS == where->used) {
        printf("need to split\n");

        if (NULL == where->parent) {
            /* root split is different. */
            return rootSplit(root);
        } else {
            /*
             * Blocks know who their parents are, because that tiny amount
             * of bookkeeping simplifies the implementation.  without it, I'd
             * have to do some tracking and unwinding.
             */
            return blockSplit(where);
        }
    }

    return;
}

static void blockPrint(block_t *blk)
{
    int i;
    block_t *b;

    for (i = 0; i <= NUM_KEYS; i++) {
        if (blk->keys[i].ptr) {
            b = blk->keys[i].ptr;
            printf("ptr->%d ", b->id);
        } else {
            printf("-- ");
        }
        if (i < blk->used) {
            printf("val: %d | ", blk->keys[i].key);
        } else {
            printf("| ");
        }
    }
    printf("\n");
}

static void depthFirstPrint(block_t *blk)
{
    int i;
    block_t *b;

    if (blk->parent == NULL) {
        printf("root: %d, used: %d | ", blk->id, blk->used);
    } else {
        printf("blk: %d, par: %d, used: %d | ",
                blk->id,
                blk->parent->id,
                blk->used);
    }

    for (i = 0; i <= NUM_KEYS; i++) {
        if (blk->keys[i].ptr) {
            b = blk->keys[i].ptr;
            printf("ptr->%d ", b->id);
        } else {
            printf("-- ");
        }
        if (i < blk->used) {
            printf("val: %d | ", blk->keys[i].key);
        } else {
            printf("| ");
        }
    }
    printf("\n");

    for (i = 0; i <= NUM_KEYS; i++) {
        if (blk->keys[i].ptr) {
            depthFirstPrint(blk->keys[i].ptr);
        }
    }
}

static void depthFirstFree(block_t *blk)
{
    int i;

    for (i = 0; i <= NUM_KEYS; i++) {
        if (blk->keys[i].ptr) {
            depthFirstFree(blk->keys[i].ptr);
        }
    }

    free(blk);
}

static void test_insertBalance(void)
{
    int i;
    int first = 1;
    int count = 50;
    block_t *f;
    block_t *root = NULL;

    counter = 0; /* to make it easier on humans. */

    root = newBlock();
    assert(NULL != root);

    for (i = first; i < (first + count); i++) {
        insert(root, i);
        //depthFirstPrint(root);
    }

    printf("\n\n");
    depthFirstPrint(root);

    /* verify we can find all nodes entered in. */
    for (i = first; i < (first + count); i++) {
        f = search(root, i);
        assert(NULL != f);
    }

    depthFirstFree(root);

    return;
}

static void test_deleteSimple(void)
{
    int i;
    int first = 1;
    int count = 4;
    block_t *f;
    block_t *root = NULL;

    counter = 0; /* to make it easier on humans. */

    root = newBlock();
    assert(NULL != root);

    for (i = first; i < (first + count); i++) {
        insert(root, i);
        //depthFirstPrint(root);
    }

    printf("\n\n");
    depthFirstPrint(root);

    /* verify we can find all nodes entered in. */
    for (i = first; i < (first + count); i++) {
        f = search(root, i);
        assert(NULL != f);
    }

    delete(root, 4);

    for (i = first; i < (first + count); i++) {
        f = search(root, i);
        if (i == count) {
            assert(NULL == f);
        } else {
            assert(NULL != f);
        }
    }

    printf("\n\n");
    depthFirstPrint(root);

    depthFirstFree(root);

    return;
}

int main(void)
{
    /*
     * Prevent innocuous code changes from breaking code that made reasonable
     * assumptions.
     */
    assert(80 == sizeof(block_t));
    assert(16 == offsetof(block_t, keys));

    test_insertBalance();
    test_deleteSimple();

    return 0;
}
