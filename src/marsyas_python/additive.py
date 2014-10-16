
import marsyas 
import json 
from marsyas_util import *


net = marsyas.system_from_script(""" 

     Network : Series
     {
	+ public frequency = 440.0 
	+ public g0 = 1.0
	+ public g1 = 0.1
	+ public g2 = 0.2 
	+ public g3 = 0.1
	israte = 44100.0
        -> Fanout 
        {	
           -> f0: Series {-> sin1: SineSource {frequency = /frequency} -> amp1: Gain {gain = /g0}} 
           -> f1: Series {-> sin2: SineSource {frequency = (2 * /frequency)} -> amp2: Gain {gain = /g1}} 
           -> f2: Series {-> sin3: SineSource {frequency = (3 * /frequency)} -> amp2: Gain {gain = /g2}} 
           -> f3: Series {-> sin4: SineSource {frequency = (4 * /frequency)} -> amp2: Gain {gain = /g3}} 
 	}
	-> sum: Sum 
 	-> out: AudioSink
     }
""")
net.updControl("/Series/Network/AudioSink/out/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
k = 1
while (1): 
	k = k+1 	
 	net.tick()
	if (k==4):
		waveform = control2array(net,
                               "Sum/sum/mrs_realvec/processedData");
		marplot(waveform)
		show()
