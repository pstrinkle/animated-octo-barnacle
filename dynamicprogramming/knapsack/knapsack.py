# This algorithm doesn't tell you what was placed in the bag, only its maximum
# value.  To get the items in the list, you can store whether the maximum for
# the call included the current item.

from json import dumps
from random import randint

MINWEIGHT = 1
MAXWEIGHT = 100
MINVALUE = 10
MAXVALUE = 1000

values = []
weights = []

def knapsack(item, remaining):
    """Returns the maximum value; but not what is in the knapsack."""

    global values
    global weights

    # last case where it's called, no value, no weight cost.
    if item == len(values):
        return 0

    if weights[item] > remaining:
        return knapsack(item+1, remaining)
    else:
        return max(knapsack(item+1, remaining),
                   knapsack(item+1, remaining-weights[item]) + values[item])

def main():
    global values
    global weights
    global included

    for i in range(10):
        weights.append(randint(MINWEIGHT, MAXWEIGHT))
    for i in range(10):
        values.append(randint(MINVALUE, MAXVALUE))

    knapsack_weight = 250
    maximum_value = knapsack(0, knapsack_weight)
    print "maximum value: %d" % maximum_value
    print "weights: %s" % dumps(weights)
    print "values: %s" % dumps(values)
    
if __name__ == "__main__":
    main()
