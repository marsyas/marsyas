
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
        "Peaker/pkr",
        "MaxArgMax/mxr"]
       ]

net = create(net_spec)
fname = net.getControl("SoundFileSource/src/mrs_string/filename")
fname.setValue_string("notes.wav")






amplitudes  = zeros(800)
frequencies = zeros(800)
pitches = zeros(800)

for i in range(800):
  net.tick()
  # multi_plots(net, ["SoundFileSource/src/mrs_realvec/processedData",
  #                   "Windowing/win/mrs_realvec/processedData",
  #                   "AutoCorrelation/acr/mrs_realvec/processedData",
  #                   "Peaker/pkr/mrs_realvec/processedData"])
  pitchres = net.getControl("mrs_realvec/processedData").to_realvec()
  amplitudes[i] = pitchres[0]
  frequencies[i] = pitchres[1]
  pitches[i] = 49 + math.floor(12 * (log(44100.0 / pitchres[1]) / log(2.0))+0.5);

figure(2)
plot(amplitudes);
figure(3)
plot(frequencies)
figure(4)
plot(pitches)
