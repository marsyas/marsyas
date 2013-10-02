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

#include <vector>

using namespace std;
using namespace Marsyas;


/* global variables for various commandline options */
int helpopt;
int usageopt;
int verboseopt;
string queryName;
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
  MRSDIAG("mugle.cpp - printUsage");
  cerr << "Usage : " << progName << " " << endl;
  cerr << endl;
  cerr << " " << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("mugle.cpp - printHelp");
  cerr << "mugle, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-v --verbose    : verbose output" << endl;
  cerr << "-q --query      : query name" << endl;
  cerr << "-h --help       : display this information " << endl;
  exit(1);
}




void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("query", "q", EMPTYSTRING);
}


void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  queryName   = cmd_options.getStringOption("query");
}



void mugle(string queryName, string collectionName)
{
  MarSystemManager mng;

  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
  spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  spectralNet->addMarSystem(featureFanout);
  spectimeFanout->addMarSystem(spectralNet);
  extractNet->addMarSystem(spectimeFanout);
  extractNet->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);



  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);


  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));
  total->addMarSystem(stats2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("KNNClassifier", "knn"));
  total->addMarSystem(mng.create("ClassOutputSink", "csink"));


  total->updctrl("KNNClassifier/knn/mrs_natural/k",3);
  total->updctrl("KNNClassifier/knn/mrs_natural/nPredictions", 3);
  total->updctrl("KNNClassifier/knn/mrs_string/mode","train");





  // link top-level controls
  total->linkctrl("mrs_string/filename",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");

  total->linkctrl("mrs_natural/pos",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");


  total->linkctrl("mrs_string/allfilenames",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");

  total->linkctrl("mrs_natural/numFiles",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");


  total->linkctrl("mrs_bool/hasData",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/hasData");
  total->linkctrl("mrs_bool/advance",
                  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");

  total->linkctrl("mrs_bool/memReset",
                  "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");

  total->linkctrl("mrs_natural/label",
                  "Annotator/ann/mrs_natural/label");



  total->updctrl("mrs_natural/inSamples", 512);




  cout << "TARGET FEATURE VECTORS" << endl;
  total->updctrl("mrs_string/filename", collectionName);

  total->updctrl("ClassOutputSink/csink/mrs_string/labelNames",
                 total->getctrl("mrs_string/allfilenames"));

  total->updctrl("ClassOutputSink/csink/mrs_natural/nLabels",
                 total->getctrl("mrs_natural/numFiles"));




  total->updctrl("KNNClassifier/knn/mrs_natural/nLabels",
                 total->getctrl("mrs_natural/numFiles"));



  mrs_natural l=0;

  while (total->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    total->updctrl("mrs_natural/label", l);
    total->tick();
    total->updctrl("mrs_bool/memReset", true);
    total->updctrl("mrs_bool/advance", true);
    l++;
    cerr << "Processed " << l << " files " << endl;
  }
  total->updctrl("KNNClassifier/knn/mrs_bool/done",true);
  total->updctrl("KNNClassifier/knn/mrs_string/mode", "predict");
  total->updctrl("ClassOutputSink/csink/mrs_string/filename", "similar.mf");
  total->updctrl("ClassOutputSink/csink/mrs_bool/silent", false);

  cout << (*total) << endl;




  cout << "QUERY FEATURE VECTORS AND MATCHING " << endl;
  total->updctrl("mrs_string/filename", queryName);
  total->tick();








}



int
main(int argc, const char **argv)
{
  MRSDIAG("mugle.cpp - main");

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

  cout << "Search for similar songs to: \n" << queryName << endl;
  cout << "In collection " << soundfiles[0] << endl;


  // play the soundfiles/collections
  mugle(queryName, soundfiles[0]);

  exit(1);
}








