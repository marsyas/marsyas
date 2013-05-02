#!/usr/bin/env python
#
################################################################################
import argparse, re
 
################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Compact MIREX triad files.')

# I/O Options
parser.add_argument('--in', required=True, dest='InputFile', action='store', default=None,
                    help='Input .txt file.')
parser.add_argument('--out', required=True, dest='OutputFile', action='store', default=None,
                    help='Output .wav file.')

args = parser.parse_args()

################################################################################
# Do conversion
#
infile = open(args.InputFile, 'r')
outfile = open(args.OutputFile, 'w')

prevChord = None
segStart = None
segEnd = None

for line in infile:
    if line.strip() != '':
        [start, end, chord] = re.split(' +', line.strip())
        if chord != prevChord:
            if prevChord != None:
                outfile.write(segStart + " " + segEnd + " " + prevChord + "\n")
            segStart = start
        prevChord = chord
        segEnd = end

outfile.write(segStart + " " + segEnd + " " + chord + "\n")
infile.close()

################################################################################
# END OF CODE
################################################################################
