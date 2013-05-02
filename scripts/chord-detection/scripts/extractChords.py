#!/usr/bin/env python
#
################################################################################
# Extract chords from a wave file using template-matching approach. Templates
# can be simple binary masks or generated mathematically or learned from other
# sound files.
#
import argparse, re
from marsyas import *
from math import sqrt
from os import path
 
################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Process a .wav file to estimate chord sequence.')

# I/O Options
parser.add_argument('--in', required=True, dest='InputFile', action='store', default=None,
                    help='Input .wav file to process.')
parser.add_argument('--seg', required=True, dest='SegmentFile', action='store', default=None, 
                    help='Input segmentation annotations. Use beat annotations for beat-synchronous chord extraction.')
parser.add_argument('--out', required=True, dest='OutputFile', action='store', default=None,
                    help='Output chord annotations.')

# Chroma Extraction Options
parser.add_argument('--freq', dest='CenterFreq', action='store', default='220.0', 
                    help='Filter bank center frequency.')
parser.add_argument('--resonance', dest='Q', action='store', default=25, 
                    help='Q for constant-Q filter bank.')
parser.add_argument('--octaves', dest='Octaves', action='store', default='4', 
                    help='Number of octaves spanned by filter bank.')

# Processing options
parser.add_argument('--win', dest='WindowLength', action='store', default=64, 
                    help='Marsyas Processing Window Length.')
parser.add_argument('--tmpl', required=True, dest='TemplateFile', action='store', default=None,
                    help='Template library to use.')
parser.add_argument('--trans', dest='MatrixFile', action='store', default=None,
                    help='Transition matrix to use.')

# Debug options
parser.add_argument('-d', dest='dbg', action='store_true', default=False,
                    help='Print debugging info.')

args = parser.parse_args()

################################################################################
# Print debug stuff
#
def dbg(string):
    if args.dbg:
        print(string)

################################################################################
# Read chord templates from file and return dictionary.
#
def readTemplates(templateFile):

    try:
        dbg("Reading chord templates from " + templateFile + ".")
        f = open(templateFile, 'r')
    except:
        print("Could not open " + templateFile + ".")
        exit(-1)

    templates = {}
    for line in f:
        inputList = line.strip().split(',')
        chroma = [float(x) for x in inputList[0:12]]
        key = inputList[12]
        templates[key] = chroma
        dbg("Found chord type '" + key + "' with chroma profile: " + ','.join(inputList[0:12]))
    f.close()
    return templates

################################################################################
# Read chord transition matrix
#
def readMatrix(matrixFile):

    if matrixFile == None:
        return None

    try:
        dbg("Reading transition matrix from " + matrixFile + ".")
        f = open(matrixFile, 'r')
    except:
        print("Could not open " + matrixFile + ".")
        exit(-1)

    matrix = {}
    for line in f:
        inputList = line.strip().split(',')

        key = inputList.pop(0)
        if key not in matrix.keys():
            matrix[key] = {}

        subkey = inputList.pop(0)
        matrix[key][subkey] = [float(x) for x in inputList]

    f.close()

    return matrix

################################################################################
# Read segment annotations and return a list of (start,end) tuples.
#
def readSegments(segmentFile):

    try:
        dbg("Reading segmentation annotations from " + segmentFile + ".")
        f = open(segmentFile, 'r')
    except:
        print("Could not open " + segmentFile + ".")
        exit(-1)

    segments = []
    prevTime = 0.0
    for line in f:
        if line.strip() != '':
            inputList = re.split(' +', line.strip())
            currTime = float(inputList[0])
            if currTime != prevTime:
                segments.append((prevTime,currTime))
                dbg("Segment " + str((prevTime,currTime)) + ".")
            prevTime = currTime
    f.close()
    return segments

################################################################################
# Normalize vectors and then evaluate distance between them.
#
def normDist(vec1, vec2):
    if sum(vec1) != 0:
        normVec1 = [x/sum(vec1) for x in vec1]
    else:
        normVec1 = vec1

    if sum(vec2) != 0:
        normVec2 = [x/sum(vec2) for x in vec2]
    else:
        normVec2 = vec2

    return sqrt(sum([(x-y)**2 for x,y in zip(normVec1, normVec2)]))

################################################################################
# Look up weights in transition Matrix
#
def getTransitionWeight(transitionMatrix, chordRoot, chordType, prevChordRoot, prevChordType):

    if transitionMatrix == None or prevChordRoot == "N" or prevChordType == "N" or prevChordRoot == None or prevChordType == None: 
        return 1.0

    noteIdx = {"A" : 0,  "A#" : 1,  "Bb" : 1, "B" : 2, "Cb" : 2, "B#" : 3, "C" : 3, "C#" : 4, "Db" : 4,
               "D" : 5,  "D#" : 6,  "Eb" : 6, "E" : 7, "Fb" : 7, "E#" : 8, "F" : 8, "F#" : 9, "Gb" : 9,
               "G" : 10, "G#" : 11, "Ab" : 11 }

    dist = noteIdx[chordRoot] - noteIdx[prevChordRoot]
    if dist < 0:
        dist += 12

    dbg("Got a distance of " + str(dist) + " from " + prevChordRoot + ":" + prevChordType + " to " + chordRoot + ":" + chordType)
    dbg("Got a transition value of " + str(transitionMatrix[prevChordType][chordType][dist]))
    
    return transitionMatrix[prevChordType][chordType][dist]

################################################################################
# Estimate chord using distance method
#
def estimateChordDist(chroma, templates, transitionMatrix, prevChordRoot, prevChordType, nCandidates):

    chordInd = ["A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"]
    chordTuples = []
    for i in range(0, 12):
        for key in templates.keys():
            chromaDist = normDist(chroma, templates[key])
            chordTuples.append([chordInd[i], key, chromaDist])
        chroma.append(chroma.pop(0))

    sortedChord = sorted(chordTuples, key=lambda chord: chord[2]) # sort by chroma distance

    candidates = sortedChord[0:nCandidates]

    for candidate in candidates:
        candidate[2] *= getTransitionWeight(transitionMatrix, candidate[0], candidate[1], prevChordRoot, prevChordType)

    sortedCandidates = sorted(candidates, key=lambda candidate: candidate[2]) # sort by chroma distance

    return [sortedCandidates[0][0], sortedCandidates[0][1]]
    
################################################################################
# 
#
def averageChroma(endFrame, fileHandle):
    [frame, avgChroma] = readChromaFrame(fileHandle)
    nFrames = 1
    while (frame < endFrame):
        [frame, chroma] = readChromaFrame(fileHandle)
        avgChroma = [x+y for x,y in zip(avgChroma, chroma)]
        nFrames += 1

    return [x/nFrames for x in avgChroma]

################################################################################
# Create the Marsyas Networks
#
mng = MarSystemManager()

# Main Analysis Network
#
nwk = mng.create("Series", "analysis")
nwk.addMarSystem(mng.create("SoundFileSource", "src"))
nwk.addMarSystem(mng.create("MixToMono", "mono"))
nwk.addMarSystem(mng.create("ChromaFilter", "dut"))
nwk.addMarSystem(mng.create("Square", "square"))
nwk.addMarSystem(mng.create("Sum", "sumobs"))

# Collapse all the octaves
fanout = mng.create("Fanout", "fanout")
for i in range(0,12):
    series = mng.create("Series",   "series_" + str(i))
    series.addMarSystem(mng.create("Selector", "select_" + str(i)))
    series.addMarSystem(mng.create("Sum",      "sum_"    + str(i)))
    fanout.addMarSystem(series)
nwk.addMarSystem(fanout)

################################################################################
# Configure the Marsyas Networks
#
nwk.updControl("mrs_natural/inSamples", int(args.WindowLength))
nwk.updControl("SoundFileSource/src/mrs_string/filename", args.InputFile)
nwk.updControl("Sum/sumobs/mrs_string/mode", "sum_observations")
nwk.updControl("ChromaFilter/dut/mrs_natural/octaves", int(args.Octaves));
nwk.updControl("ChromaFilter/dut/mrs_real/freq", float(args.CenterFreq));
nwk.updControl("ChromaFilter/dut/mrs_real/q", float(args.Q));

# Note selection
for i in range(0,12):
    select_ctrl = nwk.getControl("Fanout/fanout/Series/series_" + str(i) + "/Selector/select_" + str(i) + "/mrs_realvec/enabled").to_realvec();
    for j in range(0,12*int(args.Octaves)):
        if (j % 12) == i:
            select_ctrl[j] = 1
        else:
            select_ctrl[j] = 0
    select_ctrl[12*int(args.Octaves)] = 0
    nwk.updControl("Fanout/fanout/Series/series_" + str(i) + "/Selector/select_" + str(i) + "/mrs_realvec/enabled", MarControlPtr.from_realvec(select_ctrl));

################################################################################
# Setup stuff
#
chordTemplates   = readTemplates(args.TemplateFile)
segmentTimes     = readSegments(args.SegmentFile)
transitionMatrix = readMatrix(args.MatrixFile)
[lastStart,lastEnd] = segmentTimes[-1]
segmentTimes.append((lastEnd,999))

[start,end] = segmentTimes.pop(0)
chroma      = [0]*12
tick        = 0

try:
    dbg("Opening output file " + args.OutputFile + ".")
    outFile = open(args.OutputFile, 'w')
except:
    print("Could not open output file: " + args.OutputFile + ".")
    exit(-1)

################################################################################
# Tick the network to extract data
#
prevChordRoot = None
prevChordType = None
while nwk.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool() and len(segmentTimes):
    nwk.tick()
    tick += 1
    out = nwk.getControl("mrs_realvec/processedData").to_realvec();
    chroma = [x+y for x,y in zip(chroma, out)]
    if (float(tick)*float(args.WindowLength) / 44100.0) >= end:
        [chordRoot, chordType] = estimateChordDist(chroma, chordTemplates, transitionMatrix, prevChordRoot, prevChordType, 3)
        outFile.write(str(start) + " " + str(end) + " " + chordRoot + ":" + chordType + "\n")
        dbg("Detected chord " + chordRoot + ":" + chordType + " on interval " + str(start) + "-" + str(end))

        # Reset the accumulator
        chroma = [0]*12
        [start, end] = segmentTimes.pop(0)
        prevChordRoot = chordRoot
        prevChordType = chordType

outFile.close()
################################################################################
# END OF CODE
################################################################################
