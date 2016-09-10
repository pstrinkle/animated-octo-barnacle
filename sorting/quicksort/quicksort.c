
/*
 * All of my sorting and such is aimed at integers instead of something
 * generic.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quicksort.h"

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

void quicksort(int *input, int low, int high)
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


