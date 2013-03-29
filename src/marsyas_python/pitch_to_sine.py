#!/usr/bin/python

import marsyas
import sys
import numpy as np

def medfilt1(x=None,L=None):

    '''
    a simple median filter for 1d numpy arrays.

    performs a discrete one-dimensional median filter with window
    length L to input vector x. produces a vector the same size 
    as x. boundaries handled by shrinking L at edges; no data
    outside of x used in producing the median filtered output.
    (upon error or exception, returns None.)

    inputs:
    x, Python 1d list or tuple or Numpy array
    L, median filter window length
    output:
    xout, Numpy 1d array of median filtered result; same size as x
    
    bdj, 5-jun-2009
    '''

    # input checks and adjustments --------------------------------------------
    try:
        N = len(x)
        if N < 2:
            print 'Error: input sequence too short: length =',N
            return None
        elif L < 2:
            print 'Error: input filter window length too short: L =',L
            return None
        elif L > N:
            print 'Error: input filter window length too long: L = %d, len(x) = %d'%(L,N)
            return None
    except:
        print 'Exception: input data must be a sequence'
        return None

    xin = np.array(x)
    if xin.ndim != 1:
        print 'Error: input sequence has to be 1d: ndim =',xin.ndim
        return None
    
    xout = np.zeros(xin.size)

    # ensure L is odd integer so median requires no interpolation
    L = int(L)
    if L%2 == 0: # if even, make odd
        L += 1 
    else: # already odd
        pass 
    Lwing = (L-1)/2

    # body --------------------------------------------------------------------

    for i,xi in enumerate(xin):
        
        # left boundary (Lwing terms)
        if i < Lwing:
            xout[i] = np.median(xin[0:i+Lwing+1]) # (0 to i+Lwing)

        # right boundary (Lwing terms)
        elif i >= N - Lwing:
            xout[i] = np.median(xin[i-Lwing:N]) # (i-Lwing to N-1)
            
        # middle (N - 2*Lwing terms; input vector and filter window overlap completely)
        else:
            xout[i] = np.median(xin[i-Lwing:i+Lwing+1]) # (i-Lwing to i+Lwing)

    return xout


def run(inFilename, outFilename, medianFilter):
    mng = marsyas.MarSystemManager()

    # Create network to extract pitch
    inNet = mng.create("Series","series")

    # Add the MarSystems
    inNet.addMarSystem(mng.create("SoundFileSource", "src"))
    inNet.addMarSystem(mng.create("Stereo2Mono", "s2m"));
    inNet.addMarSystem(mng.create("ShiftInput", "si"));
    
    fanout = mng.create("Fanout","fanout")
    fanout.addMarSystem(mng.create("AubioYin", "yin"));
    fanout.addMarSystem(mng.create("Rms", "rms"));
    inNet.addMarSystem(fanout)

    # Create network to output audio
    outNet = mng.create("Series","series")
    outNet.addMarSystem(mng.create("SineSource", "src"))
    outNet.addMarSystem(mng.create("Gain", "gain"))
    outNet.addMarSystem(mng.create("SoundFileSink", "sink"))
    
    # Update controls
    inNet.updControl("SoundFileSource/src/mrs_string/filename", marsyas.MarControlPtr.from_string(inFilename))
    osrate = inNet.getControl("mrs_real/osrate").to_real()
    outNet.updControl("SoundFileSink/sink/mrs_real/israte", osrate)

    outNet.updControl("SoundFileSink/sink/mrs_string/filename", marsyas.MarControlPtr.from_string(outFilename))

    pitchData = []
    rmsData = []
    while inNet.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
        pitch = inNet.getControl("mrs_realvec/processedData").to_realvec()[0]
        rms = inNet.getControl("mrs_realvec/processedData").to_realvec()[1]
        pitchData.append(pitch)
        rmsData.append(rms)
        inNet.tick()

    outPitches = medfilt1(pitchData,medianFilter)        
    outRms = np.asarray(rmsData)
    outRms *= 5

    for i in range(0,len(outPitches)):
        outNet.updControl("SineSource/src/mrs_real/frequency", outPitches[i]);
        outNet.updControl("Gain/gain/mrs_real/gain", outRms[i]);
        outNet.tick()



if __name__ == "__main__":
    if len(sys.argv) < 4:
        print "Usage: pitch_to_sine.py in.wav out.wav medianFilter"
        sys.exit(1)

    inFilename = sys.argv[1]
    outFilename = sys.argv[2]
    medianFilter = int(sys.argv[3])
        
    run(inFilename, outFilename, medianFilter)
