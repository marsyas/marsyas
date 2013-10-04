/*
** Copyright (C) 2000-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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
// carfac_profiling
//
// Test the CARFAC filter on audio data
//
// written by sness (c) 2011 - GPL - sness@sness.net
//

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>
#include <marsyas/marsystems/CARFAC.h>

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

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("carfac_profiling.cpp - printUsage");
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
  MRSDIAG("carfac_profiling.cpp - printHelp");
  cerr << "carfac_profiling" << endl;
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
}

int assert_close(float a, float b, double delta = 0.0001) {
  if (fabs(a - b) > delta) {
    cout << "Error a=" << a << " b=" << b << " abs(a-b)" << abs(a-b) << " delta=" << delta << endl;
    exit(0);
  }
}

void carfac_profiling(string inAudioFileName)
{
  // cout << "carfac_profiling" << endl;
  // cout << "inAudioFileName=" << inAudioFileName << endl;

  MarSystemManager mng;

  // Create the network
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* carfac = mng.create("CARFAC", "carfac");
  net->addMarSystem(carfac);

  net->addMarSystem(mng.create("Gain", "gain"));

  net->updControl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net->updControl("mrs_natural/inSamples",512);

  // Just print the coefficients
  // carfac->updControl("mrs_bool/printcoeffs",true);
  // carfac->updControl("mrs_bool/printstate",false);
  // cout << carfac->toString();

  // Just print the state
  carfac->updControl("mrs_bool/printcoeffs",false);
  carfac->updControl("mrs_bool/printstate",true);
  for (int i = 0; i < 100; i++) {
    net->tick();
    // cout << "@@@@@@@@@@@@@@@@@@@@@@@@ "<< i + 1 << " @@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    // cout << carfac->toString();

  }
}



int
main(int argc, const char **argv)
{
  MRSDIAG("carfac_profiling.cpp - main");

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

  carfac_profiling(files[0]);
  exit(0);

}
