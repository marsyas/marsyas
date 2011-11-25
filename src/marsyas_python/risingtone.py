#!/usr/bin/env python

# A fun study on Marsyas and psycho-acoustics.
# The way this experiment works should be found in the book:
# Auditory Scene Analysis, (Bregman, A.)
# I have to figure out some nice
from marsyas import *
from marsyas_util import *

sineseries = ["Series/sineseries", ["SineSource/cos", "Gain/singain"]];
noiseseries = ["Series/noiseseries", ["NoiseSource/noise", "Gain/noisegain"]];
sources = ["Parallel/par", [sineseries, noiseseries]]
spec = ["Series/sys", [sources, "Sum/sum", "AudioSink/dest"]];

net = create(spec)
snet = mar_refs(spec)

fs = 44100.0 # Sampling rate for the experiment
minFreq = 440.0 # Starting frequency
maxFreq = 2000.0 # Ending frequency
frameSize = 512 # Size of the frames used in this experiment
dur = 2.5 # Duration of the experiment (seconds)
noiseOnsets = [0.5, 1.7];  # Time instants when noise onsets
noiseOffsets = [0.7, 1.8]; # Time instants when noise offsets

deltaT = frameSize/fs
currFreq = minFreq
currTime = 0

net.updControl("mrs_real/israte", fs);
net.updControl(snet['dest']+"/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
net.updControl(snet['noisegain']+"/mrs_real/gain", 0.0)
net.updControl(snet['cos']+"/mrs_real/frequency", minFreq)
net.updControl("mrs_natural/inSamples", frameSize);
net.updControl("mrs_natural/onSamples", frameSize);

currNoise = 0;
while currTime < dur:
	net.tick()
	currTime = currTime + deltaT
	if currNoise < len(noiseOnsets):
		if currTime > noiseOnsets[currNoise]:
			net.updControl(snet['noisegain']+"/mrs_real/gain", 1.0)
			net.updControl(snet['singain']+"/mrs_real/gain", 0.0)

		if currTime > noiseOffsets[currNoise]:
			net.updControl(snet['noisegain']+"/mrs_real/gain", 0.0)
			net.updControl(snet['singain']+"/mrs_real/gain", 1.0)
			currNoise = currNoise+1

	currFreq = currFreq +  ((maxFreq-minFreq) * deltaT/dur)
	net.updControl(snet['cos']+"/mrs_real/frequency", currFreq)




