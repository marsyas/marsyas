
import marsyas
import marsyas_util

source = ["Series/source", ["SoundFileSource/src", "ShiftInput/sft", "Windowing/win", "Spectrum/spk", "Whitening/wtn", "Transposer/tp1"]]
filter = ["Series/filter", ["SoundFileSource/src2", "ShiftInput/sft2", "Windowing/win2", "Spectrum/sinspk", "PowerSpectrum/pwrsin", "Transposer/sintp", "Upsample/upsin"]]
spec_analyzer = ["Series/analysis", [["Fanout/sourcefilter", [source, filter]], "Product/prod", "Transposer/tp2", "InvSpectrum/ispk", "OverlapAdd/ola", "AudioSink/asnk"]]


net = marsyas_util.create(spec_analyzer)
snet = marsyas_util.mar_refs(spec_analyzer)

net.updControl("mrs_natural/inSamples", 1024);
net.updControl("mrs_real/israte", 44100.0);

net.updControl(snet["sft"]+"/mrs_natural/winSize", 2048);
net.updControl(snet["sft2"]+"/mrs_natural/winSize", 2048);
net.updControl(snet["win"]+"/mrs_string/type", "Hanning" ); # "Hamming", "Hanning", "Triangle", "Bartlett", "Blackman"

net.updControl(snet["win2"]+"/mrs_string/type", "Hanning" ); # "Hamming", "Hanning", "Triangle", "Bartlett", "Blackman"

net.updControl(snet["pwrsin"]+"/mrs_string/spectrumType", "magnitude");

net.updControl(snet['upsin']+"/mrs_real/default", 1.0)
net.updControl(snet['upsin']+"/mrs_string/interpolation", "repeat")
net.updControl(snet['upsin']+"/mrs_natural/factor", 2)
net.updControl(snet['src2']+"/mrs_string/filename", "input.wav")
net.updControl(snet['src']+"/mrs_string/filename", "input.wav")
net.updControl(snet['asnk']+"/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

while 1:
        net.tick()
