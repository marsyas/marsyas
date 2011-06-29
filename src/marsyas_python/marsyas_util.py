#!/usr/bin/evn python

from pylab import *
import marsyas


msm = marsyas.MarSystemManager()

# create a MarSystem from a recursive list specification
def create(net):
  composite = msm.create("Gain/id") # will be overwritten
  if (len(net) == 2):
    composite = msm.create(net[0])
    msyslist = map(create,net[1])
    msyslist = map(composite.addMarSystem,msyslist)
  else:
    composite = msm.create(net)
  return composite

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
    
  if (ex > 1) and (ey > 1):
        imshow(data, cmap=cmap, aspect=aspect, extent=[sy,ey,sx,ex],
               interpolation=interpolation)
  elif (ex == 1):
	  plot(linspace(0,ey,data.shape[1]), data[0,:])
  elif (ey == 1):
      plot(linspace(0,ex,data.shape[0]), data[:,0])
  xlabel(x_label)
  ylabel(y_label)
  title(plot_title)
