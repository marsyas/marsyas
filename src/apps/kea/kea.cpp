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
string workdir_;
string distancematrix_;

 
void 
printUsage(string progName)
{
  MRSDIAG("kea.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m mode -c classifier -wd workdir -w weka file] " << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("kea.cpp - printHelp");
  cerr << "kea, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Kea - machine learning ala Weka " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-w --wekafname : .arff file for training " << endl;
  cerr << "-c --classifier : classifier to use " << endl;
  cerr << "-m --mode: mode of operation" << endl;
  cerr << "-wd --workdir: workspace directory" << endl;
  cerr << "-dm --distancematrix: distance matrix in MIREX format" << endl;
  exit(1);
}


void 
distance_matrix() 
{
  cout << "Distance matrix calculation using " << wekafname_ << endl;

  if (workdir_ != EMPTYSTRING) 
    wekafname_  = workdir_ + wekafname_;




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

  ofstream oss;
  oss.open(distancematrix_.c_str());

  
  oss << "Marsyas-kea distance matrix for MIREX 2007 Audio Similarity Exchange " << endl;

  Collection l;
  l.read(workdir_ + "extract.txt");
  


  for (int i=1; i <= l.size(); i++) 
    {
      oss << i << "\t" << l.entry(i-1) << endl;
    }



  oss << "Q/R";
  const mrs_realvec& dmx = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();




  for (int i=1; i <= nInstances; i++) 
    {
      oss << "\t" << i;
    }
  oss << endl;

  for (int i=1; i <= nInstances; i++) 
    {
      oss << i;
      for (int j=0; j < nInstances; j++)
	oss <<"\t" << dmx(i-1, j);
      oss << endl;
    }


  oss << endl;




    



}


void 
train()
{
  cout << "Training classifier using .arff file: " << wekafname_ << endl;

  if (wekafname_ == EMPTYSTRING) 
    {
      cout << "Weka .arff file not specified" << endl;
      return;
    }

  if (workdir_ != EMPTYSTRING) 
    wekafname_  = workdir_ + wekafname_;

  cout << "Training classifier using .arff file: " << wekafname_ << endl;
  

  
  MarSystemManager mng;
  
  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  // net->addMarSystem(mng.create("OneRClassifier", "gcl"));
  // net->addMarSystem(mng.create("GaussianClassifier", "gcl"));
  // net->addMarSystem(mng.create("KNNClassifier", "gcl"));
  // net->addMarSystem(mng.create("ZeroRClassifier", "gcl"));
  // net->addMarSystem(mng.create("SVMClassifier", "gcl"));
  cout << "Before addding classifier " << endl;
  net->addMarSystem(mng.create("GaussianClassifier", "gcl"));
  cout << "Added classifier" << endl;
  net->addMarSystem(mng.create("Summary", "summary"));
  // net->updctrl("WekaSource/wsrc/mrs_string/attributesToInclude", "1,2,3");
  
  net->updctrl("WekaSource/wsrc/mrs_string/filename", wekafname_);
  // net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "PercentageSplit,50%");
  net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "kFold,[NS],10");
  // net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "UseTestSet,lg.arff");
  net->updctrl("mrs_natural/inSamples", 1);


  net->updctrl("Summary/summary/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updctrl("Summary/summary/mrs_string/classNames", 
	       net->getctrl("WekaSource/wsrc/mrs_string/classNames"));
  
  
  net->updctrl("GaussianClassifier/gcl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  // net->linkctrl("GaussianClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");


  /*   net->updctrl("OneRClassifier/gcl/mrs_natural/nLabels", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->linkctrl("OneRClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");
  */ 


		    /* net->updctrl("KNNClassifier/gcl/mrs_natural/nLabels", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
		       net->linkctrl("KNNClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");
		    */ 
		    
  /* net->updctrl("ZeroRClassifier/gcl/mrs_natural/nLabels", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
		    net->linkctrl("ZeroRClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");
  */ 


  // net->updctrl("ZeroRClassifier/gcl/mrs_natural/nLabels", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  // net->linkctrl("SVMClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");

  net->linkctrl("GaussianClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");

		    

  int i = 0;
  while(net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>() == false)
    {
      string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
      net->tick();
      net->updctrl("GaussianClassifier/gcl/mrs_string/mode", mode);
      // net->updctrl("OneRClassifier/gcl/mrs_string/mode", mode);
      // net->updctrl("KNNClassifier/gcl/mrs_string/mode", mode);
      // net->updctrl("ZeroRClassifier/gcl/mrs_string/mode", mode);
      // net->updctrl("SVMClassifier/gcl/mrs_string/mode", mode);
      // net->updctrl("Classifier/gcl/mrs_string/mode", mode);
      i++;
    }

  // net->updctrl("SVMClassifier/gcl/mrs_string/mode", "predict");
  net->updctrl("GaussianClassifier/gcl/mrs_string/mode", "predict");
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
  cmd_options_.addStringOption("workdir", "wd", EMPTYSTRING);
  cmd_options_.addStringOption("distancematrix", "dm", EMPTYSTRING);
}


void 
loadOptions()
{
  helpopt_ = cmd_options_.getBoolOption("help");
  usageopt_ = cmd_options_.getBoolOption("usage");
  wekafname_ = cmd_options_.getStringOption("wekafname");
  mode_ = cmd_options_.getStringOption("mode");
  workdir_ = cmd_options_.getStringOption("workdir");
  distancematrix_ = cmd_options_.getStringOption("distancematrix");
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






