/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    extract: feature extraction for a single file
*/



#include <stdio.h>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "CommandLineOptions.h"

#include <iostream> 
#include <string> 
using namespace std;


int helpopt;
int usageopt;
int normopt;

long offset = 0;
long duration = 1000 * 44100;
long memSize = 40;
float start = 0.0f;
float length = 30.0f;
float gain = 1.0f;

#define EMPTYSTRING "MARSYAS_EMPTY" 
string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string filefeaturename = EMPTYSTRING;
string extractorName = EMPTYSTRING;


CommandLineOptions cmd_options;



void 
printUsage(string progName)
{
  MRSDIAG("extract.cpp - printUsage");
  cerr << "Usage : " << progName << " [-e extractor] [-h help] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-m memory]  [-u usage] collection1 collection2 ... collectionN" << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("extract.cpp - printHelp");
  cerr << "extract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-e --extractor  : exactor " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-m --memory  r  : memory size " << endl;
  cerr << endl;
  cerr << "Available extractors: " << endl;
  cerr << "STFT: Centroid, Rolloff Flux, ZeroCrossings " << endl;
  cerr << "MFCC: Mel-frequency Cepstral Coefficients " << endl;
  cerr << "SCF : Spectral Crest Factor (MPEG-7) " << endl;
  cerr << "SFM : Sepctral Flatness Measure (MPEG-7) " << endl;
  cerr << endl;
  cerr << "All extractors calculate means and variances over a memory size window" << endl;
  cerr << "SV can be appended in front of any extractor to extract a single vecotr (mean, variances) over a 30-second clip (for example SVSTF) " << endl;
  
  exit(1);
}




void simple_extract(string sfName)
{
  MarSystemManager mng;
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  src->updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  // spectralShape->addMarSystem(mng.create("AbsMax", "absmax"));
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->addMarSystem(mng.create("MFCC", "mfcc"));
  spectralShape->addMarSystem(mng.create("Memory", "mem"));
  MarSystem* statistics = mng.create("Fanout", "statistics");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));
  spectralShape->addMarSystem(statistics);
  
  
  // spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","decibels");
  spectralShape->updctrl("Memory/mem/natural/memSize",400);

  // spectralShape->addMarSystem(mng.create("Kurtosis", "kurtosis"));
  
  
  // spectralShape->addMarSystem(mng.create("PlotSink", "psink"));

  

  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);
  featureNetwork->addMarSystem(spectralShape);

  
  // while (featureNetwork->getctrl("SoundFileSource/src/bool/notEmpty").toBool())
  // {

  realvec in(featureNetwork->getctrl("natural/inObservations").toNatural(), 
	     featureNetwork->getctrl("natural/inSamples").toNatural());

  realvec out(featureNetwork->getctrl("natural/onObservations").toNatural(), 
	      featureNetwork->getctrl("natural/onSamples").toNatural());
  

  for (natural i=0; i < 400; i++) 
    {
      featureNetwork->process(in,out);
    }


  

  /* realvec plot(100, 400);
  plot.setval(1.0);
  

  for (natural c = 0; c < 400; c++)
    {
      for (natural r=50; r < 50 + round(out(0,c) * 50.0); r++)
	plot(r,c) = 0.0;
      for (natural r=50; r > 50 - round(out(0,c) * 50.0); r--)
	plot(r,c) = 0.0;
      
    }
  */ 
  

  
  out.write("spectrogram.plot");
  // plot.write("waveform.plot");
  
    

}


void 
newExtract(string sfName, natural memSize, string extractorStr)
{
  MarSystemManager mng;
  
  // default to STFT 
  if (extractorStr == EMPTYSTRING) 
    {
      extractorStr = "SVSTFT";
    }
  // Find proper soundfile format and create SignalSource 
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  src->updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","power");  
  
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout","spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  
  

  // add the feature to spectral shape
  spectralShape->addMarSystem(spectrumFeatures);
  

  MarSystem* wsink = mng.create("WekaSink", "wsink");
  

  MarSystem* fnet = mng.create("Series", "fnet");
  
  fnet->addMarSystem(src);
  fnet->addMarSystem(spectralShape);
  fnet->addMarSystem(wsink);
  
  fnet->updctrl("WekaSink/wsink/string/filename", "newextract.arff");
  // fnet->updctrl("Series/spectralShape/Fanout/spectrumFeatures/natural/disable", (MarControlValue)0);    
  cout << *fnet << endl;
  

  for (int i=0; i < 10; i++) 
    fnet->tick();
  
}




void extract_trainAccumulator(string sfName, natural memSize, 
			      string extractorStr)
{

  MarSystemManager mng;
  
  // default to STFT 
  if (extractorStr == EMPTYSTRING) 
    {
      extractorStr = "SVSTFT";
    }
  
  


  // Find proper soundfile format and create SignalSource 
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  src->updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","power");  
  
  // Spectrum Shape descriptors
  Fanout spectrumFeatures("spectrumFeatures");
  if (extractorStr == "SVSTFT") 
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
    }
  else if (extractorStr == "SVMFCC")
    spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
  else if (extractorStr == "SVSCF")
    spectrumFeatures.addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SVSFM")
    spectrumFeatures.addMarSystem(mng.create("SFM", "sfm"));
  else if (extractorStr == "SVSTFTMFCC")
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures.addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures.addMarSystem(mng.create("Skewness", "skn"));      
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
      spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
    }
  
  else 
    {
      cout << "Extractor " << extractorStr << " is not supported " << endl;
      cout << "extract for now only supports SV-extractors " << endl;
      exit(1);
    }


  mng.registerPrototype("SpectrumFeatures", spectrumFeatures.clone());

  // add the feature to spectral shape
  spectralShape->addMarSystem(mng.create("SpectrumFeatures", "spectrumFeatures"));
  mng.registerPrototype("SpectralShape", spectralShape->clone());

  //  add time-domain zerocrossings
  MarSystem* features = mng.create("Fanout", "features");
  features->addMarSystem(mng.create("SpectralShape", "SpectralShape"));
  
  if (extractorStr == "STFT")
    features->addMarSystem(mng.create("ZeroCrossings", "zcrs"));      
  mng.registerPrototype("Features", features->clone());
  
  // Means and standard deviation (statistics) for texture analysis 
  MarSystem* statistics = mng.create("Fanout", "statistics");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));
  mng.registerPrototype("Statistics", statistics->clone());
  
  // Weka output 
  MarSystem* wsink = mng.create("WekaSink", "wsink");

  // Build the overall feature calculation network   
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  
  featureNetwork->addMarSystem(src->clone());
  featureNetwork->addMarSystem(mng.create("Features", "features"));

  featureNetwork->addMarSystem(mng.create("Memory", "memory"));
  featureNetwork->addMarSystem(mng.create("Statistics", "statistics"));  


  // update controls I
  featureNetwork->updctrl("Memory/memory/natural/memSize", memSize);
  featureNetwork->updctrl(src->getType() + "/src/natural/inSamples", 
			 MRS_DEFAULT_SLICE_NSAMPLES);
  featureNetwork->updctrl(src->getType() + "/src/natural/pos", offset);      
  featureNetwork->addMarSystem(wsink->clone());

  // accumulate feature vectors over 30 seconds 
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("natural/nTimes", 1290);
  // add network to accumulator
  acc->addMarSystem(featureNetwork->clone());

  // Final network compute 30-second statistics 
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc->clone());
  total->addMarSystem(mng.create("Statistics", "statistics2"));
  total->addMarSystem(mng.create("PlotSink", "psink"));
  
  // update controls 
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/natural/pos", offset);      


  total->updctrl("PlotSink/psink/string/separator", ",");
  

  // Calculate duration, offest parameters if necessary 
  offset = (natural) (start 
		      * src->getctrl("real/israte").toReal() 
		      * src->getctrl("natural/nChannels").toNatural());
  duration = (natural) (length 
			* src->getctrl("real/israte").toReal() 
			* src->getctrl("natural/nChannels").toNatural());
  

  realvec in;
  realvec featureRes;
  
  in.create(total->getctrl("natural/inObservations").toNatural(), 
	    total->getctrl("natural/inSamples").toNatural());
  featureRes.create(total->getctrl("natural/onObservations").toNatural(), 
		    total->getctrl("natural/onSamples").toNatural());

  

  total->linkctrl("string/filename", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/string/filename");
  total->linkctrl("natural/nChannels", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/natural/nChannels");
  total->linkctrl("real/israte", "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/real/israte");
  total->linkctrl("natural/pos", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/natural/pos");
  total->linkctrl("natural/nChannels", 
		  "Accumulator/acc/Series/featureNetwork/AudioSink/dest/natural/nChannels");
  total->linkctrl("bool/notEmpty", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/bool/notEmpty");
  

  if (pluginName != EMPTYSTRING)
    {
      ofstream oss(pluginName.c_str());
      oss << (*total) << endl;
    }

  total->tick();

  
}



   

void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addBoolOption("normalize", "n", false);
  cmd_options.addRealOption("start", "s", 0);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addNaturalOption("memory", "m", 40);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  normopt = cmd_options.getBoolOption("normalize");
  pluginName = cmd_options.getStringOption("plugin");
  extractorName = cmd_options.getStringOption("extractor");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];  
  if (argc == 1)
    printUsage(progName);


  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();
  

  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  int i;
  i =0;
  
  Collection l;
  
  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {	
      string sfname = *sfi;
      string extractorStr = extractorName;

      //extract_trainAccumulator(sfname, memSize, extractorName);
      newExtract(sfname, memSize, extractorName);
      
      // simple_extract(sfname);
      
    }
  
  exit(0);
}






