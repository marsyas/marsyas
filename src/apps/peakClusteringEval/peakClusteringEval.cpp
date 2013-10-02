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

#include <marsyas/common_source.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Conversions.h>
#include <marsyas/FileName.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/marsystems/PeakFeatureSelect.h>
#include <marsyas/marsystems/SelfSimilarityMatrix.h>

//[TODO]
//#include <marsyas/PeUtilities.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>

using namespace std;
using namespace Marsyas;

string pluginName = EMPTYSTRING;
string inputDirectoryName = EMPTYSTRING;
string outputDirectoryName = EMPTYSTRING;
string fileName = EMPTYSTRING;
string noiseName = EMPTYSTRING;
string fileResName = EMPTYSTRING;
string mixName = EMPTYSTRING;
string filePeakName = EMPTYSTRING;

string fileMatName = EMPTYSTRING;

string fileClustName = EMPTYSTRING;
string fileVoicingName = EMPTYSTRING;
string fileF0Name = EMPTYSTRING;
string panningInfo = EMPTYSTRING;
// set the seeking frequency interval for peaks
string intervalFrequency = "250_2500";

// Global variables for command-line options
bool helpopt_ = 0;
bool usageopt_ =0;
int fftSize_ = 2048;
int winSize_ = 2048;
// if kept the same no time expansion
int hopSize_ = 512;
// nb Sines
int nbSines_ = 20;
// nbClusters
int nbClusters_ = 3;
// nbClusters
int nbSelectedClusters_ = 2;
// output buffer Size
int bopt_ = 128;
// output gain //[WTF]
mrs_real gopt_ = 1.0;
// number of accumulated frames
mrs_natural accSize_ = 10;
// number of seconds for analysing process
mrs_natural stopAnalyse_=0;

// type of similarity Metrics // test amplitude normamlise gtzan //[TODO]
string defaultSimilarityType_ = "hoabfb";//[TODO]
string similarityType_ = EMPTYSTRING;//[TODO]
// weight for similarity Metrics
realvec similarityWeight_;	//[TODO]
// store for clustered peaks
realvec peakSet_;//[TODO]
// delay for noise insertion

mrs_real noiseDelay_=0;//[TODO]
// gain for noise insertion
mrs_real noiseGain_=.8;//[TODO]
// duration for noise insertion
mrs_real noiseDuration_=0;//[TODO]

// sampling frequency
mrs_real samplingFrequency_=44100.0;
//
mrs_real timeElapsed;
//
mrs_natural nbTicks=0;
//
mrs_natural clusterFilteringType_ = 0;
//
mrs_natural fileInfo_=0;
//
mrs_natural unprecise_;
//
mrs_natural noPeakPicking_;

bool microphone_ = false;
bool analyse_ = true;
bool attributes_ = false;
bool ground_ = false;
mrs_natural synthetize_ = 0;
mrs_natural clusterSynthetize_ = -1;
bool peakStore_= true;
bool residual_ = false;

bool ignoreFrequency = false;
bool ignoreAmplitude = false;
bool ignoreHWPS = false;
bool ignorePan = false;
bool useOnsets = false;

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("peakClusteringEval.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
}

void
printHelp(string progName)
{
  MRSDIAG("peakClusteringEval.cpp - printHelp");
  cerr << "peakClusteringEval, MARSYAS" << endl;
  cerr << "report bugs to marsyas-users@lists.sourceforge.net" << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-s --sinusoids       : number of sinusoids per frame" << endl;
  cerr << "-b --buffersize      : audio buffer size" << endl;
  cerr << "-o --outputdirectoryname   : output directory path" << endl;
  cerr << "-N --noisename : name of degrading audio file " << endl;
  cerr << "-p --panning : panning informations <foreground level (0..1)>-<foreground pan (-1..1)>-<background level>-<background pan> " << endl;
  cerr << "-t --typeSimilarity : similarity information a (amplitude) f (frequency) h (harmonicity)  " << endl;
  cerr << "-q -quitAnalyse : quit processing after specified number f seconds" << endl;
  cerr << "-v  --voices: number of voices" << endl;
  cerr << "-F --clusterFiltering : cluster filtering" << endl;
  cerr << "-A --attributes : set attributes" << endl;
  cerr << "-g --ground : set ground" << endl;
  cerr << "-SC --clusterSynthetize : cluster synthetize" << endl;
  cerr << "-P --peakStore : set peak store" << endl;
  cerr << "-T --textureSize: number of frames in a texture window (T=0 uses GT)" << endl;
  cerr << "-c -clustering : number of clusters in a texture window (use c=0 for GT, c=-1 for automatically estimated)" << endl;
  cerr << "-k -keep : keep the specified number of clusters in the texture window " << endl;
  cerr << "-S --synthetise : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
  cerr << "-r --residual : output the residual sound (if the synthesis stage is selected)" << endl;
  cerr << "-i --intervalFrequency : <minFrequency>_<maxFrequency> select peaks in this interval (default 250-2500 Hz)" << endl;
  cerr << "-f --fileInfo : provide clustering parameters in the output name (s20t10i250_2500c2k1uTabfbho means 20 sines per frames in the 250_2500 Hz frequency Interval, 1 cluster selected among 2 in one texture window of 10 frames, no precise parameter estimation and using a combination of similarities abfbho)" << endl;
  cerr << "-npp --noPeakPicking : do not perform peak picking in the spectrum" << endl;
  cerr << "-u --unprecise : do not perform precise estimation of sinusoidal parameters" << endl;

  cerr << "-if --ignoreFrequency: ignore frequency similarity between peaks" << endl;
  cerr << "-ia --ignoreAmplitude: ignore amplitude similarity between peaks" << endl;
  cerr << "-ih --ignoreHWPS: ignore harmonicity (HWPS) similarity between peaks" << endl;
  cerr << "-ip --ignorePan: ignore panning similarity between peaks" << endl;

  cerr << "-uo --useOnsets: use onset detector for dynamically adjusting the length of texture windows" << endl;

  cerr << "" << endl;
  cerr << "-h --help            : display this information " << endl;
  cerr << "-us --usage : print usage" << endl;

  exit(1);
}


// original monophonic peakClustering
// peakClustering(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, ...
//                fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);

void
peakClusteringEval(realvec &peakSet, string sfName, string outsfname, string noiseName, string mixName, string intervalFrequency, string panningInfo, mrs_real noiseDelay, string T, mrs_natural N, mrs_natural Nw,
                   mrs_natural D, mrs_natural S, mrs_natural C,
                   mrs_natural accSize, mrs_natural synthetize, mrs_real *snr0)
{
  (void) peakSet;
  (void) outsfname;
  (void) panningInfo;
  (void) T;
  (void) synthetize;
  //FIXME: D is the same as hopSize_ -> fix to avoid confusion!

  MATLAB_EVAL("clear");

  MarSystemManager mng;

  cout << "Extracting Peaks and Computing Clusters..." << endl;

  //**************************************************
  // create the peakClusteringEval network
  //**************************************************
  MarSystem* mainNet = mng.create("Series", "mainNet");

  //**************************************************************************
  //create accumulator for the texture window and add it to the main network
  //**************************************************************************
  MarSystem* textWinNet = mng.create("Accumulator", "textWinNet");
  mainNet->addMarSystem(textWinNet);

  //************************************************************************
  //create Analysis Network and add it to the texture window accumulator
  //************************************************************************
  MarSystem* analysisNet = mng.create("Series", "analysisNet");
  textWinNet->addMarSystem(analysisNet);

  //************************************************************************
  //create FanInOut for mixing with a noise source and add to Analysis Net
  //************************************************************************
  MarSystem* mixer = mng.create("FanOutIn", "mixer");
  //---- create original series and add it to mixer
  MarSystem* oriNet = mng.create("Series", "oriNet");
  if (microphone_)
    oriNet->addMarSystem(mng.create("AudioSource", "src"));
  else
  {
    //oriNet->addMarSystem(mng.create("SoundFileSource", "src"));
    oriNet->addMarSystem(mng.create("MidiFileSynthSource", "src")); //[!]
  }
  oriNet->addMarSystem(mng.create("Gain", "oriGain"));
  mixer->addMarSystem(oriNet);
  //---- create a series for the noiseSource
  if(noiseName != EMPTYSTRING)
  {
    MarSystem* mixseries = mng.create("Series", "mixseries");
    if(noiseName == "white")
      mixseries->addMarSystem(mng.create("NoiseSource", "noise"));
    else
      mixseries->addMarSystem(mng.create("SoundFileSource", "noise"));

    mixseries->addMarSystem(mng.create("Delay", "noiseDelay"));
    MarSystem* noiseGain = mng.create("Gain", "noiseGain");
    mixseries->addMarSystem(noiseGain);
    // add this series in the fanout
    mixer->addMarSystem(mixseries);
  }
  //add Mixer to analysis network
  analysisNet->addMarSystem(mixer);

  //********************************************************
  // create SoundFileSink and add it to the analysis net
  //********************************************************
  if(noiseName != EMPTYSTRING)
    analysisNet->addMarSystem(mng.create("SoundFileSink", "mixSink"));


  //***********************************************************
  // create peakExtract network and add it to the analysis net
  //***********************************************************
  MarSystem* peakExtract = mng.create("Series","peakExtract");
  peakExtract->addMarSystem(mng.create("ShiftInput", "si")); //[?]

  MarSystem* stereoFo = mng.create("Fanout","stereoFo");
  // create Spectrum Network and add it to the analysis net
  MarSystem* spectrumNet = mng.create("Series", "spectrumNet");
  spectrumNet->addMarSystem(mng.create("Stereo2Mono","s2m"));

  //onset detector
  MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
  //onsetdetector->addMarSystem(mng.create("ShiftInput", "si"));
  onsetdetector->addMarSystem(mng.create("Windowing", "win"));
  onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
  onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  onsetdetector->addMarSystem(mng.create("Flux", "flux"));
  onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
  onsetdetector->addMarSystem(mng.create("Filter","filt1"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
  onsetdetector->addMarSystem(mng.create("Filter","filt2"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
  onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker"));
  spectrumNet->addMarSystem(onsetdetector);

  spectrumNet->addMarSystem(mng.create("Shifter", "sh"));
  spectrumNet->addMarSystem(mng.create("Windowing", "wi"));
  MarSystem* parallel = mng.create("Parallel", "par");
  parallel->addMarSystem(mng.create("Spectrum", "spk1"));
  parallel->addMarSystem(mng.create("Spectrum", "spk2"));
  spectrumNet->addMarSystem(parallel);
  // add spectrumNet to stereo fanout
  stereoFo->addMarSystem(spectrumNet);
  //
  //create stereo spectrum net
  MarSystem* stereoSpkNet = mng.create("Series","stereoSpkNet");
  MarSystem* LRnet = mng.create("Parallel","LRnet");
  //
  MarSystem* spkL = mng.create("Series","spkL");
  spkL->addMarSystem(mng.create("Windowing","win"));
  spkL->addMarSystem(mng.create("Spectrum","spk"));
  LRnet->addMarSystem(spkL);
  //
  MarSystem* spkR = mng.create("Series","spkR");
  spkR->addMarSystem(mng.create("Windowing","win"));
  spkR->addMarSystem(mng.create("Spectrum","spk"));
  LRnet->addMarSystem(spkR);
  //
  //add it to the stereo spectrum net series
  stereoSpkNet->addMarSystem(LRnet);
  //
  //add stereo spectrum object to stereo spectrum net
  //stereoSpkNet->addMarSystem(mng.create("StereoSpectrum","stereoSpk")); //AVENDANO
  stereoSpkNet->addMarSystem(mng.create("EnhADRess","ADRess"));//enhADRess_1
  stereoSpkNet->addMarSystem(mng.create("EnhADRessStereoSpectrum","stereoSpk")); //enhADRess_2
  //
  // add the stereo Spectrum net to the Fanout
  stereoFo->addMarSystem(stereoSpkNet);
  //
  // add the fanout to the peakExtract net
  peakExtract->addMarSystem(stereoFo);
  //
  //add peakExtract net to analysis net
  analysisNet->addMarSystem(peakExtract);

  //***************************************************************
  //add PeakConvert to main SERIES for processing texture windows
  //***************************************************************
  mainNet->addMarSystem(mng.create("PeakConvert", "conv"));
  mainNet->linkControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_natural/winSize",
                       "Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/ShiftInput/si/mrs_natural/winSize");

  //***************************************************************
  //create a FlowThru for the Clustering Network and add to main net
  //***************************************************************
  MarSystem* clustNet = mng.create("FlowThru", "clustNet");
  mainNet->addMarSystem(clustNet);

  //***************************************************************
  // create Similarities Network and add it to ClustNet
  //***************************************************************
  MarSystem* simNet = mng.create("FanOutIn", "simNet");
  simNet->updControl("mrs_string/combinator", "*");
  //
  //create Frequency similarity net and add it to simNet
  //
  MarSystem* freqSim = mng.create("Series","freqSim");
  //--------
  freqSim->addMarSystem(mng.create("PeakFeatureSelect","FREQfeatSelect"));
  freqSim->updControl("PeakFeatureSelect/FREQfeatSelect/mrs_natural/selectedFeatures",
                      PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq);
  //--------
  MarSystem* fsimMat = mng.create("SelfSimilarityMatrix","FREQsimMat");
  fsimMat->addMarSystem(mng.create("Metric","FreqL2Norm"));
  fsimMat->updControl("Metric/FreqL2Norm/mrs_string/metric","euclideanDistance");
  fsimMat->updControl("mrs_natural/calcCovMatrix", SelfSimilarityMatrix::diagCovMatrix);
  //fsimMat->updControl("mrs_string/normalize", "MinMax");
  //fsimMat->linkControl("mrs_realvec/covMatrix", "Metric/FreqL2Norm/mrs_realvec/covMatrix");
  freqSim->addMarSystem(fsimMat);
  //--------
  freqSim->addMarSystem(mng.create("RBF","FREQrbf"));
  freqSim->updControl("RBF/FREQrbf/mrs_string/RBFtype","Gaussian");
  freqSim->updControl("RBF/FREQrbf/mrs_bool/symmetricIn",true);
  //--------
  simNet->addMarSystem(freqSim);
  //
  //create Amplitude similarity net and add it to simNet
  //
  MarSystem* ampSim = mng.create("Series","ampSim");
  //--------
  ampSim->addMarSystem(mng.create("PeakFeatureSelect","AMPfeatSelect"));
  ampSim->updControl("PeakFeatureSelect/AMPfeatSelect/mrs_natural/selectedFeatures",
                     PeakFeatureSelect::pkAmplitude | PeakFeatureSelect::dBPkAmp);
  //--------
  MarSystem* asimMat = mng.create("SelfSimilarityMatrix","AMPsimMat");
  asimMat->addMarSystem(mng.create("Metric","AmpL2Norm"));
  asimMat->updControl("Metric/AmpL2Norm/mrs_string/metric","euclideanDistance");
  asimMat->updControl("mrs_natural/calcCovMatrix", SelfSimilarityMatrix::diagCovMatrix);
  //asimMat->updControl("mrs_string/normalize", "MinMax");
  //asimMat->linkControl("mrs_realvec/covMatrix", "Metric/AmpL2Norm/mrs_realvec/covMatrix");
  ampSim->addMarSystem(asimMat);
  //--------
  ampSim->addMarSystem(mng.create("RBF","AMPrbf"));
  ampSim->updControl("RBF/AMPrbf/mrs_string/RBFtype","Gaussian");
  ampSim->updControl("RBF/AMPrbf/mrs_bool/symmetricIn",true);
  //--------
  simNet->addMarSystem(ampSim);
  //
  //create HWPS similarity net and add it to simNet
  //
  MarSystem* HWPSim = mng.create("Series","HWPSim");
  //--------
  HWPSim->addMarSystem(mng.create("PeakFeatureSelect","HWPSfeatSelect"));
  HWPSim->updControl("PeakFeatureSelect/HWPSfeatSelect/mrs_natural/selectedFeatures",
                     PeakFeatureSelect::pkFrequency | PeakFeatureSelect::pkSetFrequencies | PeakFeatureSelect::pkSetAmplitudes);
  //--------
  MarSystem* HWPSsimMat = mng.create("SelfSimilarityMatrix","HWPSsimMat");
  HWPSsimMat->addMarSystem(mng.create("HWPS","hwps"));
  HWPSsimMat->updControl("HWPS/hwps/mrs_bool/calcDistance", true);
  HWPSim->addMarSystem(HWPSsimMat);
  //--------
  HWPSim->addMarSystem(mng.create("RBF","HWPSrbf"));
  HWPSim->updControl("RBF/HWPSrbf/mrs_string/RBFtype","Gaussian");
  HWPSim->updControl("RBF/HWPSrbf/mrs_bool/symmetricIn",true);
  //--------
  simNet->addMarSystem(HWPSim);
  //
  //create Panning similarity net and add it to simNet
  //
  MarSystem* panSim = mng.create("Series","panSim");
  //--------
  panSim->addMarSystem(mng.create("PeakFeatureSelect","PANfeatSelect"));
  panSim->updControl("PeakFeatureSelect/PANfeatSelect/mrs_natural/selectedFeatures",
                     PeakFeatureSelect::pkPan);
  //--------
  MarSystem* psimMat = mng.create("SelfSimilarityMatrix","PANsimMat");
  psimMat->addMarSystem(mng.create("Metric","PanL2Norm"));
  psimMat->updControl("Metric/PanL2Norm/mrs_string/metric","euclideanDistance");
  psimMat->updControl("mrs_natural/calcCovMatrix", SelfSimilarityMatrix::diagCovMatrix);
  //psimMat->updControl("mrs_string/normalize", "MinMax");
  //psimMat->linkControl("mrs_realvec/covMatrix", "Metric/PanL2Norm/mrs_realvec/covMatrix");
  panSim->addMarSystem(psimMat);
  //--------
  panSim->addMarSystem(mng.create("RBF","PANrbf"));
  panSim->updControl("RBF/PANrbf/mrs_string/RBFtype","Gaussian");
  panSim->updControl("RBF/PANrbf/mrs_bool/symmetricIn",true);
  //--------
  simNet->addMarSystem(panSim);
  //
  // LINK controls of PeakFeatureSelects in each similarity branch
  //
  simNet->linkControl("Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/totalNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
  simNet->linkControl("Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/totalNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
  simNet->linkControl("Series/panSim/PeakFeatureSelect/PANfeatSelect/mrs_natural/totalNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
  //------
  simNet->linkControl("Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/frameMaxNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
  simNet->linkControl("Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/frameMaxNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
  simNet->linkControl("Series/panSim/PeakFeatureSelect/PANfeatSelect/mrs_natural/frameMaxNumPeaks",
                      "Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //add simNet to clustNet
  clustNet->addMarSystem(simNet);
  //
  // LINK controls related to variable number of peak from PeakConvert to simNet
  //
  mainNet->linkControl("FlowThru/clustNet/FanOutIn/simNet/Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks",
                       "PeakConvert/conv/mrs_natural/totalNumPeaks");
  mainNet->linkControl("FlowThru/clustNet/FanOutIn/simNet/Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks",
                       "PeakConvert/conv/mrs_natural/frameMaxNumPeaks");

  //***************************************************************
  // create NCutNet MarSystem and add it to clustNet
  //***************************************************************
// 	MarSystem* NCutNet = mng.create("Series","NCutNet");
// 	clustNet->addMarSystem(NCutNet);
// 	//---add NCutNet components
// 	// add a stack to stack the
// 	MarSystem* stack = mng.create("Fanout","stack");
// 	NCutNet->addMarSystem(stack);
// 	stack->addMarSystem(mng.create("NormCut","NCut"));
// 	stack->addMarSystem(mng.create("Gain", "ID"));
// 	// add the cluster selection module
// 	NCutNet->addMarSystem(mng.create("PeakClusterSelect","clusterSelect"));

  // Do not select most prominent clusters anymore
  clustNet->addMarSystem(mng.create("NormCut","NCut")); //[!]

  //***************************************************************
  // create PeakLabeler MarSystem and add it to mainNet
  //***************************************************************
  MarSystem* labeler = mng.create("PeakLabeler","labeler");
  mainNet->addMarSystem(labeler);
  //---- link labeler label control to the NCut output control
  mainNet->linkControl("PeakLabeler/labeler/mrs_realvec/peakLabels", "FlowThru/clustNet/mrs_realvec/innerOut");

  //***************************************************************
  // create PeakViewSink MarSystem and add it to mainNet
  //***************************************************************
  if(peakStore_)
  {
    mainNet->addMarSystem(mng.create("PeakViewSink", "peSink"));
    mainNet->updControl("PeakViewSink/peSink/mrs_string/filename", filePeakName);
  }

  //****************************************************************
  // Create Synthesis Network
  //****************************************************************
  MarSystem* postNet = mng.create("Series", "postNet");

  // Create Peak Synth ////////////////////////////////////
  MarSystem* peakSynth = mng.create("Series", "peakSynth");
  peakSynth->addMarSystem(mng.create("PeakSynthOsc", "pso"));
  peakSynth->addMarSystem(mng.create("Windowing", "wiSyn"));
  peakSynth->addMarSystem(mng.create("OverlapAdd", "ov"));
  peakSynth->addMarSystem(mng.create("Gain", "outGain"));
// 		MarSystem *dest;
// 		dest = new SoundFileSink("dest");
// 		//dest->updControl("mrs_string/filename", outsfname);
// 		peakSynth->addMarSystem(dest);
  mng.registerPrototype("PeakSynth", peakSynth);

  // Create Bank of Synths ////////////////////////////////////////////////
  MarSystem* synthBank = mng.create("Fanout","synthBank");
  mrs_natural numSynths = 10; // HARDCODED FOR NOW [!]
  synthBank->addMarSystem(mng.create("PeakSynth", "synthCluster_0"));
  synthBank->updControl("PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_natural/peakGroup2Synth", 0);
  for(mrs_natural s=1; s < numSynths; ++s)
  {
    ostringstream oss;
    oss << "synthCluster_" << s;
    synthBank->addMarSystem(mng.create("PeakSynth", oss.str()));
    //link controls between branches
    synthBank->linkControl("PeakSynth/"+oss.str()+"/PeakSynthOsc/pso/mrs_real/samplingFreq",
                           "PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_real/samplingFreq");
    synthBank->linkControl("PeakSynth/"+oss.str()+"/PeakSynthOsc/pso/mrs_natural/delay",
                           "PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_natural/delay");
    synthBank->linkControl("PeakSynth/"+oss.str()+"/PeakSynthOsc/pso/mrs_natural/synSize",
                           "PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_natural/synSize");
    synthBank->linkControl("PeakSynth/"+oss.str()+"/Windowing/wiSyn/mrs_string/type",
                           "PeakSynth/synthCluster_0/Windowing/wiSyn/mrs_string/type");

    synthBank->updControl("PeakSynth/"+oss.str()+"/PeakSynthOsc/pso/mrs_natural/peakGroup2Synth", s);
  }

  postNet->addMarSystem(synthBank);

  // SHREDDER /////////////////////////////////////////////////
  MarSystem* synthNet = mng.create("Shredder", "synthNet");
  synthNet->addMarSystem(postNet);
  synthNet->updControl("mrs_bool/accumulate", true); //accumulate output
  mainNet->addMarSystem(synthNet);
  //link Shredder nTimes to Accumulator nTimes
  mainNet->linkControl("Shredder/synthNet/mrs_natural/nTimes",
                       "Accumulator/textWinNet/mrs_natural/nTimes");

  // add a MATLAB sink at the very end of the network!
  mainNet->addMarSystem(mng.create("PlotSink", "send2MATLAB"));
  mainNet->updControl("PlotSink/send2MATLAB/mrs_bool/sequence", false);
  mainNet->updControl("PlotSink/send2MATLAB/mrs_bool/messages", false);
  mainNet->updControl("PlotSink/send2MATLAB/mrs_bool/matlab", true);
  mainNet->updControl("PlotSink/send2MATLAB/mrs_string/matlabCommand",
                      "evaluateTextWin;");

  //****************************************************************

  ////////////////////////////////////////////////////////////////
  // update the controls
  ////////////////////////////////////////////////////////////////
  //mainNet->updControl("Accumulator/textWinNet/mrs_natural/nTimes", accSize);

  if (microphone_)
  {
    mainNet->updControl("mrs_natural/inSamples", D);
    mainNet->updControl("mrs_natural/inObservations", 1);
  }
  else
  {
    cout << ">> Loading MIDI file and synthetizing audio data..." << endl;
    mainNet->updControl("mrs_natural/inSamples", D);
    mainNet->updControl("mrs_real/israte", samplingFrequency_);
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_real/start", 10.0);
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_real/end", 20.0);
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_string/filename", sfName);
    //mainNet->updControl("mrs_natural/inObservations", 1);
    //samplingFrequency_ = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_real/osrate")->to<mrs_real>();
  }

  if(noiseName != EMPTYSTRING)
  {
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/SoundFileSource/noise/mrs_string/filename", noiseName);
    mainNet->updControl("mrs_natural/inSamples", D);
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/NoiseSource/noise/mrs_string/mode", "rand");
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Seriesmixseries/Delay/noiseDelay/mrs_real/delaySeconds",  noiseDelay);
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/Gain/noiseGain/mrs_real/gain", noiseGain_);
  }

  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/ShiftInput/si/mrs_natural/winSize", Nw+1);

  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Shifter/sh/mrs_natural/shift", 1);

  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_natural/size", N);
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_string/type", "Hanning");
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_bool/zeroPhasing", true);

  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_natural/size", N);
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_string/type", "Hanning");
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_bool/zeroPhasing", true);
  //
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_natural/size", N);
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_string/type", "Hanning");
  mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_bool/zeroPhasing", true);

  if(unprecise_)
    mainNet->updControl("PeakConvert/conv/mrs_bool/improvedPrecision", false);
  else
    mainNet->updControl("PeakConvert/conv/mrs_bool/improvedPrecision", true);

  if(noPeakPicking_)
    mainNet->updControl("PeakConvert/conv/mrs_bool/picking", false);

  mainNet->updControl("PeakConvert/conv/mrs_natural/frameMaxNumPeaks", S);
  mainNet->updControl("PeakConvert/conv/mrs_string/frequencyInterval", intervalFrequency);
  mainNet->updControl("PeakConvert/conv/mrs_natural/nbFramesSkipped", 0);//(N/D));

  //mainNet->updControl("FlowThru/clustNet/Series/NCutNet/Fanout/stack/NormCut/NCut/mrs_natural/numClusters", C); [!]
  //mainNet->updControl("FlowThru/clustNet/Series/NCutNet/PeakClusterSelect/clusterSelect/mrs_natural/numClustersToKeep", nbSelectedClusters_);//[!]
  //
  if(C > 0) //use fixed and manually set number of clusters
  {
    cout << ">> Using fixed number of clusters: " << C << " clusters." << endl;
    mainNet->updControl("FlowThru/clustNet/NormCut/NCut/mrs_natural/numClusters", C); //[!]
  }
  else if(C==0) //use GT number of clusters
  {
    cout << "** Using GT number of clusters." << endl;
    mainNet->linkControl("FlowThru/clustNet/NormCut/NCut/mrs_natural/numClusters",
                         "Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_natural/numActiveNotes");
  }
  else if(C==-1) //automatically estimate number of clusters
  {
    cout << "Automatic Estimation of number of clusters: NOT YET IMPLEMENTED! Exiting...";
    exit(0);
  }

  // 	//[TODO]
  // 	mainNet->setctrl("PeClust/peClust/mrs_natural/selectedClusters", nbSelectedClusters_);
  // 	mainNet->setctrl("PeClust/peClust/mrs_natural/hopSize", D);
  // 	mainNet->setctrl("PeClust/peClust/mrs_natural/storePeaks", (mrs_natural) peakStore_);
  // 	mainNet->updControl("PeClust/peClust/mrs_string/similarityType", T);
  //
  // 	similarityWeight_.stretch(3);
  // 	similarityWeight_(0) = 1;
  // 	similarityWeight_(1) = 10;  //[WTF]
  // 	similarityWeight_(2) = 1;
  // 	mainNet->updControl("PeClust/peClust/mrs_realvec/similarityWeight", similarityWeight_);

  if(noiseName != EMPTYSTRING)
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/SoundFileSink/mixSink/mrs_string/filename", mixName);

  mainNet->update();

  //check if input is a stereo signal
  if(mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/mrs_natural/onObservations")->to<mrs_natural>() == 1)
  {
    //if a not a stereo signal, we must set the Stereo2Mono weight to 1.0 (i.e. do no mixing)!
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Stereo2Mono/s2m/mrs_real/weight", 1.0);
  }

  //------------------------------------------------------------------------
  //check which similarity computations should be disabled (if any)
  //------------------------------------------------------------------------
  // Frequency Similarity
  if(ignoreFrequency)
  {
    cout << "** Frequency Similarity Computation disabled!" << endl;
    mainNet->updControl("FlowThru/clustNet/FanOutIn/simNet/mrs_string/disableChild",
                        "Series/freqSim");
  }
  else
    cout << "** Frequency Similarity Computation enabled!" << endl;
  // amplitude similarity
  if(ignoreAmplitude)
  {
    cout << "** Amplitude Similarity Computation disabled!" << endl;
    mainNet->updControl("FlowThru/clustNet/FanOutIn/simNet/mrs_string/disableChild",
                        "Series/ampSim");
  }
  else
    cout << "** Amplitude Similarity Computation enabled!" << endl;
  // HWPS similarity
  if(ignoreHWPS)
  {
    cout << "** HWPS (harmonicity) Similarity Computation disabled!" << endl;
    mainNet->updControl("FlowThru/clustNet/FanOutIn/simNet/mrs_string/disableChild",
                        "Series/HWPSim");
  }
  else
    cout << "** HWPS (harmonicity) Similarity Computation enabled!" << endl;
  //
  //Panning Similarity
  if(mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/mrs_natural/onObservations")->to<mrs_natural>() == 2 &&
      !ignorePan)
  {
    cout << "** Panning Similarity Computation enabled!" << endl;
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/mrs_string/enableChild",
                        "Series/stereoSpkNet");
    mainNet->updControl("FlowThru/clustNet/FanOutIn/simNet/mrs_string/enableChild",
                        "Series/panSim");
  }
  else //if not stereo or if stereo to be ignored, disable some branches
  {
    cout << "** Panning Similarity Computation disabled!" << endl;
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/mrs_string/disableChild",
                        "Series/stereoSpkNet");
    mainNet->updControl("FlowThru/clustNet/FanOutIn/simNet/mrs_string/disableChild",
                        "Series/panSim");
  }
  //
  mainNet->update(); //probably not necessary... [!]
  //------------------------------------------------------------------------

  if(noiseDuration_) //[WTF]
  {
    ostringstream ossi;
    ossi << ((noiseDelay_+noiseDuration_)) << "s";
    cout << ossi.str() << endl;
    // touch the gain directly
    //	noiseGain->updControl("0.1s", Repeat("0.1s", 1), new EvValUpd(noiseGain,"mrs_real/gain", 0.0));
  }

  //ONSET DETECTION CONFIGURATION (if enabled)
  if(useOnsets)
  {
    cout << "** Onset detector enabled -> using dynamically adjusted texture windows!" << endl;
    cout << "WinSize = " << winSize_ << endl;
    cout << "N = " << N << endl;
    cout << "Nw = " << Nw << endl;
    cout << "hopSize = " << hopSize_ << endl;
    cout << "D = " << D << endl;
    cout << "fs = " << samplingFrequency_ << endl;

    //link controls for onset detector
    onsetdetector->linkControl("Filter/filt2/mrs_realvec/ncoeffs",
                               "Filter/filt1/mrs_realvec/ncoeffs");
    onsetdetector->linkControl("Filter/filt2/mrs_realvec/dcoeffs",
                               "Filter/filt1/mrs_realvec/dcoeffs");
    //link onset detector to accumulator and if onsets enabled, set "explicitFlush" mode
    textWinNet->linkControl("mrs_bool/flush",
                            "Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

    //update onset detector controls
    onsetdetector->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", "wrongdBonsets");
    onsetdetector->updControl("Flux/flux/mrs_string/mode", "DixonDAFX06");
    realvec bcoeffs(1,3);//configure zero-phase Butterworth filter of Flux time series -> butter(2, 0.28)
    bcoeffs(0) = 0.1174;
    bcoeffs(1) = 0.2347;
    bcoeffs(2) = 0.1174;
    realvec acoeffs(1,3);
    acoeffs(0) = 1.0;
    acoeffs(1) = -0.8252;
    acoeffs(2) = 0.2946;
    onsetdetector->updControl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
    onsetdetector->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
    mrs_natural lookAheadSamples = 6;
    onsetdetector->updControl("PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples); //!!
    onsetdetector->updControl("PeakerOnset/peaker/mrs_real/threshold", 1.5); //!!!
    onsetdetector->updControl("ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);

    //set Accumulator controls for explicit flush mode
    mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize_)/hopSize_/2.0));
    cout << "Accumulator/textWinNet timesToKeep = " << winds << endl;
    mrs_real textureWinMinLen = 0.050; //secs
    mrs_real textureWinMaxLen = 1.0; //secs
    mrs_natural minTimes = (mrs_natural)(textureWinMinLen*samplingFrequency_/hopSize_);
    mrs_natural maxTimes = (mrs_natural)(textureWinMaxLen*samplingFrequency_/hopSize_);
    cout << "Accumulator/textWinNet MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
    cout << "Accumulator/textWinNet MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" <<endl;
    textWinNet->updControl("mrs_string/mode", "explicitFlush");
    textWinNet->updControl("mrs_natural/timesToKeep", winds);
    //textWinNet->updControl("mrs_string/mode","explicitFlush");
    textWinNet->updControl("mrs_natural/maxTimes", maxTimes);
    textWinNet->updControl("mrs_natural/minTimes", minTimes);

    //set MidiFileSynthSource to receive a signal for each texture window
    mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/sigNewTextWin", false);
    mainNet->linkControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/newTextWin",
                         "Accumulator/textWinNet/mrs_bool/flush");
  }
  else
  {
    cout << "** Onset detector disabled ->";
    if(accSize_>0)//manually set texture window length
    {
      cout << " using fixed length texture windows" << endl;
      cout << "Accumulator/textWinNet nTimes = " << accSize << " (i.e. " << accSize*hopSize_/samplingFrequency_ << " secs)" << endl;
      mainNet->updControl("Accumulator/textWinNet/mrs_natural/nTimes", accSize);

      //set MidiFileSynthSource to receive a signal for each texture window
      mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/sigNewTextWin", false);
      mainNet->linkControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/newTextWin",
                           "Accumulator/textWinNet/mrs_bool/flush");

    }
    else //use GT for texture window length
    {
      cout << " using GT length texture windows" << endl;

      //set texture window
      mainNet->updControl("Accumulator/textWinNet/mrs_string/mode", "explicitFlush");
      mrs_real textureWinMinLen = 0.0; //secs - let GT take care of this...
      mrs_real textureWinMaxLen = 15.0; //secs - just a majorant...
      mrs_natural minTimes = (mrs_natural)(textureWinMinLen*samplingFrequency_/hopSize_);
      mrs_natural maxTimes = (mrs_natural)(textureWinMaxLen*samplingFrequency_/hopSize_);
      cout << "Accumulator/textWinNet MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
      cout << "Accumulator/textWinNet MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" <<endl;
      mainNet->updControl("Accumulator/textWinNet/mrs_natural/maxTimes", maxTimes);
      mainNet->updControl("Accumulator/textWinNet/mrs_natural/minTimes", minTimes);

      //set MidiFileSynthSource to send a signal for each texture window
      mainNet->updControl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/sigNewTextWin", true);
      mainNet->linkControl("Accumulator/textWinNet/mrs_bool/flush",
                           "Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/newTextWin");
    }
  }

  mrs_natural delay = -(winSize_/2 - hopSize_);
  mainNet->updControl("Shredder/synthNet/Series/postNet/Fanout/synthBank/PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_real/samplingFreq", samplingFrequency_);
  mainNet->updControl("Shredder/synthNet/Series/postNet/Fanout/synthBank/PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_natural/delay", delay); // Nw/2+1
  mainNet->updControl("Shredder/synthNet/Series/postNet/Fanout/synthBank/PeakSynth/synthCluster_0/PeakSynthOsc/pso/mrs_natural/synSize", hopSize_*2);
  mainNet->updControl("Shredder/synthNet/Series/postNet/Fanout/synthBank/PeakSynth/synthCluster_0/Windowing/wiSyn/mrs_string/type", "Hanning");

  //[!]
  //if (outsfname == "MARSYAS_EMPTY")
  //	mainNet->updControl("Shredder/synthNet/Series/postNet/AudioSink/dest/mrs_natural/bufferSize", bopt_);
  // else
  //	mainNet->updControl("Shredder/synthNet/Series/postNet/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]


  //***************************************************************************************************************
  //									MAIN TICKING LOOP
  //***************************************************************************************************************
  cout <<">> Start processing..." << endl;
  //ofstream cfile("density.txt", ios::app); //[WTF] [TODO]
  mrs_real globalSnr = 0;
  mrs_natural frameCount = 0;
  //	mrs_real time=0;

  mrs_natural numTextWinds = 0;
  while(analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/hasData")->to<mrs_bool>())//(1)
  {
    mainNet->tick();
    numTextWinds++;

    //if(numTextWinds == 63)
    //{
    //	cout << "!!!!" << endl;
    //}


    if (!microphone_)
    {
      //bool temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/hasData")->to<mrs_bool>();
      //bool temp1 = textWinNet->getctrl("Series/analysisNet/FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/hasData")->to<mrs_bool>();
      //bool temp2 = analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_bool/hasData")->to<mrs_bool>();

      mrs_real timeRead =  analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_natural/pos")->to<mrs_natural>()/samplingFrequency_;
      mrs_real timeLeft;
      //if(!stopAnalyse_)
      timeLeft =  analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/MidiFileSynthSource/src/mrs_natural/size")->to<mrs_natural>()/samplingFrequency_;
      //else
      //	timeLeft = stopAnalyse_;

      // string fname = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_string/filename")->to<mrs_string>();

      printf("Processed texture window %d : %.2f / %.2f \r", (int)numTextWinds, timeRead, timeLeft);
      fflush(stdout);

      //cout << fixed << setprecision(2) << timeRead << "/" <<  setprecision(2) << timeLeft;
      ///*bool*/ temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();

      //[TODO]
      // 			mrs_real density = mainNet->getctrl("PeClust/peClust/mrs_real/clusterDensity")->to<mrs_real>();
      // 			cfile << density << " " << oriGain << endl;
      // 			//cout << oriGain << endl;

      //if (temp2 == false || (stopAnalyse_ !=0 && stopAnalyse_<timeRead))
      //	break;
    }
  }
  if(synthetize_ > -1 && residual_ && frameCount != 0)
  {
    cout << "Global SNR : " << globalSnr/frameCount << endl;
    *snr0 = globalSnr/frameCount;
  }

  if(peakStore_)
  {
// 		mainNet->updControl("PeakViewSink/peSink/mrs_real/fs", samplingFrequency_);
// 		mainNet->updControl("PeakViewSink/peSink/mrs_natural/frameSize", D);
// 		mainNet->updControl("PeakViewSink/peSink/mrs_string/filename", filePeakName);
// 		mainNet->updControl("PeakViewSink/peSink/mrs_bool/done", true);
  }

  //cout << ">> Saving .mat file with results so they can be opened in MATLAB in: " << fileMatName << endl;
  //MATLAB_EVAL("save " + fileMatName);

  if(numTextWinds > 0)
  {
    cout << ">> Saving SDR results to SDR.mat" << endl;
    MATLAB_EVAL("saveSDRresults");
  }

  cout << endl << ">> End of processing. Bye!" << endl;

  //cfile.close(); [TODO]
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "us", false);
  cmd_options.addNaturalOption("voices", "v", 1);
  cmd_options.addStringOption("noisename", "N", EMPTYSTRING);
  cmd_options.addStringOption("outputdirectoryname", "o", EMPTYSTRING);//".");
  cmd_options.addStringOption("intervalFrequency", "i", intervalFrequency);
  cmd_options.addStringOption("panning", "p", EMPTYSTRING);
  cmd_options.addStringOption("typeSimilarity", "t", defaultSimilarityType_);
  cmd_options.addNaturalOption("winsize", "w", winSize_);
  cmd_options.addNaturalOption("fftsize", "n", fftSize_);
  cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
  cmd_options.addNaturalOption("bufferSize", "b", bopt_);
  cmd_options.addNaturalOption("quitAnalyse", "q", stopAnalyse_);
  cmd_options.addNaturalOption("clustering", "c", nbClusters_);
  cmd_options.addNaturalOption("keep", "k", nbSelectedClusters_);
  cmd_options.addNaturalOption("textureSize", "T", accSize_);
  cmd_options.addNaturalOption("clusterFiltering", "F", clusterFilteringType_);
  cmd_options.addBoolOption("fileInfo", "f", 0);
  cmd_options.addBoolOption("residual", "r", 0);
  cmd_options.addBoolOption("unprecise", "u", 0);
  // cmd_options.addBoolOption("analyse", "a", analyse_);
  cmd_options.addBoolOption("attributes", "A", attributes_);
  cmd_options.addBoolOption("ground", "g", ground_);
  cmd_options.addNaturalOption("synthetize", "S", synthetize_);
  cmd_options.addBoolOption("noPeakPicking", "npp", 0);
  cmd_options.addNaturalOption("clusterSynthetize", "SC", clusterSynthetize_);
  cmd_options.addBoolOption("peakStore", "P", peakStore_);

  cmd_options.addBoolOption("ignoreFrequency", "if", ignoreFrequency);
  cmd_options.addBoolOption("ignoreAmplitude", "ia", ignoreAmplitude);
  cmd_options.addBoolOption("ignoreHWPS", "ih", ignoreHWPS);
  cmd_options.addBoolOption("ignorePan", "ip", ignorePan);
  cmd_options.addBoolOption("useOnsets", "uo", useOnsets);
}

void
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
  intervalFrequency = cmd_options.getStringOption("intervalFrequency");
  panningInfo = cmd_options.getStringOption("panning");
  outputDirectoryName = cmd_options.getStringOption("outputdirectoryname");
  similarityType_ = cmd_options.getStringOption("typeSimilarity");
  noiseName = cmd_options.getStringOption("noisename");
  winSize_ = cmd_options.getNaturalOption("winsize");
  fftSize_ = cmd_options.getNaturalOption("fftsize");
  nbSines_ = cmd_options.getNaturalOption("sinusoids");
  bopt_ = cmd_options.getNaturalOption("bufferSize");
  stopAnalyse_ = cmd_options.getNaturalOption("quitAnalyse");
  nbClusters_ = cmd_options.getNaturalOption("clustering");
  nbSelectedClusters_ = cmd_options.getNaturalOption("keep");
  accSize_ = cmd_options.getNaturalOption("textureSize");
  clusterFilteringType_ = cmd_options.getNaturalOption("clusterFiltering");
  fileInfo_ = cmd_options.getBoolOption("fileInfo");
  // analyse_ = cmd_options.getBoolOption("analyse");
  attributes_ = cmd_options.getBoolOption("attributes");
  ground_ = cmd_options.getBoolOption("ground");
  synthetize_ = cmd_options.getNaturalOption("synthetize");
  noPeakPicking_ = cmd_options.getBoolOption("noPeakPicking");
  clusterSynthetize_ = cmd_options.getNaturalOption("clusterSynthetize");
  peakStore_ = cmd_options.getBoolOption("peakStore");
  residual_ = cmd_options.getBoolOption("residual");
  unprecise_ = cmd_options.getBoolOption("unprecise");

  ignoreFrequency = cmd_options.getBoolOption("ignoreFrequency");
  ignoreAmplitude = cmd_options.getBoolOption("ignoreAmplitude");
  ignoreHWPS = cmd_options.getBoolOption("ignoreHWPS");
  ignorePan = cmd_options.getBoolOption("ignorePan");
  useOnsets = cmd_options.getBoolOption("useOnsets");
}

int
main(int argc, const char **argv)
{
  MRSDIAG("peakClusteringEval.cpp - main");

  mrs_real snr0=0;
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  string progName = argv[0];

  if (helpopt_)
    printHelp(progName);

  if (usageopt_)
    printUsage(progName);

  cerr << "peakClusteringEval configuration (-h show the options): " << endl;
  cerr << "fft size (-n)      = " << fftSize_ << endl;
  cerr << "win size (-w)      = " << winSize_ << endl;
  cerr << "sinusoids (-s)     = " << nbSines_ << endl;
  cerr << "outFile  (-f)      = " << fileName << endl;
  cerr << "outputDirectory  (-o) = " << outputDirectoryName << endl;
  cerr << "inputDirectory  (-i) = " << inputDirectoryName << endl;

  nbTicks = clock();

  // extract peaks and clusters
  // soundfile input
  string sfname;
  if (soundfiles.size() != 0)
  {
    // process several soundFiles
    for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; ++sfi)
    {
      FileName Sfname(*sfi);

      string path, outputInfo;
      if(outputDirectoryName != EMPTYSTRING)
        path = outputDirectoryName;
      else
        path =Sfname.path();
      if(fileInfo_)
      {
        stringstream outputInf;
        outputInf << "_s" << nbSines_;
        outputInf << "i" << intervalFrequency;
        outputInf << "T" << accSize_;
        outputInf << "c" << nbClusters_;
        outputInf << "k" << nbSelectedClusters_;
        if(unprecise_)
          outputInf << "u";
        outputInf << "t" << similarityType_;
        outputInf << "_";
        outputInfo = outputInf.str();
      }

      fileName = path + "/" + Sfname.nameNoExt()+outputInfo+"Sep"+"."+Sfname.ext() ;
      fileResName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Res"+ "." + Sfname.ext() ;
      mixName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Mix" + "."+ Sfname.ext() ;
      filePeakName = path + "/" + Sfname.nameNoExt() + outputInfo+".peak" ;

      fileMatName = path + Sfname.nameNoExt() + outputInfo+".mat";

      fileClustName = path + "/" + Sfname.nameNoExt()+outputInfo+ "Clust" +".txt" ;
      fileVoicingName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Voicing" +".txt" ;
      fileF0Name = path + "/" + Sfname.nameNoExt() +outputInfo+  "F0.txt" ;

      if(Sfname.ext() == "peak")
      {
        analyse_ = false;
        if(synthetize_ > -1)
          clusterSynthetize_ = synthetize_;
        else
          clusterSynthetize_ = 0;
        fileName = path + "/" + Sfname.nameNoExt()+outputInfo+".wav" ;
      }

      if(Sfname.ext() == "wav")
      {
        analyse_ = 1;
      }

      if(noiseName == "music")
      {
        string tmp = Sfname.nameNoExt();
        tmp.replace(tmp.length()-1, 1, 1, 'M');
        noiseName = Sfname.path() +tmp + "." +  Sfname.ext();
      }
      cout << noiseName << endl;

      if(analyse_)
      {
        cout << "peakClusteringEval " << Sfname.name() << endl;
        peakClusteringEval(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);
      }

    }

    timeElapsed = (clock()-nbTicks)/((mrs_real) CLOCKS_PER_SEC );
    cout << "Time elapsed: " << timeElapsed << endl;

    exit(0);
  }
}


