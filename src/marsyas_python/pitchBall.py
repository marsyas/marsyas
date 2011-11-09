#!/usr/bin/env python

# This is an ambient sound analyzer that will:
# - Get sound from the microphone
# - Show a graphical representation of the parameters of that sound

from pylab import *
from marsyas import *
from marsyas_util import *
from visual import *

# For step one, we will create the following marsyas network:
# ADC ==> pitch extractor ==> output1

spec = ["Series/system", ["AudioSource/asrc", "AubioYin/pitcher"]];#"SoundFileSink/dest"]];#, "AubioYin/pitcher"]];
#spec = ["Series/system", ["AudioSource/asrc", "Rms/pitcher"]];#"SoundFileSink/dest"]];#, "AubioYin/pitcher"]];
net = create(spec)

# We will configure the network:
gain = 1.0;
sropt = 44100.0;
copt = 1;
net.updControl("mrs_natural/inSamples", 2048);
net.updControl("mrs_real/israte", sropt);
net.updControl("AudioSource/asrc/mrs_natural/nChannels", copt);
net.updControl("AudioSource/asrc/mrs_real/gain", gain);
net.updControl("AudioSource/asrc/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True));
#net.updControl("AubioYin/pitcher/mrs_real/tolerance", 0.3);
#net.updControl("SoundFileSink/dest/mrs_string/filename", "test.wav");

# Now, we should have some visualization tool. This program uses the visual python library to work that out, so:
ball = sphere(pos = (0, 0, 0), radius = 1, color = (1, 0, 0))

# This program will do the following:
# Tick the network
# Low-pass the output using a exponent-decay filter with known coefficient
# Color the sphere so it is brighter for trebble sounds


filteredout = 0;
alpha = 0.9;
#print "GO GO GO!"
while 1:
	net.tick();
	# We will link a variable to the output control...
	output = net.getControl("mrs_realvec/processedData").to_realvec()
	filteredout = filteredout*alpha + (1-alpha)*output[0]
	print output[0], filteredout
	red = output[0]/1000.0;
	ball.color = vector(1-red, red, 0);


