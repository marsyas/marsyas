#!/usr/bin/env python
#
import argparse, re
from marsyas import *
from math import sqrt
from os import path
 
################################################################################
# Parse Command-line Arguments
#
parser = argparse.ArgumentParser(description='Process .wav files to estimate tuning frequency reference.')
parser.add_argument('file', metavar='file', type=str, nargs=1,
                    help='Input .wav file to process.')
parser.add_argument('chords', metavar='chords', type=str, nargs=1,
                    help='Input chord annotations.')
parser.add_argument('outdir', metavar='outdir', type=str, nargs=1,
                    help='Input chord annotations.')
parser.add_argument('--freq', dest='freq', action='store', default='220.0', 
                    help="Initial estimate.")
parser.add_argument('--resonance', dest='q', action='store', default=25, 
                    help="Q for constant-Q filter bank.")
parser.add_argument('--win', dest='win', action='store', default=64, 
                    help="Processing Window Length")
parser.add_argument('--octaves', dest='octaves', action='store', default='4', 
                    help="Number of octaves spanned by filter bank.")
args = parser.parse_args()

################################################################################

def rotateChroma(chroma, chordRoot):
    # Rotations
    rotation = {"A"  : 0, "N"  : 0, "A#" : 1, "Bb" : 1, "B"  : 2,  "C"  : 3,
                "C#" : 4, "Db" : 4, "D"  : 5, "D#" : 6, "Eb" : 6,  "E"  : 7,
                "F"  : 8, "F#" : 9, "Gb" : 9, "G"  : 10,"G#" : 11, "Ab" : 11}
    for i in range(0,rotation[chordRoot]):
        chroma.append(chroma.pop(0))
    return chroma


################################################################################
# Create the Marsyas Networks
#
mng = MarSystemManager()

# Main Analysis Network
#
nwk = mng.create("Series", "analysis")
nwk.addMarSystem(mng.create("SoundFileSource", "src"))
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
nwk.updControl("mrs_natural/inSamples", int(args.win))
nwk.updControl("SoundFileSource/src/mrs_string/filename", args.file[0])
nwk.updControl("Sum/sumobs/mrs_string/mode", "sum_observations")
nwk.updControl("ChromaFilter/dut/mrs_natural/octaves", int(args.octaves));
nwk.updControl("ChromaFilter/dut/mrs_real/freq", float(args.freq));
nwk.updControl("ChromaFilter/dut/mrs_real/q", float(args.q));

# Note selection
for i in range(0,12):
    select_ctrl = nwk.getControl("Fanout/fanout/Series/series_" + str(i) + "/Selector/select_" + str(i) + "/mrs_realvec/enabled").to_realvec();
    for j in range(0,12*int(args.octaves)):
        if (j % 12) == i:
            select_ctrl[j] = 1
        else:
            select_ctrl[j] = 0
    select_ctrl[12*int(args.octaves)] = 0
    nwk.updControl("Fanout/fanout/Series/series_" + str(i) + "/Selector/select_" + str(i) + "/mrs_realvec/enabled", MarControlPtr.from_realvec(select_ctrl));

################################################################################
# Tick the network and extract training data
#
try:
    chordfile = open(args.chords[0], 'r')
except:
    print("Could not open " + args.chords[0] + ".")
    exit(1)

outfilename = path.basename(args.file[0]).replace('.wav', '.arff')
outfile = open(args.outdir[0] + '/' + outfilename, 'w')
outfile.write("@relation " + outfilename + "\n")
outfile.write("@attribute Chroma0 real\n")
outfile.write("@attribute Chroma1 real\n")
outfile.write("@attribute Chroma2 real\n")
outfile.write("@attribute Chroma3 real\n")
outfile.write("@attribute Chroma4 real\n")
outfile.write("@attribute Chroma5 real\n")
outfile.write("@attribute Chroma6 real\n")
outfile.write("@attribute Chroma7 real\n")
outfile.write("@attribute Chroma8 real\n")
outfile.write("@attribute Chroma9 real\n")
outfile.write("@attribute Chroma10 real\n")
outfile.write("@attribute Chroma11 real\n")
outfile.write("@attribute output {N,maj,min,dim,aug,sus2,sus4}\n")
outfile.write("\n")
outfile.write("\n")
outfile.write("@data\n")

[start, end, chord] = chordfile.readline().split()
#print(start + " " + end + " " + chord)
start = float(start)
end = float(end)
chroma = [0]*12
tick = 0
while nwk.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
    nwk.tick()
    tick += 1
    out = nwk.getControl("mrs_realvec/processedData").to_realvec();
    chroma = [x+y for x,y in zip(chroma, out)]
    if (float(tick)*float(args.win) / 44100.0) >= end:
        if len(re.split("[:/]", chord, 1)) == 1:
            chordRoot = chord
            chordType = "maj"
        else:
            [chordRoot, chordType] = re.split("[:/]", chord, 1)

        # MIREX Triad Dictionary
        if chordType not in ["maj", "min", "dim", "aug", "sus2", "sus4"]:
            chordType = "X"
        if chordRoot == "N":
            chordType = "N"

        rotate = rotateChroma(chroma, chordRoot)
        norm = [x/sum(rotate) for x in rotate]
        if chordType != "X":
            outfile.write(','.join([str(x) for x in norm]) + "," + chordType + "\n")

        # Reset the accumulator
        chroma = [0]*12
        [start, end, chord] = chordfile.readline().split()
        start = float(start)
        end = float(end)

outfile.close()
################################################################################
# END OF CODE
################################################################################
