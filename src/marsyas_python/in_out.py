#!/usr/bin/env python

"""a quick hack to demonstrate getting data between python and
   marsyas. """

import sys
import numpy
import pylab

import marsyas
import marsyas_util

#PLOT = True
PLOT = False

def make_input(filename_input):
    series = ["Series/input", ["SoundFileSource/src"]]
    this_net = marsyas_util.create(series)
    this_net.updControl(
        "SoundFileSource/src/mrs_string/filename",
        filename_input)
    return this_net

def make_output(filename_output):
    series = ["Series/output", ["RealvecSource/real_src",
        "SoundFileSink/dest"]]
    this_net = marsyas_util.create(series)
    this_net.updControl("mrs_natural/inSamples", 512)
    this_net.updControl("mrs_real/israte", 44100.0)
    this_net.updControl(
        "SoundFileSink/dest/mrs_string/filename",
        filename_output)
    return this_net



def main():
    try:    
        filename_input = sys.argv[1]
        filename_output = sys.argv[2]
    except:
        print "USAGE: ./in_out.py input_filename.wav output_filename.wav"
        exit(1)
    
    input_net = make_input(filename_input)
    output_net = make_output(filename_output)

    notempty = input_net.getControl("SoundFileSource/src/mrs_bool/hasData")
    input_net_end_control = input_net.getControl("mrs_realvec/processedData")
    output_net_begin_control = output_net.getControl(
        "RealvecSource/real_src/mrs_realvec/data")
    output_net_begin = marsyas.realvec(512)
    while notempty.to_bool():
        ### get input data
        input_net.tick()
        input_net_end = input_net_end_control.to_realvec()

        ### do something with it
        for i in range(input_net_end.getSize()):
            output_net_begin[i] = 0.5*input_net_end[i]
        output_net_begin_control.setValue_realvec(output_net_begin)
        if PLOT:
            pylab.plot(input_net_end, label="input")
            pylab.plot(output_net_begin, label="output")
            pylab.legend()
            pylab.show()

        ### set output data
        output_net.tick()


main()

