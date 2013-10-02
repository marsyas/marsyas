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
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

#include <vector>

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */
int helpopt;
int usageopt;
int verboseopt;
string fileName;
string pluginName;
CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("speakerSeg.cpp - printUsage");
  cerr << "Usage : " << progName << " [-f outputfile] [-p pluginName] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("speakerSeg.cpp - printHelp");
  cerr << "speakerSeg, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Play the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose output " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  exit(1);
}

// Play a collection l of soundfiles
void speakerSeg(vector<string> soundfiles)
{
  MRSDIAG("speakerSeg.cpp - speakerSeg");

  MarSystemManager mng;
  string sfName;

  //create main processing network
  MarSystem* pnet = mng.create("Series", "pnet");

// 	// Output destination is either audio or soundfile
// 	MarSystem* dest;
// 	if (fileName == EMPTYSTRING)	// audio output
// 		dest = mng.create("AudioSink", "dest");
// 	else 				// filename output
// 	{
// 		dest = mng.create("SoundFileSink", "dest");
// 	}

  //create feature extraction network for calculating LSP-10
  MarSystem* featExtractor = mng.create("Series", "featExtractor");
  featExtractor->addMarSystem(mng.create("SoundFileSource", "src"));

  //MarSystem *lpcSeries = mng.create("Series/lpcSeries");
  // lpcSeries->addMarSystem(mng.create("LPCnet", "lpc"));
  // lpcSeries->addMarSystem(mng.create("LSP", "lsp"));

  // MarSystem *features = mng.create("Fanout/features");
  // MarSystem *rms = mng.create("Rms/rms");
  // features->addMarSystem(lpcSeries);
  // features->addMarSystem(rms);

  // featExtractor->addMarSystem(features);


  // do this in the loop belo
  // featExtractor->updControl("LPCnet/lpc/mrs_natural/order", 10);	//hardcoded [!]

  featExtractor->addMarSystem(mng.create("Windowing/win"));
  featExtractor->addMarSystem(mng.create("Spectrum/spk"));
  featExtractor->addMarSystem(mng.create("PowerSpectrum/pspk"));


  MarSystem* features = mng.create("Fanout/features");
  features->addMarSystem(mng.create("Centroid/ctd"));
  features->addMarSystem(mng.create("Rolloff/rlf"));
  features->addMarSystem(mng.create("Flux/flux"));
  features->addMarSystem(mng.create("MFCC/mfcc"));

  featExtractor->addMarSystem(features);



  // based on the nr of features (in this case, the LSP order = 10),
  // calculate the minimum number of frames each speech segment should have
  // in order to avoid ill calculation of the corresponding covariance matrices.
  // To have a good estimation of the covariance matrices
  // the number of data points (i.e. feature vectors) should be at least
  // equal or bigger than d(d+1)/2, where d = cov matrix dimension.
  // mrs_real d = (mrs_real)featExtractor->getctrl("LPCnet/lpc/mrs_natural/order")->to<mrs_natural>();
  mrs_real d = (mrs_real)featExtractor->getctrl("mrs_natural/onObservations")->to<mrs_natural>();




  mrs_natural minSegFrames = (mrs_natural)ceil(0.5*d*(d+1.0)); //0.5*d*(d+1.0) or 0.5*d*(d-1.0) [?]

  //speakerSeg processes data at each tick as depicted bellow,
  //which includes 4 speech sub-segments (C1, C2, C3, C4) that will be used for detecting
  //speaker changes:
  //
  // data processed at each tick
  // |------------------------|
  //      C1        C2
  // |----+----|----+----|
  //      |----+----|----+----|
  //           C3        C4
  // |--->|
  //   hop
  //
  // So, the speech segments C1, C2, C3 and C4 should have a minimum of minSegFrames in order
  // to avoid cov ill-calculation. Consequently, at each tick, at least 5/2*minSegFrames should
  // be processed, with a hop set to 1/2*minSegFrames (which could be changed, but for now is fixed).

  //create an accumulator for creating hopsize new feature vectors at each tick
  MarSystem* accum = mng.create("Accumulator", "accum");
  accum->addMarSystem(featExtractor);
  accum->updControl("mrs_natural/nTimes", minSegFrames/2);



  //accum->updControl("mrs_natural/nTimes", ceil(minSegFrames/2.0));

  //add accumuated feature extraction to main processing network
  pnet->addMarSystem(accum);

  //create a circular buffer for storing most recent LSP10 speech data
  // pnet->addMarSystem(mng.create("Memory", "mem"));
  // pnet->updControl("Memory/mem/mrs_natural/memSize", 5); //see above for an explanation why we use memSize = 5

  pnet->addMarSystem(mng.create("ShiftInput", "si"));
  pnet->updControl("ShiftInput/si/mrs_natural/winSize", 5 * (minSegFrames/2));


  //add a BIC change detector
  pnet->addMarSystem(mng.create("BICchangeDetector", "BICdet"));


  // link top-level controls
  pnet->linkControl("mrs_string/filename","Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_string/filename");
  pnet->linkControl("mrs_natural/inSamples","Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_natural/inSamples");
  pnet->linkControl("mrs_real/israte", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_real/israte");
  pnet->linkControl("mrs_natural/pos", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_natural/pos");
  pnet->linkControl("mrs_bool/hasData", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_bool/hasData");

  // play each collection or soundfile
  vector<string>::iterator sfi;
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string fname = *sfi;
    //clear any memory data and any stored models
    // pnet->updControl("Memory/mem/mrs_bool/reset", true);
    pnet->updControl("BICchangeDetector/BICdet/mrs_bool/reset", true);
    //set new file name
    pnet->updControl("mrs_string/filename", fname);
    // need to override the control to be 25 ms every time the file is read in.
    pnet->updControl("mrs_natural/inSamples",128); // hardcoded for 8kHz
// 		if (fileName != EMPTYSTRING) // soundfile output instead of audio output
// 			pnet->updControl("SoundFileSink/dest/mrs_string/filename", fileName);
//
// 		if (fileName == EMPTYSTRING)	// audio output
// 		{
// 			pnet->updControl("AudioSink/dest/mrs_natural/bufferSize", 256);
// 			pnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
// 		}

    MarControlPtr hasDataPtr_ =
      pnet->getctrl("mrs_bool/hasData");

    while (hasDataPtr_->isTrue())
    {
      pnet->tick();
    }
    pnet->tick();
    pnet->tick();

  }

  // output network description to cout
  if ((pluginName == EMPTYSTRING) && (verboseopt)) // output to stdout
  {
    cout << (*pnet) << endl;
  }
  else if (pluginName != EMPTYSTRING)             // output to plugin
  {
    ofstream oss(pluginName.c_str());
    oss << (*pnet) << endl;
  }
  delete pnet;
}


void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
}

int
main(int argc, const char **argv)
{
  MRSDIAG("speakerSeg.cpp - main");

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
  speakerSeg(soundfiles);

  exit(0);
}






