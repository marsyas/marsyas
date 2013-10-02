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

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include "Messager.h"
#include <marsyas/CommandLineOptions.h>

#include <string>

using namespace std;
using namespace Marsyas;





/* global variables for various commandline options */
int helpopt;
int usageopt;
int verboseopt;
string fileName;
string pluginName;
float start = 0.0f;
float length = 1000.0f;
float gain = 1.0f;
float repetitions = 1;
mrs_natural offset;
mrs_natural duration;


CommandLineOptions cmd_options;



void
printUsage(string progName)
{
  MRSDIAG("sfplay.cpp - printUsage");
  cerr << "Usage : " << progName << " [-g gain] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-f outputfile] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYA supported format" << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("sfplay.cpp - printHelp");
  cerr << "sfplay, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose output " << endl;

  cerr << "-f --filename   : output filename " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-r --repetitions: number of repetitions" << endl;

  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  exit(1);
}


void sfplaygui(Collection l, mrs_natural offset, mrs_natural duration, mrs_real start, mrs_real length, mrs_real gain, string outName)
{
  MarSystemManager mng;
  string sfName = l.entry(0);
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", sfName);

  Messager* messager =0;
  messager = new Messager(2,2001);


  MarSystem *dest;
  MarSystem *playbacknet = mng.create("Series", "playbacknet");

  if (src == NULL)
  {
    string errmsg = "Skipping file: " + sfName + " (problem with reading)";
    MRSWARN(errmsg);
  }
  else
  {
    if (outName == EMPTYSTRING)		// audio output
      dest = mng.create("AudioSink", "dest");
    else 				// filename output
    {
      dest = mng.create("SoundFileSink", "dest");
      dest->updctrl("mrs_string/filename", outName);
    }
    // create playback network
    playbacknet->addMarSystem(src);
    playbacknet->addMarSystem(mng.create("Gain", "gt"));
    playbacknet->addMarSystem(dest);



    int type;
    mrs_natural i;

    mrs_natural nChannels = playbacknet->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
    mrs_real srate = playbacknet->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();


    // playback offset & duration
    offset = (mrs_natural) (start * srate * nChannels);
    duration = (mrs_natural) (length * srate * nChannels);


    for (i=0; i < l.size(); i++)
    {
      sfName = l.entry(i);
      cerr << "[" << start << ":" << (start + length) << "] - [" << offset << ":" << (offset + duration) << "] - " <<  sfName << "-" << endl;

      playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

      mrs_natural nChannels = src->getctrl("mrs_natural/nChannels")->to<mrs_natural>();
      mrs_real srate = src->getctrl("mrs_real/israte")->to<mrs_real>();

      // playback offset & duration
      offset = (mrs_natural) (start * srate * nChannels);
      duration = (mrs_natural) (length * srate * nChannels);

      // udpate controls
      // playbacknet.updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES/64);
      playbacknet->updctrl("mrs_natural/inSamples", 256);
      playbacknet->updctrl("Gain/gt/mrs_real/gain", gain);

      playbacknet->updctrl("SoundFileSource/src/mrs_natural/pos", offset);
      playbacknet->updctrl(dest->getType() + "/dest/mrs_natural/nChannels",
                           src->getctrl("mrs_natural/nChannels")->to<mrs_natural>());

      mrs_natural wc=0;
      mrs_natural samplesPlayed = 0;
      mrs_natural onSamples = playbacknet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
      string message;
      bool done = false;

      while ((playbacknet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()) && (duration > samplesPlayed) && !done)
      {


        type = messager->nextMessage();
        if (type < 0)
          done = true;
        else
        {
          message = messager->getMessage();
          stringstream inss(message);
          string cname = "";
          mrs_real dur;
          mrs_real val;
          inss >> cname >> dur >> val;
          val = val / 100.0;
          if (cname != "")
            playbacknet->updctrl(cname,val);
        }
        playbacknet->tick();
        wc ++;
        samplesPlayed += onSamples;
      }
      cerr << "Played " << wc << " slices of " << onSamples << " samples"
           << endl;


    }

  }


  // delete messager;
}






void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  mrs_natural attempts  =0;


  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
  {
    attempts++;
  }
  else
  {
    from1 >> l;
    l.setName(name.substr(0, name.rfind(".", name.length())));
  }


  if (attempts == 1)
  {
    string warn;
    warn += "Problem reading collection ";
    warn += name;
    warn += " - tried both default mf directory and current working directory";
    MRSWARN(warn);
    exit(1);

  }
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
}






int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];

  if (argc == 1)
    printUsage(progName);

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();


  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  int i;




  i = 0;
  Collection l;


  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string fname = *sfi;
    string ext = fname.substr(fname.rfind(".", fname.length()), fname.length());
    if (ext == ".mf")
      readCollection(l,fname);
    else
      l.add(fname);
    i++;
  }
  sfplaygui(l, offset, duration, start, length, gain, fileName);
  exit(1);
}






