
# Proof-of-concept implementation of the monophonic transcription
# system described by Bello, Monti and Sandler in ISMIR 2000.

#!/usr/bin/evn python

from pylab import *
import marsyas
import sys

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
               [
                 "SoundFileSource/src",
                 ["FlowThru/thru",
                  [
                    "Windowing/win",
                    "AutoCorrelation/acr",
                    "Peaker/pkr",
                    "MaxArgMax/mxr",
                    "Gain/id"
                    ]
                  ]
               ]
               ]

sine1 = ["Series/sine1",
         ["SineSource/src",
          "Gain/gain"
          ]
         ]

sine2 = ["Series/sine2",
         ["SineSource/src",
          "Gain/gain"
           ]
         ]

sine_spec = ["Series/playBack",
             [["Fanout/sines",
              [sine1, sine2]
              ]
             ]
             ]

sine_spec = ["Series/playBack",
             [
               ["Fanout/sines",
                [sine1, sine2]],
               "Sum/sum"
             ]]


net_spec = ["Series/net",
            [
              ["Fanout/mix",
               [pitch_spec,sine_spec]
               ],
              "SoundFileSink/dest"]
            ]

net = create(net_spec)

fname = net.getControl("Fanout/mix/Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
fname.setValue_string(sys.argv[1])
dest = net.getControl("SoundFileSink/dest/mrs_string/filename");
dest.setValue_string("output.wav")
freq1 = net.getControl("Fanout/mix/Series/playBack/Fanout/sines/Series/sine1/SineSource/src/mrs_real/frequency");
freq2 = net.getControl("Fanout/mix/Series/playBack/Fanout/sines/Series/sine2/SineSource/src/mrs_real/frequency");
amp1 = net.getControl("Fanout/mix/Series/playBack/Fanout/sines/Series/sine1/Gain/gain/mrs_real/gain");
amp2 = net.getControl("Fanout/mix/Series/playBack/Fanout/sines/Series/sine1/Gain/gain/mrs_real/gain");



amplitudes  = zeros(200)
frequencies = zeros(200)
pitches = zeros(200)
net.updControl("mrs_natural/inSamples", 2048)

for i in range(200):
  net.tick()
  # multi_plots(net, ["Fanout/mix/Series/pitchExtract/SoundFileSource/src/mrs_realvec/processedData",
  #                   "Fanout/mix/Series/pitchExtract/Windowing/win/mrs_realvec/processedData",
  #                   "Fanout/mix/Series/pitchExtract/AutoCorrelation/acr/mrs_realvec/processedData"]);
  # "Windowing/win/mrs_realvec/processedData",
  #                   "AutoCorrelation/acr/mrs_realvec/processedData",
  #                   "Peaker/pkr/mrs_realvec/processedData"])
  # raw_input("Press Enter to continue")
  pitchres = net.getControl("Fanout/mix/Series/pitchExtract/FlowThru/thru/MaxArgMax/mxr/mrs_realvec/processedData").to_realvec()

    
  amplitudes[i] = pitchres[0]
  frequencies[i] = 44100.0 / pitchres[1]
  if pitchres[0] < 0.0001:
    frequencies[i] = 5.0
    amplitudes[i] = 0.0
    pitches[i] = 20
  else:
    pitches[i] = 49 + math.floor(12 * (log(frequencies[i] / pitchres[1]) / log(2.0))+0.5);
  
  if (frequencies[i] > 1100):
    frequencies[i] = 5.0
    amplitudes[i] = 0.0
    pitches[i] = 20
  else:
    pitches[i] = 49 + math.floor(12 * (log(frequencies[i] / pitchres[1]) / log(2.0))+0.5);
    
  if (pitches[i] < 20.0):
    pitches[i]= 20.0
  
    
  freq1.setValue_real(frequencies[i])
  freq2.setValue_real(frequencies[i] * 2)
  amp1.setValue_real(amplitudes[i]);
  amp2.setValue_real(amplitudes[i] * 0.5);

  
figure(2)
plot(amplitudes);
figure(3)
plot(frequencies)
figure(4)
plot(pitches)
