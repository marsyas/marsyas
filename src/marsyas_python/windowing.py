#!/usr/bin/env python

# This source code will serve as good tutorial on how to program a more complex MarSystem network using a compact syntax.
# This syntax will be interpreted by the functions in marsyas_util.py (which should be imported, as well).
# This program should be the next step in learning after helloworld.py.
# Some more advanced controls will be shown, and how to specify networks in a more compact way (using marsyas_util)
# as well.

from pylab import *
import sys
from matplotlib import pyplot
from marsyas import *
from marsyas_util import *

# marsyas_util gives us, automatically, the line:
# msm = marsyas.MarSystemManager()

# This is a parenthesis-like notation, in which we will specify all the basic blocks in our topology.
# The specification works like this:
# Every time you specify a new subsystem, it should be preceded by a [, which will close when the specification ends
# Also, all subsystems are specified in the following list, so, you will get things like this:
# ["Begin/System", ["SystemItem1", "SystemItem2"]]
# That is very similar to LISP, but with brackets instead of parenthesis.
spec = ["Series/MySystem",
					[["Fanout/additive",
						["SineSource/src1",
						"SineSource/src2"]],
					"Sum/sum",
					["Fanout/analyzers",
						[["Series/branch1",
							["Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]],
						["Series/branch2",
							["Windowing/win","Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]],
						["Series/branch3",
							["Windowing/win","Spectrum/spk","PowerSpectrum/pspk","Gain/gain"]]]]]]

# We will call the create() function, which is defined in the marsyas_util.py file. It will create a marsystem just
# as specified aboce.
net = create(spec)

# Now, we are going to link specific controls of our objects to top-level controls, so we can access them in
# an easier way. Below, we will make code to access Fanout/additive/SineSource/src1/mrs_real/frequency
# using simply mrs_real/frequency1

# make top level controls for the frequencies of the two oscillators 
net.linkControl("Fanout/additive/SineSource/src1/mrs_real/frequency", "mrs_real/frequency1");
net.linkControl("Fanout/additive/SineSource/src2/mrs_real/frequency", "mrs_real/frequency2");

# You can link two or more controls to the same top-level control, so the same control will change parameters for multiple
# controls in the network.

# map all spectrum types to the same top level control 
net.linkControl("Fanout/analyzers/Series/branch1/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")
net.linkControl("Fanout/analyzers/Series/branch2/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")
net.linkControl("Fanout/analyzers/Series/branch3/PowerSpectrum/pspk/mrs_string/spectrumType","mrs_string/spectrumType")

#	# make top level controls for the 3 spectrums 
net.linkControl("Fanout/analyzers/Series/branch1/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum1")
net.linkControl("Fanout/analyzers/Series/branch2/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum2")
net.linkControl("Fanout/analyzers/Series/branch3/PowerSpectrum/pspk/mrs_realvec/processedData","mrs_realvec/spectrum3")

# Now, we will setup our analyzers in each analyzer branch. 
# In branch1, we have no window, hence it is a rectangular-shaped window.
# In branch2, we will have a hamming window, which is the default (hence it does not have to be changed)
# In branch3, we will have a hann window, which is set below:

#	# make the third branch window by a Hann window instead of the default Hamming 
net.updControl("Fanout/analyzers/Series/branch3/Windowing/win/mrs_string/type", "Hann");


# Using the getControl() method, we access the controls for a certain component of our net.
# The following commands will set that we want our output for the spectrumTupe
# (remember, we linked that to all three spectral analyzers above!)
spectrumType = net.getControl("mrs_string/spectrumType");
spectrumType.setValue_string("decibels")

# We are doing something similar here. Note that this is equivalent to using the net.updControl() method properly,
# but it might save you some work later. After you have executed this, uncomment the last two lines of this block and
# see what the output is like!
freq1 = net.getControl("mrs_real/frequency1");
freq2 = net.getControl("mrs_real/frequency2");
freq1.setValue_real(1500.0);
freq2.setValue_real(3000.0);
#net.updControl("mrs_real/frequency1", 2000.0);
#net.updControl("mrs_real/frequency2", 4000.0);

# These lines are important, because they will link an output control (the processedData) to a variable
# which we can easily access.
outData1 = net.getControl("mrs_realvec/spectrum1");
outData2 = net.getControl("mrs_realvec/spectrum2");
outData3 = net.getControl("mrs_realvec/spectrum3");

# And then, we run our network for one tick (we only need one frame!).
net.tick()

# These commands below are for plotting.
plot(linspace(0,11050, 257),outData1.to_realvec(), label="Rectangular")
plot(linspace(0,11050, 257),outData2.to_realvec(), label="Hamming")
plot(linspace(0,11050, 257),outData3.to_realvec(), label="Hanning")

xlabel("Frequency (Hz)")
ylabel("Magnitude (dB)");
suptitle("Marsyas windowing demo");
legend()

# save .svg and .ps versions of the figure 
savefig('windowing.svg')
savefig('windowing.ps')

show()

# Try drawing the network we are running
# When you are finished, go to phone.py to continue the tutorial.


