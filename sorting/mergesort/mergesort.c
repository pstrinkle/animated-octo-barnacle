
#include <stdlib.h>
#include <stdio.h>

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

static int *
_mergesort(int *input, int count)
{
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

int main(void)
{
    int unsorted[] = {0, 2, 6, 4, 7, 1, 10, 8, 9, 15, 3};

    printf("unsorted: ");
    dumplist(unsorted, NUM_ELEMENTS(unsorted));

    int *sorted = _mergesort(unsorted, NUM_ELEMENTS(unsorted));

    printf("sorted: ");
    dumplist(sorted, NUM_ELEMENTS(unsorted));

    free(sorted);

    return 0;
}
