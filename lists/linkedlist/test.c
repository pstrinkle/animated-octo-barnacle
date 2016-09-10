
#include <stdlib.h>
#include <stdio.h>

#include "linkedlist.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

extern struct item *head;

int main(void)
{
    int i;
    int input[] = {0, 3, 8, 6, 4, 10, 9, 15, 11};

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        printf("%d ", input[i]);
    }
    printf("\n");

    for (i = 0; i < NUM_ELEMENTS(input); i++) {
        push(input[i]);
    }

    printList(head);

    freelist(head);

    return 0;
}

