#!/usr/bin/env python

from math import sqrt
import argparse
from marsyas import *
from os import path, mkdir, makedirs
import subprocess

################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Process .chroma files for chord extraction.')
parser.add_argument('files', metavar='files', type=str, nargs='+',
                    help='List of files to extract chroma features from')
parser.add_argument('--output', dest='template', action='store', default='template.dat', 
                    help="Output template file.")
args = parser.parse_args()

################################################################################
# Template Categories from MIREX standard triads.
# 
templates = { "N"    : [0]*12,
              "maj"  : [0]*12,
              "min"  : [0]*12,
              "aug"  : [0]*12,
              "dim"  : [0]*12,
              "sus2" : [0]*12,
              "sus4" : [0]*12 }

################################################################################
# Loop through ARFF files which contain normalized chroma (total chroma = 1) 
# and sum. Then re-normalize.
# 
for filename in args.files:
    f = open(filename, 'r')
    for line in f:
        if line[0] != '@' and line.strip() != '':
            inputList = line.strip().split(',')
            chroma = [float(x) for x in inputList[0:12]]
            chordType = inputList[12]
            templates[chordType] = [x+y for x,y in zip(templates[chordType], chroma)]
    f.close()

################################################################################
# Write results to a file
#
f = open(args.template, 'w')
for key in templates.keys():
    total = sum(templates[key])
    if total > 0:
        templates[key] = [x/total for x in templates[key]]
    f.write(','.join([str(x) for x in templates[key]]) + ',' + key + '\n')
f.close()
            
#
# END OF CODE
#
