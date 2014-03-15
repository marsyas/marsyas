/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/Accumulator.h>
#include <marsyas/marsystems/Fanout.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/TimeLine.h>
#include <marsyas/FileName.h>
#include <marsyas/common_source.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
int normopt;
bool tline;

mrs_natural offset = 0;
mrs_real duration = 120.0f;
mrs_natural memSize = 1;
mrs_natural winSize = 512;
mrs_natural hopSize = 512;
mrs_real samplingRate_ = 22050.0;
mrs_natural accSize_ = 20000;  // approximately 5 minutes at 44100 sr, 512 win
mrs_real start = 0.0;
mrs_real length = -1.0;
mrs_real gain = 1.0;
mrs_bool pluginMute = 0.0;
mrs_bool csvOutput = false;
mrs_bool playback = false;
mrs_bool lexiconopt = false;
mrs_natural cmopt = 40;

mrs_bool stereo_ = false;
mrs_bool featExtract_ = false;

mrs_bool spsf_ = false;
mrs_bool mfcc_ = false;
mrs_bool chroma_ = false;
mrs_bool sfm_ = false;
mrs_bool scf_ = false;
mrs_natural downSample = 1;

mrs_bool ctd_ = false;
mrs_bool rlf_ = false;
mrs_bool flx_ = false;
mrs_bool lsp_ = false;
mrs_bool lpcc_ = false;
mrs_bool beat_ = false;

mrs_bool only_stable_ = false;
mrs_bool regression_ = false;

// SAI/VQ mode
mrs_bool saivq_mode_ = false;


mrs_bool single_vector_ = false;

mrs_bool spectralFeatures_ = false;
mrs_bool zcrs_ = false;
mrs_bool timbralFeatures_ = false;
mrs_bool shuffle_;
mrs_bool mic_;


#define DEFAULT_EXTRACTOR "STFT"
#define DEFAULT_CLASSIFIER  "SVM"

string workspaceDir = "./";
string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string extractorName = "REFACTORED";
string classifierName = EMPTYSTRING;
string collectionName = EMPTYSTRING;
string predictCollection = EMPTYSTRING;
string testCollection = EMPTYSTRING;

CommandLineOptions cmd_options;

// map table with pointers to the functions that create
// each specific and supported feature extraction MarSystem
typedef MarSystem* (*FeatureExtractorFactory)();
map<string,FeatureExtractorFactory >  featExtractors;
map<string, string> featExtractorDesc;

//////////////////////////////////////////////////////////////////////////
// Feature Extractors creation Functions
//////////////////////////////////////////////////////////////////////////
MarSystem* createExtractorFromFile()
{
  MarSystemManager mng;

  //this opens a .mpl file and creates in run-time
  //the MarSystem network for feature extraction.
  //NOTE:
  //the network should not contain any source MarSystem
  //since that is added by bextract
  ifstream mplFile(extractorName.c_str());

  return mng.getMarSystem(mplFile);
}


MarSystem* createBeatHistogramFeatureNetwork()
{
  MarSystemManager mng;

  MarSystem *beatTracker = mng.create("Series/beatTracker");


  MarSystem *onset_strength = mng.create("Series/onset_strength");
  MarSystem *accum = mng.create("Accumulator/accum");
  MarSystem *fluxnet = mng.create("Series/fluxnet");
  fluxnet->addMarSystem(mng.create("SoundFileSource", "src"));
  fluxnet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  fluxnet->addMarSystem(mng.create("ShiftInput", "si"));
  fluxnet->addMarSystem(mng.create("Windowing", "windowing1"));
  fluxnet->addMarSystem(mng.create("Spectrum", "spk"));
  fluxnet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  fluxnet->addMarSystem(mng.create("Flux", "flux"));
  accum->addMarSystem(fluxnet);

  onset_strength->addMarSystem(accum);
  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));
  onset_strength->addMarSystem(mng.create("Filter", "filt1"));
  onset_strength->addMarSystem(mng.create("Reverse", "reverse1"));
  onset_strength->addMarSystem(mng.create("Filter", "filt2"));
  onset_strength->addMarSystem(mng.create("Reverse", "reverse2"));

  beatTracker->addMarSystem(onset_strength);

  MarSystem *tempoInduction = mng.create("FlowThru/tempoInduction");
  tempoInduction->addMarSystem(mng.create("AutoCorrelation", "acr"));
  tempoInduction->addMarSystem(mng.create("BeatHistogram", "histo"));

  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  tempoInduction->addMarSystem(hfanout);
  tempoInduction->addMarSystem(mng.create("Sum", "hsum"));

  tempoInduction->addMarSystem(mng.create("BeatHistoFeatures", "bhf"));
  beatTracker->addMarSystem(tempoInduction);

  mrs_natural winSize = 256;
  mrs_natural hopSize = 128;
  mrs_natural  bwinSize = 2048;
  mrs_natural bhopSize = 128;

  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", bhopSize);
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize",bwinSize);


  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.0564;
  bcoeffs(1) = 0.1129;
  bcoeffs(2) = 0.0564;
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0000;
  acoeffs(1) = -1.2247;
  acoeffs(2) = 0.4504;
  onset_strength->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
  onset_strength->updControl("Filter/filt2/mrs_realvec/dcoeffs", acoeffs);


  onset_strength->updControl("Accumulator/accum/Series/fluxnet/PowerSpectrum/pspk/mrs_string/spectrumType", "logmagnitude");
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "Laroche2003");

  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/endBin", 800);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/factor", 16.0);


  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  tempoInduction->updControl("Fanout/hfanout/Gain/id1/mrs_real/gain", 1.0);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.9);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to0", true);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to1", true);

  onset_strength->updControl("Accumulator/accum/Series/fluxnet/ShiftInput/si/mrs_natural/winSize", winSize);

  beatTracker->updControl("mrs_natural/inSamples", hopSize);

  return beatTracker;
}


MarSystem* createBEATextrator()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "beatExtrator");
  extractor->addMarSystem(mng.create("SoundFileSource", "src1"));
  extractor->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  extractor->addMarSystem(mng.create("ShiftInput", "si"));
  extractor->addMarSystem(mng.create("DownSampler", "initds"));
  extractor->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  extractor->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  extractor->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  extractor->addMarSystem(mng.create("OnePole", "lpf"));
  extractor->addMarSystem(mng.create("Norm", "norm"));
  {
    // Extra gain added for compensating the cleanup of the Norm Marsystem,
    // which used a 0.05 internal gain for some unknown reason.
    // \todo is this weird gain factor actually required?
    extractor->addMarSystem(mng.create("Gain", "normGain"));
    extractor->updControl("Gain/normGain/mrs_real/gain", 0.05);
  }
  extractor->addMarSystem(mng.create("Sum", "sum"));
  extractor->addMarSystem(mng.create("DownSampler", "ds"));
  extractor->addMarSystem(mng.create("AutoCorrelation", "acr"));
  extractor->addMarSystem(mng.create("Peaker", "pkr"));
  extractor->addMarSystem(mng.create("MaxArgMax", "mxr"));
  extractor->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
  extractor->addMarSystem(mng.create("BeatHistogramFromPeaks", "histo"));
  // extractor->addMarSystem(mng.create("PlotSink", "psink"));
  // extractor->addMarSystem(mng.create("Reassign", "reassign"));
  extractor->addMarSystem(mng.create("BeatHistoFeatures", "bhf"));
  extractor->linkControl("mrs_natural/winSize", "ShiftInput/si/mrs_natural/winSize");
  return extractor;
}

MarSystem* createSTFTextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Fanout", "STFTextractor");

  // Time domain features
  extractor->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

  // Frequency domain features
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));

  MarSystem* spectrumFeatures = mng.create("STFT_features", "spectrumFeatures");
  spectralShape->addMarSystem(spectrumFeatures);
  extractor->addMarSystem(spectralShape);

  extractor->linkControl("mrs_natural/winSize", "Series/spectralShape/PowerSpectrumNet/powerSpect/mrs_natural/winSize");
  extractor->linkControl("mrs_string/enableChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/enableChild");
  extractor->linkControl("mrs_string/disableChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/disableChild");

  return extractor;
}

MarSystem* createMFCCextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "MFCCextractor");
  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  extractor->addMarSystem(mng.create("MFCC", "mfcc"));

  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

MarSystem* createSTFTMFCCextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "STFTMFCCextractor");
  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  extractor->addMarSystem(spectrumFeatures);
  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

MarSystem* createSCFextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "SCFextractor");
  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  extractor->addMarSystem(mng.create("SCF", "scf"));

  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

MarSystem* createSFMextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "SFMextractor");
  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  extractor->addMarSystem(mng.create("SFM", "sfm"));

  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

MarSystem* createSFMSCFextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "SFMSCFextractor");

  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));
  spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));

  extractor->addMarSystem(spectrumFeatures);

  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

MarSystem* createLSPextractor()
{
  MarSystemManager mng;

  mrs_natural order = 18;
  cout << "LSP order = " << order << endl;

  MarSystem* extractor = mng.create("Series","LSPextractor");

  //first add a LPC calculation net
  extractor->addMarSystem(mng.create("LPCnet", "lpcNet"));
  //add the LSP calculation
  extractor->addMarSystem(mng.create("LSP", "lsp"));

  //must create a link otherwise the outmost composite will know nothing
  //about this order change (which affects output nr of observations) and
  //consequently will not update the network accordingly!
  extractor->linkControl("mrs_natural/order", "LPCnet/lpcNet/mrs_natural/order");
  extractor->updControl("mrs_natural/order", order);

  extractor->linkControl("mrs_natural/winSize", "LPCnet/lpcNet/mrs_natural/winSize");

  return extractor;
}

MarSystem* createLPCCextractor()
{
  MarSystemManager mng;

  mrs_natural order = 12; //12 order LPCC
  cout << "LPCC order = " << order << endl;

  MarSystem* extractor = mng.create("Series","LPCCextractor");

  //first add a LPC calculation net
  extractor->addMarSystem(mng.create("LPCnet", "lpcNet"));
  //add the LPCC calculation
  extractor->addMarSystem(mng.create("LPCC", "lpcc"));

  //must create a link otherwise the outmost composite will know nothing
  //about this order change (which affects output nr of observations) and
  //consequently will not update the network accordingly!
  extractor->linkControl("mrs_natural/order", "LPCnet/lpcNet/mrs_natural/order");
  extractor->updControl("mrs_natural/order", order);

  extractor->linkControl("mrs_natural/winSize", "LPCnet/lpcNet/mrs_natural/winSize");

  return extractor;
}

int
printUsage(string progName)
{
  MRSDIAG("bextract.cpp - printUsage");
  cerr << "Usage : " << progName << " [-e extractor] [-h help] [-s start(seconds)] [-l length(seconds)] [-m memory]  [-u usage] collection1 collection2 ... collectionN" << endl;
  cerr << endl;
  return 0;
}

int
printHelp(string progName)
{
  MRSDIAG("bextract.cpp - printHelp");
  cerr << "bextract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u  --usage        : display short usage info" << endl;
  cerr << "-v  --verbose      : verbose output" << endl;
  cerr << "-c  --collection   : use files in this collection [only for MIREX 2007]" << endl;
  cerr << "-n  --normalize    : enable normalization" << endl;
  cerr << "-as --accSize      : accumulator size" << endl;
  cerr << "-cl --classifier   : classifier name" << endl;
  cerr << "-pr --predict      : predict class for files in collection" << endl;
  cerr << "-fe --featExtract  : only extract features" << endl;
  cerr << "-tc --test         : test collection" << endl;
  cerr << "-st --stereo       : use stereo feature extraction" << endl;
  cerr << "-ds --downsample   : downsampling factor" << endl;
  cerr << "-h  --help         : display this information " << endl;
  cerr << "-e  --extractor    : extractor " << endl;
  cerr << "-p  --plugin       : output plugin name " << endl;
  cerr << "-pm --pluginmute   : mute the plugin " << endl;
  cerr << "-csv --csvoutput   : output confidence values in sfplugin in csv format"<<endl;
  cerr << "-pb --playback     : playback during feature extraction " << endl;
  cerr << "-s  --start        : playback start offset in seconds " << endl;
  cerr << "-sh --shuffle      : shuffle collection file before processing" << endl;

  cerr << "-l  --length       : playback length in seconds " << endl;
  cerr << "-m  --memory       : memory size " << endl;
  cerr << "-w  --wekafile     : weka .arff filename " << endl;
  cerr << "-od --outputdir    : output directory for output of files" << endl;
  cerr << "-ws --windowsize   : analysis window size in samples " << endl;
  cerr << "-hp --hopsize      : analysis hop size in samples " << endl;
  cerr << "-t  --timeline     : flag 2nd input collection as timelines for the 1st collection"<<endl;
  cerr << "-os  --onlyStable  : only output 'stable' frames (silently omit the rest)"<<endl;
  cerr << "-rg  --regression  : print regression labels instead of classification labels"<<endl;
  cerr << endl;

  cerr << "Available extractors: " << endl;
  cerr << "---------------------" << endl;
  map<string, FeatureExtractorFactory >::const_iterator it;
  for(it = featExtractors.begin(); it != featExtractors.end(); ++it)
  {
    cerr << it->first << ": " << featExtractorDesc[it->first] << endl;
  }
  cerr << endl;
  cerr << "NOTE:" << endl;
  cerr << "All extractors calculate means and variances over a memory size window" << endl;
  cerr << "SV can be appended in front of any extractor to extract a single vector (mean, variances) over a 30-second clip (for example SVSTFT) " << endl;

  return 0;
}



void
beatHistogramFeatures(MarSystem* beatTracker, string sfName, realvec& beatfeatures)
{
  // cout << "Calculating Beat Histogram Features: " << sfName << endl;
  beatTracker->updControl("Series/onset_strength/Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);

  //mrs_natural bwinSize = 2048;
  mrs_natural bhopSize = 128;
  mrs_natural hopSize = 128;
  //mrs_natural winSize = 256;


  vector<mrs_real> bpms;
  vector<mrs_real> secondary_bpms;
  // mrs_real bin;

  //int extra_ticks = bwinSize/bhopSize;
  mrs_realvec tempos(2);
  mrs_realvec tempo_scores(2);
  tempo_scores.setval(0.0);
  mrs_realvec estimate;

  int counter = 0;

  ofstream oss;
  oss.open("beatTracker.mpl");
  oss << *beatTracker;

  mrs_natural size_in_bytes = beatTracker->getctrl("Series/onset_strength/Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
  mrs_natural num_ticks = (size_in_bytes / (hopSize * bhopSize)) + 1;
  // for now hard-wire to about 30 seconds
  num_ticks = 40;



  mrs_natural ticks = 0;

  while (1)
  {


    beatTracker->tick();
    ticks++;


    // TODO: estimate should only be written after the last tick. This is redundant copying.
    estimate = beatTracker->getctrl("FlowThru/tempoInduction/BeatHistoFeatures/bhf/mrs_realvec/processedData")->to<mrs_realvec>();


    if (num_ticks - ticks < 1)
    {
      break;
    }

    counter++;
  }


  // TODO this is redundant, just return estimate after calling estimate.transpose();
  for (int i=0; i < beatfeatures.getSize(); i++)
    beatfeatures(i) = estimate(i);

  beatTracker->updControl("FlowThru/tempoInduction/BeatHistogram/histo/mrs_bool/reset", true);

}



void
tempo_histoSumBands(MarSystem* total1, string sfName, realvec& beatfeatures,
                    realvec& iwin, realvec& estimate)
{
  estimate.setval(0.0);

  mrs_real srate;

  // prepare network

  // update the controls
  // input filename with hopSize/winSize

  total1->updControl("SoundFileSource/src1/mrs_string/filename", sfName);
  srate = total1->getctrl("SoundFileSource/src1/mrs_real/osrate")->to<mrs_real>();

  mrs_natural ifactor = 8;
  total1->updControl("DownSampler/initds/mrs_natural/factor", ifactor);

  mrs_natural winSize = (mrs_natural) ((srate / 22050.0) * 2 * 65536);
  mrs_natural hopSize = winSize / 16;

  offset = (mrs_natural) (start * srate);

  // only do 30 seconds
  duration = (mrs_natural) (30.0 * srate);

  total1->updControl("mrs_natural/inSamples", hopSize);
  total1->updControl("SoundFileSource/src1/mrs_natural/pos", offset);
  total1->updControl("SoundFileSource/src1/mrs_natural/inSamples", hopSize);
  total1->updControl("ShiftInput/si/mrs_natural/winSize", winSize);
  total1->updControl("ShiftInput/si/mrs_bool/reset", true);
  total1->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 3);

  // wavelet filterbank envelope extraction controls
  total1->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total1->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total1->updControl("DownSampler/ds/mrs_natural/factor", factor);

  srate = total1->getctrl("DownSampler/initds/mrs_real/osrate")->to<mrs_real>();

  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM
  mrs_natural pkinS = total1->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) -
                          (mrs_natural)(srate * 60.0 / (factor *62.0))) / (pkinS * 1.0);
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 200.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 50.0));
  total1->updControl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total1->updControl("Peaker/pkr/mrs_real/peakStrength", 0.5);
  total1->updControl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total1->updControl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total1->updControl("Peaker/pkr/mrs_real/peakGain", 2.0);
  total1->updControl("BeatHistogramFromPeaks/histo/mrs_natural/startBin", 0);
  total1->updControl("BeatHistogramFromPeaks/histo/mrs_natural/endBin", 250);
  total1->updControl("BeatHistogramFromPeaks/histo/mrs_bool/reset", true);

  // prepare vectors for processing
  /* realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
   total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
   realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  */

  mrs_natural onSamples;

  int numPlayed =0;
  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;

  // vector of bpm estimate used to calculate median
  onSamples = total1->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

  total1->updControl("SoundFileSource/src1/mrs_natural/pos", 0);

  while (total1->getctrl("SoundFileSource/src1/mrs_bool/hasData")->to<mrs_bool>())
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

void
bextract_trainStereoSPS(vector<Collection> cls, string classNames,
                        string wekafname, mrs_natural memSize)
{
  (void) memSize;
  cout << "STEREO SPS" << endl;
  cout << "classNames = "  << classNames << endl;
  cout << "wekafname = "  << wekafname << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  // playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);
  playbacknet->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));
  total->addMarSystem(mng.create("SVMClassifier", "svmcl"));

  total->updControl("mrs_natural/inSamples", 1024);

  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  // cout << *total << endl;
  Collection l;

  if (!collection_has_labels)
  {
    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", classNames);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (size_t cj=0; cj < cls.size(); cj++)
    {
      Collection l = cls[cj];

      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
      for (mrs_natural i=0; i < l.size(); ++i)
      {
        total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
        cout << "Processing" << l.entry(i) << endl;
        total->tick();
      }
    }
  }
  else
  {

    l = cls[0];

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "train");

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
      cout << "Processing" << l.entry(i) << endl;
      total->tick();
    }
  }


  if (testCollection != EMPTYSTRING)
  {
    Collection m;
    m.read(testCollection);
    if (wekafname != EMPTYSTRING)
      total->updControl("WekaSink/wsink/mrs_string/filename", "predict.arff");
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "predict");

    ofstream prout;
    prout.open(predictCollection.c_str());

    for (mrs_natural i=0; i < m.size(); ++i)//iterate over collection files
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      total->tick();
      mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
      prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
    }
  }
}

void
bextract_trainStereoSPSMFCC(vector<Collection> cls, string classNames,
                            string wekafname, mrs_natural memSize)
{
  (void) memSize;
  cout << "STEREO SPS+MFCC" << endl;
  cout << "classNames = "  << classNames << endl;
  cout << "wekafname = "  << wekafname << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* ffanout = mng.create("Fanout", "ffanout");

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("MFCC", "leftMFCC"));
  left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("MFCC", "rightMFCC"));
  right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  MarSystem* secondbranch = mng.create("Series", "secondbranch");
  MarSystem* stereobranches1 = mng.create("Parallel", "stereobranches1");
  MarSystem* left1 = mng.create("Series", "left1");
  MarSystem* right1 = mng.create("Series", "right1");

  left1->addMarSystem(mng.create("Windowing", "hamleft1"));
  left1->addMarSystem(mng.create("Spectrum", "spkleft1"));
  right1->addMarSystem(mng.create("Windowing", "hamright1"));
  right1->addMarSystem(mng.create("Spectrum", "spkright1"));

  stereobranches1->addMarSystem(left1);
  stereobranches1->addMarSystem(right1);

  secondbranch->addMarSystem(stereobranches1);
  secondbranch->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  secondbranch->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  secondbranch->addMarSystem(mng.create("TextureStats", "texturests"));

  playbacknet->addMarSystem(ffanout);
  ffanout->addMarSystem(stereobranches);
  ffanout->addMarSystem(secondbranch);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));
  total->addMarSystem(mng.create("SVMClassifier", "svmcl"));

  total->updControl("mrs_natural/inSamples", 1024);

  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  Collection l;

  if (!collection_has_labels)
  {

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", classNames);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (size_t cj=0; cj < cls.size(); cj++)
    {
      Collection l = cls[cj];
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
      for (mrs_natural i=0; i < l.size(); ++i)
      {
        total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
        cout << "Processing" << l.entry(i) << endl;
        total->tick();
      }
    }
  }
  else
  {
    l = cls[0];

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "train");

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
      cout << "Processing" << l.entry(i) << endl;
      total->tick();
    }
  }

  if (testCollection != EMPTYSTRING)
  {
    Collection m;
    m.read(testCollection);
    if (wekafname != EMPTYSTRING)
      total->updControl("WekaSink/wsink/mrs_string/filename", "predict.arff");
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "predict");

    ofstream prout;
    prout.open(predictCollection.c_str());

    for (mrs_natural i=0; i < m.size(); ++i)//iterate over collection files
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      total->tick();
      mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
      prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
    }
  }
}

void
bextract_trainStereoMFCC(vector<Collection> cls, string classNames,
                         string wekafname, mrs_natural memSize)
{
  (void) memSize;
  cout << "STEREO MFCC" << endl;
  cout << "classNames = "  << classNames << endl;
  cout << "wekafname = "  << wekafname << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("MFCC", "leftMFCC"));
  left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("MFCC", "rightMFCC"));
  right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));
  total->addMarSystem(mng.create("SVMClassifier", "svmcl"));
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  total->updControl("mrs_natural/inSamples", 1024);

  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  // cout << *total << endl;
  Collection l;

  if (!collection_has_labels)
  {
    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", classNames);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (size_t cj=0; cj < cls.size(); cj++)
    {
      Collection l = cls[cj];
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
      for (mrs_natural i=0; i < l.size(); ++i)
      {
        total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
        cout << "Processing" << l.entry(i) << endl;
        total->tick();
      }
    }
  }
  else
  {
    l = cls[0];

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "train");

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
      cout << "Processing" << l.entry(i) << endl;
      total->tick();
    }
  }

  if (testCollection != EMPTYSTRING)
  {
    Collection m;
    m.read(testCollection);
    if (wekafname != EMPTYSTRING)
      total->updControl("WekaSink/wsink/mrs_string/filename", "predict.arff");
    total->updControl("SVMClassifier/svmcl/mrs_string/mode", "predict");

    ofstream prout;
    prout.open(predictCollection.c_str());

    for (mrs_natural i=0; i < m.size(); ++i)//iterate over collection files
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      total->tick();
      mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
      prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
    }
  }



}
//---------------------------------------------------------------------------------------------------------
void
bextract_trainADRessStereoSPS(vector<Collection> cls, string classNames,
                              string wekafname, mrs_natural memSize)
{
  (void) memSize;
  cout << "ADRess STEREO SPS" << endl;
  cout << "classNames = "  << classNames << endl;
  cout << "wekafname = "  << wekafname << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  // playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);
  playbacknet->addMarSystem(mng.create("ADRess", "aspk"));
  playbacknet->addMarSystem(mng.create("ADRessStereoSpectrum", "asspk"));
  playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  total->updControl("mrs_natural/inSamples", 1024);

  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  // cout << *total << endl;

  if (!collection_has_labels)
  {
    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", classNames);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (size_t cj=0; cj < cls.size(); cj++)
    {
      Collection l = cls[cj];

      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
      for (mrs_natural i=0; i < l.size(); ++i)
      {
        total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
        cout << "Processing" << l.entry(i) << endl;
        total->tick();
      }
    }
  }
  else
  {
    Collection l;
    l = cls[0];

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
      cout << "Processing" << l.entry(i) << endl;
      total->tick();
    }
  }
}

void
bextract_trainADRessStereoSPSMFCC(vector<Collection> cls, string classNames,
                                  string wekafname, mrs_natural memSize)
{
  (void) memSize;
  cout << "ADRess STEREO SPS+MFCC" << endl;
  cout << "classNames = "  << classNames << endl;
  cout << "wekafname = "  << wekafname << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* ffanout = mng.create("Fanout", "ffanout");

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("MFCC", "leftMFCC"));
  left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("MFCC", "rightMFCC"));
  right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  MarSystem* secondbranch = mng.create("Series", "secondbranch");
  MarSystem* stereobranches1 = mng.create("Parallel", "stereobranches1");
  MarSystem* left1 = mng.create("Series", "left1");
  MarSystem* right1 = mng.create("Series", "right1");

  left1->addMarSystem(mng.create("Windowing", "hamleft1"));
  left1->addMarSystem(mng.create("Spectrum", "spkleft1"));
  right1->addMarSystem(mng.create("Windowing", "hamright1"));
  right1->addMarSystem(mng.create("Spectrum", "spkright1"));

  stereobranches1->addMarSystem(left1);
  stereobranches1->addMarSystem(right1);

  secondbranch->addMarSystem(stereobranches1);
  secondbranch->addMarSystem(mng.create("ADRess", "aspk"));
  secondbranch->addMarSystem(mng.create("ADRessStereoSpectrum", "asspk"));
  secondbranch->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  secondbranch->addMarSystem(mng.create("TextureStats", "texturests"));

  playbacknet->addMarSystem(ffanout);
  ffanout->addMarSystem(stereobranches);
  ffanout->addMarSystem(secondbranch);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  total->updControl("mrs_natural/inSamples", 1024);

  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  if (!collection_has_labels)
  {
    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", classNames);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (size_t cj=0; cj < cls.size(); cj++)
    {
      Collection l = cls[cj];
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
      for (mrs_natural i=0; i < l.size(); ++i)
      {
        total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
        cout << "Processing" << l.entry(i) << endl;
        total->tick();
      }
    }
  }
  else
  {
    Collection l;
    l = cls[0];

    total->updControl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
      cout << "Processing" << l.entry(i) << endl;
      total->tick();
    }
  }
}

void bextract_trainAccumulator(vector<Collection> cls, mrs_natural label,
                               string pluginName, string classNames,
                               string wekafname,
                               mrs_natural memSize, string extractorStr,
                               bool withBeatFeatures)
{
  (void) label;
  (void) pluginName;
  if (withBeatFeatures)
    cout << "with beat features" << endl;


  mrs_bool collection_has_labels = false;

  if ((cls.size() == 1)&&(cls[0].hasLabels()))
  {
    collection_has_labels = true;
  }

  MRSDIAG("bextract.cpp - bextract_trainAccumulator");
  mrs_natural cj;

  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  if (normopt)
    cout << "NORMALIZE ENABLED" << endl;

  MarSystemManager mng;


  ////////////////////////////////////////////////
  //////////////////////////
  // create the file source
  //////////////////////////////////////////////////////////////////////////
  MarSystem* src = mng.create("SoundFileSource", "src");

  //////////////////////////////////////////////////////////////////////////
  // Feature Extractor
  //////////////////////////////////////////////////////////////////////////
  // create the correct feature extractor using the table of known
  // feature extractors:

  MarSystem* featExtractor = (*featExtractors[extractorStr])();
  featExtractor->updControl("mrs_natural/winSize", winSize);

  //////////////////////////////////////////////////////////////////////////
  // Build the overall feature calculation network
  //////////////////////////////////////////////////////////////////////////
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);

  // convert stereo files to mono
  featureNetwork->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  featureNetwork->addMarSystem(featExtractor);

  //////////////////////////////////////////////////////////////////////////
  // Texture Window Statistics (if any)
  //////////////////////////////////////////////////////////////////////////
  if(memSize != 0)
  {
    featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
    featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
  }

  //////////////////////////////////////////////////////////////////////////
  // update controls
  //////////////////////////////////////////////////////////////////////////
  featureNetwork->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  featureNetwork->updControl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

  //////////////////////////////////////////////////////////////////////////
  // accumulate feature vectors over 30 seconds
  //////////////////////////////////////////////////////////////////////////
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", accSize_);

  //////////////////////////////////////////////////////////////////////////
  // add network to accumulator
  //////////////////////////////////////////////////////////////////////////
  acc->addMarSystem(featureNetwork->clone());

  //////////////////////////////////////////////////////////////////////////
  // WEKA output
  //////////////////////////////////////////////////////////////////////////
  MarSystem* wsink = mng.create("WekaSink", "wsink");

  //////////////////////////////////////////////////////////////////////////
  // Annotator
  //////////////////////////////////////////////////////////////////////////
  MarSystem* annotator = mng.create("Annotator", "annotator");

  //////////////////////////////////////////////////////////////////////////
  // 30-second statistics
  //////////////////////////////////////////////////////////////////////////
  MarSystem* statistics = mng.create("Fanout", "statistics2");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));

  //////////////////////////////////////////////////////////////////////////
  // Final network compute 30-second statistics
  //////////////////////////////////////////////////////////////////////////
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->addMarSystem(statistics);

  // get parameters
  mrs_real srate = samplingRate_;

  total->updControl("mrs_natural/inSamples", winSize);
  total->updControl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/mrs_natural/pos", offset);

  // Calculate duration, offset parameters if necessary
  offset = (mrs_natural) (start * samplingRate_ );
  duration = (mrs_natural) (length * samplingRate_);

  //////////////////////////////////////////////////////////////////////////
  // main loop for extracting the features
  //////////////////////////////////////////////////////////////////////////
  //mrs_natural wc = 0;
  //mrs_natural samplesPlayed =0;
  string className = "";
  realvec beatfeatures;
  beatfeatures.create((mrs_natural)8,(mrs_natural)1);
  realvec estimate;
  estimate.create((mrs_natural)8,(mrs_natural)1);
  realvec in;
  realvec timbreres;
  realvec fullres;
  realvec afullres;

  in.create(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
            total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  timbreres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  if (withBeatFeatures)
  {
    if (extractorStr == "BEAT")
    {
      fullres.create(8, 1);
      afullres.create(8+1, 1);
      annotator->updControl("mrs_natural/inObservations", 8);
    }
    else
    {
      fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 8,
                     total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
      afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 8 + 1,
                      total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
      annotator->updControl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>()+8);
    }
  }
  else
  {
    fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

    afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 1,
                    total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    annotator->updControl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  }

  annotator->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  annotator->updControl("mrs_real/israte", total->getctrl("mrs_real/israte"));

  MarSystem* gcl = mng.create("SVMClassifier" ,"gcl");

  if (wekafname != EMPTYSTRING)
  {
    wsink->updControl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
    wsink->updControl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
    wsink->updControl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));
  }

  gcl->updControl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
  gcl->updControl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  gcl->updControl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));

  realvec gclres;
  gclres.create(gcl->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                gcl->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_natural timbreSize = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_natural beatSize = 8;

  MarSystem *total1 = NULL;

  if (withBeatFeatures)
  {
    if (extractorStr == "BEAT")
    {
      total1 = createBEATextrator();
      annotator->updControl("mrs_string/inObsNames", total1->getctrl("mrs_string/onObsNames")->to<mrs_string>());
    }
    else
    {
      total1 = createBEATextrator();
      annotator->updControl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames")->to<mrs_string>() + total1->getctrl("mrs_string/onObsNames")->to<mrs_string>());
    }
  }
  else
  {
    annotator->updControl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames"));
  }
  if (wekafname != EMPTYSTRING)
    wsink->updControl("mrs_string/inObsNames", annotator->getctrl("mrs_string/onObsNames"));

  realvec iwin;

  //iterate over collections
  Collection m,l;
  if (!collection_has_labels)
  {
    if (wekafname != EMPTYSTRING)
    {
      wsink->updControl("mrs_string/labelNames",classNames);
      wsink->updControl("mrs_natural/nLabels", (mrs_natural)cls.size());
      wsink->updControl("mrs_string/filename", wekafname);
    }

    for (cj=0; cj < (mrs_natural)cls.size(); cj++)
    {
      Collection l = cls[cj];
      if (wekafname != EMPTYSTRING)
      {
        wsink->updControl("mrs_string/filename", wekafname);
        cout << "Writing weka .arff file to :" << wekafname << endl;
      }
      for (mrs_natural i=0; i < l.size(); ++i)//iterate over collection files
      {
        // cout << beatfeatures << endl;
        total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
        if (withBeatFeatures)
        {
          srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
          iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!

          tempo_histoSumBands(total1, l.entry(i), beatfeatures,
                              iwin, estimate);
        }
        total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
        //wc = 0;
        //samplesPlayed = 0;
        // total->updControl("WekaSink/wsink/mrs_natural/label", cj);
        annotator->updControl("mrs_natural/label", cj);
        // wsink->updControl("mrs_natural/label", cj);
        if (extractorStr != "BEAT")
        {
          total->process(in, timbreres);
          for (int t=0; t < timbreSize; t++)
            fullres(t,0) = timbreres(t,0);
        }

        if (withBeatFeatures)
        {
          if (extractorStr == "BEAT")
          {
            for (int t=0; t < beatSize; t++)
              fullres(t, 0) = beatfeatures(t,0);
          }
          else
          {
            for (int t=0; t < beatSize; t++)
              fullres(t+timbreSize, 0) = beatfeatures(t,0);
          }
        }
        annotator->process(fullres, afullres);
        if (wekafname != EMPTYSTRING)
          wsink->process(afullres, afullres);
        cerr << "Processed " << l.entry(i) << endl;
      }
    }
  }
  else
  {
    l = cls[0];
    if (workspaceDir != EMPTYSTRING)
    {
      string outCollection = workspaceDir + "extract.txt";
      l.write(outCollection);
      cout << "Writing extract collection to :" << outCollection << endl;
    }
    if (wekafname != EMPTYSTRING)
    {
      wsink->updControl("mrs_string/labelNames",l.getLabelNames());
      wsink->updControl("mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
      wsink->updControl("mrs_string/filename", wekafname);
      cout << "Writing weka .arff file to :" << wekafname << endl;
    }

    // gcl->updControl("mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
    gcl->updControl("mrs_string/mode", "train");

    for (mrs_natural i=0; i < l.size(); ++i)//iterate over collection files
    {
      // cout << beatfeatures << endl;
      total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
      if (withBeatFeatures)
      {
        srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();

        iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!
        tempo_histoSumBands(total1, l.entry(i), beatfeatures,
                            iwin, estimate);
      }
      total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
      //wc = 0;
      //samplesPlayed = 0;
      annotator->updControl("mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));

      if (extractorStr != "BEAT")
      {
        total->process(in, timbreres);

        // concatenate timbre and beat vectors
        for (int t=0; t < timbreSize; t++)
          fullres(t,0) = timbreres(t,0);
      }
      if (withBeatFeatures)
      {
        if (extractorStr == "BEAT")
        {
          for (int t=0; t < beatSize; t++)
            fullres(t, 0) = beatfeatures(t,0);
        }
        else
        {
          for (int t=0; t < beatSize; t++)
            fullres(t+timbreSize, 0) = beatfeatures(t,0);
        }
      }
      annotator->process(fullres, afullres);
      if (wekafname != EMPTYSTRING)
        wsink->process(afullres, afullres);
      gcl->process(afullres, gclres);
      cerr << "Processed " << l.entry(i) << endl;
    }
  }

  if (testCollection != EMPTYSTRING)
  {
    m.read(testCollection);
    if (wekafname != EMPTYSTRING)
      wsink->updControl("mrs_string/filename", "predict.arff");
    gcl->updControl("mrs_string/mode", "predict");

    ofstream prout;
    prout.open(predictCollection.c_str());

    for (mrs_natural i=0; i < m.size(); ++i)//iterate over collection files
    {
      // cout << beatfeatures << endl;
      total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", m.entry(i));
      if (withBeatFeatures)
      {
        srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
        iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!
        tempo_histoSumBands(total1, m.entry(i), beatfeatures,
                            iwin, estimate);
      }
      total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
      //wc = 0;
      //samplesPlayed = 0;
      annotator->updControl("mrs_natural/label", 0);

      if (extractorStr != "BEAT")
      {
        total->process(in, timbreres);

        // concatenate timbre and beat vectors
        for (int t=0; t < timbreSize; t++)
          fullres(t,0) = timbreres(t,0);
      }
      if (withBeatFeatures)
      {
        if (extractorStr == "BEAT")
        {
          for (int t=0; t < beatSize; t++)
            fullres(t, 0) = beatfeatures(t,0);
        }
        else
        {
          for (int t=0; t < beatSize; t++)
            fullres(t+timbreSize, 0) = beatfeatures(t,0);
        }
      }
      annotator->process(fullres, afullres);
      if (wekafname != EMPTYSTRING)
        wsink->process(afullres, afullres);
      gcl->process(afullres, gclres);
      cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)gclres(0,0)) << endl;
      prout << m.entry(i) << "\t" << l.labelName((mrs_natural)gclres(0,0)) << endl;
    }
  }

  delete featureNetwork;
}


void
bextract_train(vector<Collection> cls, Collection cl,
               mrs_natural label,
               string pluginName, string classNames,
               string wekafname,  mrs_natural memSize,
               string extractorStr,
               string classifierName)
{
  (void) label;
  MRSDIAG("bextract.cpp - bextract_train");

  cout << "Old bextract_train" << endl;

  // hack for backward compatibility
  // this function is being depracated
  // use instead bextract_train_refactored
  if (length == -1.0f)
    length = 30.0f;

  MarSystemManager mng;
  vector<TimeLine> timeLines;
  if (classifierName == EMPTYSTRING)
    classifierName = DEFAULT_CLASSIFIER;

  if (extractorStr == EMPTYSTRING)
    extractorStr = DEFAULT_EXTRACTOR;

  //////////////////////////////////////////////////////////////////////////
  // Find proper sound file format and create SignalSource
  //////////////////////////////////////////////////////////////////////////
  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updControl("mrs_string/filename", sfName);
  // src->updControl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // src->updControl("mrs_natural/inSamples", 2048);

  // Calculate duration, offset parameters if necessary
  if (start > 0.0)
    offset = (mrs_natural) (start
                            * src->getctrl("mrs_real/israte")->to<mrs_real>()
                            * src->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

  duration = (mrs_natural) (length
                            * src->getctrl("mrs_real/israte")->to<mrs_real>()
                            * src->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

  cout << "duration = " << duration << endl;
  cout << "offset = " << offset << endl;

  //////////////////////////////////////////////////////////////////////////
  // Feature Extractor
  //////////////////////////////////////////////////////////////////////////
  // create the correct feature extractor using the table of known
  // feature extractors:
  MarSystem* featExtractor = (*featExtractors[extractorStr])();

  featExtractor->updControl("mrs_natural/winSize", winSize);

  //////////////////////////////////////////////////////////////////////////
  // Build the overall feature calculation network
  //////////////////////////////////////////////////////////////////////////
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);

  if (pluginName != EMPTYSTRING)
  {
    // create audio sink and mute it
    // it is stored in the output plugin
    // which can be used for real-time classification
    MarSystem* dest = mng.create("AudioSink", "dest");
    dest->updControl("mrs_bool/mute", true);
    featureNetwork->addMarSystem(dest);
  }

  //add the feature extraction network
  featureNetwork->addMarSystem(featExtractor);//mng.create("FeatureExtractor", "featExtractor"));

  //////////////////////////////////////////////////////////////////////////
  //texture window statistics (optional)
  //////////////////////////////////////////////////////////////////////////
  if(memSize != 0)
  {
    featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
    featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
  }

  //////////////////////////////////////////////////////////////////////////
  // update controls I
  //////////////////////////////////////////////////////////////////////////
  // src has to be configured with hopSize frame length in case a ShiftInput
  // is used in the feature extraction network
  featureNetwork->updControl("mrs_natural/inSamples", hopSize);
  featureNetwork->updControl(src->getType() + "/src/mrs_natural/pos", offset);

  //////////////////////////////////////////////////////////////////////////
  // add the Annotator
  //////////////////////////////////////////////////////////////////////////
  featureNetwork->addMarSystem(mng.create("Annotator", "annotator"));

  //////////////////////////////////////////////////////////////////////////
  // add WEKA sink
  //////////////////////////////////////////////////////////////////////////
  if (wekafname != EMPTYSTRING)
    featureNetwork->addMarSystem(mng.create("WekaSink", "wsink"));

  //////////////////////////////////////////////////////////////////////////
  // add classifier and confidence majority calculation
  //////////////////////////////////////////////////////////////////////////
  cout << "classifierName = " << classifierName << endl;
  if (classifierName == "GS")
    featureNetwork->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
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

  // FileName Sfname(sfName);
  // Sfname.path()+
  //   featureNetwork->updControl("Confidence/confidence/mrs_string/fileName", Sfname.nameNoExt());

  //////////////////////////////////////////////////////////////////////////
  // link controls
  //////////////////////////////////////////////////////////////////////////
  featureNetwork->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
  featureNetwork->linkControl("SoundFileSource/src/mrs_string/currentlyPlaying", "Confidence/confidence/mrs_string/fileName");
  featureNetwork->linkControl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
  featureNetwork->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  featureNetwork->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  featureNetwork->linkControl("mrs_bool/initAudio", "AudioSink/dest/mrs_bool/initAudio");

  MarControlPtr ctrl_filename_ = featureNetwork->getctrl("SoundFileSource/src/mrs_string/filename");
  MarControlPtr ctrl_hasData_ = featureNetwork->getctrl("SoundFileSource/src/mrs_bool/hasData");

  //////////////////////////////////////////////////////////////////////////
  // main loop for extracting features
  //////////////////////////////////////////////////////////////////////////
  //***********************************
  // if no timelines are being used...
  //***********************************
  if(!tline)
  {
    mrs_natural wc = 0;
    mrs_natural samplesPlayed =0;
    mrs_natural onSamples = featureNetwork->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

    if (classifierName == "GS")
      featureNetwork->updControl("GaussianClassifier/gaussian/mrs_natural/nClasses", (mrs_natural)cls.size());
    else if (classifierName == "ZeroR")
      featureNetwork->updControl("ZeroRClassifier/zeror/mrs_natural/nClasses", (mrs_natural)cls.size());
    else if (classifierName == "KNN")
      featureNetwork->updControl("KNNClassifier/knn/mrs_natural/nLabels", (mrs_natural)cls.size());

    //configure Confidence
    featureNetwork->updControl("Confidence/confidence/mrs_natural/nLabels", (int)cls.size());
    featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
    featureNetwork->updControl("Confidence/confidence/mrs_string/labelNames",classNames);
    featureNetwork->updControl("Confidence/confidence/mrs_bool/print",true);

    Collection l = cl;
    mrs_natural nLabels = (mrs_natural) l.getNumLabels();

    if (wekafname != EMPTYSTRING)
    {
      featureNetwork->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
      featureNetwork->updControl("WekaSink/wsink/mrs_natural/nLabels", nLabels);
      featureNetwork->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
      featureNetwork->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
    }

    if (classifierName == "GS")
      featureNetwork->updControl("GaussianClassifier/gaussian/mrs_natural/nClasses", nLabels);
    else if (classifierName == "ZeroR")
      featureNetwork->updControl("ZeroRClassifier/zeror/mrs_natural/nClasses", nLabels);
    else if (classifierName == "KNN")
      featureNetwork->updControl("KNNClassifier/knn/mrs_natural/nLabels", nLabels);

    //configure Confidence
    featureNetwork->updControl("Confidence/confidence/mrs_natural/nLabels", nLabels);
    featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
    featureNetwork->updControl("Confidence/confidence/mrs_string/labelNames", l.getLabelNames());
    featureNetwork->updControl("Confidence/confidence/mrs_bool/print",true);

    //iterate over audio files, extract features and label
    for (mrs_natural i=0; i < l.size(); ++i)
    {
      //reset texture analysis stats between files
      // if(memSize != 0)
      // featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset", true);

      featureNetwork->updControl("Annotator/annotator/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));

      featureNetwork->updControl(ctrl_filename_, l.entry(i));
      wc = 0;
      samplesPlayed = 0;
      while (ctrl_hasData_->to<mrs_bool>() && (duration > samplesPlayed))
      {
        featureNetwork->tick();
        wc++;
        samplesPlayed = wc * onSamples;
      }
      featureNetwork->tick();
      cout << "Processed " << l.entry(i) << endl;
    }
  }

  //**********************
  // if using timelines
  //**********************
  else
  {
    TimeLine tline;
    mrs_natural numClasses;
    //1st collection is the audio file collection
    Collection audioColl = cls[0];
    //2nd collection  is a collection of timeline files
    //(should be in the same order as the audio files)
    Collection tlColl = cls[1];

    // for each audiofile/timeline pair in collection
    // (and ignore all audio files that have no timeline file)
    // run feature extraction and train classifiers
    for(mrs_natural i=0; i <(mrs_natural)tlColl.size(); ++i)
    {
      //load Audio file from the collection
      featureNetwork->updControl("SoundFileSource/src/mrs_string/filename", audioColl.entry(i));

      //load timeline for i-th audio file
      tline.load(tlColl.entry(i),"");

      //get number of classes in the timeline
      numClasses = (mrs_natural)tline.numClasses();

      // create a label for each class
      classNames = "";
      ostringstream sstr;
      for(mrs_natural c=0; c < numClasses; ++c)
        sstr << "class_" << c << ",";
      classNames = sstr.str();

      //configure classifiers
      if (classifierName == "GS")
        featureNetwork->updControl("GaussianClassifier/gaussian/mrs_natural/nLabels", numClasses);
      else if (classifierName == "ZeroR")
        featureNetwork->updControl("ZeroRClassifier/zeror/mrs_natural/nLabels", numClasses);
      else if (classifierName == "KNN")
        featureNetwork->updControl("KNNClassifier/knn/mrs_natural/nLabels", numClasses);

      //configure Confidence
      featureNetwork->updControl("Confidence/confidence/mrs_natural/nLabels", numClasses);
      featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
      featureNetwork->updControl("Confidence/confidence/mrs_string/labelNames",classNames);
      featureNetwork->updControl("Confidence/confidence/mrs_bool/print",true);

      //configure WEKA sink
      if (wekafname != EMPTYSTRING)
      {
        featureNetwork->updControl("WekaSink/wsink/mrs_natural/nLabels", numClasses);
        featureNetwork->updControl("WekaSink/wsink/mrs_string/labelNames",classNames);
        featureNetwork->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
        string name = audioColl.entry(i).substr(0, audioColl.entry(i).length()-4) + "_" + extractorStr + ".arff";
        featureNetwork->updControl("WekaSink/wsink/mrs_string/filename", name);
      }

      cout << endl;
      cout << "**************************************" << endl;
      cout << "AudioFile: " << audioColl.entry(i) << endl;
      cout << "TimeLine : " << tlColl.entry(i) << endl;
      cout << "**************************************" << endl;

      //iterate over timeline regions
      for (mrs_natural r = 0; r < tline.numRegions(); ++r)
      {
        cout << "-----------------------------------------------" << endl;
        cout << "Region " << r+1 << "/" << tline.numRegions() << endl;
        cout << "Region start   = " << tline.regionStart(r) << endl;
        cout << "Region classID = " << tline.regionClass(r) << endl;
        cout << "Region end     = " << tline.regionEnd(r) << endl;

        // set current region class in Annotator
        featureNetwork->updControl("Annotator/annotator/mrs_natural/label", tline.regionClass(r));

        // set current region class in WEKA sink
        if (wekafname != EMPTYSTRING)
        {
          featureNetwork->updControl("WekaSink/wsink/mrs_natural/label", tline.regionClass(r)); //[?]
        }

        // reset texture analysis stats between regions
        if(memSize != 0)
          featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset", true);

        //define audio region boundaries
        mrs_natural start = (mrs_natural)(tline.regionStart(r) * tline.lineSize()); //region start sample
        mrs_natural end = (mrs_natural)(tline.regionEnd(r) * tline.lineSize()); //region end sample

        mrs_natural fileSize = featureNetwork->getctrl(src->getType() + "/src/mrs_natural/size")->to<mrs_natural>();
        if(end > fileSize)
        {
          end = fileSize;
          cout << "WARNING:" << endl;
          cout << "Region end sample behind EOF!" << endl;
          cout << "Setting region end sample to EOF." << endl;
        }

        featureNetwork->updControl(src->getType() + "/src/mrs_natural/pos", start);
        //featureNetwork->updControl("mrs_natural/inSamples", hopSize); //[?]
        //featureNetwork->updControl("mrs_natural/inSamples", tline.lineSize_);//[?]

        //iterate over audio region
        //(using the winSize and hopSize defined by the user)
        mrs_natural numWindows = 0;
        while(featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>() + winSize <= end)
        {
          //cout << "pos = " << featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>() << endl;
          featureNetwork->tick();
          numWindows++;
          cout << '.';
        }
        cout << endl;

        mrs_natural lastpos = featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>();
        if((end - lastpos) > 0)
        {
          cout << "Dropped last frame: " << endl;
          cout << "Dropped " << end - lastpos << " samples from current segment." << endl;
          cout << "Dropped " <<(lastpos + winSize) - end << " samples from next segment." << endl;
        }

        cout << "Processed " << numWindows << " frames!"  << endl;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // prepare network for classification
  //////////////////////////////////////////////////////////////////////////
  if (classifierName == "GS")
  {
    featureNetwork->updControl("GaussianClassifier/gaussian/mrs_string/mode","predict");
  }
  else if (classifierName == "ZeroR")
  {
    featureNetwork->updControl("ZeroRClassifier/zeror/mrs_string/mode","predict") ;
  }
  else if (classifierName == "KNN")
  {
    featureNetwork->updControl("KNNClassifier/knn/mrs_string/mode","predict");
    featureNetwork->updControl("KNNClassifier/knn/mrs_natural/k",3); //[!] hardcoded!!!
  }

  featureNetwork->tick();

  if (pluginName != EMPTYSTRING && !pluginMute)
  {
    featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/initAudio", true);//[!][?] this still does not solves the problem of sfplugin being unable to play audio...
  }

  if (wekafname != EMPTYSTRING)
    featureNetwork->updControl("WekaSink/wsink/mrs_bool/mute", true);

  featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", false);

  //////////////////////////////////////////////////////////////////////////
  // output trained classifier models
  //////////////////////////////////////////////////////////////////////////
  if (pluginName == EMPTYSTRING) // output to stdout
    cout << (*featureNetwork) << endl;
  else // save to .mpl file
  {
    ofstream oss(pluginName.c_str());
    oss << (*featureNetwork) << endl;
  }

  delete featureNetwork;
}

void
selectClassifier(MarSystem *msys,string classifierName )
{
  cout << "classifierName = " << classifierName << endl;

  if (classifierName == "GS")
    msys->updControl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  if (classifierName == "ZEROR")
  {
    cout << "Selecting ZEROR" << endl;
    msys->updControl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
    cout << *msys << endl;
  }
  if (classifierName == "SVM")
    msys->updControl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");
}

void
selectFeatureSet(MarSystem *featExtractor)
{
  if (chroma_)
    featExtractor->updControl("mrs_string/enableSPChild", "Series/chromaPrSeries");
  if (mfcc_)
    featExtractor->updControl("mrs_string/enableSPChild", "MFCC/mfcc");
  if (sfm_)
    featExtractor->updControl("mrs_string/enableSPChild", "SFM/sfm");
  if (scf_)
    featExtractor->updControl("mrs_string/enableSPChild", "SCF/scf");
  if (rlf_)
    featExtractor->updControl("mrs_string/enableSPChild", "Rolloff/rlf");
  if (flx_)
    featExtractor->updControl("mrs_string/enableSPChild", "Flux/flux");
  if (lsp_)
    featExtractor->updControl("mrs_string/enableLPCChild", "Series/lspbranch");
  if (lpcc_)
    featExtractor->updControl("mrs_string/enableLPCChild", "Series/lpccbranch");
  if (ctd_)
    featExtractor->updControl("mrs_string/enableSPChild", "Centroid/cntrd");
  if (zcrs_)
    featExtractor->updControl("mrs_string/enableTDChild", "ZeroCrossings/zcrs");
  if (spectralFeatures_)
  {
    featExtractor->updControl("mrs_string/enableSPChild", "Centroid/cntrd");
    featExtractor->updControl("mrs_string/enableSPChild", "Flux/flux");
    featExtractor->updControl("mrs_string/enableSPChild", "Rolloff/rlf");
  }
  if (timbralFeatures_)
  {
    featExtractor->updControl("mrs_string/enableTDChild", "ZeroCrossings/zcrs");

    featExtractor->updControl("mrs_string/enableSPChild", "MFCC/mfcc");
    featExtractor->updControl("mrs_string/enableSPChild", "Centroid/cntrd");
    featExtractor->updControl("mrs_string/enableSPChild", "Flux/flux");
    featExtractor->updControl("mrs_string/enableSPChild", "Rolloff/rlf");

    featExtractor->updControl("mrs_string/enableSPChild", "Series/chromaPrSeries");
  }
}


void
bextract_train_refactored(string pluginName,  string wekafname,
                          mrs_natural memSize, string classifierName,
                          mrs_bool single_vector)
{
  MRSDIAG("bextract.cpp - bextract_train_refactored");
  MarSystemManager mng;

  cout << "wekafname = " << wekafname << endl;


  // Overall extraction and classification network
  MarSystem* bextractNetwork = mng.create("Series", "bextractNetwork");

  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

  // Add a fanout for sound file and audio source ...
  MarSystem *fanout = mng.create("Fanout", "fanout");

  // Add a sound file source (which can also read collections)
  MarSystem *src = mng.create("SoundFileSource", "src");
  fanout->addMarSystem(src);

  // Add a live audio source for realtime classification
#ifdef MARSYAS_AUDIOIO
  MarSystem *mic = mng.create("AudioSource", "mic");
  mic->updControl("mrs_natural/nChannels", 1);  //stereo
  fanout->addMarSystem(mic);
#endif

  // Add the fanout to our feature Network ...
  featureNetwork->addMarSystem(fanout);

  featureNetwork->updControl("mrs_real/israte", 44100.0);   //sampling rate  [!hardcoded]
  bextractNetwork->updControl("mrs_real/israte", 44100.0);   //sampling rate  [!hardcoded]

  // Disable Microphone for training the classifier ...
  featureNetwork->updControl("Fanout/fanout/mrs_natural/disable", 1);
  //featureNetwork->updControl("Fanout/fanout/AudioSource/mic/mrs_bool/initAudio", false);

  // Add a TimelineLabeler, if necessary
  if(tline)
  {
    featureNetwork->addMarSystem(mng.create("TimelineLabeler", "timelineLabeler"));
  }

  // create audio sink and mute it it is stored in the output plugin
  // that can be used for real-time classification
  if ((pluginName != EMPTYSTRING) && (!pluginMute))
  {
    MarSystem* dest = mng.create("AudioSink", "dest");
    dest->updControl("mrs_bool/mute", false);
    featureNetwork->addMarSystem(dest);
  }



  // Select whether stereo or mono feature extraction is to be used
  if (stereo_ == true)
  {
    MarSystem* stereoFeatures = mng.create("StereoFeatures", "stereoFeatures");
    selectFeatureSet(stereoFeatures);
    featureNetwork->addMarSystem(stereoFeatures);
  }
  else
  {
    featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));
    MarSystem* featExtractor = mng.create("TimbreFeatures", "featExtractor");
    selectFeatureSet(featExtractor);
    featureNetwork->addMarSystem(featExtractor);
  }

  // texture statistics - disable if memSize is 0
  if (memSize != 0)
  {
    featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
    featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
    featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset", true);
  }

  // Use accumulator if computing single vector / file
  if (single_vector)
  {
    MarSystem* acc = mng.create("Accumulator", "acc");
    acc->updControl("mrs_natural/maxTimes", accSize_);
    acc->updControl("mrs_string/mode", "explicitFlush");
    acc->updControl("mrs_natural/timesToKeep", 1);
    acc->addMarSystem(featureNetwork);
    bextractNetwork->addMarSystem(acc);
    MarSystem* song_statistics = mng.create("Fanout", "song_statistics");
    song_statistics->addMarSystem(mng.create("Mean", "mn"));
    song_statistics->addMarSystem(mng.create("StandardDeviation", "std"));
    bextractNetwork->addMarSystem(song_statistics);

    bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
                                 "mrs_string/filename"); // added Fanout ...
    bextractNetwork->linkControl("mrs_bool/hasData",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/hasData"); // added Fanout ...
    bextractNetwork->linkControl("mrs_natural/pos",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ...
    bextractNetwork->linkControl("mrs_real/duration",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration"); // added Fanout ...
    if ((pluginName != EMPTYSTRING) && (!pluginMute)) {
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
                                   "mrs_bool/initAudio");
    }
    bextractNetwork->linkControl("mrs_string/previouslyPlaying",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/previouslyPlaying"); // added Fanout ...


    bextractNetwork->linkControl("mrs_string/currentlyPlaying",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying"); // added Fanout ...

    bextractNetwork->linkControl("mrs_bool/currentCollectionNewFile",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");

    bextractNetwork->linkControl("Accumulator/acc/mrs_bool/flush",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");

    bextractNetwork->linkControl(
      "Accumulator/acc/Series/featureNetwork/TimbreFeatures/featExtractor/Series/spectralShape/STFT_features/spectrumFeatures/Flux/flux/mrs_bool/reset",
      "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");

    if (memSize != 0) {
      bextractNetwork->linkControl(
        "Accumulator/acc/Series/featureNetwork/TextureStats/tStats/mrs_bool/reset",
        "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile"
      );
    }

    if(tline)
    {
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabelFile",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                   "Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance"); // added Fanout ...
    }
    else
    {
      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/previousLabel");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
    }

    bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                 "mrs_natural/advance");
  }
  else // running feature extraction
  {
    bextractNetwork->addMarSystem(featureNetwork);
    // link controls to top-level to make life simpler
    bextractNetwork->linkControl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
                                 "mrs_string/filename");
    bextractNetwork->linkControl("mrs_bool/hasData",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/hasData");
    bextractNetwork->linkControl("mrs_natural/pos",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos");
    bextractNetwork->linkControl("mrs_real/duration",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration");
    if ((pluginName != EMPTYSTRING) && (!pluginMute)) {
      bextractNetwork->linkControl("Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
                                   "mrs_bool/initAudio");
    }
    bextractNetwork->linkControl("mrs_string/currentlyPlaying",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying");

    bextractNetwork->linkControl("mrs_bool/currentCollectionNewFile",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");

    bextractNetwork->linkControl(
      "Series/featureNetwork/TimbreFeatures/featExtractor/Series/spectralShape/STFT_features/spectrumFeatures/Flux/flux/mrs_bool/reset",
      "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");


    if (memSize != 0) {
      bextractNetwork->linkControl(
        "Series/featureNetwork/TextureStats/tStats/mrs_bool/reset",
        "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/currentCollectionNewFile");
    }


    if(tline)
    {
      if (lexiconopt)
      {
        // For now hardwire for chord extraction. The lexiconLabelNames must be
        // sorted alphabetically.

        bextractNetwork->updControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_bool/useLexicon", true);
        bextractNetwork->updControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/lexiconNLabels", 25);
        bextractNetwork->updControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/lexiconLabelNames", "A:min,A:maj,Bb:min,Bb:maj,C:min,C:maj,C#:min,C#:maj,Db:min,Db:maj,E:min,E:maj,Eb:min,Eb:maj,F:min,F:maj,F#:min,F#:maj,Gb:min,Gb:maj,G:min,G:maj,G#:min,G#:maj,N");
      }


      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabelFile",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel");
      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos");
      bextractNetwork->linkControl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance");

      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelNames");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/nLabels");




    }
    else
    {
      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
    }
  }




  mrs_natural beatFeaturesSize = 0;
  MarSystem * beatTracker = NULL;


  if (single_vector && beat_)
  {
    beatTracker = createBeatHistogramFeatureNetwork();

    bextractNetwork->addMarSystem(mng.create("Inject/inject"));
    beatFeaturesSize = beatTracker->getctrl("FlowThru/tempoInduction/BeatHistoFeatures/bhf/mrs_natural/onObservations")->to<mrs_natural>();
    bextractNetwork->updControl("Inject/inject/mrs_natural/injectSize", beatFeaturesSize);

    bextractNetwork->updControl("Inject/inject/mrs_string/injectNames", beatTracker->getctrl("FlowThru/tempoInduction/BeatHistoFeatures/bhf/mrs_string/onObsNames")->to<mrs_string>());

  }
  realvec beatfeatures(beatFeaturesSize);


  // labeling, weka output, classifier and confidence for real-time output
  bextractNetwork->addMarSystem(mng.create("Annotator", "annotator"));
  if (wekafname != EMPTYSTRING)
    bextractNetwork->addMarSystem(mng.create("WekaSink", "wsink"));
  if (only_stable_) {
    bextractNetwork->updControl("WekaSink/wsink/mrs_bool/onlyStable", true);
    if (single_vector) {
      bextractNetwork->linkControl(
        "WekaSink/wsink/mrs_bool/resetStable",
        "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/startStable");
    } else {
      bextractNetwork->linkControl(
        "WekaSink/wsink/mrs_bool/resetStable",
        "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/startStable");
    }
  }
  if (regression_) {
    bextractNetwork->updControl("WekaSink/wsink/mrs_bool/regression", true);
    if (single_vector) {
      bextractNetwork->updControl(
        "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/regression",
        true);
    } else {
      bextractNetwork->updControl(
        "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/regression",
        true);
    }
  }


  if (!featExtract_)
  {
    bextractNetwork->addMarSystem(mng.create("Classifier", "cl"));
    bextractNetwork->addMarSystem(mng.create("Confidence", "confidence"));
    bextractNetwork->updControl("Confidence/confidence/mrs_natural/memSize", cmopt);

    if (csvOutput) {
      bextractNetwork->updControl("Confidence/confidence/mrs_bool/csvOutput", csvOutput);
    }

    // link confidence and annotation with SoundFileSource that plays the collection
    bextractNetwork->linkControl("Confidence/confidence/mrs_string/fileName",
                                 "mrs_string/filename");
  }

  bextractNetwork->linkControl("Annotator/annotator/mrs_real/label",
                               "mrs_real/currentLabel");

  // links with WekaSink
  if (wekafname != EMPTYSTRING)
  {
    if (single_vector)
      bextractNetwork->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
                                   "mrs_string/previouslyPlaying");
    else
      bextractNetwork->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
                                   "mrs_string/currentlyPlaying");

    bextractNetwork->linkControl("WekaSink/wsink/mrs_string/labelNames",
                                 "mrs_string/labelNames");
    bextractNetwork->linkControl("WekaSink/wsink/mrs_natural/nLabels", "mrs_natural/nLabels");
  }

  // src has to be configured with hopSize frame length in case a ShiftInput
  // is used in the feature extraction network
  bextractNetwork->updControl("mrs_natural/inSamples", hopSize);
  if (stereo_)
    featureNetwork->updControl("StereoFeatures/stereoFeatures/mrs_natural/winSize",
                               winSize);
  else
    featureNetwork->updControl("TimbreFeatures/featExtractor/mrs_natural/winSize",
                               winSize);

  if (start > 0.0)
    offset = (mrs_natural) (start * src->getctrl("mrs_real/israte")->to<mrs_real>());

  bextractNetwork->updControl("mrs_natural/pos", offset);
  bextractNetwork->updControl("mrs_real/duration", length);

  // confidence is silent during training
  if (!featExtract_)
  {
    bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
    bextractNetwork->updControl("Confidence/confidence/mrs_bool/print",true);
    if (single_vector)
      bextractNetwork->updControl("Confidence/confidence/mrs_natural/memSize", 1);

    // select classifier to be used
    selectClassifier(bextractNetwork, classifierName);
  }



  // load the collection which is automatically created by bextract
  // based on the command-line arguments

  if (workspaceDir != EMPTYSTRING)
    bextractNetwork->updControl("mrs_string/filename", workspaceDir + "bextract_single.mf");
  else
    bextractNetwork->updControl("mrs_string/filename", "bextract_single.mf");

  // play sound if playback is enabled
  if (pluginName != EMPTYSTRING && playback)
  {
    featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  }

  // don't use linkControl so that only value is copied once and linking doesn't
  // remain for the plugin
  if (!featExtract_)
  {
    bextractNetwork->updControl("Confidence/confidence/mrs_string/labelNames",
                                bextractNetwork->getctrl("mrs_string/labelNames"));
    bextractNetwork->updControl("Classifier/cl/mrs_natural/nClasses",
                                bextractNetwork->getctrl("mrs_natural/nLabels"));
    bextractNetwork->updControl("Confidence/confidence/mrs_natural/nLabels",
                                bextractNetwork->getctrl("mrs_natural/nLabels"));
  }


  // setup WekaSink - has to be done after all updates so that changes are correctly
  // written to file
  if (wekafname != EMPTYSTRING)
  {
    bextractNetwork->updControl("WekaSink/wsink/mrs_natural/downsample", downSample);

    cout << "Downsampling factor = " << downSample << endl;

    bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
  }






  // main processing loop for training
  MarControlPtr ctrl_hasData = bextractNetwork->getctrl("mrs_bool/hasData");
  MarControlPtr ctrl_currentlyPlaying = bextractNetwork->getctrl("mrs_string/currentlyPlaying");
  MarControlPtr ctrl_previouslyPlaying = bextractNetwork->getctrl("mrs_string/previouslyPlaying");
  MarControlPtr ctrl_currentCollectionNewFile = bextractNetwork->getctrl("mrs_bool/currentCollectionNewFile");

  mrs_string previouslyPlaying, currentlyPlaying;


  int n = 0;
  int advance = 0;

  vector<string> processedFiles;
  map<string, realvec> processedFeatures;

  bool seen;
  realvec fvec;
  int label;



  while (ctrl_hasData->to<mrs_bool>())
  {
    if (single_vector)
    {
      currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
      previouslyPlaying = ctrl_previouslyPlaying->to<mrs_string>();


      // round value
      label = static_cast<int>(bextractNetwork->getctrl("mrs_real/currentLabel")->to<mrs_real>() + 0.5);

      seen = false;

      for (size_t j=0; j<processedFiles.size(); j++)
      {

        if (processedFiles[j] == currentlyPlaying)
          seen = true;
      }

      // seen = false;

      if (seen)
      {

        advance ++;
        if (wekafname != EMPTYSTRING)
          bextractNetwork->updControl("WekaSink/wsink/mrs_string/injectComment", "% filename " + currentlyPlaying);
        bextractNetwork->updControl("mrs_natural/advance", advance);
        label = static_cast<int>(bextractNetwork->getctrl("mrs_real/currentLabel")->to<mrs_real>() + 0.5);


        fvec = processedFeatures[currentlyPlaying];
        fvec(fvec.getSize()-1) = label;

        if (wekafname != EMPTYSTRING)
        {
          bextractNetwork->updControl("WekaSink/wsink/mrs_realvec/injectVector", fvec);
          bextractNetwork->updControl("WekaSink/wsink/mrs_bool/inject", true);
        }

      }
      else
      {
        bextractNetwork->updControl("mrs_natural/advance", advance);
        if (beat_)
        {
          beatHistogramFeatures(beatTracker, currentlyPlaying, beatfeatures);
          bextractNetwork->updControl("Inject/inject/mrs_realvec/inject", beatfeatures);
        }

        currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
        bextractNetwork->tick();
        fvec = bextractNetwork->getctrl("Annotator/annotator/mrs_realvec/processedData")->to<mrs_realvec>();

        processedFiles.push_back(currentlyPlaying);
        processedFeatures[currentlyPlaying] = fvec;
        cout<< "Processed: " << n << " - " << currentlyPlaying << endl;
        advance = 0;
      }
      n++;

    }
    else // running feature extraction
    {
      bextractNetwork->tick();
      currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
      if (ctrl_currentCollectionNewFile->to<mrs_bool>())
      {
        // if (memSize != 0)
        // featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset",  true);
        cout << "Processing: " << n << " - " << currentlyPlaying << endl;
        n++;
      }

    }



  }


  if (tline) {
    // prepare for generic use
    bextractNetwork->updControl(
      "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles", EMPTYSTRING, NOUPDATE);
  }


  cout << "Finished feature extraction" << endl;
  if (featExtract_) {
    if (pluginName != EMPTYSTRING) {
      ofstream oss(pluginName.c_str());
      oss << (*bextractNetwork) << endl;
    }
    delete bextractNetwork;
    return;
  }


  // prepare network for real-time playback/prediction
  bextractNetwork->updControl("Classifier/cl/mrs_string/mode","predict");

  cout << "Finished classifier training" << endl;
  if (mic_)
    cout << "Microphone input used" << endl;
  // have the plugin play audio
  if (pluginName != EMPTYSTRING && !pluginMute)
  {
    featureNetwork->updControl("mrs_real/israte", 44100.0);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/initAudio", true);

    // mute Audio since we are listening with mic at runtime ...
    if (mic_)
      featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", true);
    else
      featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
  }

  /*
    if (pluginName != EMPTYSTRING && pluginMute)
        {
                cout << "PLUGINMUTE" << endl;
                featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", true);
                featureNetwork->updControl("AudioSink/dest/mrs_bool/initAudio", false);
        }
  */

  // init mic audio ...
  if (mic_)
  {
    bextractNetwork->updControl("mrs_real/israte", 44100.0);   //sampling rate
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_natural/nChannels", 1);        //stereo
    bextractNetwork->linkControl( "mrs_bool/initAudio" , "Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_bool/initAudio" ); //important link!!!
  }


  // finally disable the Soundfile Input in Fanout ...
  if (mic_)
  {
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/mrs_natural/disable", 0);
    // ... and enable live audio source ...
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/mrs_natural/enable", 1);
  }


  // don't output to WekaSink
  if (wekafname != EMPTYSTRING)
    bextractNetwork->updControl("WekaSink/wsink/mrs_bool/mute", false);

  // enable confidence
  bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", false);

  // output trained classifier models
  if (pluginName == EMPTYSTRING) // output to stdout
    ;

  // cout << (*bextractNetwork) << endl;
  else // save to .mpl file
  {
    ofstream oss(pluginName.c_str());
    oss << (*bextractNetwork) << endl;
  }

  // predict optional test collection
  if (testCollection != EMPTYSTRING)
  {
    bextractNetwork->updControl("mrs_natural/advance", 0);
    if (single_vector)
    {

      if (pluginName != EMPTYSTRING && !pluginMute)
        featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", true);

      Collection m;
      m.read(testCollection);

      Collection l;
      if (workspaceDir != EMPTYSTRING)
        l.read(workspaceDir + "bextract_single.mf");
      else
        l.read("bextract_single.mf");
      bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);



      if (wekafname != EMPTYSTRING)
      {
        bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", workspaceDir + "predict.arff");
      }

      bextractNetwork->updControl("Classifier/cl/mrs_string/mode", "predict");

      ofstream prout;
      prout.open(predictCollection.c_str());

      int correct_instances = 0;
      int num_instances = 0;

      bextractNetwork->updControl("mrs_string/filename", testCollection);
      bextractNetwork->updControl("mrs_string/labelNames", l.getLabelNames());

      ofstream ofs;
      string pluginname;
      pluginname = workspaceDir + "bextract.mpl";
      ofs.open(pluginname.c_str());
      ofs << *bextractNetwork << endl;
      ofs.close();



      while (ctrl_hasData->to<mrs_bool>())
      {
        if (beat_)
        {
          beatHistogramFeatures(beatTracker, currentlyPlaying, beatfeatures);
          bextractNetwork->updControl("Inject/inject/mrs_realvec/inject", beatfeatures);
        }

        currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
        bextractNetwork->tick();
        if (memSize != 0)
          featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset",
                                     true);


        mrs_realvec pr = bextractNetwork->getctrl("Classifier/cl/mrs_realvec/processedData")->to<mrs_realvec>();
        cout << "Predicting " << currentlyPlaying << "\t" << "as \t" << l.labelName((mrs_natural)pr(0,0)) << endl;

        // if (single_vector)
        // {
        // bextractNetwork->updControl("mrs_natural/advance", 1);
        // }


        if ((mrs_natural)pr(0,0) == (mrs_natural)(pr(1,0)))
          correct_instances++;
        num_instances++;

        prout << currentlyPlaying << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
      }
      cout << "Correct instances = " << correct_instances << "/" << num_instances << endl;
    }

    else
    {

      cout << "bextract_train_refactored: predicting test collection: " << testCollection << endl;
      bextractNetwork->updControl("mrs_string/filename", testCollection);

      while (ctrl_hasData->to<mrs_bool>())
      {
        currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
        if (currentlyPlaying != previouslyPlaying)
          cout << "Processing : " << currentlyPlaying << endl;

        bextractNetwork->tick();
        previouslyPlaying = currentlyPlaying;
      }
    }
  }



  delete bextractNetwork;
  return;
}



// train with multiple feature vectors/file
void bextract_train_rmsilence(vector<Collection> cls, mrs_natural label,
                              string pluginName, string classNames,
                              string wekafname,  mrs_natural memSize,
                              string extractorStr,
                              string classifierName)
{
  (void) label;
  if (classifierName == EMPTYSTRING)
    classifierName = "SMO";

  MRSDIAG("bextract.cpp - bextract_train");
  mrs_natural cj;

  Collection linitial = cls[0];
  string sfName = linitial.entry(0);

  // default
  if (extractorStr == EMPTYSTRING)
    extractorStr = "STFT";

  MarSystemManager mng;

  // Find proper soundfile format and create SignalSource
  MarSystem *srm = mng.create("SilenceRemove", "srm");
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updControl("mrs_string/filename", sfName);

  srm->addMarSystem(src);

  // src->updControl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // srm->updControl("mrs_natural/inSamples", 2048);
  // srm->updControl("mrs_natural/inSamples", 2048);

  // Calculate duration, offest parameters if necessary
  if (start > 0.0f)
    offset = (mrs_natural) (start
                            * src->getctrl("mrs_real/israte")->to<mrs_real>());

  if (length != 30.0f)
    duration = (mrs_natural) (length
                              * src->getctrl("mrs_real/israte")->to<mrs_real>());

  // create audio sink and mute it
  // it is stored in the output plugin
  // which can be used for real-time classification
  MarSystem* dest=NULL;

  if (pluginName != EMPTYSTRING) // output to stdout
  {
    dest = mng.create("AudioSink", "dest");
    dest->updControl("mrs_bool/mute", true);
  }

  // Calculate windowed power spectrum and then
  // calculate specific feature sets
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->updControl("Spectrum/spk/mrs_real/cutoff", 1.0);
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updControl("PowerSpectrum/pspk/mrs_string/spectrumType","power");

  // Spectrum Shape descriptors
  MarSystem *spectrumFeatures = mng.create("Fanout",  "spectrumFeatures");
  if (extractorStr == "STFT")
  {
    spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
    spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
    spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  }
  else if (extractorStr == "STFTMFCC")
  {
    spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
    spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
    spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
    spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  }
  else if (extractorStr == "MFCC")
    spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  else if (extractorStr == "SCF")
    spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SFM")
    spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));
  else
  {
    cerr << "Extractor " << extractorStr << " is not supported " << endl;
    return;
  }

  // add the feature to spectral shape
  spectralShape->addMarSystem(spectrumFeatures);

  //  add time-domain zerocrossings
  MarSystem* features = mng.create("Fanout", "features");
  features->addMarSystem(spectralShape);

  if (extractorStr == "STFT")
    features->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

  // Means and standard deviation (statistics) for texture analysis
  MarSystem* statistics = mng.create("Fanout", "statistics");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));

  // Weka output
  MarSystem* wsink = mng.create("WekaSink", "wsink");

  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(srm->clone());

  if (pluginName != EMPTYSTRING) // don't put audio object
    featureNetwork->addMarSystem(dest);

  cout << "featureNetwork = " << (*featureNetwork) << endl;

  featureNetwork->addMarSystem(features);
  featureNetwork->addMarSystem(mng.create("Memory", "memory"));

  featureNetwork->addMarSystem(statistics);
  if (classifierName == "SMO")
    featureNetwork->addMarSystem(mng.create("NormMaxMin", "norm"));

  // update controls I
  featureNetwork->updControl("Memory/memory/mrs_natural/memSize", memSize);
  // featureNetwork->updControl("mrs_natural/inSamples",
  // MRS_DEFAULT_SLICE_NSAMPLES);

  featureNetwork->updControl("mrs_natural/inSamples", winSize);
  featureNetwork->updControl("SilenceRemove/srm/" + src->getType() + "/src/mrs_natural/pos", offset);
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
    featureNetwork->updControl("GaussianClassifier/gaussian/mrs_natural/nClasses", (mrs_natural)cls.size());
  else if (classifierName == "ZeroR")
    featureNetwork->updControl("ZeroRClassifier/zeror/mrs_natural/nClasses", (mrs_natural)cls.size());
  else if (classifierName == "KNN")
    featureNetwork->updControl("KNNClassifier/knn/mrs_natural/nLabels", (mrs_natural)cls.size());
  else if (classifierName == "SMO")
    featureNetwork->updControl("SMO/smo/mrs_natural/nLabels", (mrs_natural)cls.size());

  featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
  featureNetwork->updControl("Confidence/confidence/mrs_string/labelNames",classNames);
  featureNetwork->updControl("WekaSink/wsink/mrs_string/labelNames",classNames);

  // link controls
  featureNetwork->linkControl("mrs_string/filename",
                              "SilenceRemove/srm/SoundFileSource/src/mrs_string/filename");
  featureNetwork->linkControl("mrs_real/israte",
                              "SilenceRemove/srm/SoundFileSource/src/mrs_real/israte");
  featureNetwork->linkControl("mrs_natural/pos",
                              "SilenceRemove/srm/SoundFileSource/src/mrs_natural/pos");
  featureNetwork->linkControl("mrs_bool/hasData",
                              "SilenceRemove/srm/SoundFileSource/src/mrs_bool/hasData");

  mrs_natural wc = 0;
  mrs_natural samplesPlayed =0;
  mrs_natural onSamples = featureNetwork->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

  // main loop for extracting the features
  featureNetwork->updControl("Confidence/confidence/mrs_natural/nLabels", (int)cls.size());
  string className = "";

  MarControlPtr donePtr = featureNetwork->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_bool/hasData");
  MarControlPtr memResetPtr = featureNetwork->getctrl("Memory/memory/mrs_bool/reset");
  MarControlPtr fluxResetPtr = featureNetwork->getctrl("Flux/flux/mrs_bool/reset");
  MarControlPtr fnamePtr = featureNetwork->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_string/filename");
  MarControlPtr annLabelPtr = featureNetwork->getctrl("Annotator/annotator/mrs_natural/label");
  MarControlPtr nlabelsPtr = featureNetwork->getctrl("WekaSink/wsink/mrs_natural/nLabels");
  MarControlPtr wekaDownsamplePtr = featureNetwork->getctrl("WekaSink/wsink/mrs_natural/downsample");
  MarControlPtr wekaFnamePtr = featureNetwork->getctrl("WekaSink/wsink/mrs_string/filename");

  for (cj=0; cj < (mrs_natural)cls.size(); cj++)
  {
    Collection l = cls[cj];
    featureNetwork->setctrl(annLabelPtr, cj);
    featureNetwork->setctrl(nlabelsPtr, (mrs_natural)cls.size());
    featureNetwork->setctrl(wekaDownsamplePtr, 40);
    if (wekafname == EMPTYSTRING)
      featureNetwork->updControl(wekaFnamePtr, "weka.arff");
    else
      featureNetwork->updControl(wekaFnamePtr, wekafname);
    // featureNetwork->updControl("WekaSink/wsink/mrs_natural/label", cj);

    cout << "Class " << cj << " is " << l.name() << endl;

    featureNetwork->setctrl(memResetPtr, true);
    featureNetwork->setctrl(fluxResetPtr, true);

    for (mrs_natural i=0; i < l.size(); ++i)
    {
      featureNetwork->setctrl(memResetPtr, true);
      featureNetwork->setctrl(fluxResetPtr, true);
      featureNetwork->updControl(fnamePtr, l.entry(i));
      wc = 0;
      samplesPlayed = 0;
      while ((donePtr->isTrue()) && (duration > samplesPlayed))
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
    featureNetwork->updControl("GaussianClassifier/gaussian/mrs_bool/done",true);
  else if (classifierName == "ZeroR")
    featureNetwork->updControl("ZeroRClassifier/zeror/mrs_bool/done",true);
  else if (classifierName == "KNN")
    featureNetwork->updControl("KNNClassifier/knn/mrs_bool/done",true);
  else if (classifierName == "SMO")
    featureNetwork->updControl("SMO/smo/mrs_bool/done",true);

  if (classifierName == "SMO")
    featureNetwork->updControl("NormMaxMin/norm/mrs_string/mode", "predict");
  featureNetwork->tick();               // train classifier

  // prepare network for classification
  if (classifierName == "GS")
  {
    featureNetwork->updControl("GaussianClassifier/gaussian/mrs_bool/done",false);
    featureNetwork->updControl("GaussianClassifier/gaussian/mrs_string/mode","predict");
  }
  else if (classifierName == "ZeroR")
  {
    featureNetwork->updControl("ZeroRClassifier/zeror/mrs_bool/done",false);
    featureNetwork->updControl("ZeroRClassifier/zeror/mrs_string/mode","predict") ;
  }
  else if (classifierName == "KNN")
  {
    featureNetwork->updControl("KNNClassifier/knn/mrs_bool/done",false);
    featureNetwork->updControl("KNNClassifier/knn/mrs_string/mode","predict");
    featureNetwork->updControl("KNNClassifier/knn/mrs_natural/k",3);
  }
  else if (classifierName == "SMO")
  {
    featureNetwork->updControl("SMO/smo/mrs_bool/done",false);
    featureNetwork->updControl("SMO/smo/mrs_string/mode","predict");
  }

  /* if (pluginName != EMPTYSTRING)
   {
   featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
   featureNetwork->updControl("AudioSink/dest/mrs_bool/init", false);
   }
  */

  featureNetwork->updControl("Confidence/confidence/mrs_bool/mute", false);
  featureNetwork->updControl("WekaSink/wsink/mrs_bool/mute", true);

  if (pluginName == EMPTYSTRING) // output to stdout
    cout << (*featureNetwork) << endl;
  else
  {
    ofstream oss(pluginName.c_str());
    oss << (*featureNetwork) << endl;
  }

  delete featureNetwork;
}

int
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  mrs_natural attempts=0;

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
    return 1;
  }
  return 0;
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("collection", "c", EMPTYSTRING);
  cmd_options.addBoolOption("normalize", "n", false);
  cmd_options.addRealOption("start", "s", 0);
  cmd_options.addRealOption("length", "l", -1.0f);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addStringOption("wekafile", "w", EMPTYSTRING);
  cmd_options.addStringOption("extractor", "e", "REFACTORED");
  cmd_options.addBoolOption("lexicon", "lx", false);
  cmd_options.addNaturalOption("memory", "m", 20);
  cmd_options.addNaturalOption("windowsize", "ws", 512);
  cmd_options.addNaturalOption("accSize", "as", accSize_);
  cmd_options.addNaturalOption("hopsize", "hp", 512);
  cmd_options.addStringOption("classifier", "cl", EMPTYSTRING);
  cmd_options.addBoolOption("timeline", "t", false);
  cmd_options.addBoolOption("pluginmute", "pm", false);
  cmd_options.addBoolOption("csvoutput", "csv", false);
  cmd_options.addBoolOption("playback", "pb", false);
  cmd_options.addStringOption("outputdir", "od", EMPTYSTRING);
  cmd_options.addStringOption("predict", "pr", EMPTYSTRING);
  cmd_options.addStringOption("test", "tc", EMPTYSTRING);
  cmd_options.addBoolOption("stereo", "st", false);
  cmd_options.addNaturalOption("downsample", "ds", 1);
  cmd_options.addBoolOption("shuffle", "sh", false);
  cmd_options.addBoolOption("mic", "mc", false);
  cmd_options.addNaturalOption("confMemSize", "cm", 40);

  // feature selection options
  cmd_options.addBoolOption("StereoPanningSpectrumFeatures", "spsf", false);
  cmd_options.addBoolOption("MelFrequencyCepstralCoefficients","mfcc", false);
  cmd_options.addBoolOption("Chroma", "chroma", false);
  cmd_options.addBoolOption("SpectralFlatnessMeasure","sfm", false);
  cmd_options.addBoolOption("SpectralCrestFactor","scf", false);
  cmd_options.addBoolOption("SpectralCentroid","ctd", false);
  cmd_options.addBoolOption("SpectralRolloff","rlf", false);
  cmd_options.addBoolOption("SpectralFlux","flx", false);
  cmd_options.addBoolOption("SpectralFeatures", "spfe", false);
  cmd_options.addBoolOption("ZeroCrossings", "zcrs", false);
  cmd_options.addBoolOption("LineSpectralPair", "lsp", false);
  cmd_options.addBoolOption("LinearPredictionCepstralCoefficients", "lpcc", false);
  cmd_options.addBoolOption("BeatFeatures", "bf", false);
  cmd_options.addBoolOption("TimbralFeatures", "timbral", false);
  cmd_options.addBoolOption("SingleVector", "sv", false);
  cmd_options.addBoolOption("featExtract", "fe", false);
  cmd_options.addBoolOption("saivq", "saivq", false);
  cmd_options.addBoolOption("onlyStable", "os", false);
  cmd_options.addBoolOption("regression", "rg", false);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  lexiconopt = cmd_options.getBoolOption("lexicon");
  cmopt = cmd_options.getNaturalOption("confMemSize");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  normopt = cmd_options.getBoolOption("normalize");
  collectionName = cmd_options.getStringOption("collection");
  pluginName = cmd_options.getStringOption("plugin");
  wekafname = cmd_options.getStringOption("wekafile");
  extractorName = cmd_options.getStringOption("extractor");
  classifierName = cmd_options.getStringOption("classifier");
  memSize = cmd_options.getNaturalOption("memory");
  winSize = cmd_options.getNaturalOption("windowsize");
  hopSize = cmd_options.getNaturalOption("hopsize");
  accSize_ = cmd_options.getNaturalOption("accSize");
  tline = cmd_options.getBoolOption("timeline");
  pluginMute  = cmd_options.getBoolOption("pluginmute");
  csvOutput  = cmd_options.getBoolOption("csvoutput");
  playback = cmd_options.getBoolOption("playback");
  workspaceDir = cmd_options.getStringOption("outputdir");
  predictCollection = cmd_options.getStringOption("predict");
  testCollection = cmd_options.getStringOption("test");
  downSample = cmd_options.getNaturalOption("downsample");
  shuffle_ = cmd_options.getBoolOption("shuffle");
  mic_ = cmd_options.getBoolOption("mic");
  stereo_ = cmd_options.getBoolOption("stereo");
  featExtract_ = cmd_options.getBoolOption("featExtract");
  saivq_mode_ = cmd_options.getBoolOption("saivq");

  // feature selection options
  spsf_ = cmd_options.getBoolOption("StereoPanningSpectrumFeatures");
  mfcc_ = cmd_options.getBoolOption("MelFrequencyCepstralCoefficients");
  chroma_ = cmd_options.getBoolOption("Chroma");
  sfm_ = cmd_options.getBoolOption("SpectralFlatnessMeasure");
  scf_ = cmd_options.getBoolOption("SpectralCrestFactor");
  ctd_ = cmd_options.getBoolOption("SpectralCentroid");
  rlf_ = cmd_options.getBoolOption("SpectralRolloff");
  flx_ = cmd_options.getBoolOption("SpectralFlux");
  lsp_ = cmd_options.getBoolOption("LineSpectralPair");
  lpcc_ = cmd_options.getBoolOption("LinearPredictionCepstralCoefficients");
  beat_ = cmd_options.getBoolOption("BeatFeatures");
  spectralFeatures_ = cmd_options.getBoolOption("SpectralFeatures");
  zcrs_ = cmd_options.getBoolOption("ZeroCrossings");
  timbralFeatures_ = cmd_options.getBoolOption("TimbralFeatures");

  single_vector_ = cmd_options.getBoolOption("SingleVector");

  only_stable_ = cmd_options.getBoolOption("onlyStable");
  regression_ = cmd_options.getBoolOption("regression");

  // default feature set
  if ((mfcc_ == false) &&
      (sfm_ == false)  &&
      (scf_ == false)  &&
      (ctd_ == false)  &&
      (rlf_ == false)  &&
      (flx_ == false)  &&
      (spectralFeatures_ == false) &&
      (zcrs_ == false) &&
      (timbralFeatures_ == false) &&
      (lsp_ == false) &&
      (lpcc_ == false) &&
      (chroma_ ==false))
  {
    timbralFeatures_ = true;
  }
}

void bextract(vector<string> soundfiles, mrs_natural label,
              string pluginName, string classNames,
              string wekafname,  mrs_natural memSize,
              string extractorStr,
              string classifierName)
{
  (void) label;
  (void) memSize;
  (void) classifierName;
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
  // over a 0.5-second window (20 analysis frames)
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
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum", "spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->addMarSystem(textureFeatures);

  // Label each class
  spectralShape->addMarSystem(mng.create("Annotator", "anno"));
  spectralShape->addMarSystem(mng.create("WekaSingk", "wsink"));
  spectralShape->addMarSystem(mng.create("GaussianClassifier", "gsc"));
  spectralShape->addMarSystem(mng.create("Confidence", "conf"));

  vector<string>::iterator si;
  mrs_natural classCount = 0;

  if (wekafname == EMPTYSTRING)
    spectralShape->updControl("WekaSink/wsink/mrs_string/filename", "weka.arff");
  else
    spectralShape->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

  spectralShape->updControl("GaussianClassifier/gsc/mrs_natural/nClasses", (mrs_natural)soundfiles.size());

  spectralShape->linkControl("mrs_bool/hasData",
                             "SoundFileSource/src/mrs_bool/hasData");
  spectralShape->linkControl("mrs_string/filename",
                             "SoundFileSource/src/mrs_string/filename");
  spectralShape->linkControl("mrs_natural/pos",
                             "SoundFileSource/src/mrs_natural/pos");

  spectralShape->updControl("GaussianClassifier/gsc/mrs_string/mode","train");
  spectralShape->updControl("WekaSink/wsink/mrs_string/labelNames",classNames);
  spectralShape->updControl("Confidence/conf/mrs_string/labelNames",classNames);

  for (si = soundfiles.begin(); si != soundfiles.end(); ++si)
  {
    cout << "Processing class " << classCount << " collection: "
         << *si << endl;
    spectralShape->updControl("SoundFileSource/src/mrs_string/filename", *si);
    spectralShape->updControl("Annotator/anno/mrs_natural/label", classCount);
    classCount ++;

    while(spectralShape->getctrl("mrs_bool/hasData")->to<mrs_bool>())
    {
      spectralShape->tick();
    }
  }
  spectralShape->updControl("GaussianClassifier/gsc/mrs_bool/done",true);

  spectralShape->tick(); // train classifier

  spectralShape->updControl("GaussianClassifier/gsc/mrs_string/mode","predict");

  if (pluginName == EMPTYSTRING) // output to stdout
    cout << (*spectralShape) << endl;
  else
  {
    ofstream oss(pluginName.c_str());
    oss << (*spectralShape) << endl;
  }
}

void
mirex_bextract()
{
  cout << "MIREX 2007 bextract" << endl;
  cout << "Extracting features for files in collection : " << collectionName << endl;
  cout << "Predicting class for files in collection : " << predictCollection << endl;

  // Get the first filename just to initialize correctly the network
  Collection l;
  l.read(collectionName);
  string sfName = l.entry(0);

  MarSystemManager mng;

  MarSystem* src = mng.create("SoundFileSource", "src");

  MarSystem* featExtractor = (*featExtractors[extractorName])();
  featExtractor->updControl("mrs_natural/winSize", winSize);

  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);
  featureNetwork->addMarSystem(featExtractor);

  // Texture Window Statistics (if any)
  if(memSize != 0)
  {
    featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
    featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
  }

  featureNetwork->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  featureNetwork->updControl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

  if (extractorName == EMPTYSTRING)
  {
    cout << "Please specify feature extractor" << endl;
    return;
  }

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", accSize_);
  acc->addMarSystem(featureNetwork);

  MarSystem* statistics = mng.create("Fanout", "statistics2");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->addMarSystem(statistics);
  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  total->updControl("mrs_natural/inSamples", winSize);

  mrs_natural nLabels = (mrs_natural)l.getNumLabels();

  if (wekafname != EMPTYSTRING)
  {
    cout << "WekaSink nLabels = " << nLabels << endl;
    total->updControl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
    total->updControl("WekaSink/wsink/mrs_natural/nLabels", nLabels);
    total->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
  }

  for (mrs_natural i=0; i < l.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
    cout << "Label = " << l.labelEntry(i) << endl;
    cout << "LabelID = " << l.labelNum(l.labelEntry(i)) << endl;
    total->updControl("Annotator/ann/mrs_natural/label", (mrs_natural)l.labelNum(l.labelEntry(i)));
    cout << "Extracting: " << l.entry(i) << endl;
    total->tick();
  }

  cout << "Extracted features to: " << wekafname << endl;
  string outCollection;
  if (workspaceDir != EMPTYSTRING)
    outCollection = workspaceDir + "extract.txt";
  else
    outCollection = "extract.txt";
  l.write(outCollection);
  cout << "Wrote collection to: " << outCollection << endl;
}

int
saivq_train_refactored(string pluginName,  string wekafname,
                       mrs_natural memSize, string classifierName,
                       mrs_bool single_vector)
{
  (void) memSize;
  MRSDIAG("bextract.cpp - aim_train_refactored");
  cout << "BEXTRACT SAI/VQ REFACTORED" << endl;

#ifndef MARSYAS_ANN
  cout << "You need to enable the WITH_ANN option using cmake when compiling Marsyas in order to run this function" << endl;
  return(0);

#endif

  MarSystemManager mng;


  // Overall extraction and classification network
  MarSystem* bextractNetwork = mng.create("Series", "bextractNetwork");


  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

  // Add a fanout for sound file and audio source ...
  MarSystem *fanout = mng.create("Fanout", "fanout");

  // Add a sound file source (which can also read collections)
  MarSystem *src = mng.create("SoundFileSource", "src");
  fanout->addMarSystem(src);

  // Add a live audio source for realtime classification
  MarSystem *mic = mng.create("AudioSource", "mic");
  mic->updControl("mrs_natural/nChannels", 1);  //stereo
  fanout->addMarSystem(mic);

  // Add the fanout to our feature Network ...
  featureNetwork->addMarSystem(fanout);

  featureNetwork->updControl("mrs_real/israte", 44100.0);   //sampling rate  [!hardcoded]

  // Disable Microphone for training the classifier ...
  featureNetwork->updControl("Fanout/fanout/mrs_natural/disable", 1);
  //featureNetwork->updControl("Fanout/fanout/AudioSource/mic/mrs_bool/initAudio", false);

  // Add a TimelineLabeler, if necessary
  if(tline)
  {
    featureNetwork->addMarSystem(mng.create("TimelineLabeler", "timelineLabeler"));
  }

  // create audio sink and mute it it is stored in the output plugin
  // that can be used for real-time classification
  if (pluginName != EMPTYSTRING)
  {
    MarSystem* dest = mng.create("AudioSink", "dest");
    dest->updControl("mrs_bool/mute", true);
    featureNetwork->addMarSystem(dest);
  }


  // sness - TODO add Stereo to SAI/VQ
  stereo_ = false;

  // Select whether stereo or mono feature extraction is to be used
  if (stereo_ == true)
  {
    MarSystem* stereoFeatures = mng.create("StereoFeatures", "stereoFeatures");
    selectFeatureSet(stereoFeatures);
    featureNetwork->addMarSystem(stereoFeatures);
  }
  else
  {
    featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));
    // MarSystem* featExtractor = mng.create("TimbreFeatures", "featExtractor");
    // selectFeatureSet(featExtractor);
    // featureNetwork->addMarSystem(featExtractor);

    featureNetwork->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
    featureNetwork->addMarSystem(mng.create("AimHCL", "aimhcl"));
    featureNetwork->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
    featureNetwork->addMarSystem(mng.create("AimSAI", "aimsai"));
    featureNetwork->addMarSystem(mng.create("AimBoxes", "aimBoxes"));
    featureNetwork->addMarSystem(mng.create("AimVQ", "aimvq"));

  }

  // // texture statistics
  // featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
  // featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
  // featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset", true);

  // Use accumulator if computing single vector / file
  if (single_vector)
  {
    MarSystem* acc = mng.create("Accumulator", "acc");
    acc->updControl("mrs_natural/nTimes", accSize_);
    acc->addMarSystem(featureNetwork);
    bextractNetwork->addMarSystem(acc);
    MarSystem* song_statistics = mng.create("Fanout", "song_statistics");
    song_statistics->addMarSystem(mng.create("Sum", "sum"));
    song_statistics->updControl("Sum/sum/mrs_string/mode", "sum_observations");
    // song_statistics->addMarSystem(mng.create("Mean", "mn"));
    // song_statistics->addMarSystem(mng.create("StandardDeviation", "std"));
    bextractNetwork->addMarSystem(song_statistics);

    bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
                                 "mrs_string/filename"); // added Fanout ...
    bextractNetwork->linkControl("mrs_bool/hasData",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/hasData"); // added Fanout ...
    bextractNetwork->linkControl("mrs_natural/pos",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ...
    bextractNetwork->linkControl("mrs_real/duration",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration"); // added Fanout ...
    if (pluginName != EMPTYSTRING)
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
                                   "mrs_bool/initAudio");
    bextractNetwork->linkControl("mrs_string/currentlyPlaying",
                                 "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying"); // added Fanout ...

    if(tline)
    {
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabelFile",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ...
      bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                   "Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance"); // added Fanout ...

      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelNames");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
    }
    else
    {
      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
    }

    bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                 "mrs_natural/advance");
  }
  else // running feature extraction
  {
    bextractNetwork->addMarSystem(featureNetwork);
    // link controls to top-level to make life simpler
    bextractNetwork->linkControl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
                                 "mrs_string/filename");
    bextractNetwork->linkControl("mrs_bool/hasData",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/hasData");
    bextractNetwork->linkControl("mrs_natural/pos",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos");
    bextractNetwork->linkControl("mrs_real/duration",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration");
    if (pluginName != EMPTYSTRING)
      bextractNetwork->linkControl("Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
                                   "mrs_bool/initAudio");
    bextractNetwork->linkControl("mrs_string/currentlyPlaying",
                                 "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying");

    if(tline)
    {
      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabelFile",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel");
      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
      bextractNetwork->linkControl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos");
      bextractNetwork->linkControl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance");

      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelNames");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/nLabels");


    }
    else
    {
      bextractNetwork->linkControl("mrs_real/currentLabel",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/currentLabel");
      bextractNetwork->linkControl("mrs_natural/nLabels",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
      bextractNetwork->linkControl("mrs_string/labelNames",
                                   "Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");
    }
  }



  // For now dummy test of Inject - eventually will be used for
  // features that require a second pass over the file such as
  // Beat Histogram features

  mrs_natural beatFeaturesSize = 0;
  MarSystem * beatTracker = NULL;


  if (single_vector && beat_)
  {
    beatTracker = createBeatHistogramFeatureNetwork();

    bextractNetwork->addMarSystem(mng.create("Inject/inject"));
    beatFeaturesSize = beatTracker->getctrl("FlowThru/tempoInduction/BeatHistoFeatures/bhf/mrs_natural/onObservations")->to<mrs_natural>();
    bextractNetwork->updControl("Inject/inject/mrs_natural/injectSize", beatFeaturesSize);

    bextractNetwork->updControl("Inject/inject/mrs_string/injectNames", beatTracker->getctrl("FlowThru/tempoInduction/BeatHistoFeatures/bhf/mrs_string/onObsNames")->to<mrs_string>());

  }
  realvec beatfeatures(beatFeaturesSize);


  // labeling, weka output, classifier and confidence for real-time output
  bextractNetwork->addMarSystem(mng.create("Annotator", "annotator"));
  if (wekafname != EMPTYSTRING)
    bextractNetwork->addMarSystem(mng.create("WekaSink", "wsink"));


  if (!featExtract_)
  {
    bextractNetwork->addMarSystem(mng.create("Classifier", "cl"));
    bextractNetwork->addMarSystem(mng.create("Confidence", "confidence"));


    // link confidence and annotation with SoundFileSource that plays the collection
    bextractNetwork->linkControl("Confidence/confidence/mrs_string/fileName",
                                 "mrs_string/filename");
  }

  bextractNetwork->linkControl("Annotator/annotator/mrs_real/label",
                               "mrs_real/currentLabel");

  // links with WekaSink
  if (wekafname != EMPTYSTRING)
  {
    bextractNetwork->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
                                 "mrs_string/currentlyPlaying");

    bextractNetwork->linkControl("WekaSink/wsink/mrs_string/labelNames",
                                 "mrs_string/labelNames");
    bextractNetwork->linkControl("WekaSink/wsink/mrs_natural/nLabels", "mrs_natural/nLabels");
  }

  // src has to be configured with hopSize frame length in case a ShiftInput
  // is used in the feature extraction network
  bextractNetwork->updControl("mrs_natural/inSamples", hopSize);
  // if (stereo_)
  //    featureNetwork->updControl("StereoFeatures/stereoFeatures/mrs_natural/winSize",
  //                                                    winSize);
  // else
  //    featureNetwork->updControl("TimbreFeatures/featExtractor/mrs_natural/winSize",
  //                                                    winSize);

  if (start > 0.0)
    offset = (mrs_natural) (start * src->getctrl("mrs_real/israte")->to<mrs_real>());
  bextractNetwork->updControl("mrs_natural/pos", offset);
  bextractNetwork->updControl("mrs_real/duration", length);

  // confidence is silent during training
  if (!featExtract_)
  {
    bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);
    bextractNetwork->updControl("Confidence/confidence/mrs_bool/print",true);
    if (single_vector)
      bextractNetwork->updControl("Confidence/confidence/mrs_natural/memSize", 1);

    // select classifier to be used
    selectClassifier(bextractNetwork, classifierName);
  }



  // load the collection which is automatically created by bextract
  // based on the command-line arguments

  if (workspaceDir != EMPTYSTRING)
    bextractNetwork->updControl("mrs_string/filename", workspaceDir + "bextract_single.mf");
  else
    bextractNetwork->updControl("mrs_string/filename", "bextract_single.mf");

  // play sound if playback is enabled
  if (pluginName != EMPTYSTRING && playback)
  {
    featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
    featureNetwork->updControl("mrs_bool/initAudio", true);
  }

  // don't use linkControl so that only value is copied once and linking doesn't
  // remain for the plugin
  if (!featExtract_)
  {
    bextractNetwork->updControl("Confidence/confidence/mrs_string/labelNames",
                                bextractNetwork->getctrl("mrs_string/labelNames"));
    bextractNetwork->updControl("Classifier/cl/mrs_natural/nClasses",
                                bextractNetwork->getctrl("mrs_natural/nLabels"));
    bextractNetwork->updControl("Confidence/confidence/mrs_natural/nLabels",
                                bextractNetwork->getctrl("mrs_natural/nLabels"));
  }


  // setup WekaSink - has to be done after all updates so that changes are correctly
  // written to file
  if (wekafname != EMPTYSTRING)
  {
    bextractNetwork->updControl("WekaSink/wsink/mrs_natural/downsample", downSample);

    cout << "Downsampling factor = " << downSample << endl;

    bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", wekafname);
  }






  // main processing loop for training
  MarControlPtr ctrl_hasData = bextractNetwork->getctrl("mrs_bool/hasData");
  MarControlPtr ctrl_currentlyPlaying = bextractNetwork->getctrl("mrs_string/currentlyPlaying");
  mrs_string previouslyPlaying, currentlyPlaying;


  int n = 0;
  int advance = 0;

  vector<string> processedFiles;
  map<string, realvec> processedFeatures;

  bool seen;
  realvec fvec;
  int label;




  while (ctrl_hasData->to<mrs_bool>())
  {


    if (single_vector)
    {
      currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
      // round
      label = static_cast<int>(bextractNetwork->getctrl("mrs_real/currentLabel")->to<mrs_real>() + 0.5);
      seen = false;


      for (size_t j=0; j<processedFiles.size(); j++)
      {
        cout << "processedFiles" << processedFiles[j] << endl;

        if (processedFiles[j] == currentlyPlaying)
          seen = true;
      }


      if (seen)
      {
        advance++;
        bextractNetwork->updControl("mrs_natural/advance", advance);

        if (wekafname != EMPTYSTRING)
          bextractNetwork->updControl("WekaSink/wsink/mrs_string/injectComment", "% filename " + currentlyPlaying);

        fvec = processedFeatures[currentlyPlaying];
        fvec(fvec.getSize()-1) = label;

        if (wekafname != EMPTYSTRING)
        {
          bextractNetwork->updControl("WekaSink/wsink/mrs_realvec/injectVector", fvec);

          bextractNetwork->updControl("WekaSink/wsink/mrs_bool/inject", true);
        }
      }
      else
      {
        if (beat_)
        {
          beatHistogramFeatures(beatTracker, currentlyPlaying, beatfeatures);
          bextractNetwork->updControl("Inject/inject/mrs_realvec/inject", beatfeatures);
        }

        bextractNetwork->tick();
        // featureNetwork->updControl("TextureStats/tStats/mrs_bool/reset",
        //                                              true);
        fvec = bextractNetwork->getctrl("Annotator/annotator/mrs_realvec/processedData")->to<mrs_realvec>();

        bextractNetwork->updControl("mrs_natural/advance", advance);
        processedFiles.push_back(currentlyPlaying);
        cout << processedFiles.size() << endl;

        processedFeatures[currentlyPlaying] = fvec;
        cout << "Processed: " << n << " - " << currentlyPlaying << endl;
        advance = 1;
        bextractNetwork->updControl("mrs_natural/advance", 1);
      }
      n++;

    }
    else
    {
      bextractNetwork->tick();
      currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
      if (currentlyPlaying != previouslyPlaying)
      {
        cout << "Processed: " << n << " - " << currentlyPlaying << endl;
        n++;

      }

      previouslyPlaying = currentlyPlaying;
    }



  }



  cout << "Finished feature extraction" << endl;
  if (featExtract_)
    return 0;


  // prepare network for real-time playback/prediction
  bextractNetwork->updControl("Classifier/cl/mrs_string/mode","predict");

  cout << "Finished classifier training" << endl;
  if (mic_)
    cout << "Microphone input used" << endl;
  // have the plugin play audio
  if (pluginName != EMPTYSTRING && !pluginMute)
  {
    featureNetwork->updControl("mrs_real/israte", 44100.0);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
    featureNetwork->updControl("AudioSink/dest/mrs_bool/initAudio", true);

    // mute Audio since we are listening with mic at runtime ...
    if (mic_)
      featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", true);
    else
      featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
  }

  // init mic audio ...
  if (mic_)
  {
    bextractNetwork->updControl("mrs_real/israte", 44100.0);   //sampling rate
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_natural/nChannels", 1);        //stereo
    bextractNetwork->linkControl( "mrs_bool/initAudio" , "Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_bool/initAudio" ); //important link!!!
  }


  // finally disable the Soundfile Input in Fanout ...
  if (mic_)
  {
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/mrs_natural/disable", 0);
    // ... and enable live audio source ...
    bextractNetwork->updControl("Series/featureNetwork/Fanout/fanout/mrs_natural/enable", 1);
  }


  // don't output to WekaSink
  if (wekafname != EMPTYSTRING)
    bextractNetwork->updControl("WekaSink/wsink/mrs_bool/mute", false);

  // enable confidence
  bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", false);

  // output trained classifier models
  if (pluginName == EMPTYSTRING) // output to stdout
    ;

  // cout << (*bextractNetwork) << endl;
  else // save to .mpl file
  {
    ofstream oss(pluginName.c_str());
    oss << (*bextractNetwork) << endl;
  }

  // predict optional test collection
  if (testCollection != EMPTYSTRING)
  {
    bextractNetwork->updControl("mrs_natural/advance", 0);
    if (single_vector)
    {
      if (pluginName != EMPTYSTRING && !pluginMute)
        featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", true);

      Collection m;
      m.read(testCollection);

      Collection l;
      if (workspaceDir != EMPTYSTRING)
        l.read(workspaceDir + "bextract_single.mf");
      else
        l.read("bextract_single.mf");
      bextractNetwork->updControl("Confidence/confidence/mrs_bool/mute", true);



      if (wekafname != EMPTYSTRING)
        bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", "predict.arff");
      bextractNetwork->updControl("Classifier/cl/mrs_string/mode", "predict");

      ofstream prout;
      prout.open(predictCollection.c_str());

      int correct_instances = 0;
      int num_instances = 0;

      bextractNetwork->updControl("mrs_string/filename", testCollection);
      bextractNetwork->updControl("mrs_string/labelNames", l.getLabelNames());

      ofstream ofs;
      ofs.open("bextract.mpl");
      ofs << *bextractNetwork << endl;
      ofs.close();



      while (ctrl_hasData->to<mrs_bool>())
      {
        bextractNetwork->tick();
        currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();

        mrs_realvec pr = bextractNetwork->getctrl("Classifier/cl/mrs_realvec/processedData")->to<mrs_realvec>();
        cout << "Predicting " << currentlyPlaying << "\t" << "as \t" << l.labelName((mrs_natural)pr(0,0)) << endl;

        if (single_vector)
        {
          bextractNetwork->updControl("mrs_natural/advance", 1);
        }


        if ((mrs_natural)pr(0,0) == (mrs_natural)(pr(1,0)))
          correct_instances++;
        num_instances++;

        prout << currentlyPlaying << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
      }
      cout << "Correct instances = " << correct_instances << "/" << num_instances << endl;
    }

    else
    {

      cout << "bextract_train_refactored: predicting test collection: " << testCollection << endl;
      bextractNetwork->updControl("mrs_string/filename", testCollection);

      while (ctrl_hasData->to<mrs_bool>())
      {
        currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
        if (currentlyPlaying != previouslyPlaying)
          cout << "Processing : " << currentlyPlaying << endl;

        bextractNetwork->tick();
        previouslyPlaying = currentlyPlaying;
      }
    }
  }



  delete bextractNetwork;
  return 0;
}

int
main(int argc, const char **argv)
{
  MRSDIAG("bextract.cpp - main");

  //////////////////////////////////////////////////////////////////////////
  // Supported Feature extractors
  //////////////////////////////////////////////////////////////////////////
  // list of supported feature extractors and their creation functions
  featExtractors["MPL_FILE"] = &createExtractorFromFile;
  featExtractors["STFT"] = &createSTFTextractor;
  featExtractors["MFCC"] = &createMFCCextractor;
  featExtractors["STFTMFCC"] = &createSTFTMFCCextractor;
  featExtractors["SCF"] = &createSCFextractor;
  featExtractors["SFM"] = &createSFMextractor;
  featExtractors["SFMSCF"] = &createSFMSCFextractor;
  featExtractors["LSP"] = &createLSPextractor;
  featExtractors["LPCC"] = &createLPCCextractor;
  featExtractors["BEAT"] = &createBEATextrator;
  featExtractors["REFACTORED"] = &createSTFTMFCCextractor;

  // short description of each extractor
  featExtractorDesc["MPL_FILE"] = "not yet implemented...";
  featExtractorDesc["STFT"] = "Centroid, Rolloff, Flux, ZeroCrossings ";
  featExtractorDesc["MFCC"] = "Mel-frequency Cepstral Coefficients ";
  featExtractorDesc["STFTMFCC"] = "Centroid, Rolloff Flux, ZeroCrossings, Mel-frequency Cepstral Coefficients";
  featExtractorDesc["SCF"] = "Spectral Crest Factor (MPEG-7)";
  featExtractorDesc["SFM"] = "Spectral Flatness Measure (MPEG-7)";
  featExtractorDesc["SFMSCF"] = "SCF and SFM features";
  featExtractorDesc["LSP"] = "Linear Spectral Pairs";
  featExtractorDesc["LPCC"] = "LPC derived Cepstral coefficients ";
  featExtractorDesc["BEAT"] = "Beat histogram features";
  featExtractorDesc["REFACTORED"] = "Dummy extractor for refactored bextract";

  //////////////////////////////////////////////////////////////////////////

  string progName = argv[0];
  if (argc == 1)
  {
    printUsage(progName);
    return 0;
  }

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  if (helpopt)
    return printHelp(progName);

  if (usageopt)
    return printUsage(progName);

  if (!workspaceDir.empty() && !FileName(wekafname).isAbsolute())
    wekafname = workspaceDir + wekafname;

  //////////////////////////////////////////////////////////////////////////
  // Print analysis options
  //////////////////////////////////////////////////////////////////////////
  cout << endl;
  cout << "Window Size (in samples): " << winSize << endl;
  cout << "Hop Size (in samples): " << hopSize << endl;
  cout << "Memory Size (in analysis windows):" << memSize << endl;
  cout << "Accumulator size (in analysis windows):" << accSize_ << endl;
  cout << endl;
  cout << "Extractor = " << extractorName << endl;
  cout << endl;

  cout << "collectionName = " << collectionName << endl;
  if (collectionName != EMPTYSTRING)
  {
    if (extractorName.substr(0,2) == "SV")
      extractorName = extractorName.substr(2, extractorName.length());
    mirex_bextract();
    return 0;
  }

  int i = 0;
  Collection l;

  string classNames = "";
  vector<Collection> cls;

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;


  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string sfname = *sfi;
    Collection l;
    int status = readCollection(l,sfname);
    if (status != 0) {
      return status;
    }

    if (!l.hasLabels())
    {
      l.labelAll(l.name());
      classNames += (l.name()+',');
    }


    cls.push_back(l);
    ++i;
  }

  Collection single;
  single.concatenate(cls);
  if (single.getSize() == 0)
  {
    MRSERR("Collection has no files  - exiting");
    return 1;
  }

  if (shuffle_)
    single.shuffle();

  if (workspaceDir != EMPTYSTRING)
    single.write(workspaceDir + "bextract_single.mf");
  else
    single.write("bextract_single.mf");

  string extractorStr = extractorName;

  //SINGLE-VALUE Extractor
  if (extractorStr.substr(0,2) == "SV")
  {
    bool withBeatFeatures = extractorName.substr(extractorName.length()-4, extractorName.length()) == "BEAT";
    string extrName = extractorName.substr(2, extractorName.length());

    cout << "extrName = " << extrName << endl;

    if(featExtractors.find(extrName)== featExtractors.end())
    {
      cout << extractorStr << ": Unsupported extractor!" << endl;
      return 1;
    }

    bextract_trainAccumulator(cls, i, pluginName, classNames, wekafname, memSize, extrName,
                              withBeatFeatures);
  }
  //REMOVE_SILENCE Extractor
  else if (extractorStr.substr(0,2) == "RS")
  {
    string extrName = extractorName.substr(2, extractorName.length());
    if(featExtractors.find(extrName)== featExtractors.end())
    {
      cout << extractorStr << ": Unsupported extractor!" << endl;
      return 1;
    }
    bextract_train_rmsilence(cls, i, pluginName, classNames, wekafname, memSize, extrName, classifierName);
  }
  else if (extractorStr == "STEREOSPS")
  {
    bextract_trainStereoSPS(cls, classNames, wekafname, memSize);
  }
  else if (extractorStr == "STEREOMFCC")
  {
    bextract_trainStereoMFCC(cls, classNames, wekafname, memSize);
  }
  else if (extractorStr == "STEREOSPSMFCC")
  {
    bextract_trainStereoSPSMFCC(cls, classNames, wekafname, memSize);
  }
  //---------------- ADRess extractors -----------------------------
  else if (extractorStr == "ADRessSTEREOSPS")
  {
    bextract_trainADRessStereoSPS(cls, classNames, wekafname, memSize);
  }
  else if (extractorStr == "ADRessSTEREOSPSMFCC")
  {
    bextract_trainADRessStereoSPSMFCC(cls, classNames, wekafname, memSize);
  }
  //----------------------------------------------------------------
  else if (extractorStr == "BEAT")
  {
    bool withBeatFeatures = true;
    string extrName;
    extrName = extractorStr;
    cout << "extrName = " << extrName << endl;
    bextract_trainAccumulator(cls, i, pluginName, classNames, wekafname, memSize, extrName,
                              withBeatFeatures);
  }
  //NORMAL Extractor
  else
  {
    if(featExtractors.find(extractorStr)== featExtractors.end())
    {
      cout << extractorStr << ": Unsupported extractor!" << endl;
      return 1;
    }
    if (extractorStr != "REFACTORED")
    {
      bextract_train(cls, single, i, pluginName, classNames, wekafname, memSize,
                     extractorName, classifierName);
    }
    else
    {
      if (classifierName == EMPTYSTRING)
        classifierName = DEFAULT_CLASSIFIER;

      if (saivq_mode_) {
        saivq_train_refactored(pluginName, wekafname, memSize, classifierName, single_vector_);
      } else {
        bextract_train_refactored(pluginName, wekafname, memSize, classifierName, single_vector_);
      }
    }
  }

  return 0;
}
