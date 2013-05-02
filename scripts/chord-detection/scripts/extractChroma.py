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
parser.add_argument('outfile', metavar='outfile', type=str, nargs=1,
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
nwk.addMarSystem(mng.create("Transposer", "trans"))
nwk.addMarSystem(mng.create("PlotSink", "sink"))

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

nwk.updControl("PlotSink/sink/mrs_string/filename", args.outfile[0])
nwk.updControl("PlotSink/sink/mrs_bool/sequence", MarControlPtr.from_bool(False))
nwk.updControl("PlotSink/sink/mrs_bool/single_file", MarControlPtr.from_bool(True))


################################################################################
# Tick the network and extract training data
#

while nwk.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
    nwk.tick()

################################################################################
# END OF CODE
################################################################################
