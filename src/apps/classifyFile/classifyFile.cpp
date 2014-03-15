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

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <fstream>
#include <iostream>

using namespace std;
using namespace Marsyas;

string pluginName = EMPTYSTRING;
CommandLineOptions cmd_options;



int helpopt;
int usageopt;
bool verboseopt = false;


bool onetick = false;



void
printUsage(string progName)
{
  MRSDIAG("classifyFile.cpp - printUsage");
  cerr << "Usage : " << progName << " [-c classifier] [-v] file" << endl;
  cerr << endl;
  cerr << "where file is a sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("classifyFile.cpp - printHelp");
  cerr << "classifyFile, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : show the classifier " << endl;
  cerr << "-c --classifier : plugin file " << endl;
  exit(1);
}



void classifyFile(vector<string> soundfiles, string pluginName)
{

  MRSDIAG("classifyFile.cpp - classifyFile");
  // read the plugin from the file
  ifstream pluginStream(pluginName.c_str());



  MarSystemManager mngr;
  MarSystem* msys = mngr.getMarSystem(pluginStream);



  if (msys == 0)
  {
    cout << "Manager does not support system " << endl;
    exit(1);
  }
  mrs_real srate;

  // run audio through the plugin for each file in the collection
  // assume there is a SoundFileSource in the plugin
  // and an audio sink
  string sfName;

  // output the plugin

  // cout << (*msys) << endl;



  vector<string>::iterator sfi;
  mrs_natural count = 0;





  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    count++;


    // udpate source filename
    sfName = *sfi;
    msys->updctrl("mrs_string/filename", sfName);



    mrs_natural size = msys->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
    mrs_natural inSamples = msys->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_natural/inSamples")->to<mrs_natural>();

    mrs_natural memSize = (size / inSamples);
    memSize /= 2;

    // hardwire to approximately 15 seconds
    memSize = 600;


    msys->updctrl("Confidence/confidence/mrs_natural/memSize", memSize);

    if (verboseopt)
      cout << (*msys) << endl;



    srate = msys->getctrl("mrs_real/israte")->to<mrs_real>();


    mrs_natural samplesPlayed = 0;
    mrs_natural onSamples = msys->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    mrs_natural wc = 0;
    int i;




    if (onetick)
    {
      msys->tick();
    }
    else
    {
      // if (verboseopt)
      cout << sfName << " ";
      //

      // while (msys->getctrl("mrs_bool/hasData")->to<mrs_bool>())
      for (i=0; i < memSize-1; i++)
      {
        if (msys->getctrl("mrs_bool/hasData")->to<mrs_bool>() == false)
        {
          break;
        }
        msys->tick();
        wc++;
        samplesPlayed += onSamples;
      }

      if (i >= 10)
      {
        msys->updctrl("Confidence/confidence/mrs_bool/print", true);
        msys->updctrl("Confidence/confidence/mrs_bool/forcePrint", true);
        msys->tick();
      }
      else
        cout << "silence 100.0" << endl;


    }

    msys->updctrl("Confidence/confidence/mrs_bool/print",false);
    msys->updctrl("Confidence/confidence/mrs_bool/forcePrint", false);
    msys->updctrl("Memory/memory/mrs_bool/reset", true);
    msys->updctrl("SilenceRemove/srm/SoundFileSource/src/mrs_string/filename", "");

  }



  delete msys;
}


void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("classifier", "c", EMPTYSTRING);
}


void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt  = cmd_options.getBoolOption("verbose");
  pluginName = cmd_options.getStringOption("classifier");
}






int
main(int argc, const char **argv)
{
  MRSDIAG("classifyFile.cpp - main");

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

  if (pluginName == EMPTYSTRING)
  {
    printHelp(progName);
    cout << "No classifier (-c argument) specified. Existing" << endl;
    exit(1);
  }


  classifyFile(soundfiles, pluginName);
  exit(0);
}






