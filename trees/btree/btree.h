
#ifndef _BTREE_H
#define _BTREE_H

/******************************************************************************
 * Macros
 *****************************************************************************/

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
 * Implementation
 *****************************************************************************/

/* create a new empty block. */
block_t *newBlock(void);

block_t *search(block_t *blk, int value);
void delete(block_t *root, int value);
/* insert the value into the tree starting at the specified block. */
void insert(block_t *root, int value);
/* print one block. */
void blockPrint(block_t *blk);
/* print each block in a dfs pattern. */
void depthFirstPrint(block_t *blk);
void printTree(const char *lead, block_t *blk);
void depthFirstFree(block_t *blk);

#endif
