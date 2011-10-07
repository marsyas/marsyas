#!/usr/bin/env python

# Exercises from the book:
# Designing Sound - Andy Farnell
# Practice 1 - Pedestrian walk
#
# It is a simple beeping sound
#
# Network description:
# Square wave \\
# 							==> Multiplication ==> Output
# Sinusoid    //

from marsyas import *
from marsyas_util import *

# We don't have a square wave generator in Marsyas (yet?), so we will have to change this network to:
#
# Sinusoid ==> Gain ==> Output
maxGain = 0.3;
netlist = ["Series/beeper", ["SineSource/src", "Gain/gain", "AudioSink/dest"]];
pnet = create(netlist);
pnet.updControl("mrs_real/israte", 44100.0);
pnet.updControl("SineSource/src/mrs_real/frequency", 2500.0);
pnet.updControl("Gain/gain/mrs_real/gain", 1.0*maxGain);
pnet.updControl("mrs_natural/inSamples", 256)
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

# And now we tick(), and keep track of elapsed time!
time = 0.0
time2 = 0.0
while 1:
	pnet.tick()
	time=time+256/44100.0; # This is the input buffer size divided by fs
	time2 = time2+256/44100.0; # This timer will not reset
	if time > 0.1:
		pnet.updControl("Gain/gain/mrs_real/gain", 1.0*maxGain);

	if time > 0.2:
		pnet.updControl("Gain/gain/mrs_real/gain", 0.0);
		time=0.0;

	if time2 > 5.0: # How many seconds do you want this do play before you quit?
		break;

