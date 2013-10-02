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
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>

#include <marsyas/marsystems/MarSystemTemplateBasic.h>
#include <marsyas/marsystems/MarSystemTemplateAdvanced.h>
#include <marsyas/marsystems/Spectrum2ACMChroma.h>
#include <marsyas/sched/EvValUpd.h>
#include <marsyas/Collection.h>
#include <marsyas/NumericLib.h>


#ifdef MARSYAS_MIDIIO
#include <RtMidi.h>
#endif

#ifdef MARSYAS_PNG
#include <pngwriter.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <time.h>

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;
int helpopt;
int usageopt;
int verboseopt;
int audiosynthopt;
mrs_real thresholdopt;
int confidenceopt;


int
printUsage(string progName)
{
  MRSDIAG("onsets.cpp - printUsage");
  cerr << "Usage : " << progName << "[-as] fileName" << endl;
  cerr << "where fileName is a sound file in a MARSYAS supported format" << endl;
  cerr << endl;
  return (1);
}

int
printHelp(string progName)
{
  MRSDIAG("onsets.cpp - printHelp");
  cerr << "onsets, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------------------" << endl;
  cerr << "Detect the onsets in the sound file provided as argument" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "fileName" << endl;
  cerr << endl;
  cerr << "where file is a sound file in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose output " << endl;
  cerr << "-as --audiosynth: synthesize onsets and mix with original sound" << endl;
  cerr << "-th --threshold : a positive floating number for thresholding the novelty function" << endl;
  cerr << "-co --confidence : output confidence of onsets" << endl;

  return(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addBoolOption("audiosynth", "as", false);
  cmd_options.addRealOption("threshold", "th", 0.1);
  cmd_options.addBoolOption("confidence", "co", false);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  audiosynthopt = cmd_options.getBoolOption("audiosynth");
  thresholdopt = cmd_options.getRealOption("threshold");
  confidenceopt = cmd_options.getBoolOption("confidence");
}

void
detect_onsets(string sfName)
{
  // cout << "toying with onsets" << endl;
  MarSystemManager mng;

  // assemble the processing network
  MarSystem* onsetnet = mng.create("Series", "onsetnet");
  MarSystem* onsetaccum = mng.create("Accumulator", "onsetaccum");
  MarSystem* onsetseries= mng.create("Series","onsetseries");
  onsetseries->addMarSystem(mng.create("SoundFileSource", "src"));
  onsetseries->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]
  //onsetseries->addMarSystem(mng.create("ShiftInput", "si"));
  //onsetseries->addMarSystem(mng.create("Windowing", "win"));
  MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
  onsetdetector->addMarSystem(mng.create("ShiftInput", "si")); //<---
  onsetdetector->addMarSystem(mng.create("Windowing", "win")); //<---
  onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
  onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  onsetdetector->addMarSystem(mng.create("Flux", "flux"));
  //onsetdetector->addMarSystem(mng.create("Memory","mem"));
  onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
  onsetdetector->addMarSystem(mng.create("Filter","filt1"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
  onsetdetector->addMarSystem(mng.create("Filter","filt2"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
  onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker"));
  onsetseries->addMarSystem(onsetdetector);
  onsetaccum->addMarSystem(onsetseries);
  onsetnet->addMarSystem(onsetaccum);
  //onsetnet->addMarSystem(mng.create("ShiftOutput","so"));

  if(audiosynthopt)
  {
    //Audio synthesis
    MarSystem* onsetmix = mng.create("Fanout","onsetmix");
    onsetmix->addMarSystem(mng.create("Gain","gainaudio"));
    MarSystem* onsetsynth = mng.create("Series","onsetsynth");
    onsetsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
    onsetsynth->addMarSystem(mng.create("ADSR","env"));
    onsetsynth->addMarSystem(mng.create("Gain", "gainonsets"));
    onsetmix->addMarSystem(onsetsynth);
    onsetnet->addMarSystem(onsetmix);

    //onsetnet->addMarSystem(mng.create("AudioSink", "dest"));
    onsetnet->addMarSystem(mng.create("SoundFileSink", "fdest"));
  }


  ///////////////////////////////////////////////////////////////////////////////////////
  //link controls
  ///////////////////////////////////////////////////////////////////////////////////////
  onsetnet->linkControl("mrs_bool/hasData",
                        "Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_bool/hasData");
  //onsetnet->linkControl("ShiftOutput/so/mrs_natural/Interpolation","mrs_natural/inSamples");
  onsetnet->linkControl("Accumulator/onsetaccum/mrs_bool/flush",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");
  if (confidenceopt) {
    onsetnet->linkControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain",
                          "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/confidence");
  }

  //onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Memory/mem/mrs_bool/reset",
  //	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

  //link FILTERS coeffs
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/ncoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs");
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/dcoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs");

  ///////////////////////////////////////////////////////////////////////////////////////
  // update controls
  ///////////////////////////////////////////////////////////////////////////////////////
  FileName outputFile(sfName);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_string/filename", sfName);

  if(audiosynthopt)
    onsetnet->updControl("SoundFileSink/fdest/mrs_string/filename", outputFile.nameNoExt() + "_onsets.wav");

  mrs_real fs = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();

  mrs_natural winSize = 2048;//2048;
  mrs_natural hopSize = 512;//411;


  // mrs_natural winSize = 4096;//2048;
  // mrs_natural hopSize = 1024;//411;

  mrs_natural lookAheadSamples = 6;
  mrs_real thres = thresholdopt;

  mrs_real textureWinMinLen = 0.050; //secs
  mrs_natural minTimes = (mrs_natural) (textureWinMinLen*fs/hopSize); //12;//onsetWinSize+1;//15;
  // cout << "MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
  mrs_real textureWinMaxLen = 60.000; //secs
  mrs_natural maxTimes = (mrs_natural) (textureWinMaxLen*fs/hopSize);//1000; //whatever... just a big number for now...
  // cout << "MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" << endl;

  // best result till now are using dB power Spectrum!
  // FIXME: should fix PowerSpectrum (remove that ugly wrongdBonsets control) and use a Gain with factor of two instead.
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PowerSpectrum/pspk/mrs_string/spectrumType",
                       "wrongdBonsets");

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Flux/flux/mrs_string/mode",
                       "DixonDAFX06");

  //configure zero-phase Butterworth filter of Flux time series (from J.P.Bello TASLP paper)
  // Coefficients taken from MATLAB butter(2, 0.28)
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.1174;
  bcoeffs(1) = 0.2347;
  bcoeffs(2) = 0.1174;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs",
                       bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0;
  acoeffs(1) = -0.8252;
  acoeffs(2) = 0.2946;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs",
                       acoeffs);

  onsetnet->updControl("mrs_natural/inSamples", hopSize);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/si/mrs_natural/winSize", winSize);

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/threshold", thres); //!!!

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);

  mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize)/hopSize/2.0));
  // cout << "timesToKeep = " << winds << endl;
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/timesToKeep", winds);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_string/mode","explicitFlush");
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/maxTimes", maxTimes);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/minTimes", minTimes);

  if(audiosynthopt)
  {
    //set audio/onset resynth balance and ADSR params for onset sound
    onsetnet->updControl("Fanout/onsetmix/Gain/gainaudio/mrs_real/gain", 1.0);
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain", 0.8);
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTarget", 1.0);
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTime", winSize/80/fs); //!!!
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/susLevel", 0.0);
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/dTime", winSize/4/fs); //!!!

    //onsetnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  }


  //MATLAB Engine inits
  //used for toy_with_onsets.m
  MATLAB_EVAL("clear;");
  MATLAB_PUT(winSize, "winSize");
  MATLAB_PUT(hopSize, "hopSize");
  MATLAB_PUT(lookAheadSamples, "lookAheadSamples");
  MATLAB_EVAL("srcAudio = [];");
  MATLAB_EVAL("onsetAudio = [];");
  MATLAB_EVAL("FluxTS = [];");
  MATLAB_EVAL("segmentData = [];");
  MATLAB_EVAL("onsetTS = [];");

  ///////////////////////////////////////////////////////////////////////////////////////
  //process input file (till EOF)
  ///////////////////////////////////////////////////////////////////////////////////////
  mrs_natural timestamps_samples = 0;
  cout << "Sampling rate = " << fs << endl;

  if(audiosynthopt)
  {
    while(onsetnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
    {
      onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/nton", 1.0); //note on
      onsetnet->tick();
      timestamps_samples += onsetnet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
      cout << timestamps_samples / fs << endl; //in seconds
      //cout << timestamps_samples << endl; //in samples
      onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/ntoff", 0.0); //note off
    }
  }
  else {
    ofstream outFile;
    string outFileName = outputFile.nameNoExt() + ".output";
    outFile.open(outFileName.c_str(), ios::out);

    while(onsetnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
    {
      onsetnet->tick();
      timestamps_samples += onsetnet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
      if (confidenceopt) {
        mrs_real confidence = onsetnet->getctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/confidence")->to<mrs_real>();
        if (confidence > thresholdopt) {
          cout << timestamps_samples / fs; //in seconds
          cout << "\t";
          cout << timestamps_samples / fs;
          cout << "\t" << confidence << endl;
        }
      } else {
        cout << timestamps_samples / fs << endl; //in seconds
      }
      outFile << timestamps_samples / fs << "\t";
      outFile << (timestamps_samples / fs)+1 << "\t";
      outFile << "w" << endl;

    }

    cout << "Done writing " << outFileName << endl;
    outFile.close();
  }
  delete onsetnet;
}

int
main(int argc, const char **argv)
{
  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  if (helpopt) {
    return printHelp(progName);
  }

  vector<string> soundfiles = cmd_options.getRemaining();

  string fname0 = EMPTYSTRING;
  //string fname1 = EMPTYSTRING;

  if (soundfiles.size() > 0)
    fname0 = soundfiles[0];
  //if (soundfiles.size() > 1)
  //	fname1 = soundfiles[1];

  cout << "Marsyas onset detection" << endl;
  cout << "fname0 = " << fname0 << endl;
  //cout << "fname1 = " << fname1 << endl;

  if (soundfiles.size() > 0) {
    detect_onsets(fname0);
  }

  return 0;
}
