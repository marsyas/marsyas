
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

pitch_spec =  ["Series/pitchExtract",
               ["SoundFileSource/src",
                "Windowing/win",
                "AutoCorrelation/acr",
                "Peaker/pkr",
                "MaxArgMax/mxr",
                "Gain/id"
                ]
               ]

sine_spec = ["Series/playBack",
             ["SineSource/src",
              "Gain/gain",
              "SoundFileSink/dest"]
             ]

net_spec = ["Series/net",
            [["Fanout/mix",
             [pitch_spec, sine_spec]
             ]]
            ]

net = create(net_spec)

fname = net.getControl("Fanout/mix/Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
fname.setValue_string("notes_2.wav")
dest = net.getControl("Fanout/mix/Series/playBack/SoundFileSink/dest/mrs_string/filename");
dest.setValue_string("output.wav")
freq = net.getControl("Fanout/mix/Series/playBack/SineSource/src/mrs_real/frequency");
amp = net.getControl("Fanout/mix/Series/playBack/Gain/gain/mrs_real/gain");
amplitudes  = zeros(800)
frequencies = zeros(800)
pitches = zeros(800)



for i in range(800):
  net.tick()
  multi_plots(net, ["Fanout/mix/Series/pitchExtract/SoundFileSource/src/mrs_realvec/processedData",
                    "Fanout/mix/Series/pitchExtract/Windowing/win/mrs_realvec/processedData",
                    "Fanout/mix/Series/pitchExtract/AutoCorrelation/acr/mrs_realvec/processedData"]);
  # "Windowing/win/mrs_realvec/processedData",
  #                   "AutoCorrelation/acr/mrs_realvec/processedData",
  #                   "Peaker/pkr/mrs_realvec/processedData"])
  # raw_input("Press Enter to continue")
  pitchres = net.getControl("Fanout/mix/Series/pitchExtract/MaxArgMax/mxr/mrs_realvec/processedData").to_realvec()
  print pitchres
  amplitudes[i] = pitchres[0]
  if (amplitudes[i] > 0.005): 
    frequencies[i] = 44100.0 / pitchres[1]
    pitches[i] = 49 + math.floor(12 * (log(44100.0 / pitchres[1]) / log(2.0))+0.5);
  else:
    frequencies[i] = 0.0;
    pitches[i] = 0.0
  freq.setValue_real(frequencies[i])
  

figure(2)
plot(amplitudes);
figure(3)
plot(frequencies)
figure(4)
plot(pitches)
