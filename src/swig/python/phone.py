
import marsyas 


mng = marsyas.MarSystemManager()     


pnet = mng.create("Series/pnet")
mix  = mng.create("Fanout/mix")
mix.addMarSystem(mng.create("SineSource/src1")) 
mix.addMarSystem(mng.create("SineSource/src2")) 
pnet.addMarSystem(mix)
pnet.addMarSystem(mng.create("Sum/sum"))
pnet.addMarSystem(mng.create("Gain/gain"))
pnet.addMarSystem(mng.create("AudioSink/dest"))

pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0))
pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(350.0))
pnet.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(0.125))
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))


while(1): 
	pnet.tick()
