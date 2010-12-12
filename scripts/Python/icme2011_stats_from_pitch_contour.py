#!/usr/bin/python

#
# Calculate the stats of a pitch contour txt file
# 
# Stats include median, mean, min, max, std dev.
#

import sys
import os
import datetime
import commands
import re
import numpy as np

if len(sys.argv) != 3:
    print "Usage: icme2011_stats_from_pitch_contour.py path output_file"
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

output = open(output_filename, 'w')

output.write("@relation pitch_contour\n")
output.write("@attribute min real\n")
output.write("@attribute max real\n")
output.write("@attribute mean real\n")
output.write("@attribute median real\n")
output.write("@attribute std real\n")
output.write("@attribute output {N1,N3,N4,N47,N79}\n")
output.write("\n")
output.write("\n")
output.write("@data\n")
totals = {}
totals['N1'] = 0
totals['N3'] = 0
totals['N4'] = 0
totals['N47'] = 0
totals['N79'] = 0
for i in range(0,len(files)):
    m = re.search('([N0-9]*)', files[i])
    call = m.group(1)
    if (call == "N7" or call == "N9"):
        call = "N79"
    totals[call] += 1
    if (totals[call] < 30) :
        output.write("%f,%f,%f,%f,%f,%s\n" % (np.min(all_data[i]),
                                              np.max(all_data[i]),
                                              np.mean(all_data[i]),
                                              np.median(all_data[i]),
                                              np.std(all_data[i]),
                                              call))

