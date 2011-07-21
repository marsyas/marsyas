`
#!/usr/bin/evn python

from pylab import *
from marsyas_util import * 
import sys
import getopt
import os

# plot zerocrossings 
def zerocrossings(frame_num, winSize, input_filename):
  print "ZeroCrossings"
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Gain/gain",
                 ]
                ]
  net = create(spec)
  fname = net.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(winSize);

  for i in range(frame_num+1):
    net.tick()
    if (i==frame_num):
      figure(1);
      waveform = control2array(net,
                               "SoundFileSource/src/mrs_realvec/processedData").transpose();
      zcrs = zeros(winSize)
      zcrs_x = [];
      zcrs_y = [];
      num_zcrs = 0
      for j in range(1,winSize):
        if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
            ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
          zcrs_x.append(j)
          zcrs_y.append(0.0)
          num_zcrs = num_zcrs + 1;

      title("Time Domain Zero Crossings " + "(" + str(num_zcrs) +")")
      # plot the time domain waveform 
      marplot(waveform)
      # plot the zero-crossings with stars 
      plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      # plot a line 0.0
      plot(zcrs)
      # label the axes 
      xlabel("Time in Samples")
      ylabel("Amplitude")
      # save the figure
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)



# plot a spectrum 
def spectrum(frame_num, winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Gain/gain"
                 ]
                ]
  net = create(spec)

  fname = net.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(winSize)

  for i in range(frame_num+1):
    net.tick()
    if (i==frame_num):
      figure(1);
      data = net.getControl("PowerSpectrum/pspk/mrs_realvec/processedData").to_realvec()
      # restrict spectrum to first 93 bins corresponding approximately to 4000Hz
      spectrum = control2array(net, "PowerSpectrum/pspk/mrs_realvec/processedData", eo=93);
      # plot spectrum with frequency axis
      marplot(spectrum,
              x_label="Frequency in Hz",
              y_label="Power",
              plot_title = "Power Spectrum",
              ex=4000)
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)      



def autocorrelation(frame_num, winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AutoCorrelation/acr",
                 "Gain/gain"
                 ]
                ]
  net = create(spec)
  
  fname = net.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(winSize)

  for i in range(frame_num+1):
    net.tick()
    if (i==frame_num):
      figure(1);
      acr = control2array(net, "AutoCorrelation/acr/mrs_realvec/processedData")
      title("AutoCorrelation")
      marplot(acr, x_label = "Time in samples",
              y_label = "Correlation",
              plot_title = "AutoCorrelation")
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)      
      


def amdf(frame_num, winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AMDF/amdf",
                 "Gain/gain"
                 ]
                ]
  net = create(spec)

  fname = net.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(winSize)

  for i in range(frame_num+1):
    net.tick()
    if (i==frame_num):
      figure(1)
      amdf = control2array(net, "AMDF/amdf/mrs_realvec/processedData")
      marplot(amdf,
              plot_title = "Average Magnitude Difference Function",
              x_label = "Time in samples",
              y_label = "Difference")
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)      


def chroma(frame_num, winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Spectrum2Chroma/s2c",
                 "Gain/gain"
                 ]
                ]
  net = create(spec)

  fname = net.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(winSize)

  for i in range(frame_num+1):
    net.tick()
    if (i==frame_num):
      figure(1);
      data = net.getControl("Spectrum2Chroma/s2c/mrs_realvec/processedData").to_realvec()
      spectrum = realvec2array(data)
      title("Chroma Profile")
      plot(spectrum[0], drawstyle = 'steps')
      xlabel("Pitch Class(Chroma)")
      ylabel("Average Energy")
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)      



def spectrogram(winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Gain/gain"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [spec]]

  net = create(accum_spec)

  fname = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(winSize)
  nTimes = net.getControl("mrs_natural/nTimes")
  fsize = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_natural/size").to_natural()
  nTicks = fsize / winSize
  nTimes.setValue_natural(nTicks)

  net.tick()
  figure(1);
  spectrogram = control2array(net, "mrs_realvec/processedData", eo=60)
  marplot(spectrogram, 'bone_r', 'auto')
  output_filename = os.path.splitext(input_filename)[0] + ".png"
  savefig(output_filename)      



def correlogram(winSize, input_filename):
  spec = ["Series/pitchExtract",
          ["SoundFileSource/src",
           "Windowing/win",
           "AutoCorrelation/acr",
           "Transposer/transpose"
           ]
          ]
  
  accum_spec = ["Accumulator/acum",
                [spec]]

  net = create(accum_spec)

  fname = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(winSize)
  nTimes = net.getControl("mrs_natural/nTimes")
  fsize = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_natural/size").to_natural()
  nTicks = fsize / winSize
  nTimes.setValue_natural(nTicks)

  net.tick()
  figure(1);
  correlogram = control2array(net, "mrs_realvec/processedData")
  marplot(correlogram, 'jet', 'auto')
  output_filename = os.path.splitext(input_filename)[0] + ".png"
  savefig(output_filename)      




def amdfogram(winSize, input_filename):
  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AMDF/amdf",
                 "Transposer/transpose"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [spec]]

  net = create(accum_spec)

  fname = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(winSize)
  nTimes = net.getControl("mrs_natural/nTimes")
  fsize = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_natural/size").to_natural()
  nTicks = fsize / winSize
  nTimes.setValue_natural(nTicks)

  net.tick()
  figure(1);
  amdfogram = control2array(net, "mrs_realvec/processedData")
  marplot(amdfogram, 'jet', 'auto')
  output_filename = os.path.splitext(input_filename)[0] + ".png"
  savefig(output_filename)      




def chromagram():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Spectrum2Chroma/s2c"
                 ]
                ]

  accum_spec = ["Accumulator/accum",
                [pitch_spec]]

  mean_spec = ["Series/mean",
               [accum_spec,
                "Mean/mean"]]

  pitchnet = create(mean_spec)

  fname = pitchnet.getControl("Accumulator/accum/Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(1024)
  nTimes = pitchnet.getControl("Accumulator/accum/mrs_natural/nTimes")
  nTimes.setValue_natural(600)


  pitchnet.tick()
  figure(1);
  spectrum = control2array(pitchnet, "Accumulator/accum/mrs_realvec/processedData")
  print spectrum.shape
  marplot(spectrum,'jet', 'auto')
  savefig("chromagram.png")
  figure(2)
  mean_chroma = pitchnet.getControl("mrs_realvec/processedData").to_realvec();

  plot(mean_chroma)







def main():
  if (sys.argv[1] == "zerocrossings"):
    zerocrossings(10, 512, sys.argv[2])
  elif (sys.argv[1] == "spectrum"):
    spectrum(5, 1024, sys.argv[2])
  elif (sys.argv[1] == "autocorrelation"):
    autocorrelation(5, 1024, sys.argv[2])
  elif (sys.argv[1] == "amdf"):
    amdf(5, 1024, sys.argv[2])
  elif (sys.argv[1] == "chroma"):
    chroma(5, 512, sys.argv[2])
  elif (sys.argv[1] == "spectrogram"):
    spectrogram(1024, sys.argv[2])
  elif (sys.argv[1] == "correlogram"):
    correlogram(1024, sys.argv[2])
  elif (sys.argv[1] == "amdfogram"):
    amdfogram(1024, sys.argv[2])

  return 0 





# amdfogram()
#chromagram()


  

if __name__ == "__main__":
   main()


