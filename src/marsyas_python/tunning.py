#!/usr/bin/env python

import time
from visual import *
import math
import argparse
import marsyas
import marsyas_util


if __name__ == "__main__":
	notenames = ['A', 'A#', 'B', 'C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#']
	pitch=440.0
	parser = argparse.ArgumentParser(description='Instrument tunning software built using Marsyas')
	args = parser.parse_args()
	
	# Defining the marsyas network
	spec = ["Series/system", ["AudioSource/src", "AubioYin/pitcher", "Memory/m", "Mean/mean"]];
	net = marsyas_util.create(spec)

	# Configuring the network
	gain = 1.0
	sropt = 44100.0;
	copt = 1;
	window_size = 2048;
	net.updControl("Memory/m/mrs_natural/memSize", 2)
	net.updControl("mrs_natural/inSamples", window_size);
	net.updControl("mrs_real/israte", sropt);
	net.updControl("AudioSource/src/mrs_natural/nChannels", copt);
	net.updControl("AudioSource/src/mrs_real/gain", gain);
	net.updControl("AudioSource/src/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True));

	# Start visualization
	allowed_delta = 0.05
	box(pos=(0,0,0), length=allowed_delta, height=1, width=allowed_delta, color=color.green)
	box(pos=(1,0,0), length=allowed_delta, height=1, width=allowed_delta, color=color.red)
	box(pos=(-1,0,0), length=allowed_delta, height=1, width=allowed_delta, color=color.red)
	b = box(pos=(0,0,0), length=allowed_delta, height=1, width=allowed_delta, color=color.blue)
	label(pos=(0,-1,0), text="Marsyas-based Tunner")
	l = label(pos=(0,1,0), text="A")

	while 1:
		net.tick()
		output = net.getControl("mrs_realvec/processedData").to_realvec()
		pitch = output[0]
		if pitch > 25:
			midinote = 12 * math.log((pitch / 440.0), 2) + 69
			targetnote = round(midinote)
			delta = midinote-targetnote
			l.text=notenames[(int(targetnote)-69)%12]
			b.pos=vector(delta,0,0)
			print pitch,delta,notenames[(int(targetnote)-69)%12]



