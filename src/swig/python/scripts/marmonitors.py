#!/usr/bin/env python


from pylab import *
from marsyas_util import * 
import sys
from marsyas import MarSystemManager,MarControlPtr

msm = MarSystemManager()

# create the network and make nice interface with top-level controls
#	net = loadFromFile(pluginName)
def load_network(pluginName):
	net = msm.loadFromFile(pluginName)
	#print net.toString()
	return net



# set the controls and plot the data 								
def plot_figure(pluginName):
    net = load_network(pluginName)
    print "Loading " + pluginName

    figure(figsize=(14,8))

    for i in range(1,300):
        net.tick();

        subplot(2,3,1);
        subplots_adjust(hspace=0.40,wspace=0.20)
        hold(False)
        marplot(control2array(net, "Series/onset_strength/ShiftInput/si2/mrs_realvec/processedData"), 
                x_label="Analysis Frames", 
                y_label="Onset Strength", 
                plot_title = "Onset Strength Signal")

        subplot(2,3,2);
        hold(False)
        marplot(control2array(net, "Series/onset_strength/mrs_realvec/processedData"), 
                x_label="Analysis Frames", 
                y_label="Onset Strength", 
                plot_title = "Filtered Onset Strength Signal")
        

        
        subplot(2,3,3);
        hold(False)
        marplot(control2array(net, "FlowThru/tempoInduction/AutoCorrelation/acr/mrs_realvec/processedData"), 
                x_label = "Lag (samples)", 
                y_label = "Strength Strength", 
                plot_title = "Autocorrelation output",
                ex=200)

        
        subplot(2,3,4);
        hold(False)
        marplot(control2array(net, "FlowThru/tempoInduction/BeatHistogram/histo/mrs_realvec/processedData"), 
                x_label = "Tempo (BPM)", 
                y_label = "Beat Strength", 
                plot_title = "BeatHistogram output",
                ex=200)
        
        subplot(2,3,5);
        hold(False)
        marplot(control2array(net, "FlowThru/tempoInduction/Sum/hsum/mrs_realvec/processedData"), 
                x_label = "Tempo (BPM)", 
                y_label = "Beat Strength", 
                plot_title = "Sum output",
                ex=200)
        hold(False)
        subplot(2,3,6);
        marplot(control2array(net, "FlowThru/tempoInduction/Peaker/pkr1/mrs_realvec/processedData"), 
                x_label = "Tempo (BPM)", 
                y_label = "Beat Strength", 
                plot_title = "Peaker output",
                ex=200)
        hold(False)

        
        title = "Beat Histogram"
        thismanager = get_current_fig_manager()
        ion() 
        show()
        raw_input("Press any key to tick the network")
	


        


# call the plot function 
plot_figure(sys.argv[1])







