
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


def imageplot(imgdata, cmap = 'jet', aspect='None',img_xlabel='Samples', img_ylabel='Observations',sy=0,ey=0,sx=0,ex=0):
  if ex==0:
    ex = imgdata.shape[0]
    ey = imgdata.shape[1]
  imshow(imgdata, cmap=cmap, aspect=aspect, extent=[sy,ey,sx,ex], interpolation='nearest')
  xlabel(img_xlabel)
  ylabel(img_ylabel)



def realvec2array(inrealvec):
        outarray = zeros((inrealvec.getCols(), inrealvec.getRows()))
        k = 0;
        for i in range(0,inrealvec.getCols()):
                for j in range(0, inrealvec.getRows()):
                        outarray[i,j] = inrealvec[k]
                        k = k + 1
        return outarray

def control2array(net,cname,so=0,eo=0,st=0,et=0):
  net_control = net.getControl(cname)
  net_realvec = net_control.to_realvec()
  net_array = realvec2array(net_realvec)
  if et==0:
    et = net_array.shape[0]
    eo = net_array.shape[1]
  res_array = net_array.transpose()
  res_array = res_array[so:60,st:et]
  res_array = flipud(res_array)
  return res_array



def frequency_domain():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Gain/gain"
                 ]
                ]


  pitchnet = create(pitch_spec)


  fname = pitchnet.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(1024)


  for i in range(10):
    pitchnet.tick()
    if (i==5):
      figure(1);
      data = pitchnet.getControl("PowerSpectrum/pspk/mrs_realvec/processedData").to_realvec()
      spectrum = realvec2array(data)
      print spectrum
      #zcrs = zeros(512)
      #zcrs_x = [];
      #zcrs_y = [];
      #num_zcrs = 0
      #for j in range(1,512):
      #  if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
      #      ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
      #    zcrs_x.append(j)
      #    zcrs_y.append(0.0)
      #    num_zcrs = num_zcrs + 1;

      title("Power Spectrum")
      #plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      plot(linspace(0, 4000, 93), spectrum[0][0:93])
      # axis([0.0, 1200, 0.0, 0.015])
      #plot(zcrs)
      xlabel("Frequency in Hz")
      ylabel("Power")
      legend()
      savefig("spectrum_sine.png")



def chroma():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Spectrum2Chroma/s2c",
                 "Gain/gain"
                 ]
                ]


  pitchnet = create(pitch_spec)


  fname = pitchnet.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(1024)


  for i in range(10):
    pitchnet.tick()
    if (i==5):
      figure(1);
      data = pitchnet.getControl("Spectrum2Chroma/s2c/mrs_realvec/processedData").to_realvec()
      spectrum = realvec2array(data)
      print spectrum
      #zcrs = zeros(512)
      #zcrs_x = [];
      #zcrs_y = [];
      #num_zcrs = 0
      #for j in range(1,512):
      #  if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
      #      ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
      #    zcrs_x.append(j)
      #    zcrs_y.append(0.0)
      #    num_zcrs = num_zcrs + 1;

      title("Chroma Profile")
      #plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      #plot(linspace(0, 4000, 93), spectrum[0][0:93])
      plot(spectrum[0], drawstyle = 'steps')
      # axis([0.0, 1200, 0.0, 0.015])
      #plot(zcrs)
      xlabel("Pitch Class(Chroma)")
      ylabel("Average Energy")
      legend()
      savefig("chroma_clarinet.png")




def spectrogram():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "Spectrum/spk",
                 "PowerSpectrum/pspk",
                 "Gain/gain"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [pitch_spec]]

  pitchnet = create(accum_spec)

  fname = pitchnet.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(1024)
  nTimes = pitchnet.getControl("mrs_natural/nTimes")
  nTimes.setValue_natural(350)


  pitchnet.tick()
  figure(1);
  spectrum = control2array(pitchnet, "mrs_realvec/processedData")
  print spectrum.shape
  imageplot(spectrum, 'bone_r', 'auto')
  savefig("spectrogram_notes.png")



def correlogram():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AutoCorrelation/acr",
                 "Transposer/transpose"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [pitch_spec]]

  pitchnet = create(accum_spec)

  fname = pitchnet.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(1024)
  nTimes = pitchnet.getControl("mrs_natural/nTimes")
  nTimes.setValue_natural(400)


  pitchnet.tick()
  figure(1);
  spectrum = control2array(pitchnet, "mrs_realvec/processedData")
  print spectrum.shape
  imageplot(spectrum, 'bone_r', 'auto')
  savefig("correlogram_notes.png")


def amdfogram():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AMDF/amdf",
                 "Transposer/transpose"
                 ]
                ]

  accum_spec = ["Accumulator/acum",
                [pitch_spec]]

  pitchnet = create(accum_spec)

  fname = pitchnet.getControl("Series/pitchExtract/SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples")
  inSamples.setValue_natural(1024)
  nTimes = pitchnet.getControl("mrs_natural/nTimes")
  nTimes.setValue_natural(1000)


  pitchnet.tick()
  figure(1);
  spectrum = control2array(pitchnet, "mrs_realvec/processedData")
  print spectrum.shape
  imageplot(spectrum, 'bone_r', 'auto')
  savefig("spectrogram_sine.png")




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
  imageplot(spectrum,'jet', 'auto')
  savefig("chromagram.png")
  figure(2)
  mean_chroma = pitchnet.getControl("mrs_realvec/processedData").to_realvec();

  plot(mean_chroma)




def autocorrelation():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AutoCorrelation/acr",
                 "Gain/gain"
                 ]
                ]


  pitchnet = create(pitch_spec)


  fname = pitchnet.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(1024)


  for i in range(10):
    pitchnet.tick()
    if (i==5):
      figure(1);
      data = pitchnet.getControl("AutoCorrelation/acr/mrs_realvec/processedData").to_realvec()
      acr = realvec2array(data)

      #zcrs = zeros(512)
      #zcrs_x = [];
      #zcrs_y = [];
      #num_zcrs = 0
      #for j in range(1,512):
      #  if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
      #      ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
      #    zcrs_x.append(j)
      #    zcrs_y.append(0.0)
      #    num_zcrs = num_zcrs + 1;

      title("AutoCorrelation")
      #plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      plot(acr)
      # axis([0.0, 1200, 0.0, 0.015])
      #plot(zcrs)
      xlabel("Time in samples")
      ylabel("Correlation")
      legend()
      savefig("autocorrelation_clarinet.png")



def amdf():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Windowing/win",
                 "AMDF/amdf",
                 "Gain/gain"
                 ]
                ]


  pitchnet = create(pitch_spec)


  fname = pitchnet.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])
  inSamples = pitchnet.getControl("mrs_natural/inSamples");
  inSamples.setValue_natural(1024)


  for i in range(10):
    pitchnet.tick()
    if (i==5):
      figure(1);
      data = pitchnet.getControl("AMDF/amdf/mrs_realvec/processedData").to_realvec()
      acr = realvec2array(data)

      #zcrs = zeros(512)
      #zcrs_x = [];
      #zcrs_y = [];
      #num_zcrs = 0
      #for j in range(1,512):
      #  if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
      #      ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
      #    zcrs_x.append(j)
      #    zcrs_y.append(0.0)
      #    num_zcrs = num_zcrs + 1;

      title("Average Magnitude Difference Function")
      #plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      plot(acr)
      # axis([0.0, 1200, 0.0, 0.015])
      #plot(zcrs)
      xlabel("Time in samples")
      ylabel("Magnitude Difference")
      legend()
      savefig("amdf_clarient.png")






def zerocrossings():
  pitch_spec = ["Series/pitchExtract",
                ["SoundFileSource/src",
                 "Gain/gain",
                 ]
                ]


  pitchnet = create(pitch_spec)


  fname = pitchnet.getControl("SoundFileSource/src/mrs_string/filename")
  fname.setValue_string(sys.argv[1])



  for i in range(10):
    pitchnet.tick()
    if (i==5):
      figure(1);
      waveform = control2array(pitchnet, "SoundFileSource/src/mrs_realvec/processedData").transpose();
      zcrs = zeros(512)
      zcrs_x = [];
      zcrs_y = [];
      num_zcrs = 0
      for j in range(1,512):
        if (((waveform[j-1] > 0.0) and (waveform[j] < 0.0)) or
            ((waveform[j-1] < 0.0) and (waveform[j] > 0.0))) :
          zcrs_x.append(j)
          zcrs_y.append(0.0)
          num_zcrs = num_zcrs + 1;

      title("Time Domain Zero Crossings " + "(" + str(num_zcrs) +")")
      plot(zcrs_x, zcrs_y, 'r*', drawstyle = 'steps', markersize=8)
      plot(waveform)
      plot(zcrs)
      xlabel("Time in Samples")
      ylabel("Amplitude")
      legend()
      savefig("zcrs.png")


#zerocrossings()
#frequency_domain()
#chroma()
#autocorrelation()
#amdf()

#spectrogram()
# correlogram()
# amdfogram()
chromagram()
