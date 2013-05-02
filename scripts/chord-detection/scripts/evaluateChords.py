#!/usr/bin/env python

import argparse, re

################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Evaluate chord annotations.')
parser.add_argument('chordfile', metavar='chordfile', type=str, nargs=1,
                    help='File containing annotated chords.')
parser.add_argument('groundtruth', metavar='groundtruth', type=str, nargs=1,
                    help='Ground truth annotations.')
parser.add_argument('-d', dest='dbg', action='store_true', default=False,
                    help='Debugging info.')
parser.add_argument('--report', dest='report', action='store', default=None,
                    help='Output comparison report.')
args = parser.parse_args()

################################################################################
# Print debug stuff
#
def dbg(string):
    if args.dbg:
        print(string)

################################################################################
# Return a string defining the chord.
#
def toPitchClassString(chord):
    rotation = {"A"  : 0, "N"  : 0, "A#" : 1, "Bb" : 1, "B"  : 2,  "Cb" : 2, "B#" : 3, "C"  : 3,
                "C#" : 4, "Db" : 4, "D"  : 5, "D#" : 6, "Eb" : 6,  "E"  : 7, "Fb" : 7, "E#" : 8, 
                "F"  : 8, "F#" : 9, "Gb" : 9, "G"  : 10,"G#" : 11, "Ab" : 11}
    templates = { "maj"  : [1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0],
                  "min"  : [1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0],
                  "aug"  : [1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0],
                  "dim"  : [1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0],
                  "sus2" : [1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0],
                  "sus4" : [1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0],
                  "N"    : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] }
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

    if chordType in templates.keys() and chordRoot != "X":
        chordList = templates[chordType]
        chordList = chordList[-1*rotation[chordRoot]:] + chordList[:-1*rotation[chordRoot]]
        return (chord,''.join([str(x) for x in chordList]))
    else:
        return (chord,"X")

################################################################################
# Use method described in MIREX Chord Estimation Task: 
# - Resample ground truth and annotations at 10ms interval
# - Convert to pitch class 
# - Compare
def getChordList(chordFile):
    f = open(chordFile, 'r')
    curTime = None
    chordList = []
    for line in f:
        if line.strip() != '':
            [start, end, chord] = line.strip().split();
            start = float(start)
            end = float(end)
            if curTime == None:
                curTime = round(start * 100.0)/100.0 # Nearest 10ms
            while curTime < end:
                curTime += 0.01
                chordList.append(toPitchClassString(chord))
    return chordList

def getMatchingNotes(str1, str2):
    str1bin = int(str1, 2)
    str2bin = int(str2, 2)
    return str.count(str(bin(str1bin&str2bin)[2:]), '1')

def compareChordList(chordList1, chordList2):    
    match = [0]*4
    rol   = 0;
    ror   = 0;
    total = 0;
    for i in range(0, min(len(chordList1), len(chordList2))):
        if chordList1[i][1] != "X" and chordList2[i][1] != "X":
            total += 1
            if chordList1[i][1] == chordList2[i][1]:
                match[3] += 1
            else:
                match[getMatchingNotes(chordList1[i][1], chordList2[i][1])] += 1
            tmp = chordList2[i][1]
            if chordList1[i][1] == (tmp[-1] + tmp[0:-1]):
                rol += 1
            if chordList1[i][1] == (tmp[1:] + tmp[0]):
                ror += 1
        dbg(str(chordList1[i]) + "," + str(chordList2[i]))
    return [match[3], match[2], match[1], match[0], rol, ror, total]

################################################################################
# Read Files and compare..
list1 = getChordList(args.chordfile[0])
list2 = getChordList(args.groundtruth[0])
[three, two, one, zero, rol, ror, total] = compareChordList(list1, list2)
print(str(three) + "," + str(two) + "," + str(one) + "," + str(zero) + "," + str(total) + "," + str(rol) + "," + str(ror))

if args.report:
    reportFile = open(args.report, 'w')
    for i in range(0,min(len(list1),len(list2))):
        reportFile.write(str(i*0.01) + " " + list1[i][0] + " " + list1[i][1] + " " + list2[i][0] + " " + list2[i][1] + "\n")
    
    reportFile.close()


                                    
