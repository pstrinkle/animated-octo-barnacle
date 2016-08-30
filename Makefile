
PKGS = lists/linkedlist \
	lists/sortedlinkedlist \
	sorting/countingsort \
	sorting/insertionsort \
	sorting/mergesort \
	sorting/quicksort \
	trees/binarysearchtree \
	tables/hashtable \
	dynamicprogramming/fibonacci \
	dynamicprogramming/knapsack

all: $(PKGS)
$(PKGS):
	$(MAKE) -C $@

clean:
	for dir in $(PKGS); do \
		$(MAKE) -C $$dir clean; \
	done;

.PHONY: all $(PKGS) clean
