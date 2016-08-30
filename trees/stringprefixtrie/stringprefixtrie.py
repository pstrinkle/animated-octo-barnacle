
from json import dumps

class StringPrefixTrie(object):
    """Implemented via hash tables here."""

    def __init__(self):
        self.root = {}

        return

    def search(self, key):
        """Search the prefix trie for key."""

        node = self.root

        for i in range(len(key)):
            letter = key[i]
            if letter in node:
                pass
            else:
                node[letter] = {}

            if i == len(key)-1:
                if 'words' in node[letter]:
                    if key in node[letter]['words']:
                        return True
                else:
                    return False

            node = node[letter]

        return False

    def dumpit(self):
        """Print it all out."""

        print dumps(self.root, indent=2)

    def insert(self, key):
        """Insert the key into the prefix trie.
        
        This does zero prefix compression, which it could, which would 
        compress it a bit, so likely something I'll do later.  Prefix
        compression is costly if the trie is still being updated regularly,
        because you'll need to undo the compression from time to time.
        """

        # walk each letter, going deeper into the trie.
        node = self.root

        for i in range(len(key)):
            letter = key[i]
            if letter in node:
                pass
            else:
                node[letter] = {}

            if i == len(key)-1:
                if 'words' in node[letter]:
                    node[letter]['words'].append(key)
                else:
                    node[letter] = {'words' : [key]}

            node = node[letter]

        return
        
def main():

    s = StringPrefixTrie()

    words = ('asdf', 'asde', 'as', 'tea', 'ted', 'ten', 'qwerty', 'qwe',
             'what', 'how')
    for word in words:
        s.insert(word)

    print s.search('asdf')

    return

if __name__ == "__main__":
    main()
