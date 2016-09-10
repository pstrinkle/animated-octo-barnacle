

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quicksort.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

/* could move into a library that I share between my code (later) */
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

    quicksort(unsorted, 0, NUM_ELEMENTS(unsorted)-1);

    printf("sorted: ");
    dumplist(unsorted, NUM_ELEMENTS(unsorted));

    return 0;
}

