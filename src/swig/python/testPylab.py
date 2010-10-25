# EXAMPLE OF USING Marsyas together with PyLab
# for MATLAB-like plotting
# run using ipython -pylab testPylab.py foo.wav 

#!/usr/bin/env python

from pylab import *
import sys
from marsyas import MarSystemManager,MarControlPtr

msm = MarSystemManager()

file = msm.create("SoundFileSource","file")
sl = msm.create("StretchLinear", "sl")
gain = msm.create("Gain", "g") 
pipe = msm.create("Series","pipe")

pipe.addMarSystem(file)
pipe.addMarSystem(sl)
pipe.addMarSystem(gain)

filename = pipe.getControl("SoundFileSource/file/mrs_string/filename")
notempty = pipe.getControl("SoundFileSource/file/mrs_bool/hasData")
outData1  = pipe.getControl("SoundFileSource/file/mrs_realvec/processedData")
outData2 = pipe.getControl("StretchLinear/sl/mrs_realvec/processedData")
alpha = pipe.getControl("StretchLinear/sl/mrs_real/stretch")

alpha.setValue_real(2.0)

try:
	filename.setValue_string(sys.argv[1])
except:
	print "run using ipython -pylab testPylab.py foo.wav"
	exit()

for i in range(1,2):  # Show first five windows of the time domain waveform 
	pipe.tick()
	plotdata1 = outData1.to_realvec()
	plotdata2 = outData2.to_realvec()
	figure()
	plot(plotdata1)
	axis('tight')
	xticks ( arange(0,513,64), arange(512 * (i-1), (512 *(i)+1), 64)  )
	figure()
	raw_input("Press enter to continue")
	plot(plotdata2)

	show()








