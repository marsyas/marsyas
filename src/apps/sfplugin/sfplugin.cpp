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

#include <marsyas/export.h>
#include <cstdio>
#include <cstdlib>

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

#include <fstream>
#include <iostream>

using namespace std;
using namespace Marsyas;

string pluginName = EMPTYSTRING;
string fileName = EMPTYSTRING;

CommandLineOptions cmd_options;

int helpopt;
int usageopt;
bool verboseopt;

long offset = 0;
long duration = 10000 * 44100;
float start = 0.0f;
float length = 10000.0f;
float gain = 1.0f;
float repetitions = 1;
bool loop = false;
bool onetick = false;
bool pluginMute = false;

int
printUsage(string progName)
{
  MRSDIAG("sfplugin.cpp - printUsage");
  cerr << "Usage : " << progName << " [-c collection] [-g gain] [-s start(seconds)] [-l length(seconds)] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  return (1);
}

int
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
  cerr << "-u --usage       : display short usage info" << endl;
  cerr << "-h --help        : display this information " << endl;
  cerr << "-v --verbose     : verbose output " << endl;
  cerr << "-g --gain        : linear volume gain " << endl;
  cerr << "-sa --start      : playback start offset in seconds " << endl;
  cerr << "-ln --length     : playback length in seconds " << endl;
  cerr << "-pl --plugin     : plugin file " << endl;
  cerr << "-o --output      : output file " << endl;
  cerr << "-r --repetitions : number of repetitions " << endl;
  cerr << "-pm --pluginMute : don't play audio " << endl;
  return (1);
}

int sfplugin(vector<string> soundfiles, string pluginName)
{
  MRSDIAG("sfplugin.cpp - sfplugin");
  // read the plugin from the file
  ifstream pluginStream(pluginName.c_str());

  MarSystemManager mngr;

  MRS_WARNINGS_OFF;
  MarSystem* msys = mngr.getMarSystem(pluginStream);
  MRS_WARNINGS_ON;
  if (msys == 0)
  {
    cout << "Manager could not load MarSystem from plugin file" << endl;
    return (1);
  }
  mrs_real srate;

  // run audio through the plugin for each file in the collection
  // assume there is a SoundFileSource in the plugin
  // and an audio sink
  string sfName;

  // output the plugin
  if (verboseopt)
    cout << *msys << endl;

  vector<string>::iterator sfi;


  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    // update source filename
    sfName = *sfi;

    msys->updControl("mrs_string/filename", sfName);
    if (!pluginMute) {
      msys->updControl("mrs_bool/initAudio", true);
    }

    // hack for teligence
    //    mrs_natural size = msys->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
    //    mrs_natural inSamples = msys->getctrl("SoundFileSource/src/mrs_natural/inSamples")->to<mrs_natural>();
    //  cout << "size = " << size << endl;
    //  cout << "inSamples = " << inSamples << endl;
    //  mrs_natural memSize = size / inSamples;
    //  cout << "memSize = " << memSize << endl;
    //  msys->updControl("Confidence/confidence/mrs_natural/memSize", memSize);
    //  cout << (*msys) << endl;
    // end of hack

    srate = msys->getctrl("mrs_real/israte")->to<mrs_real>();

    // playback offset and duration
    offset = (mrs_natural) (start * srate);
    duration = (mrs_natural) (length * srate);

    msys->updControl("mrs_natural/pos", offset);

    mrs_natural samplesPlayed = 0;
    mrs_natural onSamples = msys->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    mrs_natural wc = 0;

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
        if (msys->getctrl("mrs_bool/hasData")->to<mrs_bool>() == false)
        {
          if (loop)
            msys->updControl("mrs_natural/pos", 0);
          else
            break;
        }
        mrs_natural pos = msys->getctrl("mrs_natural/pos")->to<mrs_natural>();
        if (pos >= (offset+duration)) {
          break;
        }
      }
    }
  }
  delete msys;
  return 0;
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("start", "sa", 0.0f);
  cmd_options.addStringOption("output", "o", EMPTYSTRING);
  cmd_options.addRealOption("length", "ln", 10000.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addStringOption("plugin", "pl", EMPTYSTRING);
  cmd_options.addRealOption("repetitions", "r", 1.0);
  cmd_options.addBoolOption("loop", "l", false);
  cmd_options.addBoolOption("onetick", "o", false);
  cmd_options.addBoolOption("pluginMute", "pm", false);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = (float)cmd_options.getRealOption("start");
  length = (float)cmd_options.getRealOption("length");
  repetitions = (float)cmd_options.getRealOption("repetitions");
  gain = (float)cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
  loop = cmd_options.getBoolOption("loop");
  onetick = cmd_options.getBoolOption("onetick");
  verboseopt = cmd_options.getBoolOption("verbose");
  pluginMute = cmd_options.getBoolOption("pluginMute");
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

  return sfplugin(soundfiles, pluginName);
}

