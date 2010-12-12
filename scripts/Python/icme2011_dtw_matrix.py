#!/usr/bin/python

#
# Run pitchdtw on all .txt files in this directory, and generate a
# matrix from this
#

import sys
import os
import datetime
import commands
import re

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
# Calculate the DTW distance between each pair of files
#
results = []    
for i in range(0,len(files)):
    results.append([])
    for j in range(0,len(files)):
        command = "pitchdtw %s/%s.txt %s/%s.txt" % (path, files[i], path, files[j])
        print command
        a = commands.getoutput(command)
        print a
        results[i].append(a)

#    
# Print the results
#
# print "|%11s" % "",
# for i in range(0,len(files)):
#     print "|%11s" % files[i],
# print "|"

# for i in range(0,len(files)):
#     print "|%11s" % files[i],
#     for j in range(0,len(files)):
#         print "|%11.0f" % float(results[i][j]),
#     print "|"

output_file = open(output_filename, 'w')

for i in range(0,len(files)):
    output_file.write("%s" % files[i],)
    for j in range(0,len(files)):
        output_file.write(" %.1f" % float(results[i][j]),)
    output_file.write("\n")
