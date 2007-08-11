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

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "Fanout.h"
#include "CommandLineOptions.h"

#include <iostream> 
#include <string> 

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
int normopt;

mrs_natural offset = 0;
mrs_natural duration = 1000 * 44100;
mrs_natural memSize = 40;
mrs_real start = 0.0f;
mrs_real length = 30.0f;
mrs_real gain = 1.0f;

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
  
  MarSystem* fnet = mng.create("Series", "fnet");
  
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  // spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","decibels"); 

  MarSystem* specDesc = mng.create("Fanout", "specDesc");
  specDesc->addMarSystem(mng.create("Centroid", "cntrd"));
  specDesc->addMarSystem(mng.create("Rolloff", "rolloff"));
  spectralShape->addMarSystem(specDesc);
  
  fnet->addMarSystem(src);
  fnet->addMarSystem(spectralShape);
  

  cout << *fnet << endl;
  

  

  
  realvec in(fnet->getctrl("mrs_natural/inObservations")->toNatural(),
             fnet->getctrl("mrs_natural/inSamples")->toNatural());

  realvec out(fnet->getctrl("mrs_natural/onObservations")->toNatural(),
              fnet->getctrl("mrs_natural/onSamples")->toNatural());

  for (mrs_natural i=0; i < 400; i++) 
    {
      fnet->process(in,out);
    }
  
  out.write("spectrogram.plot");
}


void 
newExtract(string sfName, mrs_natural memSize, string extractorStr)
{
  MarSystemManager mng;
  
  // default to STFT 
  if (extractorStr == EMPTYSTRING) 
    {
      extractorStr = "SVSTFT";
    }
  // Find proper soundfile format and create SignalSource 
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");  
  
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout","spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("entroid", "cntrd"));
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
  
  fnet->updctrl("WekaSink/wsink/mrs_string/filename", "newextract.arff");
  // fnet->updctrl("Series/spectralShape/Fanout/spectrumFeatures/mrs_natural/disable", 0);    
  cout << *fnet << endl;
  

  for (int i=0; i < 10; i++) 
    fnet->tick();
  
}




void extract_trainAccumulator(string sfName, mrs_natural memSize, 
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
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");  
  
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
  featureNetwork->updctrl("Memory/memory/mrs_natural/memSize", memSize);
  featureNetwork->updctrl("mrs_natural/inSamples", 
			 MRS_DEFAULT_SLICE_NSAMPLES);
  featureNetwork->updctrl(src->getType() + "/src/mrs_natural/pos", offset);      
  featureNetwork->addMarSystem(wsink->clone());

  // accumulate feature vectors over 30 seconds 
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 1290);
  // add network to accumulator
  acc->addMarSystem(featureNetwork->clone());

  // Final network compute 30-second statistics 
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc->clone());
  total->addMarSystem(mng.create("Statistics", "statistics2"));
  total->addMarSystem(mng.create("PlotSink", "psink"));
  
  // update controls 
  total->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/mrs_natural/pos", offset);      


  total->updctrl("PlotSink/psink/mrs_string/separator", ",");
  

  // Calculate duration, offest parameters if necessary 
  offset = (mrs_natural) (start 
		      * src->getctrl("mrs_real/israte")->toReal() 
		      * src->getctrl("mrs_natural/nChannels")->toNatural());
  duration = (mrs_natural) (length 
			* src->getctrl("mrs_real/israte")->toReal() 
			* src->getctrl("mrs_natural/nChannels")->toNatural());
  

  realvec in;
  realvec featureRes;
  
  in.create(total->getctrl("mrs_natural/inObservations")->toNatural(), 
	    total->getctrl("mrs_natural/inSamples")->toNatural());
  featureRes.create(total->getctrl("mrs_natural/onObservations")->toNatural(), 
		    total->getctrl("mrs_natural/onSamples")->toNatural());

  

  total->linkctrl("mrs_string/filename", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename");
  total->linkctrl("mrs_natural/nChannels", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/nChannels");
  total->linkctrl("mrs_real/israte", "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/israte");
  total->linkctrl("mrs_natural/pos", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos");
  total->linkctrl("mrs_natural/nChannels", 
		  "Accumulator/acc/Series/featureNetwork/AudioSink/dest/mrs_natural/nChannels");
  total->linkctrl("mrs_bool/notEmpty", 
		  "Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_bool/notEmpty");
  

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
      // newExtract(sfname, memSize, extractorName);
      
      simple_extract(sfname);
      
    }
  
  exit(0);
}






