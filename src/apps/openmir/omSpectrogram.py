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

def spectrogram(audioFile, startSec, endSec, lowHz, highHz, winSize, hopSize,
                spectrumType, width, height):

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
    if (height == "native") and (width == "native"):
        height = winSize / 2
        width = hopSize * durationSec

    if (height != "native") and (width == "native"):
        pxPerItem = int(height) / float(winSize / 2.)
        # TODO(sness) - Why do we have to multiply this by 4?  Check the math above
        width = int(ticksToRun * pxPerItem) * 4

    out = smp.imresize(out,(int(height),int(width)))
    im = smp.toimage(out)

    im.save('test.png', "PNG")
    
    return response

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print "Usage: omSpectrogram.py outFile audioFile startSec endSec lowHz highHz winSize hopSize spectrumType width height"
        sys.exit(1)
        
    run("test.png", "test.wav", 0, 1, 0, 8000, 1024, 1024, "magnitude", 100, 100)
