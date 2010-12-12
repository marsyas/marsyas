#!/usr/bin/python

#
# Run DTW on all .txt files in this directory, and generate a
# matrix from this
#

import sys
import os
import datetime
import commands
import re
import numpy as np
import matplotlib.pyplot as plt
import mlpy

if len(sys.argv) != 3:
    print "Usage: icme2001_dtw_matrix.py path output_file"
    exit(0);

#
# The path that we're going to look for text files in
#    
path = sys.argv[1]
output_filename = sys.argv[2]

#
# Make a list of all the text files in path
#
files = []    
for f in os.listdir(path):
    files.append(f[0:-4])

#
# Read in all the values from this into a numpy array
#
all_data = []    
for i in range(0,len(files)):
    # A normal python array to read the data into
    data = []

    # Open the file and read all the lines into data
    filename = "%s/%s.txt" % (path, files[i])
    file = open(filename, "r")
    line = file.readline()
    while line:
        data.append(float(line))
        line = file.readline()

    a = np.array(data)
    all_data.append(a)

dtw = mlpy.Dtw(onlydist=False)

results = []    
for i in range(0,len(all_data)):
    results.append([])
    for j in range(0,len(all_data)):
        a = dtw.compute(all_data[i], all_data[j])
        results[i].append(a)

output_file = open(output_filename, 'w')

for i in range(0,len(files)):
    output_file.write("%s" % files[i],)
    for j in range(0,len(files)):
        output_file.write(" %.1f" % float(results[i][j]),)
    output_file.write("\n")
        


