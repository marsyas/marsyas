#!/usr/bin/evn python

from pylab import *
import marsyas
import string

msm = marsyas.MarSystemManager()

# create a MarSystem from a recursive list specification
def create(net):
    composite = msm.create("Gain/id") # will be overwritten
    if net.__class__ == marsyas.MarSystem:
        composite = net
    elif len(net) == 2:
        composite = msm.create(net[0])
        msyslist = map(create,net[1])
        msyslist = map(composite.addMarSystem,msyslist)
    else:
        composite = msm.create(net)
    return composite

# return a dictionary containing the string references to all elements of a list specification
def mar_refs(net, level_predicate="", level="top"):
    if level_predicate == None:
        level_predicate = ""
    out = {}; # This is the output dictionary
    next_level = ""
    if (len(net) == 2):
        if level!="top": # In the top level, I don't add the group specification
            next_level = level_predicate+net[0]+"/"
    # the problem is that a string is a list and ["Something/name","Something/else"] is a legitimate network
    if ([].__class__ == net[1].__class__):
        for subnet in net[1]:
            out.update(mar_refs(subnet, next_level, "nontop"))
    else:
        n = net.split("/")
        out[n[1]] = level_predicate+net
    return out



# convert a realvec to a numpy array 
def realvec2array(inrealvec):
    outarray = zeros((inrealvec.getCols(), inrealvec.getRows()))
    k = 0;
    for i in range(0,inrealvec.getCols()):
        for j in range(0, inrealvec.getRows()):
            outarray[i,j] = inrealvec[k]
            k = k + 1
    return outarray

# convert a realvec control to a numpy array. By specifying so, eo, st, et
# it is possible to return a subset of the samples and observations.

def control2array(net,cname,so=0,eo=0,st=0,et=0):
    net_control = net.getControl(cname)
    net_realvec = net_control.to_realvec()
    net_array = realvec2array(net_realvec)
    if et==0:
        et = net_array.shape[0]
    if eo==0:
        eo = net_array.shape[1]
    res_array = net_array.transpose()
    res_array = res_array[so:eo,st:et]
    if (et != 1):
        res_array = flipud(res_array)
    return res_array

# convenience function for plotting an array that corresponds
# to a Marsyas realvec 

def marplot(data, cmap = 'jet', aspect='None', x_label='Samples',
            y_label='Observations',sy=0,ey=0,sx=0,ex=0,
            interpolation='bicubic', plot_title = "Marsyas plot"):
    if ex==0:
        ex = data.shape[0]
    if ey==0:
        ey = data.shape[1]
    if (data.shape[0] > 1) and (data.shape[1] > 1):
        imshow(data, cmap=cmap, aspect=aspect, extent=[sy,ey,sx,ex],
               interpolation=interpolation)
    elif (data.shape[0] == 1):
        plot(linspace(0,ex,data.shape[1]), data[0,:])
    elif (data.shape[1] == 1):
        plot(linspace(0,ey,data.shape[0]), data[:,0])
    xlabel(x_label)
    ylabel(y_label)
    title(plot_title)
