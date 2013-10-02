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

#include <cstdio>
#include <cstdlib>

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Conversions.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>
#include <marsyas/common_source.h>

//[TODO] get rid of this messy include...
#include <marsyas/PeUtilities.h>

#include <string>

using namespace std;
using namespace Marsyas;

string pluginName = EMPTYSTRING;
string inputDirectoryName = EMPTYSTRING;
string outputDirectoryName = EMPTYSTRING;
string fileName = EMPTYSTRING;
string noiseName = EMPTYSTRING;
string fileResName = EMPTYSTRING;
string filePeakName = EMPTYSTRING;
string panningInfo = EMPTYSTRING;
string intervalFrequency = "50-4000";
string harmonizeFileName = EMPTYSTRING;

// Global variables for command-line options
bool helpopt_ = 0;
bool usageopt_ =0;
bool peakStore_=0;
int fftSize_ = 2048;
int winSize_ = 2048;
// if kept the same no time expansion
int hopSize_ = 512;
// nb Sines
int nbSines_ = 50;
// nbClusters
int nbClusters_ = 3;
// output buffer Size
int bopt_ = 128;
// output gain
mrs_real gopt_ = 1.0;
// number of accumulated frames
mrs_natural accSize_ = 1;
// type of similarity Metrics
string similarityType_ = "ho";
// store for clustered peaks
realvec peakSet_;
// delay for noise insertion
mrs_real noiseDelay_=0;
// gain for noise insertion
mrs_real noiseGain_=.1;
// duration for noise insertion
mrs_real noiseDuration_=0;
// sampling frequency
mrs_real samplingFrequency_=44100;

bool microphone_ = false;
bool analyse_ = false;
mrs_natural synthetize_ = -1;

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("peakClustering.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
}

void
printHelp(string progName)
{
  MRSDIAG("peakSynth.cpp - printHelp");
  cerr << "peakSynth, MARSYAS, Copyright Mathieu Lagrange " << endl;
  cerr << "report bugs to lagrange@uvic.ca" << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-s --sinusoids       : number of sinusoids per frame" << endl;
  cerr << "-b --buffersize      : audio buffer size" << endl;
  cerr << "-v --voices          : number of voices" << endl;
  cerr << "-f --filename        : output filename" << endl;
  cerr << "-I --inputdirectoryname : set input directory" << endl;
  cerr << "-i --intervalFrequency : interval frequency" << endl;
  cerr << "-A --analyse         : peak analyse" << endl;
  cerr << "-P --PeakStore       : set peak store" << endl;
  cerr << "-o --outputdirectoryname   : output directory path" << endl;
  cerr << "-p --panning : panning informations <foreground level (0..1)>-<foreground pan (-1..1)>-<background level>-<background pan> " << endl;
  cerr << "-S --synthetise : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
  cerr << "-H --harmonize : change the frequency accoring to the file provided" << endl;
  cerr << "" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;


  exit(1);
}


// original monophonic PeVocoding
void
peVocode(string sfName, string outsfname, mrs_natural N, mrs_natural Nw,
         mrs_natural D, mrs_natural S, mrs_natural synthetize)
{
  mrs_natural nbFrames_=0, harmonize_=0;
  realvec harmonizeData_;
  MarControlPtr ctrl_harmonize_;
  cout << "Extracting Peaks and Clusters" << endl;
  MarSystemManager mng;

  // create the phasevocoder network
  MarSystem* pvseries = mng.create("Series", "pvseries");

  if(analyse_)
  {
    // add original source
    if (microphone_)
      pvseries->addMarSystem(mng.create("AudioSource", "src"));
    else
      pvseries->addMarSystem(mng.create("SoundFileSource", "src"));
    // create analyser
    pvseries->addMarSystem(mng.create("PeakAnalyse", "peA"));

    ////////////////////////////////////////////////////////////////
    // update the controls
    ////////////////////////////////////////////////////////////////
    if (microphone_)
    {
      pvseries->updControl("mrs_natural/inSamples", D);
      pvseries->updControl("mrs_natural/inObservations", 1);
    }
    else
    {
      pvseries->updControl("SoundFileSource/src/mrs_string/filename", sfName);
      pvseries->updControl("mrs_natural/inSamples", D);
      pvseries->updControl("mrs_natural/inObservations", 1);
      samplingFrequency_ = pvseries->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
      cout << "SAMPLING FREQ = " << samplingFrequency_ << endl;

    }

    pvseries->updControl("PeakAnalyse/peA/ShiftInput/si/mrs_natural/winSize", Nw+1);
    pvseries->updControl("PeakAnalyse/peA/Windowing/wi/mrs_natural/size", N);
    pvseries->updControl("PeakAnalyse/peA/Windowing/wi/mrs_string/type", "Hanning");
    pvseries->updControl("PeakAnalyse/peA/Windowing/wi/mrs_bool/zeroPhasing", true);
    pvseries->updControl("PeakAnalyse/peA/Shifter/sh/mrs_natural/shift", 1);
    pvseries->updControl("PeakAnalyse/peA/PvFold/fo/mrs_natural/Decimation", D); // useless ?
    pvseries->updControl("PeakAnalyse/peA/PeakConvert/conv/mrs_natural/Decimation", D);
    pvseries->updControl("PeakAnalyse/peA/PeakConvert/conv/mrs_natural/frameMaxNumPeaks", S);
    pvseries->updControl("PeakAnalyse/peA/PeakConvert/conv/mrs_natural/nbFramesSkipped", (N/D));
  }
  else //read analysis data (i.e. peaks) from a .peak file
  {
    // create PeakViewSource
    MarSystem *peSource = mng.create("PeakViewSource", "peSource");
    pvseries->addMarSystem(peSource);
    peSource->updControl("mrs_string/filename", sfName);
  }

  if(peakStore_)
  {
    pvseries->addMarSystem(mng.create("peakViewSink", "peSink"));
    pvseries->updControl("PeakViewSink/peSink/mrs_string/filename", filePeakName);
  }

  if(harmonizeFileName != "MARSYAS_EMPTY")
  {
    harmonizeData_.read(harmonizeFileName);
    if(!harmonizeData_.getSize())
      cout << "Unable to open "<< harmonizeFileName << endl;
    harmonize_=1;

    // ctrl_harmonize_->setValue(0, 0.);
    synthetize = 3;
  }

  if(synthetize>-1 )
  {
    synthNetCreate(&mng, outsfname, microphone_, synthetize);
    MarSystem *peSynth = mng.create("PeSynthetize", "synthNet");
    pvseries->addMarSystem(peSynth);
    cout << Nw << " " << D << " " << Nw+1-D << endl;

    synthNetConfigure (pvseries, sfName, outsfname, fileResName, panningInfo, 1, Nw, D, S, 1, microphone_, synthetize, bopt_, -D, samplingFrequency_ );

    cout << "samplingFrequency_ " << samplingFrequency_ << endl;

  }

  if(harmonize_)
  {
    ctrl_harmonize_= pvseries->getctrl("PeSynthetize/synthNet/Series/postNet/PeakSynthOscBank/pso/mrs_realvec/harmonize");
    MarControlAccessor acc(ctrl_harmonize_);
    realvec& harmonize = acc.to<mrs_realvec>();
    harmonize.stretch(harmonizeData_.getCols());
  }

  //	mrs_real time=0;
  if(analyse_ || synthetize > -1)
    while(1)
    {
      pvseries->tick();

      if (harmonize_) //[WTF]
      {
        for (mrs_natural i=0 ; i<harmonizeData_.getCols() ; ++i) //[WTF] ?!?!?!?!?!?!
          //ctrl_harmonize_->setValue(i, 0.0);
          //ctrl_harmonize_->setValue(1, 1.0);
          //ctrl_harmonize_->setValue(2, 0.1);
          if (harmonizeData_.getRows() > nbFrames_)
            ctrl_harmonize_->setValue(i, harmonizeData_(nbFrames_, i) != 0);
          else
          {
            ctrl_harmonize_->setValue(i, 0);
            cout << "Harmonize file too short" << endl;
          }
      }
      nbFrames_++;
      if (!microphone_)
      {
        bool temp;
        if(analyse_)
        {
          temp = pvseries->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();
          mrs_real timeRead =  pvseries->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>()/samplingFrequency_;
          mrs_real timeLeft =  pvseries->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>()/samplingFrequency_;
          printf("Reading Audio File: %.2f / %.2f \r", timeRead, timeLeft);
        }
        else
        {
          temp =	pvseries->getctrl("PeakViewSource/peSource/mrs_bool/hasData")->to<mrs_bool>();
          samplingFrequency_ = pvseries->getctrl("PeakViewSource/peSource/mrs_real/osrate")->to<mrs_real>();
          mrs_real timeRead =  pvseries->getctrl("PeakViewSource/peSource/mrs_natural/pos")->to<mrs_natural>()/samplingFrequency_;
          mrs_real timeLeft =  pvseries->getctrl("PeakViewSource/peSource/mrs_natural/size")->to<mrs_natural>()/samplingFrequency_;
          printf("Readinf peak file: %.2f / %.2f \r", timeRead, timeLeft);
        }

        if (temp == false)
          break;
      }
    }

  if(peakStore_)
  {
    pvseries->updControl("PeakViewSink/peSink/mrs_real/fs", samplingFrequency_);
    pvseries->updControl("PeakViewSink/peSink/mrs_natural/frameSize", D);
    pvseries->updControl("PeakViewSink/peSink/mrs_string/filename", filePeakName);
    pvseries->updControl("PeakViewSink/peSink/mrs_bool/done", true);

    //MATLAB_PUT(peakSet_, "peaks");
    //MATLAB_EVAL("plotPeaks(peaks)");

    realvec realTry(nbFrames_, 5); //[WTF]
    realTry.setval(0);
    for (mrs_natural i=0 ; i<nbFrames_ ; ++i)
    {
      realTry(i, 1) = 20;
      realTry(i, 2) = .8;
      realTry(i, 3) = .25;
      realTry(i, 4) = .6;
    }
    ofstream tryFile;
    string harmonizeName = filePeakName+"HarmoStream";
    tryFile.open(harmonizeName.c_str());
    tryFile<< realTry;
  }
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addNaturalOption("voices", "v", 1);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addStringOption("outputdirectoryname", "o", EMPTYSTRING);
  cmd_options.addStringOption("inputdirectoryname", "I", EMPTYSTRING);
  cmd_options.addNaturalOption("winsize", "w", winSize_);
  cmd_options.addNaturalOption("fftsize", "n", fftSize_);
  cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
  cmd_options.addNaturalOption("bufferSize", "b", bopt_);
  cmd_options.addStringOption("intervalFrequency", "i", intervalFrequency);
  cmd_options.addStringOption("panning", "p", EMPTYSTRING);
  cmd_options.addStringOption("Harmonize", "H", EMPTYSTRING);
  cmd_options.addNaturalOption("synthetize", "S", synthetize_);
  cmd_options.addBoolOption("analyse", "A", analyse_);
  cmd_options.addBoolOption("PeakStore", "P", peakStore_);
}


void
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  fileName   = cmd_options.getStringOption("filename");
  inputDirectoryName = cmd_options.getStringOption("inputdirectoryname");
  outputDirectoryName = cmd_options.getStringOption("outputdirectoryname");
  winSize_ = cmd_options.getNaturalOption("winsize");
  fftSize_ = cmd_options.getNaturalOption("fftsize");
  nbSines_ = cmd_options.getNaturalOption("sinusoids");
  bopt_ = cmd_options.getNaturalOption("bufferSize");
  intervalFrequency = cmd_options.getStringOption("intervalFrequency");
  panningInfo = cmd_options.getStringOption("panning");
  harmonizeFileName = cmd_options.getStringOption("Harmonize");
  synthetize_ = cmd_options.getNaturalOption("synthetize");
  analyse_ = cmd_options.getBoolOption("analyse");
  peakStore_ = cmd_options.getBoolOption("PeakStore");
}

int
main(int argc, const char **argv)
{
  MRSDIAG("sftransform.cpp - main");

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

  cerr << "peakSynth configuration (-h show the options): " << endl;
  cerr << "fft size (-n)      = " << fftSize_ << endl;
  cerr << "win size (-w)      = " << winSize_ << endl;
  cerr << "sinusoids (-s)     = " << nbSines_ << endl;
  cerr << "outFile  (-f)      = " << fileName << endl;
  cerr << "outputDirectory  (-o) = " << outputDirectoryName << endl;
  cerr << "inputDirectory  (-i) = " << inputDirectoryName << endl;

  // soundfile input
  string sfname;
  if (soundfiles.size() != 0)
  {
    // process several soundFiles
    for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; ++sfi)
    {
      FileName Sfname(*sfi);
      /*	if(outputDirectoryName == EMPTYSTRING)
      	{
      	outputDirectoryName = ".";
      	}*/

      if(Sfname.ext() == "peak")
      {
        analyse_ = 0;
        if(synthetize_ == -1)
          synthetize_ = 0;
        peakStore_=0;
      }
      if(Sfname.ext() == "wav")
      {
        analyse_ = 1;
      }

      string path;
      if(outputDirectoryName != EMPTYSTRING)
        path = outputDirectoryName;
      else
        path =Sfname.path();

      fileName = path + "/" + Sfname.nameNoExt() + "Syn.wav" ;
      filePeakName = path + "/" + Sfname.nameNoExt() + ".peak" ;
      cout << "Pevocoding " << Sfname.name() << endl;

      peVocode(*sfi, fileName, fftSize_, winSize_, hopSize_, nbSines_, synthetize_);
    }
  }
  else
  {
    cout << "Using live microphone input" << endl;
    microphone_ = true;
    peVocode("microphone", fileName, fftSize_, winSize_, hopSize_, nbSines_, synthetize_);
  }

  exit(0);
}


