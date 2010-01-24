
import marsyas 


mng = marsyas.MarSystemManager()     


pnet = mng.create("Series/pnet")
mix  = mng.create("Fanout/mix")
mix.addMarSystem(mng.create("SineSource/src1")) 
mix.addMarSystem(mng.create("SineSource/src2")) 
pnet.addMarSystem(mix)
pnet.addMarSystem(mng.create("Sum/sum"))

pnet.addMarSystem(mng.create("Clip/clip"))
pnet.addMarSystem(mng.create("Biquad/biquad"))

tline = mng.create("Fanout/tline")
tline_b1 = mng.create("Series/tline_b1");
tline_b2 = mng.create("Series/tline_b2");
tline_b1.addMarSystem(mng.create("Gain/gain2"));
tline_b1.addMarSystem(mng.create("Biquad/biquad2"));
tline_b2.addMarSystem(mng.create("Clip/clip2"));
tline_b2.addMarSystem(mng.create("Gain/gain3"));

tline.addMarSystem(tline_b1);
tline.addMarSystem(tline_b2);

pnet.addMarSystem(tline);
pnet.addMarSystem(mng.create("Sum/sum2"))
pnet.addMarSystem(mng.create("Biquad/hpf1"));
pnet.addMarSystem(mng.create("Biquad/hpf2"));
pnet.addMarSystem(mng.create("Gain/gain"));
pnet.addMarSystem(mng.create("AudioSink/dest"))

pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0))
pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(350.0))
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
pnet.updControl("Clip/clip/mrs_real/range", marsyas.MarControlPtr.from_real(0.9))
pnet.updControl("Fanout/tline/Series/tline_b2/Clip/clip2/mrs_real/range", marsyas.MarControlPtr.from_real(0.4))
pnet.updControl("Fanout/tline/Series/tline_b1/Gain/gain2/mrs_real/gain", marsyas.MarControlPtr.from_real(0.5))
pnet.updControl("Fanout/tline/Series/tline_b2/Gain/gain3/mrs_real/gain", marsyas.MarControlPtr.from_real(0.15))
pnet.updControl("Fanout/tline/Series/tline_b1/Biquad/biquad2/mrs_real/frequency", marsyas.MarControlPtr.from_real(400.0))
pnet.updControl("Fanout/tline/Series/tline_b1/Biquad/biquad2/mrs_real/resonance", marsyas.MarControlPtr.from_real(3.0))


pnet.updControl("Biquad/biquad/mrs_real/frequency", marsyas.MarControlPtr.from_real(2000.0))
pnet.updControl("Biquad/biquad/mrs_real/resonance", marsyas.MarControlPtr.from_real(12.0))
pnet.updControl("Biquad/biquad/mrs_string/type", marsyas.MarControlPtr.from_string("bandpass"))


pnet.updControl("Biquad/hpf1/mrs_real/frequency", marsyas.MarControlPtr.from_real(90.0))
pnet.updControl("Biquad/hpf1/mrs_string/type", marsyas.MarControlPtr.from_string("highpass"))
pnet.updControl("Biquad/hpf1/mrs_real/frequency", marsyas.MarControlPtr.from_real(90.0))
pnet.updControl("Biquad/hpf1/mrs_string/type", marsyas.MarControlPtr.from_string("highpass"))
pnet.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(10.0))

while(1): 
	pnet.tick()
