#include <cstdio>
#include <cstdlib>
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
string inputdir_;
string outputdir_;
string distancematrix_;
string classifier_;
 
void 
printUsage(string progName)
{
  MRSDIAG("kea.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m mode -c classifier -id inputdir -od outputdir -w weka file] " << endl;
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
  cerr << "-cl --classifier : classifier to use " << endl;
  cerr << "-m --mode: mode of operation" << endl;
  cerr << "-id --inputdir: input directory" << endl;
  cerr << "-od --outputdir: output directory" << endl;
  cerr << "-dm --distancematrix: distance matrix in MIREX format" << endl;
  exit(1);
}


void 
distance_matrix() 
{
  cout << "Distance matrix calculation using " << wekafname_ << endl;

  wekafname_  = inputdir_ + wekafname_;

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
  l.read(inputdir_ + "extract.txt");
  


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
pca() 
{
  cout << "Principal Component Analysis of .arff file" << endl;
  if (wekafname_ == EMPTYSTRING) 
    {
      cout << "Weka .arff file not specified" << endl;
      return;
    }

  wekafname_  = inputdir_ + wekafname_;

  cout << "PCA using .arff file: " << wekafname_ << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  MarSystem* accum = mng.create("Accumulator", "accum");
  MarSystem* wsrc = mng.create("WekaSource", "wsrc");
  accum->addMarSystem(wsrc);
  accum->updctrl("WekaSource/wsrc/mrs_string/filename", wekafname_);
  mrs_natural nInstances = 
    accum->getctrl("WekaSource/wsrc/mrs_natural/nInstances")->to<mrs_natural>();
  accum->updctrl("mrs_natural/nTimes", nInstances);

  net->addMarSystem(accum);
  net->addMarSystem(mng.create("PCA", "pca"));
  net->addMarSystem(mng.create("NormMaxMin", "norm"));
  net->addMarSystem(mng.create("WekaSink", "wsink"));

  net->updctrl("PCA/pca/mrs_natural/npc", 3);
  net->updctrl("NormMaxMin/norm/mrs_bool/ignoreLast", true);
  net->updctrl("NormMaxMin/norm/mrs_string/mode", "twopass");
  net->updctrl("NormMaxMin/norm/mrs_real/lower", 0.0);
  net->updctrl("NormMaxMin/norm/mrs_real/upper", 512.0);
  
  net->updctrl("WekaSink/wsink/mrs_natural/nLabels", 
	       net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_natural/nClasses"));
  net->updctrl("WekaSink/wsink/mrs_string/labelNames", net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_string/classNames"));
  net->updctrl("WekaSink/wsink/mrs_string/filename", "pca_out.arff");

  net->tick();


  // the output of the PCA 
 const mrs_realvec& pca_transformed_data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

 cout << "Output first transformed feature using PCA" << endl;
 
 for (int t=0; t < pca_transformed_data.getCols(); t++) 
   {
     cout << pca_transformed_data(0,t) << "\t";
     cout << pca_transformed_data(1,t) << "\t";
     cout << pca_transformed_data(2,t) << "\t";
     cout << pca_transformed_data(3,t) << "\t";
     cout << endl;
   }

  

  

}

void 
train_predict() 
{
	

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

  wekafname_  = inputdir_ + wekafname_;

  cout << "Training classifier using .arff file: " << wekafname_ << endl;
  cout << "Classifier type : " << classifier_ << endl;

  
  MarSystemManager mng;
  
  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  net->addMarSystem(mng.create("Classifier", "cl"));
  net->addMarSystem(mng.create("Summary", "summary"));

  if (classifier_ == "GS")
    net->updctrl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  if (classifier_ == "ZEROR") 
    net->updctrl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");    
  if (classifier_ == "SVM")   
    net->updctrl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");    
  // net->updctrl("WekaSource/wsrc/mrs_string/attributesToInclude", "1,2,3");
  

  // net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "PercentageSplit,50%");
  net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "kFold,NS,10");
  // net->updctrl("WekaSource/wsrc/mrs_string/validationMode", "UseTestSet,lg.arff");
  net->updctrl("WekaSource/wsrc/mrs_string/filename", wekafname_);
  net->updctrl("mrs_natural/inSamples", 1);

  net->updctrl("Summary/summary/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updctrl("Summary/summary/mrs_string/classNames", 
	       net->getctrl("WekaSource/wsrc/mrs_string/classNames"));
  
  net->updctrl("Classifier/cl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->linkctrl("Classifier/cl/mrs_string/mode", "Summary/summary/mrs_string/mode");  

  int i = 0;
  while(net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>() == false)
    {
      string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
      net->tick();
      net->updctrl("Classifier/cl/mrs_string/mode", mode);
	  cout << "kea mode = " << mode << endl;
	  
      i++;
    }

  net->updctrl("Classifier/cl/mrs_string/mode", "predict");
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
  cmd_options_.addStringOption("inputdir", "id", "./");
  cmd_options_.addStringOption("outputdir", "od", "./");
  cmd_options_.addStringOption("distancematrix", "dm", "dm.txt");
  cmd_options_.addStringOption("classifier", "cl", "GS");
}


void 
loadOptions()
{
  helpopt_ = cmd_options_.getBoolOption("help");
  usageopt_ = cmd_options_.getBoolOption("usage");
  wekafname_ = cmd_options_.getStringOption("wekafname");
  mode_ = cmd_options_.getStringOption("mode");
  inputdir_ = cmd_options_.getStringOption("inputdir");
  outputdir_ = cmd_options_.getStringOption("outputdir");
  distancematrix_ = cmd_options_.getStringOption("distancematrix");
  classifier_ = cmd_options_.getStringOption("classifier");
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
  if (mode_ == "pca")
    pca();
  exit(0);
}






