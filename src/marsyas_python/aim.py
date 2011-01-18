#!/usr/bin/env python


# Plotting auditory image model stuff


from pylab import *
import sys
from matplotlib import pyplot
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
def create_network():

	net = series("net", create(["SoundFileSource/src",
				    "AimPZFC2/aimpzfc", 
				    "AimHCL2/aimhcl2",
				    "Sum/sum", 
				    "AutoCorrelation/acr", 
				    "BeatHistogram/histo",
				    "Peaker/pkr", 
				    "MaxArgMax/mxr"]))
	return net


def realvec2array(inrealvec):
    	outarray = zeros((inrealvec.getCols(), inrealvec.getRows()))
	k = 0;
 	for i in range(0,inrealvec.getCols()):
		for j in range(0, inrealvec.getRows()):
			outarray[i,j] = inrealvec[k]
			k = k + 1
	return outarray


def pca(data):
   values, vecs = linalg.eigh(cov(data))
   perm = argsort(-values)
   return values[perm], vecs[:, perm]





# set the controls and plot the data
def plot_figure(fname, duration):
	net = create_network()

	filename = net.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = net.getControl("mrs_natural/inSamples")
#	factor = net.getControl("DownSampler/downsampler/mrs_natural/factor") 
	mode = net.getControl("Sum/sum/mrs_string/mode");
	acr_compress = net.getControl("AutoCorrelation/acr/mrs_real/magcompress");

	filename.setValue_string(fname)
	# winSize = int(float(duration) * 44100.0);
	winSize = int(16 * 4096);
	inSamples.setValue_natural(winSize)
	mode.setValue_string("sum_samples");
	# factor.setValue_natural(32)
	acr_compress.setValue_real(0.85);
	srate = 44100.0
       	filterbank_output = net.getControl("AimHCL2/aimhcl2/mrs_realvec/processedData")

	net.updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
	net.updControl("BeatHistogram/histo/mrs_natural/endBin", 200);
	

	
	for i in range(1,8):
		net.tick()

       	data = filterbank_output.to_realvec()
	imgdata = realvec2array(data) 
	ossdata = net.getControl("Sum/sum/mrs_realvec/processedData").to_realvec();
	acrdata = net.getControl("AutoCorrelation/acr/mrs_realvec/processedData").to_realvec();
	bhistodata = net.getControl("BeatHistogram/histo/mrs_realvec/processedData").to_realvec();

	peaks = net.getControl("Peaker/pkr/mrs_realvec/processedData");
	figure()
	plot(peaks.to_realvec())

	max_peak = net.getControl("mrs_realvec/processedData");
	print max_peak.to_realvec()

	figure()
	plot(ossdata)
	figure() 
	plot(acrdata)
	figure()
	plot(bhistodata)
	print imgdata.shape
	print ossdata.getSize()
	# (values, vecs) = pca(imgdata.transpose())
	# figure()
	# plot(vecs[1])
	# figure()
	# plot(vecs[2])
	
	# figure()
	# plot(vecs[3])
	
	# figure()
	# plot(vecs[4])
	
	# figure()
	# plot(vecs[5])
	# print vecs.shape
	
	figure()
	imshow(imgdata.transpose(), cmap = 'gray', aspect='auto', extent=[0.0, winSize /  srate, 1, 78])
	show();
	raw_input("Press any key to continue")

	


# call the plot function
plot_figure(sys.argv[1], sys.argv[2])
