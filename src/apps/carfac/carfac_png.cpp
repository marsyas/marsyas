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


//
// carfacpng
//
// A program that uses Marsyas to generate a PNG of an input audio
// file.  The PNG can be either the waveform or the spectrogram of the
// audio file.
//
// written by sness (c) 2009 - GPL - sness@sness.net
//

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

#ifdef MARSYAS_PNG
#include "pngwriter.h"
#endif

#include <vector>
#include <iomanip>

using namespace std;
using namespace Marsyas;

//
// Global variables for various commandline options
//
mrs_natural helpopt_;
mrs_natural usageopt_;
mrs_natural verboseopt_;
mrs_natural windowSize_;
mrs_natural hopSize_;
mrs_natural memorySize_;
mrs_real gain_;
mrs_natural highFreq_;
mrs_natural lowFreq_;

mrs_natural position_;
mrs_natural ticks_;
mrs_string mode_;
mrs_real start_, length_;
mrs_natural width_, height_;
mrs_string fontfile_;




CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("carfacpng.cpp - printUsage");
  cerr << "Usage : " << progName << " in.wav out.png" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.png is the name of the PNG file to be generated" << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("carfacpng.cpp - printHelp");
  cerr << "carfacpng" << endl;
  cerr << "-------------------------------------------------------------" << endl;
  cerr << "Generate a PNG of an input audio file.  The PNG can either be" << endl;
  cerr << "the waveform or the spectrogram of the audio file" << endl;
  cerr << endl;
  cerr << "written by sness (c) 2010 GPL - sness@sness.net" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " in.wav [out.png]" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.png is the optional name of the PNG file to be generated" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage        : display short usage info" << endl;
  cerr << "-h --help         : display this information " << endl;
  cerr << "-v --verbose      : verbose output" << endl;
  cerr << "------------------------------------------" << endl;

  cerr << "-m --mode         : mode (waveform, spectrogram, json, html, neptune)" << endl;
  cerr << "-ws --windowsize  : windows size in samples " << endl;
  cerr << "-hs --hopsize     : hop size in samples (for spectrogram)" << endl;
  cerr << "-ms --memorysize  : memory size in samples (for correlogram)" << endl;
  cerr << "-g --gain         : gain for spectrogram (for spectrogram)" << endl;
  cerr << "-mnf --maxfreq    : maximum frequency (for spectrogram)" << endl;
  cerr << "-mxf --minfreq    : minimum frequency (for spectrogram)" << endl;
  cerr << "-p --position     : position to start at in the audio file" << endl;
  cerr << "-t --ticks        : how many times to tick the network" << endl;
  cerr << "-s --start        : start in seconds " << endl;
  cerr << "-l --length       : length in seconds " << endl;
  cerr << "-wd --width        : width of resulting png in pixels " << endl;
  cerr << "-hg --height       : height of resulting png in pixels " << endl;
  cerr << "-ff --fontfile     : true type font file .ttf " << endl;

  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addNaturalOption("windowsize", "ws", 512);
  cmd_options.addNaturalOption("hopsize", "hs", 256);
  cmd_options.addNaturalOption("memorysize", "ms", 300);
  cmd_options.addRealOption("gain", "g", 1.5);
  cmd_options.addNaturalOption("maxfreq", "mxf", 22050);
  cmd_options.addNaturalOption("minfreq", "mnf", 0);
  cmd_options.addNaturalOption("ticks", "t", -1);
  cmd_options.addNaturalOption("position", "p", 0);
  cmd_options.addStringOption("mode" , "m", "spectrogram");
  cmd_options.addRealOption("start", "s", 0.0);
  cmd_options.addRealOption("length", "l", -1.0);
  cmd_options.addNaturalOption("width", "wd", -1);
  cmd_options.addNaturalOption("height", "hg", -1);
}


void
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  verboseopt_ = cmd_options.getBoolOption("verbose");
  windowSize_ = cmd_options.getNaturalOption("windowsize");
  memorySize_ = cmd_options.getNaturalOption("memorysize");
  hopSize_ = cmd_options.getNaturalOption("hopsize");
  gain_ = cmd_options.getRealOption("gain");
  highFreq_ = cmd_options.getNaturalOption("maxfreq");
  lowFreq_ = cmd_options.getNaturalOption("minfreq");
  position_ = cmd_options.getNaturalOption("position");
  ticks_ = cmd_options.getNaturalOption("ticks");
  mode_ = cmd_options.getStringOption("mode");
  start_ = cmd_options.getRealOption("start");
  length_ = cmd_options.getRealOption("length");
  width_ = cmd_options.getNaturalOption("width");
  height_ = cmd_options.getNaturalOption("height");
  fontfile_ = cmd_options.getStringOption("fontfile");
}


int getFileLengthForSummaryITD(string inFileName, double& min, double& max, double& average) {

  realvec processedData;
  double dataLength = 0;
  double dataTotal = 0.0;

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);

  mrs_real srate = net->getControl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  if ((position_ == 0) && (start_ != 0.0))
    position_ = (mrs_natural) (srate * start_);

  if ((ticks_ == -1) && (length_ != -1.0))
    ticks_ = (mrs_natural) ((length_ * srate) / windowSize_);

  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize_);
  net->updControl("ShiftInput/si/mrs_natural/winSize", windowSize_);
  net->updControl("mrs_natural/inSamples", int(hopSize_));


  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1
  mrs_natural nChannels = net->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();

  double maxBin = fftBins * (highFreq_ / (frequency / nChannels));
  double minBin = fftBins * (lowFreq_ / (frequency / nChannels));
  // cout << "maxBin = " << maxBin << endl;

  int length = 0;
  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
          && (ticks_ == -1 || length < ticks_)) {
    net->tick();
    length++;

    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (int i = minBin; i < maxBin; ++i) {
      for (int j = 0; j < processedData.getCols(); j++) {
        if (processedData(i,j) < min)
          min = processedData(i,j);
        if (processedData(i,j) > max)
          max = processedData(i,j);
        dataLength += 1;
        dataTotal += processedData(i,j);
      }
    }
  }

  delete net;

  average = dataTotal / dataLength;

  if (verboseopt_) {
    cout << "length=" << length << endl;
    cout << "max=" << max << endl;
    cout << "min=" << min << endl;
    cout << "average=" << average << endl;
  }

  return length;
}



void outputSummaryITDPNG(string inFileName, string outFileName)
{

  cout << "SummaryITD" << endl;

#ifdef MARSYAS_PNG
  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;
  double average;

  int length = getFileLengthForSummaryITD(inFileName,min,max,average);


  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize_);
  net->updControl("ShiftInput/si/mrs_natural/winSize", windowSize_);
  net->updControl("mrs_natural/inSamples", int(hopSize_));

  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  double pngLength = length;

  mrs_natural nChannels = net->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();

  double pngHeight = fftBins * ((highFreq_-lowFreq_) / (frequency / nChannels));

  if (verboseopt_)
  {
    cout << "highFreq_ = " << highFreq_ << endl;
    cout << "lowFreq_ = " << lowFreq_ << endl;
    cout << "fftBins = " << fftBins << endl;
    cout << "pngLength = " << pngLength << endl;
    cout << "pngHeight = " << pngHeight << endl;
    cout << "width = " << width_ << endl;
    cout << "height = " << height_ << endl;
  }

  pngwriter png(int(pngLength),int(pngHeight),0,outFileName.c_str());

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  double diff;
  double pdiff;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    diff = 0.0;

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);

      normalizedData = ((data - min) / (max - min)) * gain_;


      diff += normalizedData;

      // Make the spectrogram black on white instead of white on black
      // TODO - Add the ability to generate different color maps, like Sonic Visualiser
      colour = 1.0 - normalizedData;
      if (colour > 1.0) {
        colour = 1.0;
      }
      if (colour < 0.0) {
        colour = 0.0;
      }

      y = i;
      png.plot(int(x),int(y),colour,colour,colour);
    }
    /* if (fabs(pdiff-diff) > 4.0)
       for (int i=0; i < 20; i++)
       png.plot(int(x),pngHeight- i, 1.0, 0.0, 0.0);
    */
    pdiff = diff;

    x++;

  }

  // png.plot_text(const_cast<char *>(fontfile_.c_str()), 12, 20, 20, 0.0, "THIS IS A SPECTROGRAM", 1.0, 0.0, 0.0);

  if (width_ !=-1)
    pngLength = width_;
  if (height_ != -1)
    pngHeight = height_;
  if ((width_ !=-1)||(height_ != -1))
  {
    png.scale_wh(pngLength, pngHeight);
  }
  png.close();

  delete net;
#endif
}


int
main(int argc, const char **argv)
{
  MRSDIAG("carfacpng.cpp - main");

  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> files = cmd_options.getRemaining();
  if (helpopt_)
    printHelp(progName);

  if (usageopt_)
    printUsage(progName);


  if (files.size() != 2) {
    cerr << "You must specify two files on the command line." << endl;
    cerr << "One for the input audio file and one for the output PNG file" << endl;
    printUsage(progName);
  }

#ifdef MARSYAS_PNG
  if (mode_ == "summaryitd") {
    outputSummaryITDPNG(files[0],files[1]);
  }
  exit(0);
#else
  cout << "carfacpng requires Marsyas to be compiled with the WITH_PNG option" << endl;
  exit(0);
#endif

}
