# https://www.youtube.com/watch?v=0VqawRl3Xzs

from json import dumps

def countingsort(values):
    """Is a stable sort."""

    output = []
    
    n = len(values)

    # O(n) to find maximum value in input to map range.
    k = max(values)

    # we're could leverage a hashtable for this instead of an array, the C
    # version will use an array.
    counts = []

    for i in range(k+1):
        counts.append(0)

    # there are a few ways to implement this; i've chosen one, but there is 
    # another with which I am familiar but is super pythonic and I wanted 
    # something more general
    # the other variation just loads the entire thing into the counts as a
    # list, and then dumps out.

    # get the counts.    
    for v in values:
        counts[v] += 1
        output.append(0) # to make it an array of the same size as input

    # get the totals so you have counts[i] is total <= i instead of == i.
    # prefix sums.
    total = 0
    for i in range(k+1):
        total += counts[i]
        counts[i] = total

    # start, stop, step
    for i in range(n-1, -1, -1):
        l = values[i] # the value
        output[counts[l]-1] = l
        counts[l] -= 1

    return output

def main():

    # have to use arrays deliberately and not tuples because tuples are 
    # immutable.
    initial_list = [0, 3, 8, 6, 4, 10, 9, 15, 11, 12, 6, 2, 0, 7, 7, 1, 10]
    print dumps(initial_list)

    sorted_list = countingsort(initial_list)
    print dumps(sorted_list)

    return

if __name__ == "__main__":
    main()
