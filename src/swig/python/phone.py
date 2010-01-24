
import marsyas 


mng = marsyas.MarSystemManager()     


pnet = mng.create("Series/pnet")
mix  = mng.create("Fanout/mix")
mix.addMarSystem(mng.create("SineSource/src1")) 
mix.addMarSystem(mng.create("SineSource/src2")) 
pnet.addMarSystem(mix)
pnet.addMarSystem(mng.create("Sum/sum"))

pnet.addMarSystem(mng.create("ClipAudioRange/range"))
pnet.addMarSystem(mng.create("Biquad/biquad"))
pnet.addMarSystem(mng.create("Gain/gain"))
pnet.addMarSystem(mng.create("AudioSink/dest"))

pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0))
pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(350.0))
pnet.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(1.0))
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
pnet.updControl("ClipAudioRange/range/mrs_real/range", marsyas.MarControlPtr.from_real(0.9))


pnet.updControl("Biquad/biquad/mrs_real/frequency", marsyas.MarControlPtr.from_real(2000.0))
pnet.updControl("Biquad/biquad/mrs_real/resonance", marsyas.MarControlPtr.from_real(12.0))
pnet.updControl("Biquad/biquad/mrs_string/type", marsyas.MarControlPtr.from_string("bandpass"))



while(1): 
	pnet.tick()
