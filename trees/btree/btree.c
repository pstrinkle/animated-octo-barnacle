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

/*
 * Append a key into a block's list, always append, newblk can be NULL.
 */
static void blockInsert(block_t *insert, key_t *promoted, block_t *newblk);
/* split a normal block. */
static void blockSplit(block_t *blk);
/* split the root block */
static void rootSplit(block_t *root);
/* insert the value into the tree starting at the specified block. */
static void insert(block_t *root, int value);
/* print one block. */

static block_t *findLeftSibling(block_t *me);
static void rotateRight(block_t *lSibling, block_t *me);
static block_t *findRightSibling(block_t *me);

static void deleteLeaf(block_t *where, key_t *curr);
static void delete(block_t *root, int value);

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
    if (newblk) {
        printf("blockInsert to blk: %d, key: %d, newblk: %d\n",
                insert->id,
                promoted->key,
                newblk->id);
    }

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

/*
 * Find the pointer in me->parent that points to me's immediate left neighbor.
 *
 * ;)
 */
static block_t *findLeftSibling(block_t *me)
{
    int i;
    block_t *parent = me->parent;

    /* Check all possible pointers, which includes one after ->used */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (parent->keys[i].ptr == me) {
            if (i == 0) {
                return NULL; /* no left sibling. */
            } else {
                return parent->keys[i-1].ptr;
            }
        }
    }

    return NULL; /* weird, not really possible. */
}

static void rotateRight(block_t *lSibling, block_t *me)
{
    int i;
    block_t *parent = me->parent;
    /*
     * XXX: yes, b-tree of integers for now, but this is easy to generalize.
     */
    int promote;
    key_t demote;

    /* We need to pull the last key from lSibling, and erase it.  Nothing
     * fancy required here because it's always the last key and it's a leaf.
     *
     * XXX: Later, if I need to use a rotateRight in another case, say, for
     * internal node, it might _not_ be a leaf, and this code would then be
     * incorrect.  I could do a quick check for now and assert that lSibling
     * and me are leaves, but for now, I'll just review these todos when I
     * start working on internal node deletion.
     */

    /* decrement before retrieval (for slickness :P). */
    promote = lSibling->keys[--lSibling->used].key;

    /* Because we know that we're simply rotating from a left sibling, we
     * know that the key that points to the left sibling.
     *
     * Basic cases: 3, 6, 7, 8
     *
     * So we don't need to worry about the case where the pointer is after the
     * value we need to pull, that's only for rotateLeft. ;)
     */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (parent->keys[i].ptr == lSibling) {
            demote.key = parent->keys[i].key;
            demote.ptr = NULL;
            parent->keys[i].key = promote;
            break;
        }
    }

    /* insert it into the suddenly empty leaf node. */
    blockInsert(me, &demote, NULL);

    return;
}

static block_t *findRightSibling(block_t *me)
{
    int i;
    block_t *parent = me->parent;

    for (i = 0; i <= NUM_KEYS; i++) {
        if (parent->keys[i].ptr == me) {
            if (i == NUM_KEYS) {
                return NULL;
            } else {
                return parent->keys[i+1].ptr;
            }
        }
    }

    return NULL; /* where, not really possible. */
}

static void deleteLeaf(block_t *where, key_t *curr)
{
    block_t *lSibling, *rSibling;

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

    /*
     * Ok, so we now have an empty block, so we need to recursively try to
     * merge and re-balance the tree.
     *
     * This could just mean a pull or a rotate depending, and that might be the
     * end of it.
     *
     *
     * I enumerated 8 different interesting cases for basic leaf deletion,
     * however there are more cases if recursion is required.
     *
     * Basically, the parent block is either sufficiently filled, or
     * insufficiently filled and the sibling or siblings can be sufficient or
     * insufficient.  The definition here for insufficient means, that there is
     * only one key in the block, and therefore moving that key or promoting it
     * will leave us with an empty block.
     */

    /*
     * XXX: This code only checks immediate siblings for rotation, whereas
     * really; to be generic, it could roll siblings along if any sibling has
     * > 1 key in it.
     */

    /*
     * suff. == sufficient
     * rot. == rotate
     * -> == right
     * <- left (obviously, ;) )
     *
     * from my basic 8 cases, I made the following table:
     *
     * | case | parent | left    | right   | action  |
     * |      | suff.  | sibling | sibling |         |
     * |      |        | suff.   | suff.   |         |
     * -----------------------------------------------
     * | 1    | N      | N       | N       | +       |
     * -----------------------------------------------
     * | 2    | N      | N       | Y       | rot. <- |
     * -----------------------------------------------
     * | 3    | N      | Y       | N       | rot. -> |
     * -----------------------------------------------
     * | 4    | Y      | N       | N       | ++      |
     * -----------------------------------------------
     * | 5    | Y      | N       | Y       | rot. <- |
     * -----------------------------------------------
     * | 6    | Y      | Y       | N       | rot. -> |
     * -----------------------------------------------
     * | 7    | Y      | Y       | N       | rot. -> |
     * -----------------------------------------------
     * | 8    | Y      | Y       | Y       | rot. -> |
     * -----------------------------------------------
     *
     * + more complex; detailed later.
     * ++ push neighbor down, free block, three variations for immediate
     * siblings.
     *
     * From this table I was able to readily see that the parent's sufficiency
     * isn't the lynchpin, it's the siblings.
     *
     * My implementation will favor left rotation, which is just an arbitrary
     * decision.  I could balance with a coin flip in that case, but meh, this
     * is faster only because it doesn't waste time flipping coins and produces
     * the same balanced tree.
     */

    /*
     * Determine if immediate siblings are sufficient.
     *
     * XXX: make generic and check all siblings and roll them when necessary to
     * then re-check immediate siblings.
     */

    /*
     * find siblings, you seriously will have at least one, we know that by the
     * balanced tree properties.
     */
    lSibling = findLeftSibling(where);
    rSibling = findRightSibling(where);

    fprintf(stderr,
            "left sibling: 0x%x, right sibling: 0x%x\n",
            lSibling,
            rSibling);

    /* both siblings are valid, but insufficient */
    if ((lSibling && lSibling->used == 1) &&
        (rSibling && rSibling->used == 1)) {

        fprintf(stderr,
                "left sibling and right sibling are insufficient (case 1 or 4)\n");
        return;
    }

    if (lSibling && lSibling->used > 1) {
        /*
         * If there is a left sibling and it's valid, we don't care about the
         * right.
         */

        /* rotate right. */
        return rotateRight(lSibling, where);

        /* pull last key from left sibling, make it the parent key that points
         * back down to me, and insert into me, the parent key.
         */

    } else {
        /*
         * If no left sibling, or left sibling not valid, the right sibling
         * exists and is valid.
         */

        /* rotate left. */

        /* pull first key from the sibling, make it the parent key that points
         * back down to me, and insert into me, the parent key.
         */
    }

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
     *
     * XXX: Could do this via insertblock if I upgrade that code to not always
     * just append into the block; making it more general should be helpful
     * regardless.
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
    int i;
    int input[] = {1, 2, 3, 4, 5, 8, 9, 6};
    block_t *f;
    block_t *root = NULL;

    counter = 0; /* to make it easier on humans. */

    root = newBlock();
    assert(NULL != root);

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        insert(root, input[i]);
    }

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        f = search(root, input[i]);
        assert(NULL != f);
    }

    printf("\nfully-built:\n\n");
    depthFirstPrint(root);

    delete(root, 9);

    printf("\npost-delete:\n\n");
    depthFirstPrint(root);

    depthFirstFree(root);

    return;
}

// XXX: 6, 7, 8 (also rotateRight)

/*
 * In this case, the parent has more than one key, but all of the siblings have
 * one key each.
 *
 * For internal reference, this is case 4.
 *
 *      |4|
 *   /       \
 *  |2|      |6|8|
 *  /  \    /  \  \
 * |1| |3| |5| |7| |9|
 *
 * Gotta love the attempt as ascii tree art. :D
 *
 * In this case, what happens when 9 is deleted?
 *
 * 1. Delete 9, see the block is empty so we check the conditions:
 *  - Is the parent sufficient to be demoted?
 *  - Are the siblings sufficient to be promoted?
 *
 * Being this is the first of my non-super-trivial implementation tests (yay
 * making time for TDD), I will be more thorough in explanation (I presume).
 *
 * However, this implementation really is _only_ for me.  It _is_ open source,
 * and I'd be happy if anyone else found value from this, if nothing else, as
 * a different walkthrough of the data structure than you might find elsewhere.
 *
 * XXX: Although by implementing these tests, they really aren't sufficient to
 * verify the code, beyond, the links between the blocks are properly
 * maintained; unless I implement a nice method to verify each block... which I
 * could do, and would definitely benefit from if this were more than just for
 * fun.
 */
#if 0
static void test_deleteCase4(void)
{
    int i;
    int first = 1;
    int count = 9;
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

    //delete(root, 4);

#if 0
    for (i = first; i < (first + count); i++) {
        f = search(root, i);
        if (i == count) {
            assert(NULL == f);
        } else {
            assert(NULL != f);
        }
    }
#endif

    printf("\n\n");
    depthFirstPrint(root);

    depthFirstFree(root);

    return;
}
#endif

int main(void)
{
    /*
     * Prevent innocuous code changes from breaking code that made reasonable
     * assumptions.
     */
    assert(80 == sizeof(block_t));
    assert(16 == offsetof(block_t, keys));

    //test_insertBalance();
    //test_deleteSimple();
    test_deleteCase3();

    return 0;
}
