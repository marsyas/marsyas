
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
      figure(1);
      marplot(acr, x_label = "Time in samples",
              y_label = "Correlation",
              plot_title = "AutoCorrelation")
      figure(2);
      marplot(acr);
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
      data2 = net.getControl("PowerSpectrum/pspk/mrs_realvec/processedData").to_realvec()
      print realvec2array(data2)
      spectrum = realvec2array(data)
      print spectrum
      title("Chroma Profile")
      plot(spectrum[0])
      xlabel("Pitch Class(Chroma)")
      ylabel("Average Energy")
      output_filename = os.path.splitext(input_filename)[0] + ".png"
      savefig(output_filename)      
      figure(2)
      sdata = net.getControl("PowerSpectrum/pspk/mrs_realvec/processedData").to_realvec();
      sspectrum = realvec2array(sdata);
      plot(sspectrum[0])





def something_gram(net, winSize, input_filename, output_filename,
                plot_title, colormap, start, end):

  fname = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(input_filename)
  inSamples = net.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(winSize)
  srate = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_real/osrate").to_real()
  nTimes = net.getControl("mrs_natural/nTimes")
  fsize = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_natural/size").to_natural()
  pos = net.getControl("Series/pitchExtract/SoundFileSource/src/mrs_natural/pos")
  spos = float(start) * srate
  pos.setValue_natural(int(spos))
  if (end == None):
    fend = fsize
  else:
    fend = (float(end) - float(start)) * srate 
  nTicks = int(fend / winSize)
  nTimes.setValue_natural(nTicks)
  duration = nTicks * winSize / srate
  print start
  print duration
  net.tick()
  figure(1);
  # use eo, so to limit the y-axis 
  correlogram = control2array(net, "mrs_realvec/processedData")
#  marplot(correlogram, colormap, 'auto', plot_title = plot_title, x_label = "Time(seconds)",
#          ey=duration, y_label = "Lag (samples)")
  marplot(correlogram, aspect='auto', cmap=colormap, plot_title = plot_title, x_label= "Time (seconds)", y_label = "Lag(samples)",
          sy = float(start),ey = float(start)+duration)
  print "Writing " + output_filename
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




def usage():
  print "Available options:"
  print "\tcolormap:string (valid colormaps: jet, bone, bone_r, spectral, hot)"
  print "\tstart:float (start of plot in seconds)"
  print "\tend:float (start of plot in seconds)"
  print "\tinput:string (input file .wav)"
  print "\touput:string (output file .png)"
  print "\tmethod:string (valid methods: correlogram, spectrogram, amdfogram)"
  print "\tplot_title:string"
  print "\twin_size:int"

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:], "hc:e:i:m:o:p:s:w:v", ["help","colormap=","end=","input=","method=","output=",
                                                                  "plot_title=","start=","win_size="])
  except:
    print str(err)
    usage()
    sys.exit(2)


  input_file = None
  method = None
  output_file = None
  verbose = False
  colormap = 'jet'
  plot_title = 'Marsyas plot'
  start = 0
  end = None
  win_size = 1024

  for o, a in opts:
    if o == "-v":
      verbose = True
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    elif o in ("-o", "--output"):
      output_file = a
    elif o in ("-i", "--input"):
      input_file = a
    elif o in ("-m", "--method"):
      method = a
    elif o in ("-c", "--colormap"):
      colormap = a
    elif o in ("-p", "--plot_title"):
      plot_title = a
    elif o in ("-s", "--start"):
      start = a
    elif o in ("-e", "--end"):
      end = a
    elif o in ("-w", "--win_size"):
      win_size = int(a)
    else:
       assert False, "unhandled option"

  if (input_file == None):
    print "No input .wav file specified"
    sys.exit(2)

  if (output_file == None):
    output_file = os.path.splitext(input_file)[0] + ".png"


  if (method == None):
    method = "spectrogram"

  print "start"+str(start)

  spec = ["Series/pitchExtract",
          ["SoundFileSource/src",
           "Windowing/win",
           "AutoCorrelation/acr",
           "Transposer/transpose"
           ]
          ]
  accum_spec = ["Accumulator/acum",
                [spec]]
  correlogram_net = create(accum_spec)


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

  spectrogram_net = create(accum_spec)

  spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AMDF/amdf",
                 "Transposer/transpose"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [spec]]

  amdfogram_net = create(accum_spec)


  
  if (method == "zerocrossings"):
    zerocrossings(10, 512, input_file)
  elif (method == "spectrum"):
    spectrum(5, 1024, input_file)
  elif (method == "autocorrelation"):
    autocorrelation(5, 1024, input_file)
  elif (method == "amdf"):
    amdf(5, 1024, input_file)
  elif (method == "chroma"):
    chroma(5, 4096, input_file)
  elif (method == "spectrogram"):
    something_gram(spectrogram_net, win_size, input_file, output_file,
                   plot_title, colormap, start, end)
  elif (method == "correlogram"):
    something_gram(correlogram_net, win_size, input_file, output_file,
                   plot_title, colormap, start, end)
  elif (method == "amdfogram"):
    something_gram(amdfogram_net, win_size, input_file, output_file,
                   plot_title, colormap, start, end)

  show()
  return 0 






if __name__ == "__main__":
   main()


