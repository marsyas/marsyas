#include <cstdio>
#include <marsyas/Collection.h>
#include "MrsDoctor.h"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include "SocketException.h"

using namespace Marsyas;

CommandLineOptions cmd_options;


string fileName = EMPTYSTRING;

int helpopt;
int usageopt;
long port = 0;
float gain = 1.0f;



void
printUsage(string progName)
{
  MRSDIAG("recvTCP.cpp - printUsage");
  cerr << "Usage : " << progName << " [-p port] [-g gain] [-f outputfile] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("recvTCP.cpp - printHelp");
  cerr << "recvTCP, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-p --port 	   : port number to bind for socket " << endl;
  exit(1);
}



// Play soundfile given by sfName, playbacknet contains the playback
// network of MarSystem objects
void play(mrs_real gain, string outName)
{

  NetworkTCPSource* netSrc = new NetworkTCPSource("netSrc");

  // update controls if they are passed on cmd line...
  if ( port != 0 ) {
    netSrc->updctrl("mrs_natural/dataPort", port);
    netSrc->updctrl("mrs_natural/controlsPort", port+1);
  }

  // Output destination is either audio or soundfile
  MarSystemManager mn;
  MarSystem* dest;
  if (fileName == EMPTYSTRING)	// audio output
    dest = mn.create("AudioSink", "dest");
  else 				// filename output
  {
    dest = mn.create("SoundFileSink", "dest");
    dest->updctrl("mrs_string/filename", fileName);
  }

  cout << "Creating playback network..." << endl;


  Series playbacknet("playbacknet");
  playbacknet.addMarSystem(netSrc);
  playbacknet.addMarSystem(mn.create("Gain", "gt"));
  playbacknet.addMarSystem(dest);

  // output network description to cout
  cout << playbacknet << endl;

  // setup TCP Server and wait for connection...
  netSrc->refresh();
  cout << "Connection Established with: " << netSrc->getClientAddr() << endl;

  // udpate controls
  playbacknet.updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  playbacknet.updctrl("Gain/gt/mrs_real/gain", gain);

  // may want to update this as control data from networksource...
  //if (outName == EMPTYSTRING)
  //  playbacknet.updctrl("AudioSink/dest/mrs_natural/nChannels", 1);
  //else
  //  playbacknet.updctrl("SoundFileSink/dest/mrs_natural/nChannels", 1);

  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural onSamples = playbacknet.getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  // mrs_natural repeatId = 1;

  mrs_real* controls = 0;

  while (true)
  {
    try {

      controls = playbacknet.recvControls();
      if ( controls != 0 ) {

        // get some reference controls, so if they have changed we update them
        mrs_natural inSamples = playbacknet.getctrl("mrs_natural/inSamples")->to<mrs_natural>();
        mrs_natural inObservations = playbacknet.getctrl("mrs_natural/inObservations")->to<mrs_natural>();
        mrs_real israte = playbacknet.getctrl("mrs_real/israte")->to<mrs_real>();

        if ( (mrs_natural)controls[1] != inSamples || (mrs_natural)controls[2] != inObservations
             || controls[3] != israte ) {

          playbacknet.updctrl("mrs_natural/inSamples", (mrs_natural)controls[1]);
          playbacknet.updctrl("mrs_natural/inObservations", (mrs_natural)controls[2]);
          playbacknet.updctrl("mrs_real/israte", controls[3]);
        }
      }


      playbacknet.tick();
    }
    catch( SocketException e ) {
      cerr << "Played " << wc << " slices of " << onSamples << " samples" << endl;
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
  MRSDIAG("recvTCP.cpp - main");

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



  if (soundfiles.size() == 0) {
    cout << "sfi size == 0" << endl;
    play(gain, EMPTYSTRING);

  }
  else {
    for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    {
      play(gain, *sfi);
    }
  }

  exit(1);
}



