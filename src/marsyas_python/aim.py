#!/usr/bin/env python


# Plotting auditory image model stuff

import pickle
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
				    "AimSAI/aimsai",
				    "Sum/sum",
#				    "AutoCorrelation/acr",
#				    "BeatHistogram/histo",



#				    "Peaker/pkr",
#				    "MaxArgMax/mxr"
				    ]))



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

def control2array(net,cname,so=0,eo=0,st=0,et=0):
  net_control = net.getControl(cname)
  net_realvec = net_control.to_realvec()
  net_array = realvec2array(net_realvec)
  if et==0:
    et = net_array.shape[0]
    eo = net_array.shape[1]
  res_array = net_array.transpose()
  res_array = res_array[so:eo][st:et]
  res_array = flipud(res_array)
  return res_array


def imageplot(imgdata, cmap = 'jet', aspect='None',img_xlabel='Samples', img_ylabel='Observations',sy=0,ey=0,sx=0,ex=0):
  if ex==0:
    ex = imgdata.shape[0]
    ey = imgdata.shape[1]
  imshow(imgdata, cmap=cmap, aspect=aspect, extent=[sy,ey,sx,ex])
  xlabel(img_xlabel)
  ylabel(img_ylabel)




# set the controls and plot the data
def plot_figure(fname, duration):
	net = create_network()

	filename = net.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = net.getControl("mrs_natural/inSamples")
#	factor = net.getControl("DownSampler/downsampler/mrs_natural/factor")
#	mode = net.getControl("Sum/sum/mrs_string/mode");
#	acr_compress = net.getControl("AutoCorrelation/acr/mrs_real/magcompress");

	filename.setValue_string(fname)
	# winSize = int(float(duration) * 44100.0);
	winSize = int(1400);
	inSamples.setValue_natural(winSize)
	# mode.setValue_string("sum_samples");
	# factor.setValue_natural(32)
#	acr_compress.setValue_real(0.75);
	srate = 44100.0
       	filterbank_output = net.getControl("AimHCL2/aimhcl2/mrs_realvec/processedData")

#	net.updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
#	net.updControl("BeatHistogram/histo/mrs_natural/endBin", 300);

#	net.updControl("Peaker/pkr/mrs_natural/peakStart", 50);
#	net.updControl("Peaker/pkr/mrs_natural/peakEnd", 150);

	topChannel = 50


	for i in range(1,24):
		net.tick()

		data = filterbank_output.to_realvec()
		imgdata = realvec2array(data)
#		ossdata = net.getControl("Sum/sum/mrs_realvec/processedData").to_realvec();
#		acrdata = net.getControl("AutoCorrelation/acr/mrs_realvec/processedData").to_realvec();
#		bhistodata = net.getControl("BeatHistogram/histo/mrs_realvec/processedData").to_realvec();

#		peaks = net.getControl("Peaker/pkr/mrs_realvec/processedData");
		#figure()
		#plot(peaks.to_realvec())

#		max_peak = net.getControl("mrs_realvec/processedData");
#		print max_peak.to_realvec()

		#figure()
		#plot(ossdata)
		#figure()
		#plot(acrdata)
#		figure()
#		plot(bhistodata)
		print imgdata.shape
		#print ossdata.getSize()
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
		cname = "AimHCL2/aimhcl2/mrs_realvec/processedData"
		hold(False)
		figure(1)
		imshow(imgdata.transpose(), cmap = 'jet', aspect='auto', extent=[0.0, winSize /  srate, 1, 78])

		figure(2)
		array = control2array(net, cname,0,40,0,500);
		imageplot(array,'bone_r','auto')
		title(cname);

#		a = (realvec2array(data), 'bone_r', 'auto', 0, 50, 0, 40)


#		a = {imgdata:array, cmap:'bone_r', aspect:'auto'}
#		imageplot(**a)
#		print pickle.dump(a,")
#		imageplot(*a)

		figure(3)
		imageplot(control2array(net, cname), 'jet', 'auto', 'Frequency(Hz)', 'Time(msec)', 0, winSize/srate, 0, 6000)
		figure(4);
		imageplot(control2array(net, "AimSAI/aimsai/mrs_realvec/processedData"), 'jet', 'auto')

		figure(5);
		for i in range(1,topChannel):
		  params={'axes.linewidth' : 0}
		  rcParams.update(params)

                  subplots_adjust(hspace=0.001)
		  ax = subplot(topChannel,1,i)
		  ax.plot(imgdata[0:winSize,topChannel-i]);
		  yticklabels = ax.get_yticklabels()
		  xticklabels = ax.get_xticklabels()
		  setp(yticklabels, visible=False)
		  setp(xticklabels, visible=False)
		  for tick in ax.get_xticklines():
		    tick.set_visible(False)
		  for tick in ax.get_yticklines():
		    tick.set_visible(False)





	        figure(6)
	        subplot(321);
		plot(imgdata[0:512,58]);
		subplot(322)
		plot(imgdata[0:512,66]);
		subplot(323)
		c1 = correlate(imgdata[0:512,58], imgdata[0:512,66], mode='full');
		# plot(c1[winSize/2:winSize/2+512]);
		s1 = argmax(c1);
		plot(c1[s1:s1+512]);

		# delay1 = (argmax(correlate(imgdata[1000:2000,50], imgdata[1000:2000,40], mode='full')) % 1000);

		subplot(324)
		c2 = correlate(imgdata[0:512,58], imgdata[0:512,59], mode='full');
		s2 = argmax(c2);
		plot(c2[s2:s2+512]);
		#plot(c2[winSize/2:winSize/2+512]);
		# delay2 = (argmax(correlate(imgdata[1000:2000,40], imgdata[1000:2000,30], mode='full')) % 1000);

		subplot(325)
		plot(c1)
		hold(True)
		plot(c2)
		hold(False)
		subplot(326)
		plot(c2)

		# print delay1
		# print delay2

		# subplot(325)
		# hold(False)
		# plot(imgdata[1000:2000:,50]);
		# hold(True)
		# plot(imgdata[1000-delay1:2000-delay1:,40]);
		# hold(False)
		# # figure();
		# subplot(326)
		# hold(False)
		# plot(imgdata[1000:2000:,40]);
		# hold(True)
		# plot(imgdata[1000+delay2:2000+delay2:,30]);
		# hold(False)
		# # show();


		corr_image = zeros((78,78))
		mean_period = 0;
		for i in range(0,78):
		  for j in range(0,78):
		    a = correlate(imgdata[0:512,i],imgdata[0:512,j], mode='full');

		    offset = argmax(a);
		    b = a[offset:offset+512];
		    period = 0
		    if (size(b) > 4):
		      for k in range(2,size(b)-1):
			if ((b[k] >= b[k-1]) and (b[k] >= b[k+1])):
			  period = k
			  break

		    # figure(6)
		    # plot(b)
		    # raw_input("Press any key to continue")

		    mean_period = mean_period + period
		    corr_image[i,j] = b[period]

		print (mean_period / (78 * 78))
		figure(7);
		imshow(corr_image, cmap = 'jet', aspect='auto');
		raw_input("Press any key to continue")



# call the plot function
plot_figure(sys.argv[1], sys.argv[2])
