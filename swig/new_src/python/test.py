#!/usr/bin/env python

from marsyas import MarSystemManager

msm = MarSystemManager()

file = msm.create("SoundFileSource","file")
sink = msm.create("AudioSink","sink")
gain = msm.create("Gain", "gain")
pipe = msm.create("Series","pipe")

pipe.addMarSystem(file)
pipe.addMarSystem(gain)
pipe.addMarSystem(sink)

filename = pipe.getControl("SoundFileSource/file/mrs_string/filename")
notempty = pipe.getControl("SoundFileSource/file/mrs_bool/notEmpty")

filename.setValue("le-disko.ogg")
pipe.updControl("AudioSink/sink/mrs_bool/initAudio",True)

while notempty.to_bool():
	pipe.tick
