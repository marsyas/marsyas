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
    bextract: batch feature extraction 
*/



#include <stdio.h>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "CommandLineOptions.h"

#include <string> 
using namespace std;

int helpopt;
int usageopt;
int normopt;

long offset = 0;
long duration = 1000 * 44100;
long memSize = 40;
natural winSize = 512;
float start = 0.0f;
float length = 30.0f;
float gain = 1.0f;

#define EMPTYSTRING "MARSYAS_EMPTY" 
string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string filefeaturename = EMPTYSTRING;
string extractorName = EMPTYSTRING;
string classifierName = EMPTYSTRING;



CommandLineOptions cmd_options;



void 
printUsage(string progName)
{
  MRSDIAG("bextract.cpp - printUsage");
  cerr << "Usage : " << progName << " [-e extractor] [-h help] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-m memory]  [-u usage] collection1 collection2 ... collectionN" << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("bextract.cpp - printHelp");
  cerr << "bextract, MARSYAS, Copyright George Tzanetakis " << endl;
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



void 
tempo_histoSumBands(MarSystem* total1, string sfName, realvec& beatfeatures,
		    realvec& iwin, realvec& estimate)
{

  
  estimate.setval(0.0);
  
  natural nChannels;
  real srate;
  
  // prepare network 
  
  // update the controls 
  // input filename with hopSize/winSize 


  total1->updctrl("SoundFileSource/src1/string/filename", sfName);

  nChannels = total1->getctrl("SoundFileSource/src1/natural/nChannels").toNatural();
  srate = total1->getctrl("SoundFileSource/src1/real/israte").toReal();

  natural ifactor = 8;
  total1->updctrl("DownSampler/initds/natural/factor", ifactor);  
  
  natural winSize = (natural) ((srate / 22050.0) * 2 * 65536);
  natural hopSize = winSize / 16;

  
  

  offset = (natural) (start * srate * nChannels);
  // duration = (natural) (length * srate * nChannels);

  // only do 30 seconds 
  duration = (natural) (30.0 * srate * nChannels);
  
  total1->updctrl("SoundFileSource/src1/natural/inSamples", hopSize);
  total1->updctrl("SoundFileSource/src1/natural/pos", offset);      
  total1->updctrl("ShiftInput/si/natural/WindowSize", winSize);
  total1->updctrl("ShiftInput/si/natural/Decimation", hopSize);
  total1->updctrl("ShiftInput/si/bool/reset", true);
  total1->updctrl("MaxArgMax/mxr/natural/nMaximums", 3);  

  // wavelt filterbank envelope extraction controls 
  total1->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total1->updctrl("OnePole/lpf/real/alpha", 0.99f);
  natural factor = 32;
  total1->updctrl("DownSampler/ds/natural/factor", factor);  
  
  srate = total1->getctrl("DownSampler/initds/real/osrate").toReal();  




  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  natural pkinS = total1->getctrl("Peaker/pkr/natural/onSamples").toNatural();
  real peakSpacing = ((natural)(srate * 60.0 / (factor *60.0)) - 
		      (natural)(srate * 60.0 / (factor *62.0))) / (pkinS * 1.0);
  natural peakStart = (natural)(srate * 60.0 / (factor * 200.0));
  natural peakEnd   = (natural)(srate * 60.0 / (factor * 50.0));
  total1->updctrl("Peaker/pkr/real/peakSpacing", peakSpacing);
  total1->updctrl("Peaker/pkr/real/peakStrength", 0.5);
  total1->updctrl("Peaker/pkr/natural/peakStart", peakStart);
  total1->updctrl("Peaker/pkr/natural/peakEnd", peakEnd);
  total1->updctrl("Peaker/pkr/real/peakGain", 2.0);


  total1->updctrl("Histogram/histo/natural/startBin", (MarControlValue)0);

  total1->updctrl("Histogram/histo/natural/endBin", 250);
  total1->updctrl("Histogram/histo/bool/reset", true);

  
  // prepare vectors for processing 
  /* realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec estimate(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());
  */ 
  
  natural onSamples;
  
  int numPlayed =0;
  natural wc=0;
  natural samplesPlayed = 0;
  natural repeatId = 1;

  // vector of bpm estimate used to calculate median 
  onSamples = total1->getctrl("ShiftInput/si/natural/onSamples").toNatural();

  
  total1->updctrl("SoundFileSource/src1/natural/pos", (MarControlValue)0);

  

  while (total1->getctrl("SoundFileSource/src1/bool/notEmpty").toBool())
    {
      total1->process(iwin, estimate);
      numPlayed++;
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
    }
  
  
  // cout << "FINAL = " << bpms[bpms.size()-1] << endl;

  beatfeatures = estimate;
}



void bextract_trainAccumulator(vector<Collection> cls, natural label, 
			       string pluginName, string classNames, 
			       string wekafname, string filefeaturename, 
			       natural memSize, string extractorStr,
			       bool withBeatFeatures)
{

  
  cout << "bextractTrainAccumulator::extractorStr = " << extractorStr << endl;
  if (withBeatFeatures)
    cout << "with beat features" << endl;
  
  MRSDIAG("sfplay.cpp - sfplay");
  natural i;
  natural cj;
  
  
  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  if (normopt) 
    cout << "NORMALIZE ENABLED" << endl;

  
  MarSystemManager mng;  
  MarSystem* src = mng.create("SoundFileSource", "src");
  
  
  // Calculate windowed power spectrum 
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","power");  



  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  if (extractorStr == "STFT") 
    {
      spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
      // spectrumFeatures->addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
    }
  else if (extractorStr == "STFTMFCC")
    {
      spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
      // spectrumFeatures->addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
      spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
    }
  else if (extractorStr == "MFCC")
    {
      spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
    }

  else if (extractorStr == "SCF")
    spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SFM") 
    spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));
  else 
    {
      cerr << "Unsupported extractor " << extractorStr << endl;
      return;
    }
  
  
  // Register the spectrum features in manager 
  mng.registerPrototype("SpectrumFeatures", spectrumFeatures->clone());

  // add the descriptors to the spectralShape series
  spectralShape->addMarSystem(mng.create("SpectrumFeatures", "spectrumFeatures"));
  // Register the spectralShape
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
  

  // Build the overall feature calculation network 
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src->clone());
  featureNetwork->addMarSystem(mng.create("Features", "features"));
  featureNetwork->addMarSystem(mng.create("Memory", "memory"));
  featureNetwork->addMarSystem(mng.create("Statistics", "statistics"));  
  
  // update controls 
  featureNetwork->updctrl("Memory/memory/natural/memSize", memSize);
  featureNetwork->updctrl("SoundFileSource/src/string/filename", sfName);
  featureNetwork->updctrl("SoundFileSource/src/natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

  // accumulate feature vectors over 30 seconds 
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("natural/nTimes", 1298);
  // add network to accumulator
  acc->addMarSystem(featureNetwork->clone());
  
  // weka output 
  

  MarSystem* wsink = mng.create("WekaSink", "wsink");
  MarSystem* annotator = mng.create("Annotator", "annotator");

  
  // Final network compute 30-second statistics 
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc->clone());
  total->addMarSystem(mng.create("Statistics", "statistics2"));
  
  

  real srate = src->getctrl("real/osrate").toReal();
  natural nChannels = src->getctrl("natural/nChannels").toNatural();
  
  
  // update controls 
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/natural/inSamples", (natural) (srate / 22050.0) * MRS_DEFAULT_SLICE_NSAMPLES);
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/natural/pos", offset);      



  // Calculate duration, offest parameters if necessary 
  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  
  natural wc = 0;
  natural samplesPlayed =0;

  // main loop for extracting the features 
  string className = "";
  realvec beatfeatures;
  beatfeatures.create((natural)8,(natural)1);

  realvec estimate;
  estimate.create((natural)8,(natural)1);
  


  realvec in;
  realvec timbreres;
  realvec fullres;
  realvec afullres;
  
  
  
  in.create(total->getctrl("natural/inObservations").toNatural(), 
	    total->getctrl("natural/inSamples").toNatural());
  timbreres.create(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());
  
  if (withBeatFeatures)
    {
      fullres.create(total->getctrl("natural/onObservations").toNatural() + 
		     8, 
		     total->getctrl("natural/onSamples").toNatural());
      
      afullres.create(total->getctrl("natural/onObservations").toNatural() + 
		      8 + 1, 
		      total->getctrl("natural/onSamples").toNatural());
      annotator->updctrl("natural/inObservations", total->getctrl("natural/onObservations").toNatural()+8);      
    }
  else
    {
      fullres.create(total->getctrl("natural/onObservations").toNatural(), 
		     total->getctrl("natural/onSamples").toNatural());

      afullres.create(total->getctrl("natural/onObservations").toNatural() + 1,
		      total->getctrl("natural/onSamples").toNatural());  
      annotator->updctrl("natural/inObservations", total->getctrl("natural/onObservations").toNatural());      
    }
  
  annotator->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));

  
  annotator->updctrl("real/israte", total->getctrl("real/israte"));


  wsink->updctrl("natural/inSamples", annotator->getctrl("natural/onSamples"));
  wsink->updctrl("natural/inObservations", annotator->getctrl("natural/onObservations").toNatural());
  wsink->updctrl("real/israte", annotator->getctrl("real/israte"));


  wsink->updctrl("string/labelNames",classNames);

  natural timbreSize = total->getctrl("natural/onObservations").toNatural();
  natural beatSize = 8;
  

  wsink->updctrl("natural/nLabels", (natural)cls.size());  



  MarSystem *total1 = NULL;

  if (withBeatFeatures) 
    {
      total1 = mng.create("Series", "src");  
      total1->addMarSystem(mng.create("SoundFileSource", "src1"));
      total1->addMarSystem(mng.create("ShiftInput", "si"));
      total1->addMarSystem(mng.create("DownSampler", "initds"));
      total1->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
      total1->addMarSystem(mng.create("WaveletBands", "wvbnds"));
      total1->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
      total1->addMarSystem(mng.create("OnePole", "lpf"));
      total1->addMarSystem(mng.create("Norm", "norm"));
      total1->addMarSystem(mng.create("Sum", "sum"));
      total1->addMarSystem(mng.create("DownSampler", "ds"));
      total1->addMarSystem(mng.create("AutoCorrelation", "acr"));
      total1->addMarSystem(mng.create("Peaker", "pkr"));
      total1->addMarSystem(mng.create("MaxArgMax", "mxr"));
      total1->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
      total1->addMarSystem(mng.create("Histogram", "histo"));
      // total1->addMarSystem(mng.create("PlotSink", "psink"));

      // total1->addMarSystem(mng.create("Reassign", "reassign"));
      
      total1->addMarSystem(mng.create("BeatHistoFeatures", "bhf"));
      annotator->updctrl("string/inObsNames", total->getctrl("string/onObsNames").toString() + total1->getctrl("string/onObsNames").toString());

    }
  else
    {
      annotator->updctrl("string/inObsNames", total->getctrl("string/onObsNames"));  
    }
  wsink->updctrl("string/inObsNames", annotator->getctrl("string/onObsNames"));
  realvec iwin;
  
  if (wekafname == EMPTYSTRING) 
    wsink->updctrl("string/filename", "weka.arff");
  else 
    wsink->updctrl("string/filename", wekafname);
  


  for (cj=0; cj < (natural)cls.size(); cj++)
    {
      Collection l = cls[cj];
      for (i=0; i < l.size(); i++)
	{


	  
	  // cout << beatfeatures << endl;
	  

	  total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/string/filename", l.entry(i));

	  if (withBeatFeatures) 
	    {
	      srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/real/osrate").toReal();
	      iwin.create((natural)1, (natural)(((srate / 22050.0) * 2 * 65536) / 16));
	      tempo_histoSumBands(total1, l.entry(i), beatfeatures, 
				  iwin, estimate);
	    }


	  total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/natural/pos", offset);
	  wc = 0;  	  
	  samplesPlayed = 0;
	  // total->updctrl("WekaSink/wsink/natural/label", cj);
	  annotator->updctrl("natural/label", cj);
	  // wsink->updctrl("natural/label", cj);
	  
	  total->process(in, timbreres);

	  // concatenate timbre and beat vectors 

	  
	  for (int t=0; t < timbreSize; t++)
	    fullres(t,0) = timbreres(t,0);

	  if (withBeatFeatures)
	    {
	      for (int t=0; t < beatSize; t++)
		fullres(t+timbreSize, 0) = beatfeatures(t,0);
	    }
	  
	  annotator->process(fullres, afullres);
	  wsink->process(afullres, afullres);
	  
	  cerr << "Processed " << l.entry(i) << endl;
	}
    }

  cout << "Annotator = " << (*annotator) << endl;
  cout << "WSINK = " << (*wsink) << endl;

  
  if (pluginName == EMPTYSTRING) // output to stdout 
    cout << (*total) << endl;      
  else 
    {
      ofstream oss(pluginName.c_str());
      oss << (*total) << endl;
    }
}







// train with multiple feature vectors/file 
void bextract_train(vector<Collection> cls, natural label, 
		    string pluginName, string classNames, 
		    string wekafname,  natural memSize, 
		    string extractorStr,
		    string classifierName)
{

  
  
  if (classifierName == EMPTYSTRING) 
    classifierName = "GS";
  
  MRSDIAG("bextract.cpp - bextract_train");
  natural i;
  natural cj;

  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  // default 
  if (extractorStr == EMPTYSTRING) 
    extractorStr = "STFT";

  MarSystemManager mng;
  
  // Find proper soundfile format and create SignalSource 
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  // src->updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // src->updctrl("natural/inSamples", 2048);


  
  
  // Calculate duration, offest parameters if necessary 
  if (start > 0.0f) 
    offset = (natural) (start 
			* src->getctrl("real/israte").toReal() 
			* src->getctrl("natural/nChannels").toNatural());
  
  if (length != 30.0f) 
    duration = (natural) (length 
			  * src->getctrl("real/israte").toReal() 
			  * src->getctrl("natural/nChannels").toNatural());
  
  // create audio sink and mute it 
  // it is stored in the output plugin 
  // which can be used for real-time classification 

  
  /* MarSystem* dest=NULL;
  if (pluginName != EMPTYSTRING) // output to stdout 
    {
      dest = mng.create("AudioSink", "dest");
      dest->updctrl("bool/mute", true);
    }
  */ 
  
  
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->updctrl("Spectrum/spk/real/cutoff", 1.0);
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","power");  


  // Spectrum Shape descriptors
  Fanout spectrumFeatures("spectrumFeatures");
  if (extractorStr == "STFT") 
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures.addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
    }
  else if (extractorStr == "STFTMFCC")
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures.addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
      spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
    }
  else if (extractorStr == "MFCC")
    spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
  else if (extractorStr == "SCF")
    spectrumFeatures.addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SFM")
    spectrumFeatures.addMarSystem(mng.create("SFM", "sfm"));
  else 
    {
      cerr << "Extractor " << extractorStr << " is not supported " << endl;
      return;
    }
  

  mng.registerPrototype("SpectrumFeatures", spectrumFeatures.clone());


  // add the feature to spectral shape
  spectralShape->addMarSystem(mng.create("SpectrumFeatures", "spectrumFeatures"));
  mng.registerPrototype("SpectralShape", spectralShape->clone());

  //  add time-domain zerocrossings
  MarSystem* features = mng.create("Fanout", "featuresp");
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
  
  // if (pluginName != EMPTYSTRING) // don't put audio object
  // featureNetwork->addMarSystem(dest);
  
  featureNetwork->addMarSystem(mng.create("Features", "features"));
  

  featureNetwork->addMarSystem(mng.create("Memory", "memory"));
  featureNetwork->addMarSystem(mng.create("Statistics", "statistics"));  



  // update controls I
  featureNetwork->updctrl("Memory/memory/natural/memSize", memSize);
  // featureNetwork->updctrl(src->getType() + "/src/natural/inSamples", 
  // MRS_DEFAULT_SLICE_NSAMPLES);
  
  featureNetwork->updctrl(src->getType() + "/src/natural/inSamples", 
			  winSize);
  featureNetwork->updctrl(src->getType() + "/src/natural/pos", offset);      
  featureNetwork->addMarSystem(mng.create("Annotator", "annotator"));
  featureNetwork->addMarSystem(wsink->clone());
  

  cout << "winSize 1 = " << winSize << endl;


  // add classifier and confidence majority calculation 

  cout << "classifierName = " << classifierName << endl;
  if (classifierName == "GS")
    {
      featureNetwork->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
    }
  else if (classifierName == "ZeroR")
    featureNetwork->addMarSystem(mng.create("ZeroRClassifier", "zeror"));
  else if (classifierName == "KNN")
    featureNetwork->addMarSystem(mng.create("KNNClassifier", "knn"));
  else
    {
      cerr << "Unsuported classifier : " << classifierName << endl;
      return;
    }
  
  featureNetwork->addMarSystem(mng.create("Confidence", "confidence"));
 

  
  // update controls II 
  if (classifierName == "GS")
    featureNetwork->updctrl("GaussianClassifier/gaussian/natural/nLabels", (natural)cls.size());
  else if (classifierName == "ZeroR")
    featureNetwork->updctrl("ZeroRClassifier/zeror/natural/nLabels", (natural)cls.size());
  else if (classifierName == "KNN")
    featureNetwork->updctrl("KNNClassifier/knn/natural/nLabels", (natural)cls.size());

  featureNetwork->updctrl("Confidence/confidence/bool/mute", true);
  featureNetwork->updctrl("Confidence/confidence/string/labelNames",classNames);
  featureNetwork->updctrl("WekaSink/wsink/string/labelNames",classNames);
  
  
  // link controls
  featureNetwork->linkctrl("string/filename", 
			"SoundFileSource/src/string/filename");
  featureNetwork->linkctrl("natural/nChannels", 
			"SoundFileSource/src/natural/nChannels");
  featureNetwork->linkctrl("real/israte", 
			"SoundFileSource/src/real/israte");
  featureNetwork->linkctrl("natural/pos", 
			"SoundFileSource/src/natural/pos");

  if (pluginName != EMPTYSTRING) 
    featureNetwork->linkctrl("natural/nChannels", 
			     "AudioSink/dest/natural/nChannels");


  featureNetwork->linkctrl("bool/notEmpty", 
			"SoundFileSource/src/bool/notEmpty");
  featureNetwork->linkctrl("bool/mute", 
			"Gain/gt/bool/mute");  


  
  natural wc = 0;
  natural samplesPlayed =0;
  natural onSamples = featureNetwork->getctrl("natural/onSamples").toNatural();
  
  
  
  // main loop for extracting the features 
  featureNetwork->updctrl("Confidence/confidence/natural/nLabels", (int)cls.size());
  featureNetwork->updctrl("Confidence/confidence/bool/print",true); 
  string className = "";
  for (cj=0; cj < (natural)cls.size(); cj++)
    {
      Collection l = cls[cj];
      featureNetwork->updctrl("Annotator/annotator/natural/label", cj);
      featureNetwork->updctrl("WekaSink/wsink/natural/nLabels", (natural)cls.size());
      featureNetwork->updctrl("WekaSink/wsink/natural/downsample", 40);
      if (wekafname == EMPTYSTRING) 
	featureNetwork->updctrl("WekaSink/wsink/string/filename", "weka.arff");
      else 
	featureNetwork->updctrl("WekaSink/wsink/string/filename", wekafname);  

      // featureNetwork->updctrl("WekaSink/wsink/natural/label", cj);
      
      cout << "Class " << cj << " is " << l.name() << endl;
      
      featureNetwork->updctrl("Memory/memory/bool/reset", true);
      for (i=0; i < l.size(); i++)
	{
	  featureNetwork->updctrl("SoundFileSource/src/string/filename", l.entry(i));
	  wc = 0;  	  
	  samplesPlayed = 0;
	  
	  while ((featureNetwork->getctrl("SoundFileSource/src/bool/notEmpty").toBool()) && (duration > samplesPlayed))
	    {
	      featureNetwork->tick();
	      wc++;
	      samplesPlayed = wc * onSamples;
	    }
	  featureNetwork->tick();
	  cerr << "Processed " << l.entry(i) << endl;
	}
    }

  if (classifierName == "GS")
    featureNetwork->updctrl("GaussianClassifier/gaussian/bool/done",true);
  else if (classifierName == "ZeroR")  
    featureNetwork->updctrl("ZeroRClassifier/zeror/bool/done",true);
  else if (classifierName == "KNN")  
    featureNetwork->updctrl("KNNClassifier/knn/bool/done",true);
  
  featureNetwork->tick();		// train classifier

  // prepare network for classification

  if (classifierName == "GS")
    {
      featureNetwork->updctrl("GaussianClassifier/gaussian/bool/done",false);  
      featureNetwork->updctrl("GaussianClassifier/gaussian/string/mode","predict"); 
    }
  else if (classifierName == "ZeroR")  
    {
      featureNetwork->updctrl("ZeroRClassifier/zeror/bool/done",false);  
      featureNetwork->updctrl("ZeroRClassifier/zeror/string/mode","predict") ;
    }
  else if (classifierName == "KNN")  
    {
      featureNetwork->updctrl("KNNClassifier/knn/bool/done",false);  
      featureNetwork->updctrl("KNNClassifier/knn/string/mode","predict");
      featureNetwork->updctrl("KNNClassifier/knn/natural/k",3);
    }  
  
  if (pluginName != EMPTYSTRING) 
    {
      featureNetwork->updctrl("AudioSink/dest/bool/mute", false);
      featureNetwork->updctrl("AudioSink/dest/bool/init", false);
    }
  featureNetwork->updctrl("WekaSink/wsink/bool/mute", true);  
  featureNetwork->updctrl("Confidence/confidence/bool/mute", false);

  if (pluginName == EMPTYSTRING) // output to stdout 
    cout << (*featureNetwork) << endl;      
  else 
    {
      ofstream oss(pluginName.c_str());
      oss << (*featureNetwork) << endl;
    }
}


// train with multiple feature vectors/file 
void bextract_train_rmsilence(vector<Collection> cls, natural label, 
		    string pluginName, string classNames, 
		    string wekafname,  natural memSize, 
		    string extractorStr,
		    string classifierName)
{

  
  if (classifierName == EMPTYSTRING) 
    classifierName = "SMO";
  
  MRSDIAG("bextract.cpp - bextract_train");
  natural i;
  natural cj;

  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  // default 
  if (extractorStr == EMPTYSTRING) 
    extractorStr = "STFT";

  MarSystemManager mng;
  
  // Find proper soundfile format and create SignalSource 

  MarSystem *srm = mng.create("SilenceRemove", "srm");
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  
  srm->addMarSystem(src);
  


  // src->updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // srm->updctrl("SoundFileSource/src/natural/inSamples", 2048);
  // srm->updctrl("natural/inSamples", 2048);
  
  
  // Calculate duration, offest parameters if necessary 
  if (start > 0.0f) 
    offset = (natural) (start 
			* src->getctrl("real/israte").toReal() 
			* src->getctrl("natural/nChannels").toNatural());
  
  if (length != 30.0f) 
    duration = (natural) (length 
			  * src->getctrl("real/israte").toReal() 
			  * src->getctrl("natural/nChannels").toNatural());
  
  // create audio sink and mute it 
  // it is stored in the output plugin 
  // which can be used for real-time classification 


  MarSystem* dest=NULL;
  
  if (pluginName != EMPTYSTRING) // output to stdout 
    {
      dest = mng.create("AudioSink", "dest");
      dest->updctrl("bool/mute", true);
    }
  
  
  // Calculate windowed power spectrum and then 
  // calculate specific feature sets 
  
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->updctrl("Spectrum/spk/real/cutoff", 1.0);
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/string/spectrumType","power");  


  // Spectrum Shape descriptors
  Fanout spectrumFeatures("spectrumFeatures");
  if (extractorStr == "STFT") 
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      // spectrumFeatures.addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
    }
  else if (extractorStr == "STFTMFCC")
    {
      spectrumFeatures.addMarSystem(mng.create("Centroid", "cntrd"));
      // spectrumFeatures.addMarSystem(mng.create("Kurtosis", "krt"));
      spectrumFeatures.addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures.addMarSystem(mng.create("Flux", "flux"));
      spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
    }
  else if (extractorStr == "MFCC")
    spectrumFeatures.addMarSystem(mng.create("MFCC", "mfcc"));
  else if (extractorStr == "SCF")
    spectrumFeatures.addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SFM")
    spectrumFeatures.addMarSystem(mng.create("SFM", "sfm"));
  else 
    {
      cerr << "Extractor " << extractorStr << " is not supported " << endl;
      return;
    }
  

  mng.registerPrototype("SpectrumFeatures", spectrumFeatures.clone());


  // add the feature to spectral shape
  spectralShape->addMarSystem(mng.create("SpectrumFeatures", "spectrumFeatures"));
  mng.registerPrototype("SpectralShape", spectralShape->clone());

  //  add time-domain zerocrossings
  MarSystem* features = mng.create("Fanout", "featuresp");
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
  
  featureNetwork->addMarSystem(srm->clone());
  
  if (pluginName != EMPTYSTRING) // don't put audio object
    featureNetwork->addMarSystem(dest);
  

  cout << "featureNetwork = " << (*featureNetwork) << endl;
  
  featureNetwork->addMarSystem(mng.create("Features", "features"));
  

  featureNetwork->addMarSystem(mng.create("Memory", "memory"));
  featureNetwork->addMarSystem(mng.create("Statistics", "statistics"));  
  if (classifierName == "SMO")
    featureNetwork->addMarSystem(mng.create("NormMaxMin", "norm"));
  
  

  // update controls I
  featureNetwork->updctrl("Memory/memory/natural/memSize", memSize);
  // featureNetwork->updctrl(src->getType() + "/src/natural/inSamples", 
  // MRS_DEFAULT_SLICE_NSAMPLES);
  



  
  featureNetwork->updctrl("SilenceRemove/srm/" + src->getType() + "/src/natural/inSamples", 
			  winSize);
  featureNetwork->updctrl("SilenceRemove/srm/" + src->getType() + "/src/natural/pos", offset);      
  featureNetwork->addMarSystem(mng.create("Annotator", "annotator"));
  featureNetwork->addMarSystem(wsink->clone());
  

  // add classifier and confidence majority calculation 


  if (classifierName == "GS")
    featureNetwork->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
  else if (classifierName == "ZeroR")
    featureNetwork->addMarSystem(mng.create("ZeroRClassifier", "zeror"));
  else if (classifierName == "KNN")
    featureNetwork->addMarSystem(mng.create("KNNClassifier", "knn"));
  else if (classifierName == "SMO") 
    featureNetwork->addMarSystem(mng.create("SMO", "smo"));
  else
    {
      cerr << "Unsuported classifier : " << classifierName << endl;
      return;
    }
  
  
  featureNetwork->addMarSystem(mng.create("Confidence", "confidence"));
 
  // update controls II 
  if (classifierName == "GS")
    featureNetwork->updctrl("GaussianClassifier/gaussian/natural/nLabels", (natural)cls.size());
  else if (classifierName == "ZeroR")
    featureNetwork->updctrl("ZeroRClassifier/zeror/natural/nLabels", (natural)cls.size());
  else if (classifierName == "KNN")
    featureNetwork->updctrl("KNNClassifier/knn/natural/nLabels", (natural)cls.size());
  else if (classifierName == "SMO")
    featureNetwork->updctrl("SMO/smo/natural/nLabels", (natural)cls.size());

  
  featureNetwork->updctrl("Confidence/confidence/bool/mute", true);
  featureNetwork->updctrl("Confidence/confidence/string/labelNames",classNames);
  featureNetwork->updctrl("WekaSink/wsink/string/labelNames",classNames);
  

  // link controls
  featureNetwork->linkctrl("string/filename", 
			"SilenceRemove/srm/SoundFileSource/src/string/filename");
  featureNetwork->linkctrl("natural/nChannels", 
			"SilenceRemove/srm/src/natural/nChannels");
  featureNetwork->linkctrl("real/israte", 
			"SilenceRemove/srm/SoundFileSource/src/real/israte");
  featureNetwork->linkctrl("natural/pos", 
			"SilenceRemove/srm/SoundFileSource/src/natural/pos");

  /* if (pluginName != EMPTYSTRING) 
    featureNetwork->linkctrl("natural/nChannels", 
			     "AudioSink/dest/natural/nChannels");
  */ 


  featureNetwork->linkctrl("bool/notEmpty", 
			"SilenceRemove/srm/SoundFileSource/src/bool/notEmpty");
  featureNetwork->linkctrl("bool/mute", 
			"Gain/gt/bool/mute");  


  
  natural wc = 0;
  natural samplesPlayed =0;
  natural onSamples = featureNetwork->getctrl("natural/onSamples").toNatural();
  
  
  // main loop for extracting the features 
  featureNetwork->updctrl("Confidence/confidence/natural/nLabels", (int)cls.size());
  string className = "";
  for (cj=0; cj < (natural)cls.size(); cj++)
    {
      Collection l = cls[cj];
      featureNetwork->updctrl("Annotator/annotator/natural/label", cj);
      featureNetwork->updctrl("WekaSink/wsink/natural/nLabels", (natural)cls.size());
      featureNetwork->updctrl("WekaSink/wsink/natural/downsample", 40);
      if (wekafname == EMPTYSTRING) 
	featureNetwork->updctrl("WekaSink/wsink/string/filename", "weka.arff");
      else 
	featureNetwork->updctrl("WekaSink/wsink/string/filename", wekafname);  

      // featureNetwork->updctrl("WekaSink/wsink/natural/label", cj);
      
      cout << "Class " << cj << " is " << l.name() << endl;
      
      featureNetwork->updctrl("Memory/memory/bool/reset", true);
      for (i=0; i < l.size(); i++)
	{
	  featureNetwork->updctrl("Memory/memory/bool/reset", true);
	  featureNetwork->updctrl("SilenceRemove/srm/SoundFileSource/src/string/filename", l.entry(i));
	  wc = 0;  	  
	  samplesPlayed = 0;
	  
	  while ((featureNetwork->getctrl("SilenceRemove/srm/SoundFileSource/src/bool/notEmpty").toBool()) && (duration > samplesPlayed))
	    {
	      featureNetwork->tick();
	      wc++;
	      samplesPlayed = wc * onSamples;
	    }
	  featureNetwork->tick();
	  cerr << "Processed " << l.entry(i) << endl;
	}
    }

  if (classifierName == "GS")
    featureNetwork->updctrl("GaussianClassifier/gaussian/bool/done",true);
  else if (classifierName == "ZeroR")  
    featureNetwork->updctrl("ZeroRClassifier/zeror/bool/done",true);
  else if (classifierName == "KNN")  
    featureNetwork->updctrl("KNNClassifier/knn/bool/done",true);
  else if (classifierName == "SMO")  
    featureNetwork->updctrl("SMO/smo/bool/done",true);
  
  if (classifierName == "SMO")
    featureNetwork->updctrl("NormMaxMin/norm/bool/train", false);
  featureNetwork->tick();		// train classifier

  // prepare network for classification

  if (classifierName == "GS")
    {
      featureNetwork->updctrl("GaussianClassifier/gaussian/bool/done",false);  
      featureNetwork->updctrl("GaussianClassifier/gaussian/string/mode","predict"); 
    }
  else if (classifierName == "ZeroR")  
    {
      featureNetwork->updctrl("ZeroRClassifier/zeror/bool/done",false);  
      featureNetwork->updctrl("ZeroRClassifier/zeror/string/mode","predict") ;
    }
  else if (classifierName == "KNN")  
    {
      featureNetwork->updctrl("KNNClassifier/knn/bool/done",false);  
      featureNetwork->updctrl("KNNClassifier/knn/string/mode","predict");
      featureNetwork->updctrl("KNNClassifier/knn/natural/k",3);
    }  

  else if (classifierName == "SMO")  
    {
      featureNetwork->updctrl("SMO/smo/bool/done",false);  
      featureNetwork->updctrl("SMO/smo/string/mode","predict");
    }  

  
  /* if (pluginName != EMPTYSTRING) 
    {
      featureNetwork->updctrl("AudioSink/dest/bool/mute", false);
      featureNetwork->updctrl("AudioSink/dest/bool/init", false);
    }
  */ 
  
  featureNetwork->updctrl("Confidence/confidence/bool/mute", false);
  featureNetwork->updctrl("string/filename", "defaultfile");
  featureNetwork->updctrl("WekaSink/wsink/bool/mute", true);
  
  if (pluginName == EMPTYSTRING) // output to stdout 
    cout << (*featureNetwork) << endl;      
  else 
    {
      ofstream oss(pluginName.c_str());
      oss << (*featureNetwork) << endl;
    }
}



void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  natural attempts  =0;


  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
    {
      attempts++;
    }
  else
    {
      from1 >> l;
      l.setName(name.substr(0, name.rfind(".", name.length())));
    }

  
  if (attempts == 1) 
    {
      string warn;
      warn += "Problem reading collection ";
      warn += name; 
      warn += " - tried both default mf directory and current working directory";
      MRSWARN(warn);
      exit(1);

    }
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
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addStringOption("wekafile", "w", EMPTYSTRING);
  cmd_options.addStringOption("filefeature", "f", EMPTYSTRING);
  cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
  cmd_options.addNaturalOption("memory", "m", 40);
  cmd_options.addNaturalOption("nwinsamples", "n", 512);
  cmd_options.addStringOption("classifier", "c", EMPTYSTRING);
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
  filefeaturename   = cmd_options.getStringOption("filefeature");
  wekafname = cmd_options.getStringOption("wekafile");
  extractorName = cmd_options.getStringOption("extractor");
  classifierName = cmd_options.getStringOption("classifier");
  memSize = cmd_options.getNaturalOption("memory");
  winSize = cmd_options.getNaturalOption("nwinsamples");
  
}



void bextract(vector<string> soundfiles, natural label, 
	      string pluginName, string classNames, 
	      string wekafname,  natural memSize, 
	      string extractorStr,
	      string classifierName)
{
  
  MarSystemManager mng;
  
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  if (extractorStr == "STFT") 
    {
      spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
    }  
  else if (extractorStr == "MFCC")
    {
      spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
    }
  else if (extractorStr == "STFTMFCC")
    {
      spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
      spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));      
      spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
    }
  else
    {
      cout << "New bextract does not support extractor " << extractorStr << endl;
      return;
      
    }
  
  
  // Means and standard deviations of the spectrum features 
  // over a 1-second window (40 analysis frames) 
  MarSystem* textureFeatures = mng.create("Series", "textureFeatures");
  textureFeatures->addMarSystem(spectrumFeatures);
  textureFeatures->addMarSystem(mng.create("Memory", "textureMemory"));
  
  MarSystem* textureStats = mng.create("Fanout", "textureStats");
  textureStats->addMarSystem(mng.create("Mean", "mean"));
  textureStats->addMarSystem(mng.create("StandardDeviation", "std"));
  textureFeatures->addMarSystem(textureStats);
  

  // The main feature calculation network 
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("SoundFileSource", "src"));
  //spectralShape->addMarSystem(mng.create("AudioSink", "dest"));
  
  spectralShape->addMarSystem(mng.create("Hamming", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum", "spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->addMarSystem(textureFeatures);

  // Label each class
  spectralShape->addMarSystem(mng.create("Annotator", "anno"));
  spectralShape->addMarSystem(mng.create("WekaSink", "wsink"));
  spectralShape->addMarSystem(mng.create("GaussianClassifier", "gsc"));
  spectralShape->addMarSystem(mng.create("Confidence", "conf"));
  
  

  vector<string>::iterator si;
  natural classCount = 0;
  


  if (wekafname == EMPTYSTRING) 
    spectralShape->updctrl("WekaSink/wsink/string/filename", "weka.arff");
  else 
    spectralShape->updctrl("WekaSink/wsink/string/filename", wekafname);

  spectralShape->updctrl("GaussianClassifier/gsc/natural/nLabels", (natural)soundfiles.size());
  

  spectralShape->linkctrl("bool/notEmpty", 
			  "SoundFileSource/src/bool/notEmpty");

  spectralShape->linkctrl("string/filename", 
			  "SoundFileSource/src/string/filename");

  spectralShape->linkctrl("natural/pos", 
			  "SoundFileSource/src/natural/pos");


  
  spectralShape->updctrl("GaussianClassifier/gsc/string/mode","train");  
  spectralShape->updctrl("WekaSink/wsink/string/labelNames",classNames);
  spectralShape->updctrl("Confidence/conf/string/labelNames",classNames);



  for (si = soundfiles.begin(); si != soundfiles.end(); ++si)
    {

      cout << "Processing class " << classCount << " collection: " 
	   << *si << endl;
      spectralShape->updctrl("SoundFileSource/src/string/filename", *si);
      spectralShape->updctrl("Annotator/anno/natural/label", classCount);
      classCount ++;
      

      while(spectralShape->getctrl("bool/notEmpty").toBool()) 
      {
	spectralShape->tick();
      }


    }
  spectralShape->updctrl("GaussianClassifier/gsc/bool/done",true);

  spectralShape->tick(); // train classifier 
  spectralShape->updctrl("GaussianClassifier/gsc/string/mode","predict");     
  

  if (pluginName == EMPTYSTRING) // output to stdout 
    cout << (*spectralShape) << endl;      
  else 
    {
      ofstream oss(pluginName.c_str());
      oss << (*spectralShape) << endl;
    }
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
  
  string classNames = "";
  vector<Collection> cls;

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {	
      string sfname = *sfi;
      Collection l;
      readCollection(l,sfname);
      l.write("patata.mf");
      
      classNames += (l.name()+',');
      cls.push_back(l);
      i++;
    }
  
  string extractorStr = extractorName;
  
  if (extractorStr.substr(0,2) == "SV") 
    {
      bool withBeatFeatures = false;
      string extrName;
      
      if (extractorName.substr(extractorName.length()-4, extractorName.length()) == "BEAT")
	{
	  withBeatFeatures = true;
	  extrName = extractorName.substr(2, extractorName.length()-6);
	}
      else 
	extrName = extractorName.substr(2, extractorName.length());	

      
      bextract_trainAccumulator(cls, i, pluginName, classNames, wekafname, filefeaturename, memSize, extrName,
				withBeatFeatures);
    }
  else if (extractorStr.substr(0,2) == "RS")
    {
      
      string extrName;
      extrName = extractorName.substr(2, extractorName.length());	      
      bextract_train_rmsilence(cls, i, pluginName, classNames, wekafname, memSize, extrName, classifierName);      
    }
  
  else
    {
      cout << "Extractor = " << extractorName << endl;
      
      bextract_train(cls, i, pluginName, classNames, wekafname, memSize, extractorName, classifierName);


      // bextract(soundfiles, i, pluginName, classNames, wekafname, memSize, 
      // extractorName, classifierName);
      

  }
  
  exit(0);
}






