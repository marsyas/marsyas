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
#include <marsyas/common_source.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Conversions.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/peakView.h>
#include <marsyas/FileName.h>

#include <marsyas/PeUtilities.h> //TO DEPRECATE IN NEAR FUTURE!!! [TODO]

#include <string>

using namespace std;
using namespace Marsyas;

string pluginName = "MARSYAS_EMPTY";
string inputDirectoryName = "MARSYAS_EMPTY";
string outputDirectoryName = "MARSYAS_EMPTY";
string fileName = "MARSYAS_EMPTY";
string noiseName = "MARSYAS_EMPTY";
string fileResName = "MARSYAS_EMPTY";
string filePeakName = "MARSYAS_EMPTY";
string panningInfo = "MARSYAS_EMPTY";
string intervalFrequency = "50-4000";
string harmonizeFileName = "MARSYAS_EMPTY";

// Global variables for command-line options
bool helpopt_ = 0;
bool usageopt_ =0;
bool peakStore_=0;
int fftSize_ = 2048;
int winSize_ = 2048;
// if kept the same no time expansion
int hopSize_ = 512;
// nb Sines
int nbSines_ = 20;
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
mrs_real samplingFrequency_=1;

bool microphone_ = false;
bool analyse_ = false;
mrs_natural synthetize_ = 0;//-1;

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("WHaSp_main.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
}

void
printHelp(string progName)
{
  MRSDIAG("WHaSp_main.cpp - printHelp");
  cerr << "WHaSp, MARSYAS" << endl;
  cerr << "report bugs to lmartins@inescporto.pt" << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-s --sinusoids       : number of sinusoids per frame" << endl;
  cerr << "-b --buffersize      : audio buffer size" << endl;
  cerr << "-i --intervalFrequency: select peaks in this interval" << endl;
  cerr << "-v --voices          : number of voices" << endl;
  cerr << "-f --filename        : output to file " << endl;
  cerr << "-A --analyse         : peak analyse" << endl;
  cerr << "-S --synthetise : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
  cerr << "-P --PeakStore : set peak store" << endl;
  cerr << "-I --inputdirectoryname   : input directory path" << endl;
  cerr << "-o --outputdirectoryname   : output directory path" << endl;
  cerr << "-p --panning : panning informations <foreground level (0..1)>-<foreground pan (-1..1)>-<background level>-<background pan> " << endl;
  cerr << "-S --synthesize : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
  cerr << "-H --harmonize : change the frequency according to the file provided" << endl;
  cerr << "" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;
  exit(1);
}

void
WHaSp(string sfName, string outsfname, mrs_natural N, mrs_natural Nw,
      mrs_natural D, mrs_natural S, mrs_natural synthetize)
{
  mrs_natural nbFrames_=0, harmonize_=0;
  realvec harmonizeData_;
  MarControlPtr ctrl_harmonize_;

  MarSystemManager mng;

  // create the main network
  MarSystem* pvseries = mng.create("Series", "pvseries");

  if(analyse_)
  {
    // add original source
    if (microphone_)
      pvseries->addMarSystem(mng.create("AudioSource", "src"));
    else
      pvseries->addMarSystem(mng.create("SoundFileSource", "src"));

    //create analyzer (using composite prototype)
    pvseries->addMarSystem(mng.create("WHaSpnet", "whaspnet"));
    pvseries->linkControl("mrs_natural/frameMaxNumPeaks","WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_natural/frameMaxNumPeaks");

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
      // 			pvseries->linkControl("HWPSspectrumnet/WHASP/mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");  //!!!!!!! [!]
      // 			pvseries->linkControl("HWPSspectrumnet/WHASP/mrs_string/filename", "SoundFileSource/src/mrs_string/filename"); //!!!!!!!!! [!]
      // 			pvseries->linkControl("HWPSspectrumnet/WHASP/mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData"); //!!!!!!!! [!]

      pvseries->updControl("SoundFileSource/src/mrs_string/filename", sfName);
      pvseries->updControl("mrs_natural/inSamples", D);
      pvseries->updControl("mrs_natural/inObservations", 1);
      samplingFrequency_ = pvseries->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
    }


    //pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_natural/Decimation", D);
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_natural/winSize", Nw+1);
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_natural/FFTSize", N);
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_string/WindowType", "Hanning");
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_bool/zeroPhasing", true);
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/Shifter/sh/mrs_natural/shift", 1);
    //pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/mrs_natural/Decimation", D);
    pvseries->updControl("mrs_natural/frameMaxNumPeaks", S);
    pvseries->updControl("WHaSpnet/whaspnet/PeakAnalyse/analyse/PeakConvert/conv/mrs_natural/nbFramesSkipped", (N/D));
  }
  else
  {
    // create realvecSource
    MarSystem *peSource = mng.create("RealvecSource", "peSource");
    pvseries->addMarSystem(peSource);
    /*

      mrs_natural nbF_=0;
      realvec peakSet_;
      peakSet_.read(sfName);

      MATLAB_PUT(peakSet_, "peaks");
      MATLAB_EVAL("plotPeaks(peaks)");

      for (mrs_natural i=0 ; i<peakSet_.getRows() ; ++i)
      if(peakSet_(i, pkFrame)>nbF_)
      {
      nbF_ = peakSet_(i, pkFrame);
      }
      nbF_++;

      realvec peakSetV_(nbSines_*nbPkParameters, nbF_);
      peakSetV_.setval(0);
      peaks2V(peakSet_, peakSetV_, peakSetV_, nbSines_);

    */
    realvec peakSet_;

    //peakLoad(peakSet_, sfName, fs, S, nbFrames, D);
    peakView peakSetView(peakSet_);
    peakSetView.peakRead(sfName);
    pvseries->setctrl("RealvecSource/peSource/mrs_realvec/data", peakSet_);
    pvseries->setctrl("mrs_real/israte", peakSetView.getFs());
  }

  if(peakStore_)
  {
    // realvec sink to store peaks
    MarSystem *peSink = mng.create("RealvecSink", "peSink");
    pvseries->addMarSystem(peSink);
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
    cout << "synth " << outsfname << endl ;
    synthNetCreate(&mng, outsfname, microphone_, synthetize); //[TODO]
    MarSystem *peSynth = mng.create("PeSynthetize", "synthNet");
    pvseries->addMarSystem(peSynth);
    synthNetConfigure (pvseries, sfName, outsfname, fileResName, panningInfo, 1, Nw, D, S, 1, microphone_, synthetize, bopt_, -D, samplingFrequency_); //[TODO]
  }

  if(harmonize_)
  {
    ctrl_harmonize_= pvseries->getctrl("PeSynthetize/synthNet/Series/postNet/PeakSynthOscBank/pso/mrs_realvec/harmonize");
    MarControlAccessor acc(ctrl_harmonize_);
    realvec& harmonize = acc.to<mrs_realvec>();
    harmonize.stretch(harmonizeData_.getCols());
  }

  if(analyse_ || synthetize > -1)
    while(1)
    {
      pvseries->tick();

      nbFrames_++;
      if (!microphone_)
      {
        bool temp;
        if(analyse_)
        {
          temp = pvseries->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();
          mrs_real timeRead =  pvseries->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>()/samplingFrequency_;
          mrs_real timeLeft =  pvseries->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>()/samplingFrequency_;
          printf("%.2f / %.2f \r", timeRead, timeLeft);
        }
        else
          temp =	!pvseries->getctrl("RealvecSource/peSource/mrs_bool/done")->to<mrs_bool>();

        ///*bool*/ temp = pvseries->getctrl("PeakAnalyse/peA/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();
        if (temp == false)
          break;
      }
    }

  if(peakStore_)
  {
    realvec vec = pvseries->getctrl("RealvecSink/peSink/mrs_realvec/data")->to<mrs_realvec>();
    //peakStore(vec, filePeakName, samplingFrequency_, D);
    peakView vecView(vec);
    vecView.peakWrite(filePeakName, samplingFrequency_, D);


    MATLAB_PUT(peakSet_, "peaks");
    MATLAB_EVAL("plotPeaks(peaks)");

    realvec realTry(nbFrames_, 5);
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
  cmd_options.addStringOption("filename", "f", "MARSYAS_EMPTY");
  cmd_options.addStringOption("outputdirectoryname", "o", "MARSYAS_EMPTY");
  cmd_options.addStringOption("inputdirectoryname", "I", "MARSYAS_EMPTY");
  cmd_options.addNaturalOption("winsize", "w", winSize_);
  cmd_options.addNaturalOption("fftsize", "n", fftSize_);
  cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
  cmd_options.addNaturalOption("bufferSize", "b", bopt_);
  cmd_options.addStringOption("intervalFrequency", "i", intervalFrequency);
  cmd_options.addStringOption("panning", "p", "MARSYAS_EMPTY");
  cmd_options.addStringOption("Harmonize", "H", "MARSYAS_EMPTY");
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
  MRSDIAG("WHaSp_main.cpp - main");

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

  cerr << "WHaSp configuration (-h show the options): " << endl;
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
      if(outputDirectoryName != "MARSYAS_EMPTY")
        path = outputDirectoryName;
      else
        path =Sfname.path();

      if (fileName == "MARSYAS_EMPTY")
      {
        fileName = path + "/" + Sfname.nameNoExt() + "_WHaSp_Syn.wav" ;
      }
      filePeakName = path + "/" + Sfname.nameNoExt() + ".peak" ;
      cout << "WHaSp " << Sfname.name() << endl;

      WHaSp(*sfi, fileName, fftSize_, winSize_, hopSize_, nbSines_, synthetize_);
    }
  }
  else
  {
    cout << "Use of live microphone input not yet tested... exiting!" << endl;
    //cout << "Using live microphone input" << endl;
    //microphone_ = true;
    //WHaSp("microphone", fileName, fftSize_, winSize_, hopSize_, nbSines_, synthetize_);
  }

  exit(0);
}

