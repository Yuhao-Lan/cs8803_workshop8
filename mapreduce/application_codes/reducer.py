#!/usr/bin/env python
import sys
from itertools import groupby
from operator import itemgetter
 
 
def read_from_mapper(file, separator):
    for line in file:
        yield line.strip().split(separator, 2)
 
def main(separator = '\t'):
    data = read_from_mapper(sys.stdin, separator)
    for current_word, group in groupby(data, itemgetter(0)):
        try:
            total_count = sum(int(count) for current_word, count in group)
            print "%s%s%d" % (current_word, separator, total_count)
        except ValueError:
            pass
 
if __name__ == '__main__':
    main()