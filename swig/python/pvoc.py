#!/usr/bin/env python

from pylab import *
import sys
from marsyas import MarSystemManager,MarControlPtr


mng = MarSystemManager()

# Build the processing network 

a = ("Series", "pvseries")
pvseries = mng.create("Series", "pvseries")
pvseries.addMarSystem(mng.create("SoundFileSource", "src"));
pvseries.addMarSystem(mng.create("PhaseVocoder", "pvoc"));
pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))

filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename");
inSamples = pvseries.getControl("mrs_natural/inSamples");
winSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/winSize");
fftSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/FFTSize");
analysisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Decimation");
synthesisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Interpolation");
convertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/convertMode");
unconvertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/unconvertMode");
outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename");
notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")
magHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/magnitudes"); 
peakHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/peaks");
pos = pvseries.getControl("SoundFileSource/src/mrs_natural/pos");

resetfile = "/Users/gtzan/data/sound/ivl_orig/Pop1.wav"
infile = "/Users/gtzan/data/sound/ivl_orig/Grunge_1_note_mono.wav"
infile = "/Users/gtzan/data/sound/ivl_orig/laura_mono.wav"
infile = "/Users/gtzan/data/sound/ivl_orig/x1.wav"

#classic

filename.setValue_string(infile);
inSamples.setValue_natural(1024);
analysisHop.setValue_natural(1024);
synthesisHop.setValue_natural(1024);
winSize.setValue_natural(4096);
fftSize.setValue_natural(4096);
convertMode.setValue_string("full");
unconvertMode.setValue_string("classic");
outfname.setValue_string("classic.wav");


while notempty.to_bool():
	pvseries.tick()

#identity phaselock 

filename.setValue_string(resetfile);
filename.setValue_string(infile);
inSamples.setValue_natural(512);
analysisHop.setValue_natural(512);
synthesisHop.setValue_natural(1024);
winSize.setValue_natural(4096);
fftSize.setValue_natural(4096);
convertMode.setValue_string("full");
unconvertMode.setValue_string("identity_phaselock");
outfname.setValue_string("ilock.wav");
notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")

while notempty.to_bool():
	pvseries.tick()





# magdata = magHandle.to_realvec()
# peakdata = peakHandle.to_realvec()
# hold(False)
# plot(magdata)
# hold(True)
# plot(peakdata)
# hold(False)
# show()
