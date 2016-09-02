
from json import dumps

RED = 1
BLACK = 0

def color(value):
    if value == RED:
        return "red"
    else:
        return "black"

class Node(object):
    """Red-black Tree nodes."""

    def __init__(self, value):
        self.value = value
        self.color = RED

        # node less than
        self.left = None
        # node greater than
        self.right = None

        # parent pointer for convenience (back-link)
        self.parent = None

        return

def insert(root, node):
    """Insert a node."""

    return

def depthFirstPrint(node):
    if self.parent is None:
        # root
        print "root: ",
    else:
        print "node: ",
    
    left = "nil"
    right = "nil"

    if self.left is not None:
        left = self.left.value
    if self.right is not None:
        right = self.right.value

    print "%d color: %s, left: %d, right: %d" \
            % (self.value, color(self.color), left, right)

    if self.left:
        depthFirstPrint(self.left)
    if self.right:
        depthFirstPrint(self.right)

def main():

    root = None

    return

if __name__ == "__main__":
    main()
