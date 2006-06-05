
#include <stdio.h>

#include "Collection.h"
#include "MarSystemManager.h"  
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <fstream> 
#include <iostream>
using namespace std;


#define EMPTYSTRING "MARSYAS_EMPTY"


string pluginName = EMPTYSTRING;
string fileName = EMPTYSTRING;

CommandLineOptions cmd_options;



int helpopt;
int usageopt;

long offset = 0;
long duration = 1000 * 44100;
float start = 0.0f;
float length = 1000.0f;
float gain = 1.0f;
float repetitions = 1; 
bool loop = false;
bool onetick = false;



void 
printUsage(string progName)
{
  MRSDIAG("sfplugin.cpp - printUsage");
  cerr << "Usage : " << progName << " [-c collection] [-g gain] [-s start(seconds)] [-l length(seconds)] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("sfplugin.cpp - printHelp");
  cerr << "sfplugin, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  // cerr << "-o --offset     : playback start offset in samples " << endl;
  // cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-p --plugin     : plugin file " << endl;
  cerr << "-o --output     : output file " << endl;
  exit(1);
}



void sfplugin(vector<string> soundfiles, string pluginName)
{
  MRSDIAG("sfplugin.cpp - sfplugin");
  cout << "pluginName = " << pluginName << endl;
  // read the plugin from the file 
  ifstream pluginStream(pluginName.c_str());

  MarSystemManager mngr;
  MarSystem* msys = mngr.getMarSystem(pluginStream);

  if (msys == 0) 
    {
      cout << "Manager does not support system " << endl;
      exit(1);
    }
  real srate;
  natural nChannels;

  // run audio through the plugin for each file in the collection
  // assume there is a SoundFileSource in the plugin 
  // and an audio sink 
  string sfName;

  // output the plugin 
  cout << *msys << endl;
  
  

  
  vector<string>::iterator sfi;  
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
      // udpate source filename 
      sfName = *sfi;
      cout << "sfName = " << sfName << endl;
      
      msys->updctrl("string/filename", sfName);
      
      

      // hack for teligence 
   //    natural size = msys->getctrl("SoundFileSource/src/natural/size").toNatural();
   //    natural inSamples = msys->getctrl("SoundFileSource/src/natural/inSamples").toNatural();

     //  cout << "size = " << size << endl;
     //  cout << "inSamples = " << inSamples << endl;
     //  natural memSize = size / inSamples;
     //  cout << "memSize = " << memSize << endl;
     //  msys->updctrl("Confidence/confidence/natural/memSize", memSize);
     //  cout << (*msys) << endl;      
      
      // end of hack 
      
      
      srate = msys->getctrl("real/israte").toReal();
      
      // playback offset and duration 
      offset = (natural) (start * srate);
      duration = (natural) (length * srate);

      msys->updctrl("natural/pos", offset);     
      
      natural samplesPlayed = 0;
      natural onSamples = msys->getctrl("natural/onSamples").toNatural();
      natural wc = 0;
      
      if (onetick) 
	{
	  msys->tick();
	}
      else 
	{
	  while (1) 
	    {
	      msys->tick();	      
	      wc++;
	      samplesPlayed += onSamples;
	      
	      // rewind 
	      if (msys->getctrl("bool/notEmpty").toBool() == false)
		{
		  if (loop) 
		    msys->updctrl("natural/pos", 0);
		  else 
		    break;
		}
	    }
	}
    }
  
  delete msys;
}


void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("start", "s", 0.0f);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addRealOption("repetitions", "r", 1.0);
  cmd_options.addBoolOption("loop", "l", false);
  cmd_options.addBoolOption("onetick", "o", false);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  repetitions = cmd_options.getRealOption("repetitions");
  gain = cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
  loop = cmd_options.getBoolOption("loop");
  onetick = cmd_options.getBoolOption("onetick");
}
   





int
main(int argc, const char **argv)
{
  MRSDIAG("sfplugin.cpp - main");

  string progName = argv[0];  
  
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();
  

  if (argc == 1)
    printUsage(progName);
  

  vector<string> soundfiles = cmd_options.getRemaining();
  
  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  sfplugin(soundfiles, pluginName);
  exit(0);
}






