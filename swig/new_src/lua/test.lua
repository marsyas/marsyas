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
notempty = pipe:getControl("SoundFileSource/file/mrs_bool/notEmpty")
iniAudio = pipe:getControl("AudioSink/sink/mrs_bool/initAudio")

filename:setValue_string("test.ogg")
iniAudio:setValue_bool(true)

while notempty:to_bool()
do
	pipe:tick()
end
