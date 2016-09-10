
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stringprefixtrie.h"

#define NUM_ELEMENTS(X) (sizeof(X)/sizeof(*X))

int main(void)
{
    int i;
    node_t *root = NULL;
    root = newNode();

    const char *words[] = {"asdf", "asde", "as", "tea", "ted", "ten", "qwerty",
            "qwe", "what", "how"};

    for (i = 0; i < NUM_ELEMENTS(words); i++) {
        printf("%s\n", words[i]);
        insert(root, words[i], strlen(words[i]));
        assert(TRUE == search(root, words[i], strlen(words[i])));
    }

    //const char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    //insert(root, alphabet, strlen(alphabet));
    //assert(TRUE == search(root, alphabet, strlen(alphabet)));

    depthFirstFree(root);

    return 0;
}
