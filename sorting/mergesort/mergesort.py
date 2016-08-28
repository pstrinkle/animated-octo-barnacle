# mergesort prototype

from json import dumps

def mergesort(input, count):

    if count < 2:
        return input

    al = count / 2
    bl = count - al

    a = mergesort(input[:al], al)
    b = mergesort(input[al:], bl)

    c = []

    ai = 0
    bi = 0
    i = 0

    # do the actual sorting.
    while i < count:
        if ai < len(a) and bi < len(b):
            if a[ai] > b[bi]:
                c.append(b[bi])
                bi += 1
            else:
                c.append(a[ai])
                ai += 1
        elif ai < len(a):
            c.append(a[ai])
            ai += 1
        elif bi < len(b):
            c.append(b[bi])
            bi += 1
        i += 1

    return c

def main():

    initial_list = (0, 3, 8, 6, 4, 10, 9, 15, 11)

    print dumps(initial_list)
    print dumps(mergesort(initial_list, len(initial_list)))

    return

if __name__ == "__main__":
    main()
