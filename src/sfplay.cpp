#include <stdio.h>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <vector> 
using namespace std;

#define EMPTYSTRING "MARSYAS_EMPTY"

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
natural offset;
natural duration;
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
  playbacknet->update();
  
  natural nChannels = playbacknet->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  real srate = playbacknet->getctrl("SoundFileSource/src/real/israte").toReal();
  // playback offset 
  offset = (natural) (start * srate * nChannels);

  // update controls 
  playbacknet->updctrl("natural/inSamples", 100);
  
  if (fileName == EMPTYSTRING)	// audio output
    playbacknet->updctrl("AudioSink/dest/natural/bufferSize", 256); 

  playbacknet->updctrl("SoundFileSource/src/real/repetitions", repetitions);
  playbacknet->updctrl("SoundFileSource/src/real/duration", length);
  playbacknet->updctrl("Gain/gt/real/gain", gain);
  
  // link top-level controls 
  playbacknet->linkctrl("string/filename","SoundFileSource/src/string/filename");
  playbacknet->linkctrl("natural/nChannels","SoundFileSource/src/natural/nChannels");
  playbacknet->linkctrl("real/israte", "SoundFileSource/src/real/israte");
  playbacknet->linkctrl("natural/pos", "SoundFileSource/src/natural/pos");
  playbacknet->linkctrl("natural/loopPos", "SoundFileSource/src/natural/loopPos");
  playbacknet->linkctrl("natural/nChannels", "AudioSink/dest/natural/nChannels");
  playbacknet->linkctrl("bool/notEmpty", "SoundFileSource/src/bool/notEmpty");

 	
  
  
  // play each collection or soundfile 
  vector<string>::iterator sfi;  
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
      string fname = *sfi;
      playbacknet->updctrl("string/filename", fname);
      
      playbacknet->updctrl("natural/loopPos", offset);
      playbacknet->updctrl("natural/pos", offset);
      
      if (fileName != EMPTYSTRING) // soundfile output instead of audio output
	playbacknet->updctrl("SoundFileSink/dest/string/filename", fileName);
      
      while (playbacknet->getctrl("bool/notEmpty").toBool())	
	{
	  playbacknet->tick();
	}
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






