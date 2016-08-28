
/*
 * All of my sorting and such is aimed at integers instead of something
 * generic.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

static int partition(int *section, int low, int high)
{
    int pivot = section[low];
    int left = low+1;
    int right = high;

    /* left and right move towards each other until they flip. */
    while (left < right) {
        /* while we can go right and it's less than the pivot. */
        while (left <= right && section[left] < pivot) {
            left += 1;
        }
        while (right >= left && section[right] > pivot) {
            right -= 1;
        }

        /* the values should be swapped. */
        if (left < right) {
            int tmp = section[left];
            section[left] = section[right];
            section[right] = tmp;
        }
    }

    /* move pivot into place. */
    section[low] = section[right];
    section[right] = pivot;
    return right;
}

static void quicksort(int *input, int low, int high)
{
    int pivot;

    if (low < high) {
        pivot = partition(input, low, high);

        /* quicksort left and right portion. */
        quicksort(input, low, pivot-1);
        quicksort(input, pivot+1, high);
    }

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
