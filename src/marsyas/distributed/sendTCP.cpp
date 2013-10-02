#include <cstdio>
#include <marsyas/Collection.h>
#include "MrsDoctor.h"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;


#ifdef WIN32
#define MARSYAS_MFDIR ""
#endif


/* global variables for various commandline options */
int helpopt;
string host = EMPTYSTRING;
int port = 0;
int usageopt;
int verboseopt;
string pluginName;
float start = 0.0f;
float length = 1000.0f;
float gain = 1.0f;
float repetitions = 1;
mrs_natural offset;
mrs_natural duration;


CommandLineOptions cmd_options;


void
printUsage(string progName)
{
  MRSDIAG("sendTCP.cpp - printUsage");
  cerr << "Usage : " << progName << " [-g gain] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}


void
printHelp(string progName)
{
  MRSDIAG("sendTCP.cpp - printHelp");
  cerr << "sendTCP, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "sends audio over a TCP connection " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-p --port       : host port number " << endl;
  cerr << "-t --host	   : host IP address " << endl;
  cerr << "-r --repetitions: number of repetitions " << endl;

  exit(1);
}


// Play soundfile given by sfName, msys contains the playback
// network of MarSystem objects
void sfplayFile(MarSystem* msys, string sfName)
{
  cerr << "Playing " << sfName << " ";
  // update filename
  msys->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

  // get channels and sampling rate
  mrs_natural nChannels = msys->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  mrs_real srate = msys->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

  // playback offset & duration
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);

  // udpate controls
  // msys->updctrl("mrs_natural/inSamples", 2 * MRS_DEFAULT_SLICE_NSAMPLES);
  msys->updctrl("mrs_natural/inSamples", 512);

  msys->updctrl("Gain/gt/mrs_real/gain", gain);
  msys->updctrl("SoundFileSource/src/mrs_natural/pos", offset);


  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural onSamples = msys->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural repeatId = 1;

  // play the sound
  while (repetitions * duration > samplesPlayed)
  {
    msys->tick();			// everything happens here
    if (samplesPlayed > repeatId * duration) // rewind
    {
      msys->updctrl("SoundFileSource/src/mrs_natural/pos", offset);
      repeatId++;
    }
    wc ++;
    samplesPlayed += onSamples;

    // no duration specified so use all of source input
    if (!(msys->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()) && (repeatId == 1))
    {
      duration = samplesPlayed-onSamples;
    }
  }
  cerr << " - " << wc << " slices of " << onSamples << " samples" << endl;

}


// Play a collection l of soundfiles
void sfplay(Collection l)
{
  MRSDIAG("sfplay.cpp - sfplay");

  mrs_natural i;
  MarSystemManager mng;
  string sfName;

  // Output destination is a NetworkTCPSink
  NetworkTCPSink* dest = (NetworkTCPSink*) mng.create("NetworkTCPSink", "dest");
  dest->updctrl("mrs_natural/dataPort", port);
  dest->updctrl("mrs_string/host", host);

  // create playback network with source-gain-dest
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Gain", "gt"));
  playbacknet->addMarSystem(dest);
  playbacknet->update();

  playbacknet->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
  playbacknet->linkctrl("mrs_natural/nChannels", "SoundFileSource/src/mrs_natural/nChannels");
  playbacknet->linkctrl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
  playbacknet->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  playbacknet->linkctrl("mrs_natural/nChannels", "AudioSink/dest/mrs_natural/nChannels");
  playbacknet->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkctrl("mrs_bool/mute", "Gain/gt/mrs_bool/mute");

  dest->refresh();

  // For each file in collection playback the sound
  for (i=0; i < l.size(); i++)
  {
    sfName = l.entry(i);
    sfplayFile(playbacknet, sfName);
  }

  // output network description to cout
  if ((pluginName == EMPTYSTRING) && (verboseopt)) // output to stdout
  {
    cout << (*playbacknet) << endl;
  }
  else if (pluginName != EMPTYSTRING)             // output to plugin
  {
    ofstream oss(pluginName.c_str());
    oss << (*playbacknet) << endl;
  }


  delete playbacknet;
}


/* readCollection:
   Read collection from file name. Try default marsyas mf directory
and then current directory.
*/

void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  string collectionstr = MARSYAS_MFDIR;
  collectionstr += "/collections/";
  collectionstr += name;
  ifstream from(collectionstr.c_str());
  ifstream from1(name.c_str());
  mrs_natural attempts  =0;


  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
  {
    attempts++;
  }
  else
  {
    from1 >> l;
    l.setName(name.substr(0, name.rfind(".", name.length())));
  }


  MRSDIAG("Trying default MARSYAS mf directory: " + collectionstr);
  if (from.good() == false)
  {
    attempts++;
  }
  else
  {
    from >> l;
    l.setName(name.substr(0, name.rfind(".", name.length())));
  }




  if (attempts == 2)
  {
    string warn;
    warn += "Problem reading collection ";
    warn += name;
    warn += " - tried both default mf directory and current working directory";
    MRSWARN(warn);
  }
}


void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("start", "s", 0.0f);
  //cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("repetitions", "r", 1.0);
  cmd_options.addNaturalOption("port", "p", 5009);
  cmd_options.addStringOption("hostname", "t", "localhost");
}


void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  repetitions = cmd_options.getRealOption("repetitions");
  verboseopt = cmd_options.getBoolOption("verbose");
  gain = cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plugin");
  port = cmd_options.getNaturalOption("port");
  host = cmd_options.getStringOption("hostname");
  //filename = cmd_options.getStringOption("filename");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();


  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;


  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  // create a large collection by concatenating collections and/or soundfiles
  Collection l;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string fname = *sfi;
    string ext = "";

    if (fname.rfind(".", fname.length()) != string::npos)
      ext = fname.substr(fname.rfind(".", fname.length()), fname.length());
    if (ext == ".mf")
      readCollection(l,fname);
    else
    {
      l.add(fname);
    }
  }


  // play the collection
  sfplay(l);
  exit(1);
}





