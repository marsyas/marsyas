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
#include <marsyas/common_source.h>

#include <vector>

using namespace std;
using namespace Marsyas;

// Play a collection l of soundfiles
void sfplay(const CommandLineOptions & options)
{
  MRSDIAG("sfplay.cpp - sfplay");

  vector<string> soundfiles = options.getRemaining();
  string output_filename = options.value<string>("output");
  string output_network_filename = options.value<string>("plugin");
  bool verbose = options.has("verbose");
  float start = options.value<float>("start");
  float length = options.value<float>("length");
  float repetitions = options.value<float>("repetitions");
  float gain = options.value<float>("gain");
  int windowsize = options.value<int>("windowsize");

  MarSystemManager mng;
  string sfName;

  // Output destination is either audio or soundfile
  MarSystem* dest;
  if (output_filename.empty())  // audio output
  {
    dest = mng.create("AudioSink", "dest");
    string backend = options.value<string>("audio-backend");
    if (!backend.empty())
      dest->setControl("mrs_string/backend", backend);
  }
  else                          // filename output
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

  if (output_filename.empty()) // audio output
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
    mrs_natural offset = (mrs_natural) (start * srate * nChannels);

    playbacknet->updControl("mrs_natural/loopPos", offset);
    playbacknet->updControl("mrs_natural/pos", offset);
    playbacknet->updControl("SoundFileSource/src/mrs_real/repetitions", repetitions);
    playbacknet->updControl("SoundFileSource/src/mrs_real/duration", length);

    if (!output_filename.empty())
    {
      playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", output_filename);
    }
    else
    {
      playbacknet->updControl("AudioSink/dest/mrs_natural/bufferSize", 256);
      playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
    }

    MarControlPtr hasDataPtr_ = playbacknet->getctrl("mrs_bool/hasData");

    while (hasDataPtr_->isTrue())
    {
      playbacknet->tick();
    }
    //cout << *playbacknet << endl;
  }

  // output network description to cout
  if (output_network_filename.empty()) // output to stdout
  {
    if (verbose)
      cout << (*playbacknet) << endl;
  }
  else
  {
    ofstream oss(output_network_filename.c_str());
    oss << (*playbacknet) << endl;
  }
  delete playbacknet;
}

void
printUsage(const string & program_name, bool detailed = false)
{
  MRSDIAG("sfplay.cpp - printUsage");
  cout << "Usage: " << program_name << " [options] file1 [file2 ...]" << endl;
  if (detailed)
  {
    cout << endl;
    cout << "file1, ..., fileN are sound files in a format supported by Marsyas, or collections." << endl;
  }
}

void
printHelp(const string & program_name, const CommandLineOptions & options)
{
  MRSDIAG("sfplay.cpp - printHelp");
  cout << "sfplay, MARSYAS, Copyright George Tzanetakis " << endl;
  cout << "--------------------------------------------" << endl;
  cout << "Play sound files provided as arguments." << endl;
  cout << endl;
  printUsage(program_name, true);
  cout << endl;
  cout << "Options:" << endl;
  options.print();
}

int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string program_name( argv[0] );

  if (argc <= 1) {
    printUsage(program_name);
    return 1;
  }

  CommandLineOptions options;
  options.define_old_style<bool>("help", "h", "", "Display this information.");
  options.define_old_style<bool>("usage", "u", "", "Display short usage information.");
  options.define_old_style<bool>("verbose", "v", "", "Verbose output.");
  options.define_old_style<float>("start", "sa", "", "Playback start offest in seconds.", 0.0f);
  options.define_old_style<float>("length", "ln", "", "Playback length in seconds.", -1.0f);
  options.define_old_style<float>("repetitions", "rp", "", "Number of repetitions.", 1.0f);
  options.define_old_style<float>("gain", "g", "", "Linear volume gain.", 1.0f);
  options.define_old_style<int>("windowsize", "ws", "", "Processing windows size in samples.", 2048);
  options.define_old_style<string>("audio-backend", "a", "", "Audio backend to use for playback.");
  options.define_old_style<string>("output", "o", "", "Write output to this file instead of playing to audio device.");
  options.define_old_style<string>("plugin", "pl", "", "Save processing network into this file.");

  options.readOptions(argc, argv);

  if (options.has("help")) {
    printHelp(program_name, options);
    return 0;
  }

  if (options.has("usage")) {
    printUsage(program_name);
    return 0;
  }


  // play the soundfiles/collections
  sfplay(options);

  return(0);
}
