#!/usr/bin/python

#
# Read in a collection file of pitch contours in .txt format and linearlly interpolate
# them to have 100 values
#

import sys
import os
import datetime
import commands
import re
import numpy as np

if len(sys.argv) != 4:
    print "Usage: icme2011_linear_interpolate_pitch_contour.py pitch_contours.mf output_num_vals output_dir"
    exit(0);

#
# The path that we're going to look for text files in
#    
collection_filename = sys.argv[1]
output_num_vals = int(sys.argv[2])
output_dir = sys.argv[3]

# A list of the filenames read in
filenames = []

#
# Loop over all files in collection_file
#
collection_file = open(collection_filename, 'r')
line = collection_file.readline()
while line:
    filenames.append(line.strip())
    line = collection_file.readline()

#
# Read in all the values from this into a numpy array
#
all_data = []
for filename in filenames:
    # A normal python array to read the data into
    data = []

    # Open the file and read all the lines into data
    file = open(filename, "r")
    line = file.readline()
    while line:
        data.append(float(line))
        line = file.readline()

    a = np.array(data)
    all_data.append(a)

#
# Linearly interpolate the values in each element of all_data to
# output_num_vals values
#
for i in range(0,len(all_data)):
    x = np.linspace(0,1.0,len(all_data[i]))
    y = np.array(all_data[i])
    xvals = np.linspace(0,1.0,output_num_vals)
    yinterp = np.interp(xvals,x,y)

    # Write all data to the output file
    output_filename = "%s/%s" % (output_dir,os.path.basename(filenames[i]))
    print "output_filename=%s" % output_filename
    output = open(output_filename, 'w')
    for n in yinterp:
        output.write("%f\n" % n)

