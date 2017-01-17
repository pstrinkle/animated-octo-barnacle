# octo-barnacle
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
* graphs
  * breadth first search
  * representation
  * minimum spanning trees (yay, greedy)
* trees
  * B-tree **in progress**
  * red-black tree
  * AVL tree (maybe)
  * splay tree (maybe)
* lists
  * skip lists
* dynamic programming
  * edit distance
  * longest common sequence

# Implemented

* sorting
  * merge sort
  * insertion sort
  * counting sort
  * quicksort
* trees
  * binary search tree
  * trie
* graphs
  * depth first search (done with bst)
* lists
  * linked lists
  * sorted linked list
* tables
  * hashtable
* dynamic programming
  * fibonacci
  * knapsack (0-1)

# Notes
|sorting algorithm| Best Case | Average Case | Worst Case | Notes |
|---|:---------:|:------------:|:----------:|-------|
|mergesort| `O(nlgn)` | `O(nlgn)` | `O(nlgn)` | It builds the new array separately using `O(n)` space|
|insertion sort| `O(n)` | `O(n**2)` | `O(n**2)` | If the list is sorted or mostly this is fast.|
|counting sort| `O(n+k)` | `O(n+k)` | `O(n+k)` | `k` is highest value possible, because it builds the counting table|
|quicksort| `O(nlgn)` | `O(nlgn)` | `O(n**2)` | You can force `O(nlgn)` worst case by randomizing the input.  Traditionally isn't great on sorted input, however, by different partition schemes you can get it to `O(n)` if presorted.|

|data structure| to build | insert | search | delete | Notes|
|---|:--------:|:------:|:------:|:------:|------|
|bst | `O(nlgn)` | `O(lgn)` | `O(lgn)` | `O(lgn)` | This tree can break down to `O(n)` search performance if the input is sorted, and `O(n**2)` to build it in initially |
|b-tree| `O(nlogn)` | `O(logn)` | `O(logn)` | `O(logn)` | The base of the logarithm is the maximum children per block.|
|trie | `O(nm)` | `O(m)` | `O(m)` | `O(m)` | `m` is the item length in pieces. | 
|linked list|`O(n)` | `O(1)` | `O(n)` | `O(1)` | Building the list requires simply appending or prepending items, inserting is therefore constant, however, deleting assumes you've already found it, so it's constant time. |
|sorted linked list|`O(n**2)`| `O(n)` | `O(n)` | `O(1)` | Every node you insert might need to go to the end, there are ways to optimize against sorted input such as using a doubly-linked list and keeping track of the median value.|
|hashtable| `O(n+m)` | `O(1)` | `O(1)` | `O(1)` | `m` is the size of the table.  These really are amortized values because occasionally we'll need to grow or shrink the table.  Also if we're using a data structure to handle collisions, there can be some extra work there but it can be kept minimal by a good hashing function.|

# Study
* need to review network flow
* need to review hardware requirements

