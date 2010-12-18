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
#	net = loadFromFile(pluginName)
def load_network(pluginName):
	net = msm.loadFromFile(pluginName)
	print net.toString()
	return net



# set the controls and plot the data 								
def plot_figure(pluginName):
	net = load_network(pluginName)
	print "Loading " + pluginName

	for i in range(1,20):
		net.tick();
		
	outData1 = net.getControl("FlowThru/tempoInduction/Sum/hsum/mrs_realvec/processedData")
	outData2 = net.getControl("FlowThru/tempoInduction/Peaker/pkr1/mrs_realvec/processedData")
	plot(linspace(0,200, outData1.to_realvec().getSize()), outData1.to_realvec(), label="BeatHistogram")
	plot(linspace(0,200, outData2.to_realvec().getSize()), outData2.to_realvec(), label="Tempo Candidates")
	xlabel("Tempo (BPM)")
	ylabel("Beat Strength")
	ylim(0,30)
	legend()
	
 	savefig('monitor.ps');
	
 	show()
	



# 	# save .svg and .ps versions of the figure 
# 	savefig('windowing.svg')
# 	savefig('windowing.ps')

# 	show()


# call the plot function 
plot_figure(sys.argv[1])







