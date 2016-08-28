
from json import dumps

# my first attempt at implementing this for myself as I typically don't have
# many problems in the space of sorting.

# I wonder if there's some minimum length, I guess 2, but ... if one is the
# pivot will it work for only two items? or is sorting two items too trivial
# for this. --
#
# Also, I really prefer mergesort to this; although randomized quicksort gets
# beyond the sorted input problem.

def partition(section, low, high):
    """Partition step of quickstep (the meat of it, does the sorting).
    
    The pivot point is selected as the median of three, because this has good
    performance with already sorted data -- and ya know, most of the time you 
    need to sort something, it's mostly sorted.

    # this doesn't work if only two pieces.
    zeroth = section[low]
    last = section[high]
    middle = section[(high-low)/2]

    # if last >= zeroth >= middle
    if last >= zeroth and zeroth >= middle:
        pivot = zeroth
    # if middle >= zeroth >= last
    elif middle >= zeroth and zeroth >= last:
        pivot = zeroth
    # if zeroth >= last >= middle
    elif zeroth >= last and last >= middle:
        pivot = last
    # if middle >= last >= zeroth
    elif middle >= last and last >= zeroth:
        pivot = last
    # if zeroth >= middle >= last
    elif zeroth >= middle and middle >= last:
        pivot = middle
    # if last >= middle >= zeroth
    elif last >= middle and middle >= zeroth:
        pivot = middle

    # the above is wonky and likely not the best approach, so let's start with
    # just choosing low.
    """

    pivot = section[low]
    left = low+1
    right = high

    # left and right move towards each other until they flip.
    while left < right:
        # while we can go right and it's less than the pivot.
        while left <= right and section[left] < pivot:
            left += 1
        while right >= left and section[right] > pivot:
            right -= 1

        # the values should be swapped.
        if left < right:
            tmp = section[left]
            section[left] = section[right]
            section[right] = tmp

    # move pivot into place.
    section[low] = section[right]
    section[right] = pivot
    return right

def quicksort(input, low, high):

    if low < high:
        pivot = partition(input, low, high)

        # quicksort left and right portion.
        quicksort(input, low, pivot-1)
        quicksort(input, pivot+1, high)

    return

def main():

    initial_list = [0, 3, 8, 6, 4, 10, 9, 15, 11]

    print "unsorted: "
    print dumps(initial_list)
    print "sorted: "
    quicksort(initial_list, 0, len(initial_list)-1)
    print dumps(initial_list)

    return

if __name__ == "__main__":
    main()