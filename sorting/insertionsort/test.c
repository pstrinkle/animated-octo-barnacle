
#include <stdlib.h>
#include <stdio.h>

#include "insertionsort.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

int main(void)
{
    int i;
    int input[] = {16, 13, 8, 6, 4, 10, 9, 15, 11};

    printf("presorted: ");
    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", input[i]);
    }
    printf("\n");

    insertionsort(input, NUM_ELEMENTS(input));

    printf("sorted: ");
    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", input[i]);
    }
    printf("\n");

    return 0;
}
