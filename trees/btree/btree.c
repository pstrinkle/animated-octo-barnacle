/*
 * Basic implementation of a B-tree.
 *
 * Just about all my test cases delete the right-most key.  They don't all, but
 * too many do.  I think I favor testing this; so I should be a bit more
 * programmatically thorough. :D
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
 * Debug Macros
 *****************************************************************************/

#define INSERT_DEBUG 0
#define SPLIT_DEBUG 0
#define DELETE_DEBUG 1

#if DELETE_DEBUG
#define DELETE_DPRINTF(...) \
    do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define DELETE_DPRINTF(...) do { } while (0)
#endif

#if INSERT_DEBUG
#define INSERT_DPRINTF(...) \
    do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define INSERT_DPRINTF(...) do { } while (0)
#endif

#if SPLIT_DEBUG
#define SPLIT_DPRINTF(...) \
    do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define SPLIT_DPRINTF(...) do { } while (0)
#endif

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
static block_t *blockStorage[128];

/******************************************************************************
 * Test code start
 *****************************************************************************/

typedef void (*test_ptr_t)(void);

static block_t *test_buildTree(block_t *root, int *input, int count);
/* deleted is the value you expect to be missing, not the index in input */
static void test_verifyDeletion(block_t *root, int *input, int count, int deleted);
static void test_verifyTree(block_t *root, int *input, int count);
static int test_verifyBlock(int block, int *values, int count);

/******************************************************************************
 * Implementation start
 *****************************************************************************/

/* create a new empty block. */
static block_t *newBlock(void);

/*
 * Append a key into a block's list, always append, newblk can be NULL.
 */
static void blockAppend(block_t *insert, key_t *promoted, block_t *newblk);
/* split a normal block. */
static void blockSplit(block_t *blk);
/* split the root block */
static void rootSplit(block_t *root);
/* insert the value into the tree starting at the specified block. */
static void insert(block_t *root, int value);
/*
 * insert the value into the leaf
 * (could just call insert and specify leaf as root).
 */
static void insertLeaf(block_t *leaf, int value);

static block_t *findLeftSibling(block_t *me);
static void rotateRight(block_t *lSibling, block_t *me);
static block_t *findRightSibling(block_t *me);
static void rotateLeft(block_t *rSibling, block_t *me);
static void demoteParent(block_t *me, block_t *sub);

static void deleteLeaf(block_t *where, key_t *curr);
static void delete(block_t *root, int value);

/* print one block. */
static void blockPrint(block_t *blk);
/* print each block in a dfs pattern. */
static void depthFirstPrint(block_t *blk);
static void printTree(const char *lead, block_t *blk);

static block_t *newBlock(void)
{
    block_t *b = malloc(sizeof(block_t));
    memset(b, 0x00, sizeof(block_t));

    blockStorage[counter] = b; /* for TDD. */
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

static void blockAppend(block_t *insert, key_t *promoted, block_t *newblk)
{
    if (newblk) {
        SPLIT_DPRINTF("blockAppend to blk: %d, key: %d, newblk: %d\n",
                insert->id,
                promoted->key,
                newblk->id);
    }

    /* always. */
    if (newblk) {
        newblk->parent = insert;
    }

    /*
     * Convenient we know this will always be to either the beginning or end.
     */
    if (insert->used > 0) {
        /* Insert before? */
        if (promoted->key < insert->keys[0].key) {
            /*
             * In this insert case, the far right pointer should never be set.
             */
            assert(NULL == insert->keys[3].ptr);

            key_t *start = &(insert->keys[0]);
            /* Point to the start of the last entry. */
            key_t *end = &(insert->keys[3]);
            size_t len = (size_t)end - (size_t)start;

            memmove(&(insert->keys[1]), start, len);

            insert->keys[0].key = promoted->key;
            insert->keys[0].ptr = newblk;
            insert->used++;
            return;
        }
    } /* append. */

    insert->keys[insert->used].key = promoted->key; /* i think this is good.. */
    insert->keys[insert->used+1].ptr = newblk;
    insert->used++;

    if (NUM_KEYS == insert->used) {
        SPLIT_DPRINTF("need to split %d\n", insert->id);

        if (NULL == insert->parent) {
            SPLIT_DPRINTF("need to split root variant\n");
            return rootSplit(insert);
        } else {
            SPLIT_DPRINTF("need to call normal block split\n");
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
    SPLIT_DPRINTF("blockSplit on %d\n", blk->id);
#if SPLIT_DEBUG
    blockPrint(blk);
#endif

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

#if SPLIT_DEBUG
    {
        SPLIT_DPRINTF("blockSplit fin: \n");
        SPLIT_DPRINTF("left: %d (parent: %d) ", blk->id, blk->parent->id);
        blockPrint(blk);
        SPLIT_DPRINTF("right: %d (parent: %d) ", newRight->id, newRight->parent->id);
        blockPrint(newRight);
    }
#endif

    return blockAppend(blk->parent, &promote, newRight);
}

/*
 * Splitting the root block is presently done as a special edge case.
 */
static void rootSplit(block_t *root)
{
    int i;
    /* find middle key in block. */
    int middleIndex = NUM_KEYS / 2;

    SPLIT_DPRINTF("rootSplit...\n");
#if SPLIT_DEBUG
    blockPrint(root);
#endif

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
    lSibling->keys[lSibling->used].key = 0; /* set value to 0; just in case. */

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
    blockAppend(me, &demote, NULL);

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

static void rotateLeft(block_t *rSibling, block_t *me)
{
    int i;
    block_t *parent = me->parent;
    /*
     * XXX: yes, b-tree of integers for now, but this is easy to generalize.
     */
    int promote;
    key_t demote;

    /*
     * We want the 0th entry from the right sibling to be promoted, however,
     * this does require slightly more effort because we basically need to then
     * shift the block contents left (like we do in some of the split code.
     */

    promote = rSibling->keys[0].key;

    /* fix rSibling. */
    key_t *start = &(rSibling->keys[1]);
    block_t *end = rSibling + 1; /* points immediately after block's end. */
    size_t len = (size_t)end - (size_t)start;
    memcpy(&(rSibling->keys[0]), start, len);
    rSibling->used--;

    /*
     * The pointer to the right sibling will sit immediately to the right of
     * the key we need to demote into block "me."
     */

    /* Because we are rotating left, the rightSibling could be beyond the keys,
     * and it will never be the 0th key. :D  It could be the pointer to the
     * right of the 0th key, which is held within the 1st key. :D  So, we're
     * good.
     */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (parent->keys[i].ptr == rSibling) {
            demote.key = parent->keys[i-1].key;
            demote.ptr = NULL;
            parent->keys[i-1].key = promote;
            break;
        }
    }

    /* insert it into the suddenly empty leaf node. */
    blockAppend(me, &demote, NULL);

    return;
}

static void demoteParent(block_t *me, block_t *sub)
{
    int i;

    /*
     * The following are just ya know, if we get here and these fail it's a
     * serious non-recoverable issue.
     */

    /*
     * XXX: If we get here and me is root, and it's empty, could we have gotten
     * here?
     */
    assert(me->parent);

    block_t *parent = me->parent;
    block_t *pushedHere = NULL;

    {
        int subid = -1;
        if (sub) {
            subid = sub->id;
        }

#if DELETE_DEBUG
        DELETE_DPRINTF("demoteParent: me %d, my parent: %d, sub: %d\n",
                me->id,
                parent->id,
                subid);

        DELETE_DPRINTF("\nme: ");
        blockPrint(me);

        DELETE_DPRINTF("my parent: ");
        blockPrint(parent);

        if (sub) {
            // XXX: sub will need a new parent once it's attached.
            DELETE_DPRINTF("my sub: ");
            blockPrint(sub);
        }
#endif
    }

    /*
     * Find whomever points to me, may be left or right pointer, and demote
     * them into the corresponding neighbor block.
     */
    for (i = 0; i <= NUM_KEYS; i++) {
        if (parent->keys[i].ptr == me) {
            if (i == 0) {
                pushedHere = parent->keys[i+1].ptr;

                if (sub) {
                    key_t k;
                    k.key = parent->keys[i].key;

                    blockAppend(parent->keys[i+1].ptr, &k, sub);
                } else {
                    insertLeaf(parent->keys[i+1].ptr, parent->keys[i].key);
                }

                block_t *end = parent + 1;
                size_t len = (size_t)end - (size_t)&(parent->keys[i+1]);
                memcpy(&(parent->keys[0]), &(parent->keys[i+1]), len);
            } else {
                pushedHere = parent->keys[i-1].ptr;

                if (sub) {
                    key_t k;
                    k.key = parent->keys[i-1].key;

                    DELETE_DPRINTF("trying to append key: %d, with a pointer to: %d into %d\n",
                            k.key,
                            sub->id,
                            parent->keys[i-1].ptr->id);

                    blockAppend(parent->keys[i-1].ptr, &k, sub);
                } else {
                    insertLeaf(parent->keys[i-1].ptr, parent->keys[i-1].key);
                }

                parent->keys[i].ptr = parent->keys[i-1].ptr;

                /* now move it to the left. */
                block_t *end = parent + 1;
                size_t len = (size_t)end - (size_t)&(parent->keys[i]);
                memcpy(&(parent->keys[i-1]), &(parent->keys[i]), len);
            }

            break;
        }
    }

    parent->used--;

#if DELETE_DEBUG
    DELETE_DPRINTF("parent demoted here: blk %d\n", pushedHere->id);
    blockPrint(pushedHere);
#endif

    if (parent->used > 0) {
        return; /* yay, bail. */
    }

    /* Ok, now the parent block is empty! */
    block_t *carry = parent->keys[0].ptr;

    /* just for fun. */
    memset(&parent->keys[0], 0x00, (NUM_KEYS+1) * sizeof(key_t));

    /* was my parent the root block? */
    if (NULL == parent->parent) {
        /* this isn't an object where I can cleanly just replace the root;
         * without adding in a context structure that gets passed around
         * or something along those lines.
         */
        memcpy(parent, carry, sizeof(block_t));
        {
            int i;
            for (i = 0; i <= NUM_KEYS; i++) {
                if (parent->keys[i].ptr) {
                    parent->keys[i].ptr->parent = parent; /* fix backlinks. */
                }
            }
        }

        parent->parent = NULL;
        free(carry);

        return;
    }

#if DELETE_DEBUG
    DELETE_DPRINTF("\n\nparent (%d) is empty\n", parent->id);
    blockPrint(parent);
    DELETE_DPRINTF("don't abandon: %d, \n", carry->id);
    blockPrint(carry);
#endif

    block_t *lSib = findLeftSibling(parent);
    block_t *rSib = findRightSibling(parent);

    DELETE_DPRINTF("left sib: 0x%p, right sib: 0x%p\n", lSib, rSib);

    /* You only have a left sibling and it's insufficient. */
    if (lSib && !rSib && lSib->used == 1) {
#if DELETE_DEBUG
        DELETE_DPRINTF("lSib insufficient (no rSib):\n");
        blockPrint(lSib);
#endif

        demoteParent(parent, carry);
        free(parent);
        return;
    }

    /* You only have a right sibling and it's insufficient. */
    if (rSib && !lSib && rSib->used == 1) {
#if DELETE_DEBUG
        fprintf(stderr, "rSib insufficient (no lSib):\n");
        blockPrint(rSib);
#endif

        demoteParent(parent, carry);
        free(parent);
        return;
    }

    /* Both siblings are valid, but insufficient */
    if ((lSib && lSib->used == 1) && (rSib && rSib->used == 1)) {
#if DELETE_DEBUG
        fprintf(stderr, "both valid, insufficient:\n");
        blockPrint(lSib);
        blockPrint(rSib);
#endif

        demoteParent(parent, carry);
        free(parent);
        return;
    }

    if (lSib && lSib->used > 1) {
#if DELETE_DEBUG
        fprintf(stderr, "lSib is sufficient, so probably rotate:\n");
        blockPrint(lSib);
#endif

    } else {
#if DELETE_DEBUG
        fprintf(stderr, "rSib is sufficient, so probably rotate:\n");
        blockPrint(rSib);
#endif
    }

    assert(0);

    return;
}

static void deleteLeaf(block_t *where, key_t *curr)
{
    block_t *lSib, *rSib;

    /*
     * We know it's a leaf, so we delete it, then, we need to check if that was
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
        assert(0);
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
     * | 9 ... (not yet documented, will document when I handle it)
     *
     * + more complex; detailed later.
     * ++ push neighbor down, free block, three variations for immediate
     * siblings (4a, 4b,4c).
     *
     * From this table I was able to readily see that the parent's sufficiency
     * isn't the lynchpin, it's the siblings.
     *
     * My implementation will favor left rotation, which is just an arbitrary
     * decision.  I could balance with a coin flip in that case, but meh, this
     * is faster only because it doesn't waste time flipping coins and produces
     * the same balanced tree.
     */

    /* Determine if immediate siblings are sufficient. */

    /*
     * XXX: This code only checks immediate siblings for rotation, whereas
     * really; to be generic, it could roll siblings along if any sibling has
     * > 1 key in it.
     */

    /*
     * Find siblings, you seriously will have at least one, we know that by the
     * balanced tree properties.
     */
    lSib = findLeftSibling(where);
    rSib = findRightSibling(where);

    fprintf(stderr, "left sib: 0x%p, right sib: 0x%p\n", lSib, rSib);

    /*
     * This code is going to be effectively in two places, which means maybe
     * I can balance it more nicely; I'll refactor later.
     */

    /* You only have a left sibling and it's insufficient. */
    if (lSib && !rSib && lSib->used == 1) {
        demoteParent(where, NULL);
        free(where);
        return;
    }

    /* You only have a right sibling and it's insufficient. */
    if (rSib && !lSib && rSib->used == 1) {
        demoteParent(where, NULL);
        free(where);
        return;
    }

    /* Both siblings are valid, but insufficient */
    if ((lSib && lSib->used == 1) && (rSib && rSib->used == 1)) {
        demoteParent(where, NULL);
        free(where);
        return;
    }

    if (lSib && lSib->used > 1) {
        /*
         * If there is a left sibling and it's valid, we don't care about the
         * right.
         */

        /* rotate right. */
        return rotateRight(lSib, where);
    } else {
        /*
         * If no left sibling, or left sibling not valid, the right sibling
         * exists and is valid.
         */

        /* rotate left. */
        return rotateLeft(rSib, where);
    }

    return;
}

static void delete(block_t *root, int value)
{
    int i;
    key_t *curr;
    int leaf = 0;

    DELETE_DPRINTF("deleting: %d\n", value);

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
        assert(0);
        //deleteInternal();
    }
}

static void insertLeaf(block_t *leaf, int value)
{
    int i;
    key_t *curr;

    /*
     * This is identical to code inside insert, so that can just call this;
     * once I have fuller regression testing in place.
     */
    for (i = 0; i < leaf->used; i++) {
        curr = &(leaf->keys[i]);

        if (value < curr->key) {
            /* it goes here. */
            /* we need to shift everything to the right by one. */

            /* point immediately after the last memory slot. */
            key_t *end = &(leaf->keys[leaf->used]);
            size_t len = (size_t)end - (size_t)curr;

            (void)memmove(curr + 1, curr, len);
            curr->key = value;
            leaf->used++;
            break;
        } else {
            /* we're greater */
            if (i == (leaf->used-1)) {
                /* we're at the last slot in use; so we can just set after. */
                leaf->keys[leaf->used].key = value;
                leaf->used++;
                break;
            }
        }
    }
}

static void insert(block_t *root, int value)
{
    block_t *where;
    key_t *curr;
    int i;
    int found = 0;

    INSERT_DPRINTF("\nentered insert: %d\n", value);

    /* first dude ever; could be called if all deleted. */
    if (0 == root->used) {
        root->used++;
        root->keys[0].key = value;

        INSERT_DPRINTF("placed 0th root entry\n");
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

    INSERT_DPRINTF("found block: %d\n", where->id);

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
        INSERT_DPRINTF("need to split\n");

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

    return;
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

    return;
}

static void printTree(const char *lead, block_t *blk)
{
    printf("%s", lead);
    depthFirstPrint(blk);

    return;
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

    return;
}

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
 *   |2|     |6|     |10|        =>    |2|     |6|8|
 *  /  \    /   \    /   \             /  \    /  \  \
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
    };

    for (i = 0; i < NUM_ELEMENTS(tests); i++) {
        counter = 0; /* to make it easier on humans. */
        memset(blockStorage, 0x00, sizeof(blockStorage));
        printf("-----------------------------------------------------------\n");
        tests[i]();
    }

    return 0;
}
