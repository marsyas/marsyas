
#include <cstdio>
#include <cstdlib>

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

#include <sstream>

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;

int helpopt;
int usageopt;




void
printUsage(string progName)
{
  MRSDIAG("sfinfo.cpp - printUsage");
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("sfinfo.cpp - printHelp");
  cerr << "sfinfo, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  exit(1);
}



void sfinfo(vector<string> soundfiles)
{

  MRSDIAG("sfinfo.cpp - sfinfo");
  MarSystemManager mng;
  MarSystem* src = mng.create("SoundFileSource", "src");

  vector<string>::iterator sfi;
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string sfName = *sfi;
    src->updControl("mrs_string/filename", sfName);

    if (src == NULL)
    {
      string errmsg = "Skipping file: " + sfName + " (problem with reading)";
      MRSWARN(errmsg);
    }
    else
    {
      mrs_natural size = src->getctrl("mrs_natural/size")->to<mrs_natural>();
      mrs_real srate = src->getctrl("mrs_real/osrate")->to<mrs_real>();
      cout << "Sampling rate: " << srate << endl;
      cout << "Number of channels:  " << src->getctrl("mrs_natural/onObservations")->to<mrs_natural>() << endl;
      cout << "Length (in samples): " << size << endl;


      cout << "Duration (in seconds): " << size / srate  << endl;
      cout << "Fname: " << sfName << endl;
    }
  }

  delete src;
}


void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
}



int
main(int argc, const char **argv)
{
  MRSDIAG("sfinfo.cpp - main");

  string progName = argv[0];

  if (argc == 1)
    printUsage(progName);

  initOptions();
  cmd_options.readOptions(argc,argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();

  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);


  sfinfo(soundfiles);

  exit(0);
}
