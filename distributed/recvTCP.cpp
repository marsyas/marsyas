#include <stdio.h>
#include "Collection.h"
#include "MrsDoctor.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "SocketException.h"


CommandLineOptions cmd_options;


#define EMPTYSTRING "MARSYAS_EMPTY"
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
void play(real gain, string outName)
{
	
  NetworkTCPSource* netSrc = new NetworkTCPSource("netSrc");
  
  // update controls if they are passed on cmd line...
  if ( port != 0 ) {
  	netSrc->updctrl("natural/dataPort", port);
	netSrc->updctrl("natural/controlsPort", port+1);
  }
  
  // Output destination is either audio or soundfile 
  MarSystemManager mn;
  MarSystem* dest;
  if (fileName == EMPTYSTRING)	// audio output
    dest = mn.create("AudioSink", "dest");
  else 				// filename output
    {
      dest = mn.create("SoundFileSink", "dest");
      dest->updctrl("string/filename", fileName);
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
  playbacknet.updctrl("natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  playbacknet.updctrl("Gain/gt/real/gain", gain);
  
  // may want to update this as control data from networksource...
  //if (outName == EMPTYSTRING) 
  //  playbacknet.updctrl("AudioSink/dest/natural/nChannels", 1);
  //else 
  //  playbacknet.updctrl("SoundFileSink/dest/natural/nChannels", 1);
	
  natural wc=0;
  natural samplesPlayed = 0;
  natural onSamples = playbacknet.getctrl("natural/onSamples").toNatural();
  // natural repeatId = 1;
 
  real* controls = 0;
  
  while (true) 
  {
	  try {
		
		controls = playbacknet.recvControls();	
      		if ( controls != 0 ) {
			
			// get some reference controls, so if they have changed we update them
			natural inSamples = playbacknet.getctrl("natural/inSamples").toNatural();
			natural inObservations = playbacknet.getctrl("natural/inObservations").toNatural();
			real israte = playbacknet.getctrl("real/israte").toReal();
			
			if ( (natural)controls[1] != inSamples || (natural)controls[2] != inObservations 
					|| controls[3] != israte ) {
			
				playbacknet.updctrl("natural/inSamples", (natural)controls[1]);
				playbacknet.updctrl("natural/inObservations", (natural)controls[2]);
				playbacknet.updctrl("real/israte", controls[3]);
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



