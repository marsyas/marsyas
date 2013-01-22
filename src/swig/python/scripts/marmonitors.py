#!/usr/bin/env python

import optparse
import math

from pylab import *
from marsyas_util import * 
import sys
from marsyas import MarSystemManager,MarControlPtr

class SubPlotInfo:
    def __init__(self, control="", x="", y="", title=""):
        self.marcontrol = control
        self.x_label = x
        self.y_label = y
        self.plot_title = title

    def __str__(self):
        return "Plot info for %s" % (self.marcontrol)

msm = MarSystemManager()

# create the network and make nice interface with top-level controls
#	net = loadFromFile(pluginName)
def load_network(pluginName):
	net = msm.loadFromFile(pluginName)
	#print net.toString()
	return net



# set the controls and plot the data 								
def plot_figure(subplotinfo_list, pluginName):
    net = load_network(pluginName)
    print "Loading " + pluginName

    figure(figsize=(14,8))

    rows = (len(subplotinfo_list) / 4) + 1   # yes, integer division
    cols = math.ceil( len(subplotinfo_list) / rows )
    subplots_adjust(hspace=0.40,wspace=0.20)

    for i in range(1,300):
        net.tick();

        for pj, subplotinfo in enumerate(subplotinfo_list):
            subplot(rows, cols, pj+1);
            marplot(control2array(net, subplotinfo.marcontrol),
                x_label=subplotinfo.x_label,
                y_label=subplotinfo.y_label,
                plot_title=subplotinfo.plot_title,
                )
            hold(False)
        ion() 
        show()
        raw_input("Press any key to tick the network")


def read_config(configfile):
    subplotinfo_list = []
    lines = open(configfile).readlines()
    for line in lines:
        sl = line.split(',')
        subplotinfo = SubPlotInfo(sl[0], sl[1], sl[2], sl[3])
        subplotinfo_list.append(subplotinfo)
    return subplotinfo_list

def main():
    parser = optparse.OptionParser()
    parser.add_option("-c", "--config", type="string",
        default="", metavar="FILENAME",
        help="Config file for controls to display"
        )
    parser.add_option("-p", "--plugin", type="string",
        default="", metavar="FILENAME",
        help="Plugin file to process"
        )

    (option, args) = parser.parse_args()
    print option, args
    if option.plugin == "":
        print parser.format_help()
        exit(1)
    if option.config == "":
        subplotinfo_list = [ SubPlotInfo("mrs_realvec/processedData",
            "x", "y", "overall output") ]
    else:
        subplotinfo_list = read_config(option.config)
    
    plot_figure(subplotinfo_list, option.plugin)


if __name__ == "__main__":
    main()

