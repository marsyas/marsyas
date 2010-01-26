#!/usr/bin/env python


from marsyas import MarSystemManager,MarControlPtr

msm = MarSystemManager()

file = msm.create("SoundFileSource","file")
sink = msm.create("AudioSink","sink")
gain = msm.create("Gain", "gain")
pipe = msm.create("Series","pipe")
pipe.addMarSystem(file)
pipe.addMarSystem(gain)
pipe.addMarSystem(sink)

filename = pipe.getControl("SoundFileSource/file/mrs_string/filename")
notempty = pipe.getControl("SoundFileSource/file/mrs_bool/hasData")
iniAudio = pipe.getControl("AudioSink/sink/mrs_bool/initAudio")
outData  = pipe.getControl("Gain/gain/mrs_realvec/processedData")
import sys

for arg in sys.argv[1:] :
	filename.setValue_string(arg)

	if notempty.to_bool(): # If file has data
		iniAudio.setValue_bool(1)
		
		while notempty.to_bool(): # Play it
			pipe.tick()
			plotdata = outData.to_realvec()

