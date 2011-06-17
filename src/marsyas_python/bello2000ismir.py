
# Proof-of-concept implementation of the monophonic transcription
# system described by Bello, Monti and Sandler in ISMIR 2000.

#!/usr/bin/evn python

from pylab import *
import marsyas

msm = marsyas.MarSystemManager()


# create a MarSystem from a recursive list specification
def create(net):
  composite = msm.create("Gain/id") # will be overwritten
  if (len(net) == 2):
    composite = msm.create(net[0])
    msyslist = map(create,net[1])
    msyslist = map(composite.addMarSystem,msyslist)
  else:
    composite = msm.create(net)
  return composite

# plot a list of controls
def multi_plots(net, control_list):
  i = 1;
  for c in control_list:
    figure(1, figsize = (10,4 * len(control_list)))
    subplot(len(control_list),1,i)
    plot(net.getControl(c).to_realvec())
    title(c)

    i = i+1
    hold(False)

net_spec = ["Series/pitch_net",
       ["SoundFileSource/src",
        "Windowing/win",
        "AutoCorrelation/acr",
        "Peaker/pkr"]
       ]

net = create(net_spec)
fname = net.getControl("SoundFileSource/src/mrs_string/filename")
fname.setValue_string("notes.wav")





for i in range(100):
  net.tick()
  multi_plots(net, ["SoundFileSource/src/mrs_realvec/processedData",
                    "Windowing/win/mrs_realvec/processedData",
                    "AutoCorrelation/acr/mrs_realvec/processedData",
                    "mrs_realvec/processedData"])
  raw_input("Press any key to continue")
