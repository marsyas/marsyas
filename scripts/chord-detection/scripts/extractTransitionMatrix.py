#!/usr/bin/env python

import argparse, re, copy

################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Extract chord transition matrix from ground truth')
parser.add_argument('filelist', metavar='filelist', type=str, nargs=1,
                    help='List of files to extract transitions from')
parser.add_argument('--output', dest='matrix', action='store', default='matrix.dat', 
                    help="Output transition matrix.")
parser.add_argument('-d', dest='dbg', action='store_true', default=False, help="Debug Info..")
args = parser.parse_args()

################################################################################
# Print debug stuff
#
def dbg(string):
    if args.dbg:
        print(string)

################################################################################
# Template Categories from MIREX standard triads.
# 
templates = { "maj"  : [0]*12,
              "min"  : [0]*12,
              "aug"  : [0]*12,
              "dim"  : [0]*12,
              "sus2" : [0]*12,
              "sus4" : [0]*12 }

################################################################################
# Initialize the data structure
# 
noteIdx = {"A" : 0,  "A#" : 1,  "Bb" : 1, "B" : 2, "Cb" : 2, "B#" : 3, "C" : 3, "C#" : 4, "Db" : 4,
           "D" : 5,  "D#" : 6,  "Eb" : 6, "E" : 7, "Fb" : 7, "E#" : 8, "F" : 8, "F#" : 9, "Gb" : 9,
           "G" : 10, "G#" : 11, "Ab" : 11 }

matrix = { "maj"  : copy.deepcopy(templates),
           "min"  : copy.deepcopy(templates),
           "aug"  : copy.deepcopy(templates),
           "dim"  : copy.deepcopy(templates),
           "sus2" : copy.deepcopy(templates),
           "sus4" : copy.deepcopy(templates) }

################################################################################
# Loop through the ground truth files in the list
# 
filelist = open(args.filelist[0], 'r')
prevChordRoot = None
prevChordType = None
for line in filelist:
    filename = line.strip() + ".txt" # Ground truth = wav file + .txt
    dbg("Extracting chord transitions from " + filename + ".")
    groundtruth = open(filename, 'r')
    for chordLine in groundtruth:
        if chordLine.strip() != '':
            [start, end, chord] = re.split(' +', chordLine.strip())
            start = float(start)
            end = float(end)
    
            chordInputList = re.split('[:/]', chord, 1)
            if len(chordInputList) == 1:
                chordRoot = chord
                if chord == "N":
                    chordType = "N"
                else:
                    chordType = "maj"
            else:
                chordRoot = chordInputList[0]
                chordType = chordInputList[1]
    
            if chordRoot != 'X' and chordType in matrix.keys() and prevChordType != None and chordType != 'N':
                dist = noteIdx[chordRoot] - noteIdx[prevChordRoot]
                if dist < 0:
                    dist += 12
                matrix[prevChordType][chordType][dist] += 1
            
            if chordRoot == 'X' or chordType == 'N' or chordType not in matrix.keys():
                prevChordRoot = None
                prevChordType = None
            else:
                prevChordRoot = chordRoot
                prevChordType = chordType
            
    groundtruth.close()
filelist.close()

outfile = open(args.matrix, 'w')
for key in matrix.keys():
    total = 0
    for subkey in matrix[key].keys():
        if subkey == key:
            matrix[key][subkey][0] = 0
            
        total += sum(matrix[key][subkey])

    for subkey in matrix[key].keys():
        if total != 0:
            matrix[key][subkey] = [1.0-float(x)/float(total) for x in matrix[key][subkey]]
        else:
            matrix[key][subkey] = [1.0]*12
        
        outfile.write(','.join([key, subkey] + [str(x) for x in matrix[key][subkey]]) + '\n')

outfile.close()

#
# END OF CODE
#
