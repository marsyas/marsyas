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
import math

#
# Normalize each element in the list to the range of 0 to 1
#
# sness - There is a more elegant way to do this, something similar to:
# min_list /= np.max(np.abs(min_list),axis=0)
#
def normalize_list(input):
    output = []
    a = np.array(input)
    min = a.min()
    max = a.max()
    for m in input:
        result = (m - min) / (max-min)
        #if math.isnan(result):
        #    result = 0
        output.append(result)

    return output

if len(sys.argv) != 3:
    print "Usage: icme2011_stats_from_pitch_contour.py pitch_contours.mf output_file"
    exit(0);

#
# The path that we're going to look for text files in
#    
collection_filename = sys.argv[1]
output_filename = sys.argv[2]

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

#print all_data    
#
# Calculate min, max, median, mean, std. dev of all files
#
min_list = []
max_list = []
median_list = []
mean_list = []
std_list = []
for n in all_data:
    min_list.append(np.min(n))
    max_list.append(np.max(n))
    median_list.append(np.median(n))
    mean_list.append(np.mean(n))
    std_list.append(np.std(n))

#    
# Normalize all data to the range 0..1
#
norm_min_list = normalize_list(min_list)
norm_max_list = normalize_list(max_list)
norm_mean_list = normalize_list(mean_list)
norm_median_list = normalize_list(median_list)
norm_std_list = normalize_list(std_list)
#
# Reconstruct the original data with these new normalized values
#
all_normed_data = []
for i in range(0,len(norm_min_list)):
    a = np.array([norm_min_list[i],
                 norm_max_list[i],
                 norm_median_list[i],
                 norm_mean_list[i],
                 norm_std_list[i]])
    all_normed_data.append(a)

#
# Calculate the distance between all pairs and write to a file
#
output = open(output_filename, 'w')
for i in range(0,len(all_normed_data)):
    output.write("%s " % os.path.basename(filenames[i]))
    for j in range(0,len(all_normed_data)):
        dist = np.linalg.norm(all_normed_data[i]-all_normed_data[j])
        output.write("%f " % (dist))
    output.write("\n")
