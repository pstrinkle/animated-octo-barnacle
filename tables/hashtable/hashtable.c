#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

/* division */
static int modular(int key, int table) {
    /* 
     * Doesn't work well if both are even, and we can do better
     */
    return key % table;
}

#define ARCH_BITS (sizeof(int) * 8)
#define TABLE_POWER 16 /* m (table size) == 2**r */

/* random integer within w bits, selected ahead of time, must be odd */
static int a = 0;

/* table size is power of 2, 2**r */
static int multiplication(int key) {
    int w = ARCH_BITS;

    int i = (key * a);
    int r = TABLE_POWER; /* bits in the middle */
    int v = (i % (2**ARCH_BITS)) >> (ARCH_BITS - TABLE_POWER);

    return v;
}

int main(void)
{
    return 0;
}
