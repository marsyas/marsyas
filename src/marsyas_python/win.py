
# Simple example of Marsyas network illustrating the problem
# of trying to tap the processedData control of the last
# MarSystem in a Series. For more information see the Marsyas
# blog post entitled: The most common confusion in Marsyas. 

#!/usr/bin/evn python
from pylab import *
from marsyas_util import * 
import sys
import getopt
import os


spec = ["Series/net",
		["SoundFileSource/src",
		 "Windowing/win"
		 # One way to address the problem is to add a dummy Gain
#		 , "Gain/gain" 
		 ]
		]
net = create(spec)

fname = net.getControl("SoundFileSource/src/mrs_string/filename")
fname.setValue_string(sys.argv[1])

# The following line usese linking to make the long control path work. 
# net.linkControl("Windowing/win/mrs_realvec/processedData",
#				"mrs_realvec/processedData");

net.tick()

src_data = net.getControl("SoundFileSource/src/mrs_realvec/processedData");
win_data = net.getControl("mrs_realvec/processedData");

# Unless either a dummy gain or linking of controls is utilized the following line
# will return all zeroes.
# win_data = net.getControl("Windowing/win/mrs_realvec/processedData").to_realvec();


figure(1)
plot(src_data.to_realvec())
print(src_data.to_realvec())

figure(2)
plot(win_data.to_realvec())
print win_data.to_realvec()

show()

