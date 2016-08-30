
from json import dumps

RED = 1
BLACK = 0

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

def main():

    root = None

    return

if __name__ == "__main__":
    main()
