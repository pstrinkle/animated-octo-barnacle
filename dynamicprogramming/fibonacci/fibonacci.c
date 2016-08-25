
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

int *stor = NULL;

static int dp_recurse_fib(int n)
{
    int f;

    if (stor[n] != 0) {
        return stor[n];
    }

    if (n <= 2) {
        f = 1;
    } else {
        f = dp_recurse_fib(n - 1) + dp_recurse_fib(n - 2);
    }

    stor[n] = f;
    return f;
}

static int dp_bottom_fib(int n)
{
    int memo[2];
    int i, f;

    for (i = 1; i < n + 1; i++) {
        if (i <= 2) {
            f = 1;
        } else {
            /* n - 1, n - 2 */
            f = memo[1] + memo[0];
        }

        /* shift the values left. */
        memo[0] = memo[1]; /* move n-1 --> n-2 */
        memo[1] = f; /* set new n-1 */
    }

    return memo[1];
}

int main(void)
{
    int i;

    for (i = 1; i < 10; i++) {
        stor = malloc(sizeof(int) * (i+1));
        memset(stor, 0x00, sizeof(int) * (i+1));

        printf("%d\n", dp_recurse_fib(i));

        free(stor);
    }

    for (i = 1; i < 10; i++) {
        printf("%d\n", dp_bottom_fib(i));
    }

    return 0;
}
