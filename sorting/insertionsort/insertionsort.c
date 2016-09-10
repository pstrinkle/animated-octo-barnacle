
#include <stdlib.h>

void insertionsort(int *values, int count)
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
