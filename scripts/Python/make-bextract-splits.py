#!/global/scratch/sness/openmir/tools/python/bin/python

#
#
#

import sys
import os
import datetime
import commands
import re
import time
import simplejson as json
import random

import pprint
pp = pprint.PrettyPrinter(indent=4)

def run(inFilename,outPrefix,numFolds):
    inFile = open(inFilename, "r")
    line = inFile.readline()
    data = {}
    while line:        
        m = re.search('(.*)\t(.*)', line)
        if m is not None:
            filename = m.group(1)
            label = m.group(2)
            
            if label not in data:
                data[label] = []
                
            data[label].append(filename)
        
        line = inFile.readline()

    # Randomly shuffle folds
    for label in data:
        random.shuffle(data[label])

    # Make new data structure divided into folds
    folds = {}
    for i in range(0,numFolds):
        folds[i] = {}
        for label in data:
            folds[i][label] = []

    # Create folds from data
    fold = 0
    for label in data:
        while data[label]:
            popped = data[label].pop()
            folds[fold][label].append(popped)
            fold += 1
            if fold >= numFolds:
                fold = 0

    # Write data to files
    for i in range(0,numFolds):
        trainFile = open("%s-train-%i" % (outPrefix,i), "w")
        testFile = open("%s-test-%i" % (outPrefix,i), "w")

        for j in range(0,numFolds):
            for label in folds[j]:
                for item in folds[j][label]:
                    if j == i:
                        testFile.write("%s\t%s\n" % (item,label))
                    else:
                        trainFile.write("%s\t%s\n" % (item,label))
                        
        trainFile.close()
        testFile.close()

    
    
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print "Usage: thesis-make-bextract-obv-splits.py bextract.mf prefix- numFolds"
        sys.exit(1)

    inFilename = sys.argv[1]
    outPrefix = sys.argv[2]
    numFolds = int(sys.argv[3])
    run(inFilename,outPrefix,numFolds)
        

