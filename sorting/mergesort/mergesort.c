
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int *_mergesort(int *input, int count)
{
    /* will not happen during recursion. */
    if (input == NULL || count == 0) {
        return NULL;
    }

    if (count < 2) {
        /* XXX: return the array we received */
        int *t = malloc(sizeof(int));
        t[0] = input[0];
        return t;
    }

    /* split into two pieces. */
    int left = count / 2;
    int right = count - left;

    int *a = _mergesort(input, left);
    int *b = _mergesort(input+left, right);

    int *c = malloc(sizeof(int) * count);

    int i = 0;
    int ai = 0;
    int bi = 0;

    while (i < count)
    {
        if (ai < left && bi < right)
        {
            if (a[ai] < b[bi])
            {
                c[i] = a[ai];
                ai++;
            }
            else
            {
                c[i] = b[bi];
                bi++;
            }
        }
        else if (ai < left)
        {
            c[i] = a[ai];
            ai++;
        }
        else
        {
            c[i] = b[bi];
            bi++;
        }

        i++;
    }

    free(a);
    free(b);

    /* higher call frees this, just like above. */
    return c;
}
