#!/usr/bin/python

#
#
#

import sys
import os
import datetime
import commands
import re
import time
import simplejson as json
import marsyas
import numpy as np
import scipy.misc.pilutil as smp
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

from optparse import OptionParser

def run(audioFile, outFile, startSec, endSec, lowHz, highHz, winSize, hopSize, spectrumType, widthPx, heightPx):

    # Marsyas network
    mng = marsyas.MarSystemManager()

    net = mng.create("Series","series")
    net.addMarSystem(mng.create("SoundFileSource", "src"))
    net.addMarSystem(mng.create("Stereo2Mono", "s2m"));
    net.addMarSystem(mng.create("ShiftInput", "si"));
    net.addMarSystem(mng.create("Windowing", "win"));
    net.addMarSystem(mng.create("Spectrum","spk"));
    net.addMarSystem(mng.create("PowerSpectrum","pspk"))

    # Update Marsyas controls
    net.updControl("PowerSpectrum/pspk/mrs_string/spectrumType",
                   marsyas.MarControlPtr.from_string(str(spectrumType)))
    net.updControl("SoundFileSource/src/mrs_string/filename",
                   marsyas.MarControlPtr.from_string(audioFile))
    net.updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize)
    net.updControl("ShiftInput/si/mrs_natural/winSize", winSize)
    net.updControl("mrs_natural/inSamples", int(hopSize))

    # Sample rate and samples per tick
    networkSampleRate = net.getControl("mrs_real/osrate").to_real()
    soundFileSampleRate = net.getControl("SoundFileSource/src/mrs_real/osrate").to_real()
    insamples = net.getControl("SoundFileSource/src/mrs_natural/inSamples").to_natural()
    
    # Calculate values
    samplesToSkip = int(soundFileSampleRate * (startSec))
    durationSec = (endSec - startSec)
    ticksToRun = int(durationSec * networkSampleRate)
    _height = winSize / 2

    # Move to the correct position in the file
    net.updControl("SoundFileSource/src/mrs_natural/moveToSamplePos", samplesToSkip)

    # The array to be displayed to the user
    out = np.zeros( (_height,ticksToRun), dtype=np.double )

    # Tick the network until we are done
    for x in range(0,ticksToRun):
        net.tick()
        data = net.getControl("mrs_realvec/processedData").to_realvec()
        for y in range(0,_height):
            out[(_height - y - 1),x] = data[y]

    # Normalize and make black on white    
    out /= np.max(np.abs(out))
    out = 1.0 - out

    nyquist = 44100 / 2.;
    bins = out.shape[0]
    lowBin = int((bins / nyquist) * lowHz);
    highBin = int((bins / nyquist) * highHz);

    halfWinSize = int(hopSize / 2)
    out = out[halfWinSize - highBin:halfWinSize - lowBin, :]

    # Resize and convert the array to an image
    if (heightPx == 0) and (widthPx == 0):
        heightPx = winSize / 2
        widthPx = hopSize * durationSec

    if (heightPx != 0) and (widthPx == 0):
        pxPerItem = int(heightPx) / float(winSize / 2.)
        # TODO(sness) - Why do we have to multiply this by 4?  Check the math above
        widthPx = int(ticksToRun * pxPerItem) * 4

    out = smp.imresize(out,(int(heightPx),int(widthPx)))
    im = smp.toimage(out)
    im.save(outFile, "PNG")

if __name__ == "__main__":
    usage = "usage: %prog [options] in.wav out.png"
    parser = OptionParser(usage)
    
    parser.add_option("-s", "--startSec", dest="startSec", type = float, default = 0., 
                      help="start time in seconds")
    
    parser.add_option("-e", "--endSec", dest="endSec", type = float, default = 1.,
                      help="end time in seconds")
    
    parser.add_option("-1", "--lowHz", dest="lowHz", type = int, default = 0,
                      help="low hertz cutoff for image")
    
    parser.add_option("-2", "--highHz", dest="highHz", type = int, default = 8000,
                      help="high hertz cutoff for image")
    
    parser.add_option("-3", "--winSize", dest="winSize", type = int, default = 1024,
                      help="window size for FFT")
    
    parser.add_option("-4", "--hopSize", dest="hopSize", type = int, default = 1024,
                      help="hop size for FFT")
    
    parser.add_option("-5", "--spectrumType", dest="spectrumType", default = "decibels",
                      help="type of marsyas power spectrum (decibels, magnitude)")
    
    parser.add_option("-6", "--widthPx", dest="widthPx", type = int, default = 0,
                      help="window size for FFT")
    
    parser.add_option("-7", "--heightPx", dest="heightPx", type = int, default = 0,
                      help="window size for FFT")
    
    (options, args) = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)
    
    run(args[0], args[1], options.startSec, options.endSec, options.lowHz, options.highHz,
        options.winSize, options.hopSize, options.spectrumType, options.widthPx, options.heightPx)
