#include <stdio.h>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <vector> 
using namespace std;






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
natural offset;
natural duration;


#define EMPTYSTRING "MARSYAS_EMPTY"
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
  spectralNet->addMarSystem(mng.create("Hamming", "ham"));
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
  acc->updctrl("natural/nTimes", 1200);
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
  

  total->updctrl("KNNClassifier/knn/natural/k",3);
  total->updctrl("KNNClassifier/knn/natural/nPredictions", 3);
  total->updctrl("KNNClassifier/knn/string/mode","train");  
  

  
  

  // link top-level controls 
  total->linkctrl("string/filename",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/string/filename");  

  total->linkctrl("natural/pos",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/natural/pos");  


  total->linkctrl("string/allfilenames",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/string/allfilenames");  

  total->linkctrl("natural/numFiles",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/natural/numFiles");  


  total->linkctrl("bool/notEmpty",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/bool/notEmpty");  
  total->linkctrl("bool/advance",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/bool/advance");  

  total->linkctrl("bool/memReset",
		  "Accumulator/acc/Series/extractNet/Memory/mem/bool/reset");  

  total->linkctrl("natural/label",
		  "Annotator/ann/natural/label");



  total->updctrl("Accumulator/acc/Series/extractNet/SoundFileSource/src/natural/inSamples", 512);
  
  
  
  
  cout << "TARGET FEATURE VECTORS" << endl;
  total->updctrl("string/filename", collectionName);

  total->updctrl("ClassOutputSink/csink/string/labelNames", 
		 total->getctrl("string/allfilenames"));

  total->updctrl("ClassOutputSink/csink/natural/nLabels", 
		 total->getctrl("natural/numFiles"));




  total->updctrl("KNNClassifier/knn/natural/nLabels", 
		 total->getctrl("natural/numFiles"));
  

  
  natural l=0;

  while (total->getctrl("bool/notEmpty").toBool())
    {
      total->updctrl("natural/label", l);
      total->tick();
      total->updctrl("bool/memReset", true);
      total->updctrl("bool/advance", true);
      l++;
      cerr << "Processed " << l << " files " << endl;
    }
  total->updctrl("KNNClassifier/knn/bool/done",true);  
  total->updctrl("KNNClassifier/knn/string/mode", "predict");
  total->updctrl("ClassOutputSink/csink/string/filename", "similar.mf");
  total->updctrl("ClassOutputSink/csink/bool/silent", false);
  
  cout << (*total) << endl;




  cout << "QUERY FEATURE VECTORS AND MATCHING " << endl;
  total->updctrl("string/filename", queryName);
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







	
