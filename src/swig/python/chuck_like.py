import marsyas
import random
import math


def mtof(pitch):
     return 440.0 * math.pow(2.0, (pitch-69.0) / 12.0) 

def set_freq(control, freq):
     control.setValue_real(freq)

def dispatch(time, times, f, control, freq):
      if (len(times) > 0):
            if (time > times[0]): 
                f(control,freq)
                del times[0]

def build_network(): 
     # Assemble the network
     mng = marsyas.MarSystemManager() 
     pnet = mng.create("Series", "pnet")
     voices = mng.create("Fanout", "voices");
     voices.addMarSystem(mng.create("SineSource", "src1"));
     filt_noise = mng.create("Series", "filt_noise");
     filt_noise.addMarSystem(mng.create("NoiseSource", "src2"));
     filt_noise.addMarSystem(mng.create("Biquad", "biquad"));
     
     filt_noise.updControl("Biquad/biquad/mrs_real/frequency", marsyas.MarControlPtr.from_real(400.0));
     voices.addMarSystem(filt_noise);

     
     mix = mng.create("Sum", "mix")
     gain = mng.create("Gain", "gain")
     dest = mng.create("AudioSink", "dest")     
     pnet.addMarSystem(voices)
     pnet.addMarSystem(mix)
     pnet.addMarSystem(gain)
     pnet.addMarSystem(dest)

     pnet.linkControl("mrs_real/f1",
                      "Fanout/voices/SineSource/src1/mrs_real/frequency");
     pnet.linkControl("mrs_real/f2",
                      "Fanout/voices/Series/filt_noise/Biquad/biquad/mrs_real/frequency");



     pnet.updControl("mrs_real/israte",
		marsyas.MarControlPtr.from_real(44100.0))
     pnet.updControl("AudioSink/dest/mrs_bool/initAudio",
                     marsyas.MarControlPtr.from_bool(True))
     return pnet


pnet = build_network()

bufferSize = pnet.getControl("mrs_natural/inSamples").to_natural();
srate = pnet.getControl("mrs_real/osrate").to_real();
tstep = bufferSize * 1.0 / srate

f1 = pnet.getControl("mrs_real/f1")
f2 = pnet.getControl("mrs_real/f2")


# Bohlen/Pierce scale 
ratios = [ 1, 25.0/21, 9.0/7, 7.0/5, 5./3, 9./5, 15./7, 7./3, 25./9, 3./1]
frequencies = [x * 440 for x in ratios]
times1 = [x * 0.8 for x in ratios]           # event times
times2 = [x * 0.2 for x in range(1, 100)]


time = 0;     # time in seconds 
while(time < 6.0):
   #  dispatch(time, times1, set_freq, f1, frequencies[random.randint(1,len(frequencies)-1)])	
     dispatch(time, times2,  set_freq, f1, mtof(math.fabs(math.sin(time) * 110)))
     pnet.tick()
     time = time + tstep 

pnet.updControl("AudioSink/dest/mrs_bool/mute",
                marsyas.MarControlPtr.from_bool(True))
pnet.updControl("Gain/gain/mrs_real/gain",
                marsyas.MarControlPtr.from_real(0.0))


del pnet  
print "Done" 


