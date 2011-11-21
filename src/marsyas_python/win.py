
#!/usr/bin/evn python
from pylab import *
from marsyas_util import * 
import sys
import getopt
import os

spec = ["Series/net",
		["SoundFileSource/src",
		 "Gain/g1",
		 "Windowing/win"
		 ]
		]

net = create(spec)
fname = net.getControl("SoundFileSource/src/mrs_string/filename")
fname.setValue_string(sys.argv[1])

print net.toString()

src_data = net.getControl("Gain/g1/mrs_realvec/processedData").to_realvec();
win_data = net.getControl("Windowing/win/mrs_realvec/processedData").to_realvec();

net.tick()
net.tick()
net.tick()
net.tick()

print src_data
print win_data


