#!/usr/bin/env python

# Exercises from the book:
# Designing Sound - Andy Farnell
# Practice 1 - Pedestrian walk
#
# It is a simple beeping sound
#
# Network description:
# PWM wave @ 10 Hz   \\
# 							       ==> Product ==> Output
# Sinusoid @ 2.5 kHz //

from marsyas import *
from marsyas_util import *

maxGain = 0.3;
sinusoidSrc = ["Series/beeper", ["SineSource/src", "Gain/gain"]];
pwmSrc = "PWMSource/gainControl";
foutlist = ["Fanout/fanout", [pwmSrc, sinusoidSrc]];
netlist = ["Series/system", [foutlist, "Product/multiplication", "AudioSink/dest"]];
#print netlist
pnet = create(netlist);
pnet.updControl("mrs_real/israte", 44100.0);
pnet.updControl("Fanout/fanout/Series/beeper/SineSource/src/mrs_real/frequency", 2500.0);
pnet.updControl("Fanout/fanout/Series/beeper/Gain/gain/mrs_real/gain", maxGain);
pnet.updControl("mrs_natural/inSamples", 256)
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
pnet.updControl("Fanout/fanout/PWMSource/gainControl/mrs_real/frequency", 10.0);

# And now we tick()
while 1:
	pnet.tick()

