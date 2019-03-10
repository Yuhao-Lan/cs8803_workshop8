#!/usr/bin/env python
import sys
def read_from_input(file):
    for line in file:
        yield line.split(' ')
 
def main(separator = ' '):
    data = read_from_input(sys.stdin)
    for words in data:
        for word in words:
            # write to the reduce
            print '%s%s%d' % (word, '\t', 1)
if __name__ == '__main__':
    main()