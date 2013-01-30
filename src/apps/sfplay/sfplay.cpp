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
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "common_source.h"

#include <vector> 

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */ 
int helpopt;
int usageopt;
int verboseopt;
string fileName;
string pluginName;
float start = 0.0f;
float length = -1.0f;
float gain = 1.0f;
float repetitions = 1;
mrs_natural offset;
mrs_natural duration;
mrs_natural windowsize;
CommandLineOptions cmd_options;

int
printUsage(string progName)
{
	MRSDIAG("sfplay.cpp - printUsage");
	cerr << "Usage : " << progName << " [-g gain] [-sa start(seconds)] [-ln length(seconds)] [-o outputfile] [-pl pluginName] [-rp repetitions] [-ws windowsize(samples)] file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
	return(1);
}

int
printHelp(string progName)
{
	MRSDIAG("sfplay.cpp - printHelp");
	cerr << "sfplay, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << "Play the sound files provided as arguments " << endl;
	cerr << endl;
	cerr << "Usage : " << progName << " file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u  --usage       : display short usage info" << endl;
	cerr << "-h  --help        : display this information " << endl;
	cerr << "-v  --verbose     : verbose output " << endl;
	cerr << "-o  --output      : output to file " << endl;
	cerr << "-g  --gain        : linear volume gain " << endl;
	cerr << "-sa --start       : playback start offest in seconds " << endl;
	cerr << "-ln --length      : playback length in seconds " << endl;
	cerr << "-pl --plugin      : output plugin name " << endl;
	cerr << "-rp --repetitions : number of repetitions " << endl;
	cerr << "-ws --windowsize  : windows size in samples " << endl;
	return(1);
}


// Play a collection l of soundfiles
void sfplay(vector<string> soundfiles)
{
	MRSDIAG("sfplay.cpp - sfplay");

	MarSystemManager mng;
	string sfName;

	// Output destination is either audio or soundfile 
	MarSystem* dest;
	if (fileName == EMPTYSTRING)	// audio output
		dest = mng.create("AudioSink", "dest");
	else 				// filename output
	{
		dest = mng.create("SoundFileSink", "dest");
	}

	// create playback network with source-gain-dest
	MarSystem* playbacknet = mng.create("Series", "playbacknet");

	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	playbacknet->addMarSystem(mng.create("Gain", "gt"));
	playbacknet->addMarSystem(dest);
  
	// playback offset 


	// update controls 
	playbacknet->updControl("mrs_natural/inSamples", windowsize);
	playbacknet->updControl("Gain/gt/mrs_real/gain", gain);
  
	// link top-level controls 
	playbacknet->linkControl("mrs_string/filename","SoundFileSource/src/mrs_string/filename");
	playbacknet->linkControl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
	playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	playbacknet->linkControl("mrs_natural/loopPos", "SoundFileSource/src/mrs_natural/loopPos");
	playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  
  
	if (fileName == EMPTYSTRING)	// audio output
		playbacknet->linkControl("mrs_bool/initAudio", "AudioSink/dest/mrs_bool/initAudio");
  

	// play each collection or soundfile 
	vector<string>::iterator sfi;  
	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
		string fname = *sfi;
		playbacknet->updControl("mrs_string/filename", fname);
      
		mrs_natural nChannels = playbacknet->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
		mrs_real srate = playbacknet->getctrl("mrs_real/israte")->to<mrs_real>();

;
		offset = (mrs_natural) (start * srate * nChannels);

		playbacknet->updControl("mrs_natural/loopPos", offset);
		playbacknet->updControl("mrs_natural/pos", offset);
		playbacknet->updControl("SoundFileSource/src/mrs_real/repetitions", repetitions);
		playbacknet->updControl("SoundFileSource/src/mrs_real/duration", length);
		

      
		if (fileName != EMPTYSTRING) // soundfile output instead of audio output
			playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", fileName);
      
		if (fileName == EMPTYSTRING)	// audio output
		{
			playbacknet->updControl("AudioSink/dest/mrs_natural/bufferSize", 256); 
			playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
		}
		MarControlPtr hasDataPtr_ = 
			playbacknet->getctrl("mrs_bool/hasData");
      
		while (hasDataPtr_->isTrue())	
		{
			playbacknet->tick();
		}
		//cout << *playbacknet << endl;
    }
  
	// output network description to cout  
	if ((pluginName == EMPTYSTRING) && (verboseopt)) // output to stdout 
    {
		cout << (*playbacknet) << endl;      
    }
	else if (pluginName != EMPTYSTRING)             // output to plugin
    {
		ofstream oss(pluginName.c_str());
		oss << (*playbacknet) << endl;
    }
	delete playbacknet;
}


void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addRealOption("start", "sa", 0.0f);
	cmd_options.addStringOption("output", "o", EMPTYSTRING);
	cmd_options.addRealOption("length", "ln", -1.0f);
	cmd_options.addRealOption("gain", "g", 1.0);
	cmd_options.addStringOption("plugin", "pl", EMPTYSTRING);
	cmd_options.addRealOption("repetitions", "rp", 1.0);
	cmd_options.addNaturalOption("windowsize", "ws", 2048);
}


void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	start = (float)cmd_options.getRealOption("start");
	length = (float)cmd_options.getRealOption("length");
	repetitions = (float)cmd_options.getRealOption("repetitions");
	verboseopt = cmd_options.getBoolOption("verbose");
	gain = (float)cmd_options.getRealOption("gain");
	pluginName = cmd_options.getStringOption("plugin");
	fileName   = cmd_options.getStringOption("output");
	windowsize = cmd_options.getNaturalOption("windowsize");
}


int
main(int argc, const char **argv)
{
	MRSDIAG("sfplay.cpp - main");

	string progName = argv[0];  
	if (argc == 1)
		printUsage(progName);

	// handling of command-line options 
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();
  
	vector<string> soundfiles = cmd_options.getRemaining();
	if (helpopt) 
		return printHelp(progName);
  
	if (usageopt)
		return printUsage(progName);

	// play the soundfiles/collections 
	sfplay(soundfiles);
  
	return(0);
}
