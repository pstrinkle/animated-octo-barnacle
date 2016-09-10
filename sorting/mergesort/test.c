
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mergesort.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

static void
dumplist(int *input, int count)
{
    int i;

    printf("\nvalues: ");
    for (i = 0; i < count; i++)
    {
        printf("%d ", input[i]);
    }

    printf("(end)\n");
    return;
}

int main(void)
{
    int unsorted[] = {0, 2, 6, 4, 7, 1, 10, 8, 9, 15, 3};

    printf("unsorted: ");
    dumplist(unsorted, NUM_ELEMENTS(unsorted));

    int *sorted = _mergesort(unsorted, NUM_ELEMENTS(unsorted));

    printf("sorted: ");
    dumplist(sorted, NUM_ELEMENTS(unsorted));

    free(sorted);

    /* some tests; better with cunit. */

    /* empty list. */
    {
        int test[] = {};
        int *result = _mergesort(test, NUM_ELEMENTS(test));
        assert(result == NULL);
    }

    /* one item list, no sorting required. */
    {
        int test[] = {0};
        int verify[] = {0};
        int *result = _mergesort(test, NUM_ELEMENTS(test));
        int res = memcmp(verify, result, sizeof(int) * NUM_ELEMENTS(test));
        assert(0 == res);
        free(result);
    }

    /* two items. */
    {
        int test[] = {1, 0};
        int verify[] = {0, 1};
        int *result = _mergesort(test, NUM_ELEMENTS(test));
        int res = memcmp(verify, result, sizeof(int) * NUM_ELEMENTS(test));
        assert(0 == res);
        free(result);
    }

    /* n items. */
    {
        int test[] = {1, 0, 5, 7, 2, 9, 100, 14, 23, 3, 99};
        int verify[] = {0, 1, 2, 3, 5, 7, 9, 14, 23, 99, 100};
        int *result = _mergesort(test, NUM_ELEMENTS(test));
        int res = memcmp(verify, result, sizeof(int) * NUM_ELEMENTS(test));
        assert(0 == res);
        free(result);
    }

    return 0;
}
