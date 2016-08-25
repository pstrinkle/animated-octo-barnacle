
#include <stdlib.h>
#include <stdio.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

static void insertionsort(int *values, int count)
{
    int x;
    int i, j;

    if (NULL == values || 0 == count || 1 == count) {
        return;
    }

    for (i = 1; i < count; i++) {
        j = i;
        while (j > 0) {
            if (values[j] < values[j-1]) {
                x = values[j-1];
                values[j-1] = values[j];
                values[j] = x;
            }

            j--;
        }
    }

    return;
}

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
