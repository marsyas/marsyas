#!/usr/bin/env lua

require "marsyas"

msm = marsyas.MarSystemManager()

file = msm:create("SoundFileSource","file")
sink = msm:create("AudioSink","sink")
gain = msm:create("Gain", "gain")
pipe = msm:create("Series","pipe")

pipe:addMarSystem(file)
pipe:addMarSystem(gain)
pipe:addMarSystem(sink)

filename = pipe:getControl("SoundFileSource/file/mrs_string/filename")
notempty = pipe:getControl("SoundFileSource/file/mrs_bool/hasData")
iniAudio = pipe:getControl("AudioSink/sink/mrs_bool/initAudio")

for i = 1,#arg do -- For all given Files

	filename:setValue_string(arg[i]) -- Get file

	if notempty:to_bool() then -- Check for playable file
		iniAudio:setValue_bool(true) -- (Re)set Audio

		while notempty:to_bool() do -- Play File
			pipe:tick()
		end
	end
end
