# animated-octo-barnacle
Some fun in C and Python (standard algorithms and data structures)

In an attempt to just practice implementing algorithms and data structure with which I am familiar, but likely haven't implemented in 
a sufficiently long time, I've created this repository to hold them as examples.

I could use pyunit or cunit to verify the algorithms as I go, but that's not the
point --- this isn't formal and I'm not following deliberate TDD, but rather 
just verifying quickly the output.  So, if I was doing this for more than fun,
I'd have tests verifying each case and then changes would go through tests to
verify I didn't introduce a regression --- provided my test coverage was good.

# Planned algorithms and data structures
* sorting
  * optimized merge sort (just use insertion towards the bottom)
  * heap sort (?maybe) 
* trees
  * breadth first search
  * depth first search
* graphs
  * representation
  * minimum spanning trees (yay, greedy)
* trees
  * B-variant tree **in progress**
  * AVL tree (maybe)
  * red-black tree
  * trie
  * splay tree (maybe)
* lists
  * skipped linked lists
* dynamic programming
  * edit distance
  * longest common sequence

# Implemented
* sorting
  * merge sort
  * insertion sort
  * counting sort
  * quicksort (randomized quicksort forces average runtime ``O(n*logn)`` instead of ``O(n**2)`` worst case)
* trees
  * binary search tree
* lists
  * linked lists
  * sorted linked list
* dynamic programming
  * fibonacci
  * knapsack (0-1)

# Study
* need to review network flow
* need to review hardware requirements

