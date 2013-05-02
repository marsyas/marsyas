#!/usr/bin/env python

from math import sqrt
import argparse
from marsyas import *
from os import path, mkdir, makedirs

majTriad = [1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0]
augTriad = [1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0]
minTriad = [1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0]
dimTriad = [1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0]
noChord = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
chordInd = ["A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"]

################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Process .chroma files for chord extraction.')
parser.add_argument('chromafile', metavar='chromadir', type=str, nargs=1,
                    help='Chroma directory.')
parser.add_argument('beatfile', metavar='beatdir', type=str, nargs=1,
                    help='Beat Annotation directory.')
args = parser.parse_args()

def correlate(vec1, vec2):
    return sum([x*y for x,y in zip(vec1, vec2)])

def estimateChordCorr(chroma):

    maxCor = -1;
    maxChord = None;
    for i in range(0, 12):
#        a = [correlate(chroma, majTriad), correlate(chroma, augTriad),
#             correlate(chroma, minTriad), correlate(chroma, dimTriad)]
        a = [correlate(chroma, majTriad), correlate(chroma, minTriad), correlate(chroma, dimTriad)]
        if max(a) > maxCor:
            maxCor = max(a)
            if a.index(maxCor) == 0:
                maxChord = chordInd[i] + ":maj"
            elif a.index(maxCor) == 1:
                maxChord = chordInd[i] + ":min"
            elif a.index(maxCor) == 2:
                maxChord = chordInd[i] + ":dim"
            else:
                maxChord = chordInd[i] + ":aug"
        chroma.append(chroma.pop(0))
    return maxChord

def dist(vec1, vec2):
    normVec1 = [x/sum(vec1) for x in vec1]
    normVec2 = [x/sum(vec2) for x in vec2]
    return sqrt(sum([(x-y)**2 for x,y in zip(normVec1, normVec2)]))

def estimateChordDist(chroma):

    minCor = 999;
    minChord = None;
    for i in range(0, 12):
#        a = [dist(chroma, majTriad), dist(chroma, augTriad),
#             dist(chroma, minTriad), dist(chroma, dimTriad)]
        a = [dist(chroma, majTriad), dist(chroma, minTriad)]
        if min(a) < minCor:
            minCor = min(a)
            if a.index(minCor) == 0:
                minChord = chordInd[i] + ":maj"
            elif a.index(minCor) == 1:
                minChord = chordInd[i] + ":min"
            elif a.index(minCor) == 2:
                minChord = chordInd[i] + ":aug"
            else:
                minChord = chordInd[i] + ":dim"
        chroma.append(chroma.pop(0))
    return minChord
    
def readChromaFrame(fileHandle):

    chroma = [None] * 12
    frame = None
    for i in range(0, 12): # 12 lines of chroma.
        lineIn = fileHandle.readline().split()
        frame  = int(lineIn[0])
        chroma[int(lineIn[1])] = float(lineIn[2])
    fileHandle.readline(); # Blank Line.
    return [frame, chroma]

def averageChroma(endFrame, fileHandle):
    [frame, avgChroma] = readChromaFrame(fileHandle)
    nFrames = 1
    while (frame < endFrame):
        [frame, chroma] = readChromaFrame(fileHandle)
        avgChroma = [x+y for x,y in zip(avgChroma, chroma)]
        nFrames += 1

    return [x/nFrames for x in avgChroma]


chromaData = open(args.chromafile[0])
beatData   = open(args.beatfile[0])
    
prevTime = float(beatData.readline().split()[0])
prevChord = None

for line in beatData:
    beatTime = float(line.split()[0])
    beatSample = int(beatTime * 44100.0)
    [sample, accum] = readChromaFrame(chromaData)
    while sample < beatSample:
        [sample, chroma] = readChromaFrame(chromaData)
        accum = [x+y for x,y in zip(accum, chroma)]
        print(str(prevTime) + " " + str(beatTime) + " " + estimateChordCorr(accum))
        prevTime = beatTime

chromaData.close()
beatData.close()


