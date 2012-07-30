#!/usr/bin/env python

import argparse
import marsyas
import marsyas_util
import time
import numpy
import cv
from cv_utils import *
import math

# This program will perform real-time spectral analysis.
# TODO: Put axis indicators in the plots!
#
# The basic functionality is as follows:
# Source -> Window -> Spectra -> Output
#
# These are the parameters we want to set:
# For the analysis:
Window_len = 2048	# The number of samples in each analysis window
Window_step = 512	# The step (in samples) between two consecutive analysis
Zero_padding = 1	# After windowing, the signal will be zero-padded to this value times its length
Min_freq = 0			# Hz. The minimum frequency that will be analyzed
Max_freq = 3000		# Hz. The maximum frequency that will be analyzed

# The following lines will determine the structure of the marsystem
spec_analyzer = ["Series/analysis", ["AudioSource/asrc",  "Sum/summation", "ShiftInput/sft", "Windowing/win","Spectrum/spk","PowerSpectrum/pspk"]] 
net = marsyas_util.create(spec_analyzer)
snet = marsyas_util.mar_refs(spec_analyzer)

# This is the configuration for the MarSystem
fs = 44100.0
net.updControl("mrs_natural/inSamples", Window_step);
net.updControl("mrs_real/israte", fs);

net.updControl(snet["sft"]+"/mrs_natural/winSize", Window_len);
net.updControl(snet["win"]+"/mrs_natural/zeroPadding", Window_len * (Zero_padding-1));
net.updControl(snet["win"]+"/mrs_string/type", "Hanning"); # "Hamming", "Hanning", "Triangle", "Bartlett", "Blackman"
net.updControl(snet["asrc"]+"/mrs_natural/nChannels", 2);
net.updControl(snet["asrc"]+"/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True));
net.updControl(snet["pspk"]+"/mrs_string/spectrumType", "logmagnitude2");  # "power", "magnitude", "decibels", "logmagnitude" (for 1+log(magnitude*1000), "logmagnitude2" (for 1+log10(magnitude)), "powerdensity" 

# These variables will avoid having to re-calculate stuff
DFT_SIZE = Window_len * Zero_padding; # This is the size of the DFT
DFT_SIZE_2 = net.getControl(snet["win"]+"/mrs_natural/onSamples").to_natural();
print "Debug parameters"
print DFT_SIZE
print DFT_SIZE_2
freq_bin = fs/DFT_SIZE; # this is the frequency hop for every frequency bin in the DFT
print freq_bin
# This is the size of data that will be shown
visible_time = 10; # Seconds
minK = int(math.floor(Min_freq/freq_bin))
maxK = int(math.ceil(Max_freq/freq_bin))
deltaK = maxK-minK+1
print minK, maxK, deltaK
nTime = int(math.ceil(visible_time*(fs*1.0/Window_step)))

# Allocate memory for the image
Int_Buff = numpy.zeros([deltaK, nTime])
#print deltaK
#print nTime
mat = cv.CreateMat(nTime, deltaK, cv.CV_32FC1)
cv.NamedWindow("Marsyas Spectral Analysis", cv.CV_WINDOW_AUTOSIZE)

try:
	while 1:
		net.tick()
		out = net.getControl("mrs_realvec/processedData").to_realvec()
		out = numpy.array(out)
		out = out[minK:maxK+1]
		out = out [::-1]
		
		if numpy.max(out)>0:
			out = out/numpy.max(out)
		else:
			print numpy.max(out)
		if numpy.ndim(out)==1:
			out = numpy.array([out])
		Int_Buff = Int_Buff[:,1:]
		Int_Buff = numpy.hstack([Int_Buff,numpy.transpose(out)])
		im = array2cv(Int_Buff)
		cv.ShowImage("Marsyas Spectral Analysis", im)
		cv.WaitKey(10)
except KeyboardInterrupt:
	print "Halted!"
	pass
	
