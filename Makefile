
# There is a better way, but this is a decent first start.
PKGS = lists/linkedlist \
	lists/sortedlinkedlist \
	sorting/countingsort \
	sorting/insertionsort \
	sorting/mergesort \
	sorting/quicksort \
	trees/binarysearchtree \
	trees/btree \
	trees/stringprefixtrie \
	tables/hashtable \
	dynamicprogramming/fibonacci \
	dynamicprogramming/knapsack \
	graphs/depthfirstsearch

all: $(PKGS)
$(PKGS):
	$(MAKE) -C $@

# trees
binarysearchtree:
	$(MAKE) -C trees/binarysearchtree

btree:
	$(MAKE) -C trees/btree





clean:
	for dir in $(PKGS); do \
		$(MAKE) -C $$dir clean; \
	done;

.PHONY: all $(PKGS) clean btree binarysearchtree
