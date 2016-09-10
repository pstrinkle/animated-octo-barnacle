
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

int main(void)
{
    int i;
    int input[] = {1, 4, 6, 100, 1000, 234, 12312, 1435, 166, 132409, 111111, 12, 0, 1};
    struct hashtable ht;

    buildhashtable(&ht, SMALL_TABLE);

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        insert(&ht, input[i]);
        assert(search(&ht, input[i]) == 1);
    }

    /* minus 1 because 1 is duplicated. */
    assert(ht.n == NUM_ELEMENTS(input)-1);
    assert(ht.m == SMALL_TABLE << 2); /* it grows twice. */

    delete(&ht, 4);
    assert(ht.n == NUM_ELEMENTS(input)-2);

    freetable(&ht);

    return 0;
}
