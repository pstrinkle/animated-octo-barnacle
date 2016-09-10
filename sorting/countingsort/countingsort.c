/*
 * I decided to limit the index to 64 values, so that it's a smaller memory
 * footprint instead of MAX(uint32_t).
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int *countingsort(int *input, int count)
{
    int i, total;
    int counts[64]; /* to support numbers 0 - 63 */
    int *output = malloc(sizeof(int) * count);
    // NULLCHECK

    (void)memset(counts, 0, sizeof(counts));

    for (i = 0; i < count; i++) {
        counts[input[i]] += 1;
    }

    total = 0;
    for (i = 0; i < 64; i++) {
        total += counts[i];
        counts[i] = total;
    }

    for (i = count-1; i > -1; i--) {
        register int v = input[i];
        output[counts[v]-1] = v;
        counts[v] -= 1;
    }

    return output;
}


