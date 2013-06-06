#!/usr/bin/env python

import sys
filename = sys.argv[1]

lines = open(filename).readlines()

print "    const mrs_real svm_weights[] = {"
print "        ",
count = 0
for line in lines:
    sl = line.split()
    if len(sl) < 2:
        continue
    if sl[0] == '+':
        val = sl[1]
    else:
        val = sl[0]
    if len(sl) == 2:
        print "0,\n    };"
        print "    double svm_sum = %s;" % (val)
    else:
        print "%s," % (val),
        count += 1
        if count % 4 == 0:
            print
            print "        ",





