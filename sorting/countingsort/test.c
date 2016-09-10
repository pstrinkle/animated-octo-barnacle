
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "countingsort.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

int main(void)
{
    int i;
    int input[] = {0, 3, 8, 6, 4, 10, 9, 15, 11, 12, 6, 2, 0, 7, 7, 1, 10};

    printf("unsorted ");
    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", input[i]);
    }
    printf("\n");

    int *output = countingsort(input, NUM_ELEMENTS(input));

    printf("sorted ");
    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", output[i]);
    }
    printf("\n");

    free(output);

    return 0;
}
