
#include <cstdio>
#include <iostream>
#include <string>

#include "SocketException.h"
#include "NetworkUDPSource.h"
#include <marsyas/Collection.h>
#include "AuFileSource.h"
#include "AudioSink.h"
#include "MrsDoctor.h"
#include "Gain.h"
#include "Spectrum.h"
#include "InvSpectrum.h"
#include <marsyas/fft.h>
#include "AutoCorrelation.h"
#include "Peaker.h"
#include "SineSource.h"
#include <marsyas/system/MarSystemManager.h>
#include "HalfWaveRectifier.h"
#include <marsyas/CommandLineOptions.h>

using namespace Marsyas;

CommandLineOptions cmd_options;


string fileName = EMPTYSTRING;

int helpopt;
int usageopt;
long port = 0;
float gain = 1.0f;
bool showClient = false;


void
printUsage(string progName)
{
  MRSDIAG("recvUDP.cpp - printUsage");
  cerr << "Usage : " << progName << " [-p port] [-g gain] [-f outputfile] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("recvUDP.cpp - printHelp");
  cerr << "recvUDP, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output  to file " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-p --port 	   : port number to bind for socket " << endl;
  exit(1);
}



// Play soundfile given by sfName, playbacknet contains the playback
// network of MarSystem objects
void play(mrs_real gain, string outName)
{

  NetworkUDPSource* netSrc = new NetworkUDPSource("netSrc");

  // update controls if they are passed on cmd line...
  if ( port != 0 ) {
    netSrc->updctrl("mrs_natural/port", port);
  }


  // Output destination is either audio or soundfile
  MarSystem *dest;
  if (outName == EMPTYSTRING)
    dest = new AudioSink("dest");
  else
  {
    dest = new AuFileSink("dest");
    dest->updctrl("mrs_string/filename", outName);
  }

  cout << "Creating playback network..." << endl;

  MarSystemManager mn;
  Series playbacknet("playbacknet");
  playbacknet.addMarSystem(netSrc);
  playbacknet.addMarSystem(mn.create("Gain", "gt"));
  playbacknet.addMarSystem(dest);

  // output network description to cout
  cout << playbacknet << endl;

  // udpate controls
  playbacknet.updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  playbacknet.updctrl("Gain/gt/mrs_real/gain", gain);

  // may want to update this as control data from networksource...
  if (outName == EMPTYSTRING)
    playbacknet.updctrl("AudioSink/dest/mrs_natural/nChannels", 1);
  else
    playbacknet.updctrl("AuFileSink/dest/mrs_natural/nChannels", 1);

  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural onSamples = playbacknet.getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  // mrs_natural repeatId = 1;

  netSrc->refresh();

  while (true)
  {
    try {
      playbacknet.tick();
      if ( !showClient ) {
        cout << "Receiving from: " << netSrc->getClientAddr() << endl;
        showClient = true;
      }

    }
    catch( SocketException e ) {
      cerr << "Played " << wc << " slices of " << onSamples << " samples" << endl;
      wc = onSamples = 0;
      exit(1);
    }
    wc ++;
    samplesPlayed += onSamples;
  }

}


void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addNaturalOption("port", "p", 0);
}


void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  gain = cmd_options.getRealOption("gain");
  fileName   = cmd_options.getStringOption("filename");
  port       = cmd_options.getNaturalOption("port");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("recvUDP.cpp - main");

  string progName = argv[0];
  progName = progName.erase(0,3);

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();


  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;



  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);


  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    play(gain, *sfi);

  exit(1);
}






