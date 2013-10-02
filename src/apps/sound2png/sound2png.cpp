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
// sound2png
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
  MRSDIAG("sound2png.cpp - printUsage");
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
  MRSDIAG("sound2png.cpp - printHelp");
  cerr << "sound2png" << endl;
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
  cmd_options.addStringOption("fontfile", "ff", "FreeMonoBold.ttf");

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


int getFileLengthForWaveform(string inFileName, int windowSize_, double& min, double& max) {

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");

  // The sound file
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);


  mrs_real srate = net->getControl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  if ((position_ == 0) && (start_ != 0.0))
    position_ = (mrs_natural) (srate * start_);

  if ((ticks_ == -1) && (length_ != -1.0))
    ticks_ = (mrs_natural) ((length_ * srate) / windowSize_);

  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize_);

  // Compute the AbsMax of this window
  net->addMarSystem(mng.create("AbsMax","absmax"));

  realvec processedData;

  int length = 0;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || length < ticks_))  {
    net->tick();
    length++;

    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    if (processedData(0) < min)
      min = processedData(0);
    if (processedData(0) > max)
      max = processedData(0);
  }


  delete net;

  if (verboseopt_) {
    cout << "length=" << length << endl;
    cout << "max=" << max << endl;
    cout << "min=" << min << endl;
  }

  return length;
}

void outputWaveformPNG(string inFileName, string outFileName)
{
#ifdef MARSYAS_PNG
  int length;
  int height = 150;
  int middle_right;
  int middle_left;

  double min = 99999999999.9;
  double max = -99999999999.9;

  length = getFileLengthForWaveform(inFileName,windowSize_,min,max);
  double pngLength = length;
  double pngHeight = height;



  pngwriter png(length,height,0,outFileName.c_str());

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "net");

  // The sound file
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->setctrl("mrs_natural/inSamples", windowSize_);
  net->addMarSystem(mng.create("MaxMin","maxmin"));

  mrs_natural channels = net->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  if (verboseopt_) {
    cout << "channels=" << channels << endl;
  }

  if (channels == 2) {
    middle_right = (height/4);
    middle_left = (height/2)+(height/4);
  } else {
    middle_left = (height/2);
  }

  realvec processedData;

  // Give it a white background
  png.invert();

  // A line across the middle of the plot
  png.line(0,middle_left,length,middle_left,0,0,0);
  if (channels == 2) {
    png.line(0,middle_right,length,middle_right,0,0,0);
  }

  double x = 0;

  double y_max_right = 0;
  double y_min_right = 0;
  double y_max_right_prev = 0;
  double y_min_right_prev = 0;

  double y_max_left = 0;
  double y_min_left = 0;
  double y_max_left_prev = 0;
  double y_min_left_prev = 0;

  double draw_color;

  // If we are just displaying individual samples, make the line dark blue.
  if (windowSize_ == 1) {
    draw_color = 0.0;
  } else {
    draw_color = 0.2;
  }

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    if (channels == 2) {
      y_max_right = processedData(1,0) / 2.0 * height;
      y_min_right = processedData(1,1) / 2.0 * height;
    }

    y_max_left = processedData(0,0) / 2.0 * height;
    y_min_left = processedData(0,1) / 2.0 * height;

    //
    // Draw the right waveform
    //
    if (channels == 2) {
      // Draw a line from the maximum to the minimum value
      png.line(x,middle_right+y_min_right,x,middle_right+y_max_right,0.0,0.0,1.0);

      // Shade the middle part of the line lighter blue
      double right_height = (y_max_right - y_min_right) / 4.0;
      png.line(x,(middle_right+y_min_right)+right_height,x,(middle_right+y_max_right)-right_height,0.5,0.5,1.0);

      // Fill in any missing segments with light blue
      if (y_min_right_prev > y_max_right) {
        png.line(x,middle_right+y_min_right_prev,x,middle_right+y_max_right,draw_color,draw_color,1.0);
      } else if (y_max_right_prev < y_min_right) {
        png.line(x,middle_right+y_max_right_prev,x,middle_right+y_min_right,draw_color,draw_color,1.0);
      }
    }

    //
    // Draw the left waveform
    //
    // Draw a line from the maximum to the minimum value
    png.line(x,middle_left+y_min_left,x,middle_left+y_max_left,0.0,0.0,1.0);

    // Shade the middle part of the line lighter blue
    double left_height = (y_max_left - y_min_left) / 4.0;
    png.line(x,(middle_left+y_min_left)+left_height,x,(middle_left+y_max_left)-left_height,0.5,0.5,1.0);

    // Fill in any missing segments with light blue
    if (y_min_left_prev > y_max_left) {
      png.line(x,middle_left+y_min_left_prev,x,middle_left+y_max_left,draw_color,draw_color,1.0);
    } else if (y_max_left_prev < y_min_left) {
      png.line(x,middle_left+y_max_left_prev,x,middle_left+y_min_left,draw_color,draw_color,1.0);
    }


    y_max_right_prev = y_max_right;
    y_min_right_prev = y_min_right;
    y_max_left_prev = y_max_left;
    y_min_left_prev = y_min_left;

    x++;

  }

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
#else
  (void) inFileName;
  (void) outFileName;
#endif
}


int getFileLengthForSpectrogram(string inFileName, double& min, double& max, double& average) {

  // cout << "windowSize_ = " << windowSize_ << endl;
  // cout << "hopSize = " << hopSize << endl;


  realvec processedData;
  double dataLength = 0;
  double dataTotal = 0.0;

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
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



void outputSpectrogramPNG(string inFileName, string outFileName)
{

#ifdef MARSYAS_PNG
  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;
  double average;

  int length = getFileLengthForSpectrogram(inFileName,min,max,average);


  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
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

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);

      normalizedData = ((data - min) / (max - min));

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
    x++;
  }


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
#else
  (void) inFileName;
  (void) outFileName;
#endif
}


// variation of spectrogram generation for NEPTUNE
void
neptune_spectrogram(string inFileName, string outFileName)
{
  FileName inFile(inFileName);
  // string outFileName = inFile.nameNoExt() + ".png";
  cout << "Generating spectrogram with specific settings for NEPTUNE, Canada" << endl;
  cout << "Output file is " << outFileName << endl;
  // windowSize_ = 8192;
  // hopSize = 8192;
  // maxFreq = 8000;
  // gain = 64.0;
#ifdef MARSYAS_PNG
  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;

  double average;

  int length = getFileLengthForSpectrogram(inFileName,min,max,average);

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize_);
  net->updControl("ShiftInput/si/mrs_natural/winSize", windowSize_);
  net->updControl("mrs_natural/inSamples", int(hopSize_));

  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();

  mrs_natural nChannels = net->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();



  double pngHeight = fftBins * ((highFreq_-lowFreq_) / (frequency / nChannels));



  double pngLength = length;

  if (verboseopt_)
  {
    cout << "fftBins = " << fftBins << endl;
    cout << "pngLength = " << pngLength << endl;
    cout << "pngHeight = " << pngHeight << endl;
    cout << "gain = " << gain_ << endl;
  }


  pngwriter png(int(pngLength),int(pngHeight),0,outFileName.c_str());

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  double energy;
  double penergy;
  double denergy;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    energy = 0.0;

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);
      normalizedData = ((data - min) / (max - min)) * gain_;

      energy += normalizedData;


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

    // cout << "e1=" << energy << endl;
    // cout << "e2=" << energy / pngHeight << endl;
    // cout << "e3=" << energy / (pngHeight*gain) << endl;

    energy /= pngHeight;
    energy *= 100.0;
    denergy = fabs(penergy - energy);
    // cout << denergy  << endl;
    /* if (denergy > 6.0)
       for (int i=0; i < 40; i++)
       {
       png.plot(int(x),pngHeight- i, 1.0, 0.0, 0.0);
       png.plot(int(x),i, 1.0, 0.0, 0.0);
       }
    */

    penergy = energy;


    x++;

  }

  png.close();

  delete net;
#else
  (void) inFileName;
  (void) outFileName;
#endif
}

// variation of spectrogram generation that generates json
void
json_spectrogram(string inFileName)
{
  FileName inFile(inFileName);
  windowSize_ = 8192;
  hopSize_ = 8192;
  highFreq_ = 8000;
  gain_ = 64.0;

  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;

  double average;

  int length = getFileLengthForSpectrogram(inFileName,min,max,average);

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
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
  double pngHeight = fftBins * (highFreq_ / (frequency / 2.0));

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  double energy;
  // double penergy;
  // double denergy;

  cout << "{" << endl;
  cout << "\"width\" : " << pngLength << "," << endl;
  cout << "\"height\" : " << pngHeight << "," << endl;
  cout << "\"points_array\" : [" << endl;
  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    energy = 0.0;

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);
      normalizedData = ((data - min) / (max - min)) * gain_;

      energy += normalizedData;

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
      if (colour > 0) {
        cout << "[" << x << "," << y << "," << colour << "]," << endl;
      }

    }

    x++;

  }

  cout << "]}" << endl;

  delete net;

}

// Variation that outputs RMS and Flux
void
output_rmsflux(string inFileName)
{

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));

  // A fanout that will do both RMS and Flux calculations
  MarSystem* fanout = mng.create("Fanout","fanout");
  net->addMarSystem(fanout);

  // The branch to do the RMS
  MarSystem* rms_series = mng.create("Series","rms_series");
  rms_series->addMarSystem(mng.create("Rms", "rms"));
  fanout->addMarSystem(rms_series);

  // The branch to do the Flux
  MarSystem* flux_series = mng.create("Series","flux_series");
  flux_series->addMarSystem(mng.create("ShiftInput", "si"));
  flux_series->addMarSystem(mng.create("Windowing", "win"));
  flux_series->addMarSystem(mng.create("Spectrum","spk"));
  flux_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  flux_series->addMarSystem(mng.create("Flux", "flux"));
  fanout->addMarSystem(flux_series);

  // Update the controls with required values
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);

  realvec processedData;
  float time = 0;
  mrs_natural samples_per_tick = net->getControl("SoundFileSource/src/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real rate = net->getControl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  mrs_real sec_per_tick = samples_per_tick / rate;
  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()) {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout << time << "," << processedData(0,0) << "," << processedData(1,0) << endl;
    time += sec_per_tick;
  }

  delete net;

}

// variation of spectrogram generation that generates HTML5
void
html_spectrogram(string inFileName)
{

  FileName inFile(inFileName);
  windowSize_ = 8192;
  hopSize_ = 8192;
  highFreq_ = 8000;
  gain_ = 64.0;

  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  double min = 99999999999.9;
  double max = -99999999999.9;

  double average;

  int length = getFileLengthForSpectrogram(inFileName,min,max,average);

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
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
  double pngHeight = fftBins * (highFreq_ / (frequency / 2.0));

  realvec processedData;
  double normalizedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  double y = 0;
  double colour = 0;
  double energy;
  //double penergy;
  //double denergy;

  // Print the header
  cout << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> " << endl;
  cout << "<html xmlns=\"http://www.w3.org/1999/xhtml\"" << endl;
  cout << "xmlns:svg=\"http://www.w3.org/2000/svg\"" << endl;
  cout << "xmlns:xlink=\"http://www.w3.org/1999/xlink\"> " << endl;

  cout << "<head> " << endl;
  cout << "<title>Marsyas sound2png spectrogram</title> " << endl;

  cout << "<script type=\"text/javascript\" " << endl;
  cout << "src=\"http://www.google.com/jsapi\"></script>" << endl;
  cout << "<script type=\"text/javascript\">" << endl;
  cout << "// You may specify partial version numbers, such as \"1\" or \"1.3\"," << endl;
  cout << "//  with the same result. Doing so will automatically load the " << endl;
  cout << "//  latest version matching that partial revision pattern " << endl;
  cout << "//  (e.g. 1.3 would load 1.3.2 today and 1 would load 1.4.2)." << endl;
  cout << "google.load(\"jquery\", \"1.4.2\");" << endl;
  cout << "" << endl;
  cout << "google.setOnLoadCallback(function() {" << endl;
  cout << "// Place init code here instead of $(document).ready()" << endl;
  cout << "});" << endl;
  cout << "</script>" << endl;



  cout << "</head> " << endl;

  cout << "<body> " << endl;

  cout << "<canvas id=\"flag\" width=\"" << pngLength << "\" height=\"" << pngHeight << "\">" << endl;
  cout << "You don't support Canvas." << endl;
  cout << "</canvas> " << endl;
  cout << "<script> " << endl;
  cout << "var el= document.getElementById(\"flag\");" << endl;

  cout << "if (el && el.getContext) { " << endl;
  cout << "var context = el.getContext('2d');" << endl;
  cout << "if(context){" << endl;
  cout << "var points = {" << endl;
  cout << "\"width\" : " << pngLength << "," << endl;
  cout << "\"height\" : " << (int)pngHeight << "," << endl;
  cout << "\"points_array\" : [" << endl;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    energy = 0.0;

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);
      normalizedData = ((data - min) / (max - min)) * gain_;

      energy += normalizedData;

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
      if (colour > 0) {
        cout << "[" << x << "," << y << "," << colour << "]," << endl;
      }

    }

    x++;

  }

  cout << "]};" << endl;

  // Print the footer
  cout << "$(document).ready(function(){" << endl;
  cout << "console.log(\"ready\");" << endl;
  cout << "var el= document.getElementById(\"flag\");" << endl;
  cout << "" << endl;
  cout << "if (el && el.getContext) { " << endl;
  cout << "drawSpectrogram(el);" << endl;
  cout << "}" << endl;
  cout << "});" << endl;
  cout << "" << endl;
  cout << "function drawSpectrogram(el) {" << endl;
  cout << "var context = el.getContext('2d');" << endl;
  cout << "if(context){" << endl;
  cout << "// Create an ImageData object." << endl;
  cout << "var imgd = context.createImageData(" << pngLength << "," << (int)pngHeight << ");" << endl;
  cout << "var pix = imgd.data;" << endl;
  cout << "" << endl;
  cout << "for(i=0; i < points.points_array.length; i++) {" << endl;
  cout << "var colour = points.points_array[i][2] * 256;							  " << endl;
  cout << "var index = (points.points_array[i][0] + (points.points_array[i][1] * points.width))*4;" << endl;
  cout << "pix[index+0] = colour; // red" << endl;
  cout << "pix[index+1] = colour; // green" << endl;
  cout << "pix[index+2] = colour; // blue" << endl;
  cout << "pix[index+3] = 127;    // alpha" << endl;
  cout << "}							  " << endl;
  cout << "// Draw the ImageData object at the given (x,y) coordinates." << endl;
  cout << "context.putImageData(imgd, 0, 0);" << endl;
  cout << "}" << endl;
  cout << "}" << endl;
  cout << "}" << endl;
  cout << "}" << endl;


  cout << "</script> " << endl;
  cout << "" << endl;
  cout << "</body> " << endl;
  cout << "</html> " << endl;


  delete net;



}


void
selfSimilarity(string fname, string outfname)
{

  cout << "Calculating the self similarity matrix of " << fname << endl;

  MarSystemManager mng;


  MarSystem* featureNet = mng.create("Series/featureNet");
  featureNet->addMarSystem(mng.create("SoundFileSource/src"));
  featureNet->addMarSystem(mng.create("MixToMono/mix2mono"));
  featureNet->addMarSystem(mng.create("Rms/rms"));
  featureNet->addMarSystem(mng.create("RealvecSink/rdest"));

  featureNet->updControl("SoundFileSource/src/mrs_string/filename", fname);
  featureNet->updControl("mrs_natural/inSamples", 1024);


  for (int i = 0; i < 128; i++)
    featureNet->tick();


  mrs_realvec rms_data = featureNet->getctrl("RealvecSink/rdest/mrs_realvec/data")->to<mrs_realvec>();


  cout << rms_data << endl;



#ifdef MARSYAS_PNG
  mrs_natural rms_height = 32;
  pngwriter png1(rms_data.getCols(),rms_height, 0, "rms.png");
  png1.invert();

  rms_data.normMaxMin();

  for (int i=0; i < rms_data.getCols(); ++i)
  {
    png1.line(i, 0, i, rms_data(0,i) * rms_height, 0.0, 0.0, 1.0);
  }

  png1.close();
#endif

  MarSystem* simNet = mng.create("Series/simnet");
  simNet->updControl("mrs_natural/inSamples", rms_data.getCols());
  simNet->updControl("mrs_natural/inObservations", 1);
  simNet->addMarSystem(mng.create("RealvecSource", "src"));
  simNet->updControl("RealvecSource/src/mrs_realvec/data", rms_data);

  MarSystem* sim = mng.create("SelfSimilarityMatrix/sim");
  sim->updControl("mrs_string/normalize", "MinMax");

  MarSystem* met = mng.create("Metric/met");
  met->updControl("mrs_string/metric", "euclideanDistance");
  sim->addMarSystem(met);
  simNet->addMarSystem(sim);

  simNet->tick();

  mrs_realvec similarity_output =
    simNet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  cout << similarity_output << endl;



#ifdef MARSYAS_PNG
  pngwriter png_rms(rms_height+rms_data.getCols(), rms_height+rms_data.getCols(), 0, "simMatrix.png");

  png_rms.invert();

  // Find max and min
  double max = MINREAL;
  double min = MAXREAL;
  for (int r=0; r < similarity_output.getRows(); ++r) {
    for (int c=0; c < similarity_output.getCols(); ++c) {
      if (similarity_output(r,c) < min)
        min = similarity_output(r,c);
      if (similarity_output(r,c) > max)
        max = similarity_output(r,c);
    }
  }

  double colour;
  // Make a png of the similarity matrix
  for (int r=0; r < similarity_output.getRows(); ++r) {
    for (int c=0; c < similarity_output.getCols(); ++c) {
      colour = 1.0 - ((similarity_output(r,c) - min) / (max - min));
      png_rms.plot(rms_height+c,rms_height+r,colour,colour,colour);
    }
  }

  for (int i=0; i < rms_data.getCols(); ++i)
  {
    png_rms.line(rms_height+i, 0, rms_height+i, rms_data(0,i) * rms_height, 0.0, 0.0, 1.0);
    png_rms.line(0, rms_height+i , rms_data(0,i) * rms_height, rms_height+i, 0.0, 0.0, 1.0);
  }



  mrs_natural pngLength;
  mrs_natural pngHeight;

  if (width_ !=-1)
    pngLength = width_;
  if (height_ != -1)
    pngHeight = height_;

  if ((width_ !=-1)||(height_ != -1))
  {
    png_rms.scale_wh(pngLength, pngHeight);
  }

  png_rms.close();


#else
  (void) outfname;
#endif


}




void fftHistogram(string inFileName)
{
  double fftBins = windowSize_ / 2.0 + 1;  // N/2 + 1

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  net->addMarSystem(mng.create("Spectrum","spk"));
  net->addMarSystem(mng.create("PowerSpectrum","pspk"));
  net->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", "decibels");
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->updControl("SoundFileSource/src/mrs_natural/pos", position_);
  net->updControl("SoundFileSource/src/mrs_natural/inSamples", hopSize_);
  net->updControl("ShiftInput/si/mrs_natural/winSize", windowSize_);
  net->updControl("mrs_natural/inSamples", int(hopSize_));

  mrs_real frequency = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  double pngHeight = fftBins * (highFreq_ / (frequency / 2.0));
  realvec processedData;

  // Iterate over the whole input file by ticking, outputting columns
  // of data to the .png file with each tick
  double x = 0;
  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()
         && (ticks_ == -1 || x < ticks_))  {
    net->tick();
    processedData = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    for (int i = 0; i < pngHeight; ++i) {
      double data_y = i;

      double data = processedData(int(data_y),0);

      cout << "data=" << data << endl;
    }
    x++;
  }

  delete net;
}

void correlogramPNGs(string inFileName, string outFilePrefix)
{
#ifdef MARSYAS_PNG

  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  net->addMarSystem(mng.create("AudioSink", "dest"));
  net->addMarSystem(mng.create("Windowing", "ham"));
  net->addMarSystem(mng.create("Spectrum", "spk"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  net->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* parallel = mng.create("Parallel", "parallel");
  net->addMarSystem(parallel);

  int powerSpectrumSize = (windowSize_/2)+1;
  for (int i = 0; i < powerSpectrumSize; ++i) {
    std::stringstream ss;
    ss << "auto" << i;
    parallel->addMarSystem(mng.create("AutoCorrelation", ss.str()));
  }

  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);
  net->setctrl("mrs_natural/inSamples", windowSize_);
  net->updControl("Memory/mem/mrs_natural/memSize", memorySize_);

  net->updControl("mrs_real/israte", 44100.0);
  net->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  realvec data;

  int counter = 0;
  realvec max_data(powerSpectrumSize);

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()) {
    cout << "Processing tick " << counter << endl;
    net->tick();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    // cout << "data" << data << endl;

    // Create the png we are going to write into
    std::stringstream outFileName;
    outFileName << outFilePrefix << std::setfill('0') << std::setw(5) << counter << ".png";
    pngwriter png(int(windowSize_),int(powerSpectrumSize),0,outFileName.str().c_str());

    // Find the maximum value of the data
    max_data.setval(-999.9);
    for (int x = 0; x < memorySize_; x++) {
      for (int y = 0; y < powerSpectrumSize; y++) {
        if (data(y,x) > max_data(y)) {
          max_data(y) = data(y,x);
        }
      }
    }

    // for (int x = 0; x < memorySize; x++) {
    //   cout << "max_data(" << x << ")" << max_data(x) << endl;
    // }

    // Plot all the data points
    for (int x = 0; x < memorySize_; x++) {
      for (int y = 0; y < powerSpectrumSize; y++) {
        double color = 1.0 - (double(data(y,x)) * (1.0 / max_data(y)));
        // cout << "x=" << x << " y=" << y << " data(y,x)=" << data(y,x) << " color=" << color << endl;
        png.plot(int(x),int(y),color,color,color);
      }
    }

    png.close();
    counter++;
  }
#else
  (void) inFileName;
  (void) outFilePrefix;
#endif
}


int
main(int argc, const char **argv)
{
  MRSDIAG("sound2png.cpp - main");

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

  if (mode_ == "histogram") {
    fftHistogram(files[0]);
    exit(0);
  }

  if (mode_ == "selfsimilarity")
  {
    selfSimilarity(files[0], files[1]);
    exit(0);
  }


  if (mode_ == "neptune")
  {
    neptune_spectrogram(files[0],files[1]);
    exit(0);
  }
  if (mode_ == "json")
  {
    json_spectrogram(files[0]);
    exit(0);
  }

  if (mode_ == "html") {
    html_spectrogram(files[0]);
    exit(0);
  }

  if (mode_ == "correlogram")
  {
    correlogramPNGs(files[0],files[1]);
    exit(0);
  }

  if (mode_ == "rmsflux")
  {
    output_rmsflux(files[0]);
    exit(0);
  }

  if (files.size() != 2) {
    cerr << "You must specify two files on the command line." << endl;
    cerr << "One for the input audio file and one for the output PNG file" << endl;
    printUsage(progName);
  }

#ifdef MARSYAS_PNG
  // play the soundfiles/collections
  if (mode_ == "waveform") {
    outputWaveformPNG(files[0],files[1]);
  } else {
    outputSpectrogramPNG(files[0],files[1]);
  }

  exit(0);
#else
  cout << "sound2png requires Marsyas to be compiled with the WITH_PNG option" << endl;
  exit(0);
#endif

}






