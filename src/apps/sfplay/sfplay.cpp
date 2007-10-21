#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

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
CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
  MRSDIAG("sfplay.cpp - printUsage");
  cerr << "Usage : " << progName << " [-g gain] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void 
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
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-r --repetitions: number of repetitions " << endl;
  exit(1);
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
  playbacknet->updctrl("mrs_natural/inSamples", 100);
  playbacknet->updctrl("SoundFileSource/src/mrs_real/repetitions", repetitions);
  playbacknet->updctrl("SoundFileSource/src/mrs_real/duration", length);
  playbacknet->updctrl("Gain/gt/mrs_real/gain", gain);
  
  // link top-level controls 
  playbacknet->linkctrl("mrs_string/filename","SoundFileSource/src/mrs_string/filename");
  playbacknet->linkctrl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
  playbacknet->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  playbacknet->linkctrl("mrs_natural/loopPos", "SoundFileSource/src/mrs_natural/loopPos");
  playbacknet->linkctrl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  
  
  if (fileName == EMPTYSTRING)	// audio output
    playbacknet->linkctrl("mrs_bool/initAudio", "AudioSink/dest/mrs_bool/initAudio");
  

	// play each collection or soundfile 
  vector<string>::iterator sfi;  
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
      string fname = *sfi;
      playbacknet->updctrl("mrs_string/filename", fname);
      
      mrs_natural nChannels = playbacknet->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

      mrs_real srate = playbacknet->getctrl("mrs_real/israte")->to<mrs_real>();
      
      offset = (mrs_natural) (start * srate * nChannels);

      playbacknet->updctrl("mrs_natural/loopPos", offset);
      playbacknet->updctrl("mrs_natural/pos", offset);
      
      if (fileName != EMPTYSTRING) // soundfile output instead of audio output
	playbacknet->updctrl("SoundFileSink/dest/mrs_string/filename", fileName);
      
      if (fileName == EMPTYSTRING)	// audio output
	{
	  playbacknet->updctrl("AudioSink/dest/mrs_natural/bufferSize", 256); 
	  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
	}
      MarControlPtr notEmptyPtr_ = 
	playbacknet->getctrl("mrs_bool/notEmpty");
      
      while (notEmptyPtr_->isTrue())	
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
  cmd_options.addRealOption("start", "s", 0.0f);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("length", "l", -1.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addRealOption("repetitions", "r", 1.0);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  repetitions = cmd_options.getRealOption("repetitions");
  verboseopt = cmd_options.getBoolOption("verbose");
  gain = cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
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
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  // play the soundfiles/collections 
  sfplay(soundfiles);
  
  exit(0);
}






