#!/usr/bin/python

import marsyas

print "normal marsyas"

mng = marsyas.MarSystemManager()
net = mng.create("Series","series")
net.addMarSystem(mng.create("SoundFileSource", "src"))
net.addMarSystem(mng.create("Gain", "gain"))
net.updControl("SoundFileSource/src/mrs_string/filename", "/home/snessnet/tracks/bird1.wav")

print "filename="
ctrl_fname = net.getControl("SoundFileSource/src/mrs_string/filename");
print "##########"
print ctrl_fname.to_string()

# print net.toString()
