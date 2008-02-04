require "marsyas"

if ARGV.length < 1
	$stderr.puts "usage: #{$0} soundfile..."
	exit -1
end

msm = Marsyas::MarSystemManager.new

file = msm.create "SoundFileSource","file"
sink = msm.create "AudioSink","sink"
gain = msm.create "Gain", "gain"
pipe = msm.create "Series","pipe"

pipe.addMarSystem file
pipe.addMarSystem gain
pipe.addMarSystem sink

filename = pipe.getControl "SoundFileSource/file/mrs_string/filename"
notempty = pipe.getControl "SoundFileSource/file/mrs_bool/notEmpty"

ARGV.each do |arg|
	filename.setValue arg
	pipe.updControl "AudioSink/sink/mrs_bool/initAudio",true

	while notempty.to_bool
		pipe.tick
	end
end
