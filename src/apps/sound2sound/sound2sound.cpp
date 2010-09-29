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
// sound2sound
//
// A program that uses Marsyas to do various types of audio 
// processing/digital audio effects that takes as input a single 
// audio file and generate a single audio file that is the result of 
// the processing. 
//
// written by gtzan (c) 2010 - GPL - gtzan@cs.uvic.ca
//

#include <cstdio>
#include "Collection.h"
#include "FileName.h" 
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "common.h"


#include <vector> 
#include <iomanip>

using namespace std;
using namespace Marsyas;

//
// Global variables for various commandline options
//
int helpopt;
int usageopt;
int verboseopt;
mrs_real gain;
mrs_real frequency;
mrs_real qfactor;

mrs_string mode;
CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
	MRSDIAG("sound2sound.cpp - printUsage");
	cerr << "Usage : " << progName << " in.wav [out.wav]" << endl;
	cerr << endl;
	cerr << "where : " << endl;
	cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
	cerr << "   out.wav is the optional output filename" << endl;
	cerr << progName << "-h provides more information" << endl;
	exit(1);
}

void 
printHelp(string progName)
{
	MRSDIAG("sound2sound.cpp - printHelp");
	cerr << "sound2sound" << endl;
	cerr << "------------------------------------------------------"<< endl;

	cerr << "written by gtzan (c) 2010 GPL - gtzan@cs.uvic.ca" << endl;
	cerr << endl;
	cerr << "Usage : " << progName << " in.wav [out.wav]" << endl;
	cerr << endl;
	cerr << "where : " << endl;
	cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
	cerr << "   out.wav is the optional name of the audio file to be generated" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage        : display short usage info" << endl;
	cerr << "-h --help         : display this information " << endl;
	cerr << "-v --verbose      : verbose output" << endl;
	cerr << "------------------------------------------------------"<< endl;
	cerr << "-m --mode        : mode which can be one of [bandpass, highpass, lowpass]" << endl;
	cerr << "-g --gain        : gain" << endl;
	cerr << "-f --frequency   : frequency" << endl;
	cout << "-q --qfactor     : qfacotr" << endl;
	exit(1);
}

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addRealOption("gain", "g", 1.0);
	cmd_options.addRealOption("frequency", "f", 500);
	cmd_options.addRealOption("qfactor", "q", 1.0);
	cmd_options.addStringOption("mode" , "m", "bandpass");
}


void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	verboseopt = cmd_options.getBoolOption("verbose");
	gain = cmd_options.getRealOption("gain");
	frequency = cmd_options.getRealOption("frequency");
	qfactor = cmd_options.getRealOption("qfactor");
	mode = cmd_options.getStringOption("mode");
}


MarSystem* createNetwork()
{

	
}



void 
process(string ifname, string ofname, string mode)
{
  FileName inFile(ifname);
  if (ofname == "default.wav")
	  ofname = inFile.nameNoExt() + "Output.wav";
  
  cout << "***** sound2sound-bandpass *****" << endl;
  cout << "Input filename  = " << ifname << endl;
  cout << "Output filename = " << ofname << endl;
  MarSystemManager mng;
  MarSystem* net = mng.create("Series/net");
  net->addMarSystem(mng.create("SoundFileSource/src"));
  net->addMarSystem(mng.create("Biquad/filter"));
  net->addMarSystem(mng.create("Gain/gain"));
  net->addMarSystem(mng.create("SoundFileSink/dest"));
  
  net->updControl("SoundFileSource/src/mrs_string/filename", ifname);
  net->updControl("SoundFileSink/dest/mrs_string/filename", ofname);
  net->updControl("Biquad/filter/mrs_real/frequency", frequency);
  net->updControl("Biquad/filter/mrs_real/resonance", qfactor);
  net->updControl("Gain/gain/mrs_real/gain", gain);
  net->updControl("Biquad/filter/mrs_string/type", mode);
  
  while (net->getControl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
	  net->tick();
  }
  
  
}



int
main(int argc, const char **argv)
{
	MRSDIAG("sound2sound.cpp - main");

	string progName = argv[0];  
	if (argc == 1)
		printUsage(progName);

	// handling of command-line options 
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();
  
	vector<string> files = cmd_options.getRemaining();
	if (helpopt) 
		printHelp(progName);
  
	if (usageopt)
		printUsage(progName);

	if (files.size() == 1)
	{
		cout << "Using default output filename" << endl;
		files.push_back("default.wav");
	}
	
	process(files[0], files[1], mode);
  
	exit(0);


}






