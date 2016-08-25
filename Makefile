
PKGS = lists/linkedlist sorting/countingsort sorting/insertionsort sorting/mergesort trees/binarysearchtree
CLNPKGS = $(PKGS)

all: $(PKGS)
$(PKGS):
	$(MAKE) -C $@

clean:
	for dir in $(PKGS); do \
		$(MAKE) -C $$dir clean; \
	done;

.PHONY: all $(PKGS) clean