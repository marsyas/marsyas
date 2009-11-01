# EXAMPLE OF USING Marsyas together with PyLab
# for MATLAB-like plotting
# run using ipython -pylab chroma 

#!/usr/bin/env python

from pylab import * 
import sys
from marsyas import * 

msm = MarSystemManager()

# src  = msm.create("SineSource/src")
src  = msm.create("SoundFileSource/src")
spk = msm.create("Spectrum/spk")
pspk = msm.create("PowerSpectrum/pspk")
chroma  = msm.create("Spectrum2Chroma/chroma")
net = msm.create("Series/net")

for x in [src, spk, pspk, chroma]: 
    net.addMarSystem(x)

outData1  = net.getControl("SoundFileSource/src/mrs_realvec/processedData")
outData2  = net.getControl("PowerSpectrum/pspk/mrs_realvec/processedData")
outData3 = net.getControl("mrs_realvec/processedData")

filename = net.getControl("SoundFileSource/src/mrs_string/filename")
# freq = net.getControl("SineSource/src/mrs_real/frequency");
inSamples = net.getControl("mrs_natural/inSamples");

try:
       filename.setValue_string(sys.argv[1])
except:
        print "no filename specified "
        exit()


f = 440;
#freq.setValue_real(f);

inSamples.setValue_natural(4096);

for i in range(1,12): 
   net.tick()
   print "After tick" 
   
   plotdata1 = outData1.to_realvec()
   plotdata2 = outData2.to_realvec()
   plotdata3 = outData3.to_realvec()
   # figure()
   # plot(plotdata1)
   # figure()
   
   # plot(plotdata2)
   #figure()
   plot(plotdata3)
   f = f * 1.059463
#   freq.setValue_real(f);
   print plotdata3
   raw_input('Press enter to continue')   


