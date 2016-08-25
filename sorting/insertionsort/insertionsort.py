
from json import dumps

def insertionsort(values):    
    # start at 1st entry.
    for i in range(1, len(values)):
        j = i
        while j > 0:
            if values[j] < values[j-1]:
                x = values[j-1]
                values[j-1] = values[j]
                values[j] = x                    
            j -= 1

def main():

    # have to use arrays deliberately and not tuples because tuples are 
    # immutable.
    initial_list = [0, 3, 8, 6, 4, 10, 9, 15, 11]
    print dumps(initial_list)
    insertionsort(initial_list)
    print dumps(initial_list)
    
    second_list = [16, 3, 8, 6, 4, 10, 9, 15, 11]
    print dumps(second_list)
    insertionsort(second_list)
    print dumps(second_list)

    return

if __name__ == "__main__":
    main()
