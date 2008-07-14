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

import sys

for arg in sys.argv[1:] :
	filename.setValue_string(arg)

	i = 0
	while notempty.to_bool(): # Play it
		pipe.tick()
		plotdata = outData.to_realvec()
		hold(False)
		plot(plotdata)
		show()
		foo = raw_input("Press enter to continue >")
		print foo
		print i
		i = i + 1
