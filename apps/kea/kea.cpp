#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <vector> 

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */ 
int helpopt_;
int usageopt_;
string wekafname_;
string mode_;
CommandLineOptions cmd_options_;
 
void 
printUsage(string progName)
{
  MRSDIAG("train.cpp - printUsage");
  cerr << "Usage : " << progName << " [-w weka file] " << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("train.cpp - printHelp");
  cerr << "train, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Train a classifier " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-w --wekafname : .arff file for training " << endl;
  cerr << "-c --classifier : classifier to use " << endl;
  cerr << "-m --mode: mode of operation" << endl;
  exit(1);
}


void 
distance_matrix() 
{
  cout << "Distance matrix calculation" << endl;
  MarSystemManager mng; 

  MarSystem* net = mng.create("Series", "net");
  MarSystem* accum = mng.create("Accumulator", "accum");
  MarSystem* wsrc = mng.create("WekaSource", "wsrc");
  accum->addMarSystem(wsrc);
  accum->updctrl("WekaSource/wsrc/mrs_string/filename", wekafname_);
  mrs_natural nInstances = 
    accum->getctrl("WekaSource/wsrc/mrs_natural/nInstances")->to<mrs_natural>();
  accum->updctrl("mrs_natural/nTimes", nInstances);

  
  MarSystem* dmatrix = mng.create("SimilarityMatrix", "dmatrix");
  dmatrix->addMarSystem(mng.create("Metric", "dmetric"));
  dmatrix->updctrl("Metric/dmetric/mrs_string/metric", "euclideanDistance");
  dmatrix->updctrl("mrs_string/normalize", "MinMax");
  net->addMarSystem(accum);
  net->addMarSystem(dmatrix);

  net->tick();

  
  cout << "Marsyas-kea distance matrix for MIREX 2007 Audio Similarity Exchange " << endl;
  cout << "Q/R";

  for (int i=0; i < nInstances; i++) 
    {
      cout << "\t" << i;
    }
  cout << endl;








}

// Play a collection l of soundfiles
void train()
{
  cout << "Training classifier using .arff file: " << wekafname_ << endl;
  
  MarSystemManager mng;
  
  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  // net->addMarSystem(mng.create("OneRClassifier", "ocl"));
  net->addMarSystem(mng.create("GaussianClassifier", "gcl"));
  net->addMarSystem(mng.create("Summary", "summary"));
  // net->updctrl("WekaSource/wsrc/mrs_string/attributesToInclude", "1,2,3");

  net->updctrl("WekaSource/wsrc/mrs_string/filename", wekafname_);
  net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "PercentageSplit,50%");
  net->updctrl("mrs_natural/inSamples", 1);

  net->updctrl("Summary/summary/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updctrl("Summary/summary/mrs_string/classNames", 
	       net->getctrl("WekaSource/wsrc/mrs_string/classNames"));
  
  
  net->updctrl("GaussianClassifier/gcl/mrs_natural/nLabels", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->linkctrl("GaussianClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");
  mrs_bool training_done = false;
  

  cout << "Instances = " <<  net->getctrl("WekaSource/wsrc/mrs_natural/nInstances")->to<mrs_natural>() << endl;

  while(net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>() == false)
    {
      string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
      net->tick();
      net->updctrl("GaussianClassifier/gcl/mrs_string/mode", mode);
    }
  
  net->updctrl("Summary/summary/mrs_bool/done", true);
  net->tick();

}


void 
initOptions()
{
  cmd_options_.addBoolOption("help", "h", false);
  cmd_options_.addBoolOption("usage", "u", false);
  cmd_options_.addStringOption("wekafname", "w", EMPTYSTRING);
  cmd_options_.addStringOption("mode", "m", "train");
}


void 
loadOptions()
{
  helpopt_ = cmd_options_.getBoolOption("help");
  usageopt_ = cmd_options_.getBoolOption("usage");
  wekafname_ = cmd_options_.getStringOption("wekafname");
  mode_ = cmd_options_.getStringOption("mode");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("train.cpp - main");

  cout << "Kea - Machine Learning in Marsyas ala Weka" << endl;

  string progName = argv[0];  
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options 
  initOptions();
  cmd_options_.readOptions(argc, argv);
  loadOptions();
  
  vector<string> soundfiles = cmd_options_.getRemaining();
  if (helpopt_) 
    printHelp(progName);
  
  if (usageopt_)
    printUsage(progName);

  cout << "Mode = " << mode_ << endl;

  if (mode_ == "train") 
    train();
  if (mode_ == "distance_matrix") 
    distance_matrix();
  exit(0);
}






