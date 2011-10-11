#!/usr/bin/env python

# Exercises from the book:
# Designing Sound - Andy Farnell
# Practice 4 - Two-tone alarm generator
#
# Network: this will be done twice:
# Tone1   \\
#           ==> Multiplication => Gain => Output
# PWM     //
#
# But the second part will be obtained by changing the tone's frequency and multiplying the PWM signal by -1.

from marsyas import *
from marsyas_util import *
maxGain = 0.4

sinusoidSrc = ["Series/beeper1", ["SineSource/src", "Gain/gain"]];
pwmSrc = "PWMSource/gainControl";
foutlist = ["Fanout/fanout", [pwmSrc, sinusoidSrc]];
netlist_1 = ["Series/alarm1", [foutlist, "Product/multiplication"]];

sinusoidSrc = ["Series/beeper2", ["SineSource/src", "Gain/gain"]];
pwmSrc = ["Series/control", [["Fanout/sources", ["PWMSource/gainControl", "DCSource/dc"]], "Sum/sum"]];
foutlist = ["Fanout/fanout", [pwmSrc, sinusoidSrc]];
netlist_2 = ["Series/alarm2", [foutlist, "Product/multiplication"]];
netlist = ["Series/system", [["Fanout/fanout", [netlist_1, netlist_2]], "Sum/sum", "AudioSink/dest"]];

pnet = create(netlist);
pnet.updControl("mrs_real/israte", 44100.0);
pnet.updControl("Fanout/fanout/Series/alarm1/Fanout/fanout/Series/beeper1/SineSource/src/mrs_real/frequency", 880.0);
pnet.updControl("Fanout/fanout/Series/alarm1/Fanout/fanout/Series/beeper1/Gain/gain/mrs_real/gain", maxGain);
pnet.updControl("Fanout/fanout/Series/alarm2/Fanout/fanout/Series/beeper2/SineSource/src/mrs_real/frequency", 440.0);
pnet.updControl("Fanout/fanout/Series/alarm2/Fanout/fanout/Series/beeper2/Gain/gain/mrs_real/gain", maxGain);
pnet.updControl("Fanout/fanout/Series/alarm1/Fanout/fanout/PWMSource/gainControl/mrs_real/frequency", 2.0);
pnet.updControl("Fanout/fanout/Series/alarm2/Fanout/fanout/Series/control/Fanout/sources/PWMSource/gainControl/mrs_real/frequency", 2.0);
pnet.updControl("Fanout/fanout/Series/alarm2/Fanout/fanout/Series/control/Fanout/sources/DCSource/dc/mrs_real/level", -1.0);
pnet.updControl("mrs_natural/inSamples", 256)
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

# And now we tick()
while 1:
	pnet.tick()

