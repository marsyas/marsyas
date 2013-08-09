//
// rms
//
// Output the RMS of a file to JSON
//

#include <iostream>
#include <vector>
#include <string>

#include "common_source.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "FileName.h"

#include "MarSystemTemplateBasic.h"
#include "MarSystemTemplateAdvanced.h"

using namespace std;
using namespace Marsyas;

mrs_natural winSize = 512;
mrs_natural hopSize = 512;

CommandLineOptions cmd_options;
int helpopt;
int usageopt;
int verboseopt;

void printUsage(string progName)
{
  MRSDIAG("omRms.cpp - printUsage");
  cerr << progName << endl;
  cerr << "-ws --windowsize   : analysis window size in samples " << endl;
  cerr << "-hp --hopsize      : analysis hop size in samples " << endl;
  exit(1);
}

void initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addNaturalOption("windowsize", "ws", 512);
  cmd_options.addNaturalOption("hopsize", "hp", 512);
}

void loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  winSize = cmd_options.getNaturalOption("windowsize");
  hopSize = cmd_options.getNaturalOption("hopsize");

}

void run(string inFilename) {
  MarSystemManager mng;

  // create playback network with source-gain-dest
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  net->addMarSystem(mng.create("Rms", "rms"));

  net->updControl("SoundFileSource/src/mrs_string/filename",inFilename);
  net->updControl("mrs_natural/inSamples", hopSize);

  float soundFileSampleRate = net->getControl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  float secPerTick = hopSize / soundFileSampleRate;

  float currTime, data;
  int i = 0;
  while (net->getControl("SoundFileSource/src/mrs_bool/hasData")->isTrue())	{
    net->tick();
    data = (net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>())(0,0);
    currTime = i * secPerTick;
    cout << currTime << "\t" << data << endl;
    i++;
  }

  return;
}

int main(int argc, const char **argv)
{
  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // Handle of command-line options.
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> inFilenames = cmd_options.getRemaining();

  run(inFilenames[0]);
}
