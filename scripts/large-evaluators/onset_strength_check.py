#!/usr/bin/env python

import sys
import glob
import os
import numpy
import pylab


def oss(filename, outdirname):
    basename = os.path.basename(filename)
    outfilename = os.path.join(outdirname,
        os.path.splitext(basename)[0] + "-oss.txt")
    if not os.path.exists(outfilename):
        cmd = "tempo -m TEST_OSS_FLUX %s" % (filename)
        os.system(cmd)
        os.rename("onset_strength.txt", outfilename)
    return outfilename

def score_oss(ground_onsets, outfilename):
    print outfilename
    oss = numpy.loadtxt(outfilename)
    oss = oss[:,2]
    # correction for the 15th order (16 coefficients) FIR filter
    oss = oss[8:]

    ground = numpy.loadtxt(ground_onsets)
    ground_samples = ground * (44100.0/128)
    ground_plot = numpy.zeros(len(oss))
    height = max(oss)
    for gs in ground_samples:
        ground_plot[gs] = height
    pylab.plot(oss)
    pylab.plot(ground_plot)
    pylab.show()


def main():
    indirname = sys.argv[1]
    outdirname = sys.argv[2]
    if not os.path.exists(outdirname):
        os.makedirs(outdirname)
    filenames = glob.glob(os.path.join(indirname, "*.wav"))
    #pairs = []
    for filename in filenames:
    #for filename in filenames[:1]:
        outfilename = oss(filename, outdirname)
        ### assume they're all .wav
        ground_onsets = filename[:-4] + ".txt"
        #pairs.append( (ground_onsets, outfilename) )
        score_oss(ground_onsets, outfilename)
    



main()

