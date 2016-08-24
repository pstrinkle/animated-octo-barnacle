
from json import dumps

class Node(object):
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

    def __repr__(self, *args, **kwargs):
        
        lv = None
        rv = None
        if self.left is not None:
            lv = self.left.value
        if self.right is not None:
            rv = self.right.value
        
        return "%d.left: %s, %d.right: %s" % (self.value, lv, self.value, rv)

class BST(object):
    def __init__(self):
        self.root = None

    @staticmethod
    def _insert(parent, node, value):
        if node is None:
            n = Node(value)

            if parent.value > value:
                parent.left = n
            else:
                parent.right = n

            return

        if node.value > value:
            return BST._insert(node, node.left, value)
        else:
            return BST._insert(node, node.right, value)

    def insert(self, value):
        """Insert the value into the tree."""
        if self.root is None:
            self.root = Node(value)
            return

        if self.root.value > value:
            return BST._insert(self.root, self.root.left, value)
        else:
            return BST._insert(self.root, self.root.right, value)

    @staticmethod
    def _search(node, value):
        if node.value == value:
            return node

        if node.value > value:
            return BST._search(node.left, value)
        else:
            return BST._search(node.right, value)

    def search(self, value):
        """Return the first instance encountered."""
        if self.root is None:
            return None

        return BST._search(self.root, value)

    @staticmethod
    def _findMin(node):
        if node.left is None:
            return node
        return BST._findMin(node.left)

    @staticmethod
    def _findMax(node):
        if node.right is None:
            return node
        return BST._findMax(node.right)

    def _delete(self, parent, node, value):

        # is this the one to delete? (if not, keep searching)
        if node.value != value:
            if node.value > value:
                return self._delete(node, node.left, value)
            else:
                return self._delete(node, node.right, value)

        if node.value == value:
            print "we found it!"

            parentOf = False
            foundLeft = False

            weare = "unknown"
            if parent is not None:
                weare = "left"
                if parent.value < value:
                    weare = "right"

            # is it a leaf (if it's root and a leaf this is handled earlier and won't hit)
            if node.left is None and node.right is None:
                if weare == "right":
                    parent.right = None
                else:
                    parent.left = None
                return

            # ok, it's not a leaf!
            if node.left is None:
                # search the right path for the minimum value (and prune).
                replacement = BST._findMin(node.right)
                if node.right.value == replacement.value:
                    # I was this one's parent.
                    parentOf = True
            else:
                # search the left path for the maximum value (and prune).
                replacement = BST._findMax(node.left)
                if node.left.value == replacement.value:
                    # I was this one's parent.
                    parentOf = True
                foundLeft = True

            # If we were the immediate parent of our replacement, just swap
            # us out.
            if parentOf:
                if parent is not None:
                    if weare == "right":
                        parent.right = replacement
                    else:
                        parent.left = replacement
                else:
                    self.root = replacement

                # if we're moving this one into place we need to preserve the
                # appropriate links.
                if foundLeft:
                    replacement.right = node.right
                else:
                    replacement.left = node.left

                return
            
            # OK, so if we got here, then the replacement wasn't our immediate
            # child.

            # prune the one we just found, if it's a parent, then
            # it must only have one child or we wouldn't have selected it,
            # and then the case above for "parentOf" will be hit to avoid any
            # problems.
            self._delete(parent, node, replacement.value)

            # restore the chain!
            replacement.right = node.right
            replacement.left = node.left

            # displace node.
            if parent is not None:
                if weare == "right":
                    parent.right = replacement
                else:
                    parent.left = replacement
            
            if parent is None:
                self.root = replacement

    def delete(self, value):
        """Just delete the first instance encountered."""

        if self.root is None:
            return

        # special case.
        if self.root.value == value and self.root.left is None and self.root.right is None:
            self.root = None
            return

        return self._delete(None, self.root, value)

    @staticmethod
    def _dump(node):
        if node is None:
            return

        print "node: %s" % node
        if node.left is not None:
            BST._dump(node.left)
        if node.right is not None:
            BST._dump(node.right)

    def dumpIt(self):
        """Print the nodes!!! DFS"""

        if self.root is not None:
            print "root: %d" % self.root.value
        BST._dump(self.root)

def main():

    initial_list = (0, 3, 8, 6, 4, 10, 9, 15, 11)
    print dumps(initial_list)

    b = BST()

    for value in initial_list:
        b.insert(value)

    print "nodes!"
    print b.dumpIt()

    b.delete(8)

    print "nodes!"
    print b.dumpIt()

    b.delete(4)
    
    print "nodes!"
    print b.dumpIt()
    
    b.delete(0)

    print "nodes!"
    print b.dumpIt()

    return

if __name__ == "__main__":
    main()
