#!/usr/bin/env python

from visual import *
import math
import argparse
import marsyas
import marsyas_util

def to_cartesian(radius, theta):
	# Given radius, and theta is radians, return x,y position
	x = radius * math.cos(theta);
	y = radius * math.sin(theta);
	return x,y

def to_polar(x,y):
	# Given x,y, get the polar coordinates
	radius = math.sqrt(x**2, y**2)
	theta = math.atan2(x,y)
	return radius, theta

def normalize(minVal, maxVal, inputVal):
	# Normalizes an input beween min an max to the range 0-1. Returns 0 if inputVal outside of accepted range
	nVal = (inputVal-minVal)/(maxVal-minVal)
	if nVal<0:
		nVal =0.0;
#	if nVal>1:
#		nVal =0.0;
	return nVal

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='3D visualization of the dimensions of certain sounds. Run it and sing to your microphone to see what happens')
	parser.add_argument('--spheres', '-s', metavar='s', default='3', type=int, help='Number of spheres to visualize')
	parser.add_argument('--minf0', '-i', metavar='i', default=110, type=float, help='Minimum fundamental frequency for visualization')
	parser.add_argument('--maxf0', '-a', metavar='a', default=440, type=float, help='Maximum fundamental frequency for visualization')
	args = parser.parse_args()
	print args

	# Defining the marsyas network
	detectors = ["Fanout/detectors", ["Rms/rms", "AubioYin/pitcher", "SoundFileSink/dest"]];
	spec = ["Series/system", ["AudioSource/asrc", detectors]];
	net = marsyas_util.create(spec)

	# We will configure the network:
	gain = 1.0;
	sropt = 44100.0;
	copt = 1;
	window_size = 1024;
	net.updControl("mrs_natural/inSamples", window_size);
	net.updControl("mrs_real/israte", sropt);
	net.updControl("AudioSource/asrc/mrs_natural/nChannels", copt);
	net.updControl("AudioSource/asrc/mrs_real/gain", gain);
	net.updControl("AudioSource/asrc/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True));
	net.updControl("Fanout/detectors/SoundFileSink/dest/mrs_string/filename", "recording.wav");
#	net.updControl("Fanout/detectors/ZeroCrossings/zero/mrs_real/percentage", 0.4);
	# This part will define the scenario
	floor = box (pos=(0,0,-1), length=2, height=2, width=.5, color=color.blue)
	balls = [];
	bpos = [];
	for sn in range(args.spheres):
		theta = sn*2*math.pi/((args.spheres)*1.0);
		x,y = to_cartesian(5,theta);
		newball = sphere(pos=(x,y,0), radius=0.25, color=color.red)
		balls.append(newball);
		bpos.append([5,theta]);

	fps = sropt/window_size;
	dt = 1.0/(fps*1.0)
	vel_theta = 1;
	while 1:
		net.tick()
		output = net.getControl("mrs_realvec/processedData").to_realvec()
		radius = output[0]*10;
		vel_theta = normalize(args.minf0, args.maxf0, output[1])
		vel_theta = vel_theta * 7
		newcolor = color.green
		if radius < 0.2:
			vel_theta = 0;
			newcolor = color.red
		print output[0], output[1], vel_theta, dt

		for b in range(len(balls)):
			bpos[b][1] = bpos[b][1] + vel_theta*dt
			bpos[b][0] = radius;
			x,y = to_cartesian(bpos[b][0], bpos[b][1])
			balls[b].pos = (x,y,0)
			balls[b].color = newcolor

