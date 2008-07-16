# EXAMPLE OF USING Marsyas together with PyLab
# for MATLAB-like plotting
# run using ipython -pylab testPylab.py foo.wav 


#!/usr/bin/env python

from pylab import *
from marsyas import MarSystemManager,MarControlPtr

msm = MarSystemManager()

file = msm.create("SoundFileSource","file")
gain = msm.create("Gain", "gain")
pipe = msm.create("Series","pipe")

pipe.addMarSystem(file)
pipe.addMarSystem(gain)


filename = pipe.getControl("SoundFileSource/file/mrs_string/filename")
notempty = pipe.getControl("SoundFileSource/file/mrs_bool/notEmpty")
outData  = pipe.getControl("SoundFileSource/file/mrs_realvec/processedData")

# replace with a proper path for your system 
filename.setValue_string("/Users/gtzan/data/sound/music_speech/music/bmarsalis.au")

i = 0

plotdata = outData.to_realvec()
for i in range(1,5):  # Show first five windows of the time domain waveform 
	pipe.tick()
	plotdata = outData.to_realvec()
	hold(False)
	plot(plotdata)
	show()
	foo = raw_input("Press enter to continue >")

