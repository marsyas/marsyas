#!/usr/bin/env python

# This utility will plot beautiful spectrograms of your sound files. You will have to specify a lot of parameters,
# but the good news is, the defaults will be set so that it will fit most people's needs.
#
# The parameters you have to set are:
# - Input file name
# - Frame step / Frame length (in samples)
# - Minimum and maximum frequency for analysis
# - Minimum and maximum time for analysis
# - Output width and height

import argparse
import marsyas
import marsyas_util
import time
import numpy
import math
import matplotlib.pyplot as plt


if __name__ == '__main__':
   parser = argparse.ArgumentParser(description='Quickly plot beautiful spectrograms for your audio files.')
   parser.add_argument('--fname', dest='Filename', type=str, default='test.wav', help='Filename from where data will be extracted')
   parser.add_argument('--flen', dest='Window_len', type=int, default=2048, help='Length (samples) of the window for analysis')
   parser.add_argument('--fstep', dest='Window_step', type=int, default=1024, help='Step (samples) of the sliding window used for analysis')
   parser.add_argument('--minfreq', dest='Min_freq', type=float, default=110, help='Minimum frequency (Hz) show in the spectrogram')
   parser.add_argument('--maxfreq', dest='Max_freq', type=float, default=3000, help='Maximum frequency (Hz) show in the spectrogram')
   parser.add_argument('--maxtime', dest='Max_time', type=float, default=9000, help='Maximum time (s) show in the spectrogram')
   parser.add_argument('--zeropad', dest='Zero_padding', type=float, default=1, help='Zero padding factor (the DFT is calculated after zero-padding the input to this times the input length - use 1 for standard DFT)')
   parser.add_argument('--width', dest='Width', type=int, default=450, help='Width of the plot')
   parser.add_argument('--height', dest='Height', type=int, default=200, help='Height of the plot')
   parser.add_argument('--window', dest='Window', type=str, default='Hanning', help='Shape of the window that will be used to calculate the spectrogram')
   args = parser.parse_args()

   # Create our Marsyas network for audio analysis
   spec_analyzer = ["Series/analysis", ["SoundFileSource/src",  "Sum/summation", "Gain/gain", "ShiftInput/sft", "Windowing/win","Spectrum/spk","PowerSpectrum/pspk", "Memory/mem"]]
   net = marsyas_util.create(spec_analyzer)
   snet = marsyas_util.mar_refs(spec_analyzer)

   # Configure the network
   net.updControl(snet["src"]+"/mrs_string/filename", args.Filename)
   nSamples = net.getControl(snet["src"]+"/mrs_natural/size").to_natural()
   fs = net.getControl(snet["src"]+"/mrs_real/osrate").to_real()
   dur = nSamples/fs
   print "Opened ", args.Filename
   print "It has ", nSamples, " samples at ", fs, " samples/second to a total of ", dur," seconds"
   memFs = fs/args.Window_step # Sampling rate of the memory buffer
   dur = min(dur, args.Max_time)
   memSize = int(dur*memFs)
   net.updControl("mrs_natural/inSamples", args.Window_step);
   net.updControl(snet["gain"]+"/mrs_real/gain", args.Window_len*1.0); # This will un-normalize the DFT
   net.updControl(snet["sft"]+"/mrs_natural/winSize", args.Window_len);
   net.updControl(snet["win"]+"/mrs_natural/zeroPadding",args.Window_len * (args.Zero_padding-1));
   net.updControl(snet["win"]+"/mrs_string/type", args.Window); # "Hamming", "Hanning", "Triangle", "Bartlett", "Blackman"
   net.updControl(snet["pspk"]+"/mrs_string/spectrumType", "logmagnitude2");  # "power", "magnitude", "decibels", "logmagnitude" (for 1+log(magnitude*1000), "logmagnitude2" (for 1+log10(magnitude)), "powerdensity"
   net.updControl(snet["mem"]+"/mrs_natural/memSize", memSize)

   # Run the network to fill the memory
   for i in range(memSize):
      net.tick()

   # Gather results to a numpy array
   out = net.getControl("mrs_realvec/processedData").to_realvec()
   DFT_Size = int(len(out)*1.0/memSize)
   if numpy.ndim(out)==1:
      out = numpy.array([out])
   out = numpy.reshape(out,(memSize, DFT_Size))
   out = numpy.transpose(out)

   # Cut information that we do not want
   minK = args.Min_freq*DFT_Size/fs
   maxK = args.Max_freq*DFT_Size/fs
   out = out[minK:maxK+1]

   out = out/numpy.max(out)
   out = 1-out

   # Plot ALL the numbers!!!
   
   im=plt.imshow(out, aspect='auto', origin='lower', cmap=plt.cm.gray, extent=[0,dur,args.Min_freq,args.Max_freq])
   plt.xlabel('Time (s)')
   plt.ylabel('Frequency (Hz)')
   fig = plt.gcf()
   width_inches = args.Width/80.0
   height_inches = args.Height/80.0
   fig.set_size_inches((width_inches,height_inches))

   plt.savefig('out.png',bbox_inches='tight')
   plt.savefig('out.pdf',bbox_inches='tight')
   #plt.show()
