# EXAMPLE OF USING Marsyas together with PyLab
# for MATLAB-like plotting
# run using ipython -pylab testPylab.py foo.wav 

#!/usr/bin/env python


from pylab import *
import sys
from marsyas import MarSystemManager,MarControlPtr

# create a global MarSystemManager 
msm = MarSystemManager()


# helper function to create a list of MarSystems from a list of names
def create(msys_list_names):
	return map(msm.create,msys_list_names)

# create a Series MarSystem composed of a list of MarSystems
def series(name, msys_list):
	net = msm.create("Series/"+name)
	map(net.addMarSystem, msys_list)
	return net

# create a Fanout MarSystem composed of a list of MarSystems
def fanout(name, msys_list):
	net = msm.create("Fanout/"+name)
	map(net.addMarSystem, msys_list)
	return net

# create the network and make nice interface with top-level controls 
def create_network_fanout():

	# Input is the sum of two sinusoidal oscillators 
	additive = fanout("additive", create(["SineSource/src1","SineSource/src2"]))
	
	# The magnitude spectrum in dB is calculated without and with Windowing 
	branch1 = series("branch1",create(["Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]))
	branch2 = series("branch2",create(["Windowing/win","Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]))
	branch3 = series("branch3",create(["Windowing/win","Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]))
	
	net = series("net", [additive,
						 msm.create("Sum/sum"),
						 fanout("mix",
								[branch1,
								 branch2,
								 branch3])])

	# make top level controls for the frequencies of the two oscillators 
	net.linkControl("Fanout/additive/SineSource/src1/mrs_real/frequency", "mrs_real/frequency1");
	net.linkControl("Fanout/additive/SineSource/src2/mrs_real/frequency", "mrs_real/frequency2");

	# map all spectrum types to the same top level control 
	net.linkControl("Fanout/mix/Series/branch1/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")
	net.linkControl("Fanout/mix/Series/branch2/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")
	net.linkControl("Fanout/mix/Series/branch3/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")

	# make top level controls for the 3 spectrums 
	net.linkControl("Fanout/mix/Series/branch1/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum1")
	net.linkControl("Fanout/mix/Series/branch2/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum2")
	net.linkControl("Fanout/mix/Series/branch3/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum3")

	# make the third branch window by a Hann window instead of the default Hamming 
	net.updControl("Fanout/mix/Series/branch3/Windowing/win/mrs_string/type", "Hann");
	return net

# set the controls and plot the data 								
def plot_figure():
	net = create_network_fanout()
	
	spectrumType = net.getControl("mrs_string/spectrumType");
	spectrumType.setValue_string("decibels")
	freq1 = net.getControl("mrs_real/frequency1");
	freq2 = net.getControl("mrs_real/frequency2");
	outData1 = net.getControl("mrs_realvec/spectrum1");
	outData2 = net.getControl("mrs_realvec/spectrum2");
	outData3 = net.getControl("mrs_realvec/spectrum3");
	freq1.setValue_real(1500.0);
	freq2.setValue_real(3000.0);

	net.tick()
	plot(linspace(0,11050, 257),outData1.to_realvec(), label="No windowing")
	plot(linspace(0,11050, 257),outData2.to_realvec(), label="Hamming")
	plot(linspace(0,11050, 257),outData3.to_realvec(), label="Hanning")
	
	xlabel("Frequency (Hz)")
	ylabel("Magnitude (dB)");
	legend()


	# save .svg and .ps versions of the figure 
	savefig('windowing.svg')
	savefig('windowing.ps')

	show()


# call the plot function 
plot_figure()







