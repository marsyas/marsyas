/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    record: record a clip using AudioSource
*/

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "MidiInput.h"
#include "CommandLineOptions.h"

#include <string> 
#include <iostream>

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
mrs_real lengthopt;
mrs_real gopt;
mrs_real sropt;


#define EMPTYSTRING "MARSYAS_EMPTY" 

CommandLineOptions cmd_options;



void 
printUsage(string progName)
{
  MRSDIAG("record.cpp - printUsage");
  cerr << "Usage : " << progName << " [-l length(seconds)] [-h help] [-u usage] soundfile1 soundfile2 soundfile3" << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("record.cpp - printHelp");
  cerr << "record, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "soundfile1 soundfile2 soundfile3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << endl;
  exit(1);
}




void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("length", "l", 3.0);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("srate", "s", 44100.0);
  
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  lengthopt = cmd_options.getRealOption("length");
  gopt = cmd_options.getRealOption("gain");
  sropt = cmd_options.getRealOption("srate"); 
}



void 
record(mrs_real length, mrs_real gain, string filename) 
{
  MarSystemManager mng;
  
  MarSystem* recordNet = mng.create("Series", "recordNet");
  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest = mng.create("SoundFileSink", "dest");

  recordNet->addMarSystem(asrc);
  recordNet->addMarSystem(dest);
  
  recordNet->updctrl("AudioSource/asrc/mrs_natural/inSamples", 100);
  recordNet->updctrl("AudioSource/asrc/mrs_real/israte", sropt);
  recordNet->updctrl("AudioSource/asrc/mrs_natural/nChannels", 2);
  recordNet->updctrl("AudioSource/asrc/mrs_real/gain", gain);
  recordNet->updctrl("SoundFileSink/dest/mrs_string/filename", filename);
  
  mrs_real srate = recordNet->getctrl("AudioSource/asrc/mrs_real/israte").toReal();
  cout << "AudioSource srate =  " << srate << endl; 
  mrs_natural inSamples = recordNet->getctrl("AudioSource/asrc/mrs_natural/inSamples").toNatural();
  

  mrs_natural iterations = (mrs_natural)((srate * length) / inSamples);



  
  for (mrs_natural t = 0; t < iterations; t++) 
    {
      recordNet->tick();
    }
  
  

  
}




void 
recordVirtualSensor(mrs_real length, mrs_real gain, string filename) 
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  MarSystem* srm = mng.create("SilenceRemove", "src"); 
  
  
  MarSystem* recordNet = mng.create("Series", "recordNet");
  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest = mng.create("SoundFileSink", "dest");

  recordNet->addMarSystem(asrc);
  MidiInput* midiin = new MidiInput("midiin");
  recordNet->addMarSystem(midiin);
  // recordNet->addMarSystem(dest); 
  recordNet->updctrl("AudioSource/asrc/mrs_natural/inSamples", 4096);
  recordNet->updctrl("AudioSource/asrc/mrs_real/gain", gain);
  recordNet->updctrl("AudioSource/asrc/mrs_real/israte", 44100.0); 
  recordNet->updctrl("AudioSource/asrc/mrs_real/osrate", 44100.0); 
  
  recordNet->linkctrl("mrs_bool/notEmpty", "AudioSource/asrc/mrs_bool/notEmpty");



  srm->addMarSystem(recordNet);

  pnet->addMarSystem(srm);
  pnet->addMarSystem(dest);

  pnet->addMarSystem(mng.create("PlotSink", "psink"));
  pnet->addMarSystem(mng.create("Hamming", "ham"));
  pnet->addMarSystem(mng.create("Spectrum", "spk"));
  pnet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  MarSystem* features = mng.create("Fanout", "features");
  features->addMarSystem(mng.create("Centroid", "cntrd"));
  features->addMarSystem(mng.create("Rolloff", "rolloff"));
  features->addMarSystem(mng.create("MFCC", "mfcc"));
  pnet->addMarSystem(features);
  
  pnet->addMarSystem(mng.create("Annotator", "ann"));
  pnet->addMarSystem(mng.create("WekaSink", "wsink"));


  pnet->updctrl("WekaSink/wsink/mrs_natural/nLabels", 2);
  pnet->updctrl("WekaSink/wsink/mrs_string/labelNames", "edge, middle");
  pnet->updctrl("WekaSink/wsink/mrs_string/filename", "vsensor.arff");

  pnet->updctrl("SoundFileSink/dest/mrs_real/israte", 44100.0); 
  pnet->updctrl("SoundFileSink/dest/mrs_real/osrate", 44100.0); 
  pnet->updctrl("SoundFileSink/dest/mrs_string/filename", "vsens.au");   




  mrs_real srate = recordNet->getctrl("AudioSource/asrc/mrs_real/israte").toReal();
  
  mrs_natural inSamples = recordNet->getctrl("AudioSource/asrc/mrs_natural/inSamples").toNatural();
  cout << *recordNet << endl; 

  mrs_natural iterations = (mrs_natural)((srate * length) / inSamples);
  cout << "iterations" << endl;


  int r;
  
  for (mrs_natural t = 0; t < iterations; t++)
     {
       r = midiin->rval;
       cout << r << endl;
       if (r > 61)
	 {
	   cout << "middle" << endl;
	   pnet->setctrl("Annotator/ann/mrs_natural/label", (MarControlValue)1);
	 }
       /* else if (r > 61) 
	 {
	   cout << "middle" << endl;
	   pnet->setctrl("Annotator/ann/mrs_natural/label", (MarControlValue)1);
	 }
       */ 
       else
	 {
	   pnet->setctrl("Annotator/ann/mrs_natural/label", (MarControlValue)0);
	   cout << "edge" << endl;
	 }

		  // cout << byte3 << endl;
       
       pnet->tick();
     }
   
   // 	for (i=0; i < nBytes; i++) 
   // 	  std::cout << "Byte " << i << " = " << (int) message[i] << ", "; 
   //       std::cout << endl;
   
}


int
main(int argc, const char **argv)
{
  MRSDIAG("drumExtract.cpp - main");

  string progName = argv[0];  
  if (argc == 1)
    printUsage(progName);


  // read command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();
  

  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  int i;
  i =0;
  
  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;


  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {	
      cout << "Recording " << lengthopt << " seconds to file " << *sfi << endl;
      // recordVirtualSensor(lengthopt,gopt,  *sfi);
      record(lengthopt,gopt,  *sfi);

    }
  
  exit(0);
}






	
