#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <vector> 

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */ 
int helpopt;
int usageopt;
string wekafname;
CommandLineOptions cmd_options;
 
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
  exit(1);
}


// Play a collection l of soundfiles
void train()
{
  cout << "Training classifier using .arff file: " << wekafname << endl;
}


void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addStringOption("wekafname", "w", EMPTYSTRING);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  wekafname = cmd_options.getStringOption("wekafname");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("train.cpp - main");

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

  train();
  
  exit(0);
}






