
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))
#define MAX(X, Y) ((X) > (Y)) ? (X) : (Y)

#define NUMITEMS 10
#define KNAPSACK_WEIGHT 250

#define MINWEIGHT 1
#define MAXWEIGHT 100
#define MINVALUE 10
#define MAXVALUE 1000

static int *values = NULL;
static int *weights = NULL;

static int knapsack(int item, int remaining) {

    if (item == NUMITEMS) {
        return 0;
    }

    if (weights[item] > remaining) {
        return knapsack(item+1, remaining);
    } else {
        return MAX(knapsack(item+1, remaining),
                   knapsack(item+1, remaining-weights[item]) + values[item]);
    }
}

static int ranged_random(int min, int max) {
    int x = random();
    x %= max;
    if (x < min) {
        x = min; /* not ideal. but whatever. :D */
    }

    return x;
}

int main(void)
{
    int i, max;
    values = malloc(sizeof(int) * NUMITEMS);
    weights = malloc(sizeof(int) * NUMITEMS);

    /* srand is the normal unix, but on darwin, it's srandom */
    srandom(time(NULL));

    for (i = 0; i < NUMITEMS; i++) {
        values[i] = ranged_random(MINVALUE, MAXVALUE);
        weights[i] = ranged_random(MINWEIGHT, MAXWEIGHT);
    }

    max = knapsack(0, KNAPSACK_WEIGHT);

    printf("maximum knapsack value: %d\n", max);

    free(values);
    free(weights);

    return 0;
}
