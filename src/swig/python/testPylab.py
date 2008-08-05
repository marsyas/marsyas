# EXAMPLE OF USING Marsyas together with PyLab
# for MATLAB-like plotting
# run using ipython -pylab testPylab.py foo.wav 

#!/usr/bin/env python

from pylab import *
import sys
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

try:
	filename.setValue_string(sys.argv[1])
except:
	print "run using ipython -pylab testPylab.py foo.wav"
	exit()

for i in range(1,5):  # Show first five windows of the time domain waveform 
	pipe.tick()
	plotdata = outData.to_realvec()
	hold(False)
	figure(i)
	plot(plotdata)
	axis('tight')
	xticks ( arange(0,513,64), arange(512 * (i-1), (512 *(i)+1), 64)  )
	show()
