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
mrs_natural copt;
mrs_natural yearopt;

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
  cerr << "-l --length     : record length in seconds " << endl;
  cerr << "-s --srate      : samping rate " << endl;
  cerr << "-c --channels   : number of channels to record " << endl;
  cerr << endl;
  exit(1);
}




void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("length", "l", 48.0);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("srate", "s", 44100.0);
  cmd_options.addNaturalOption("channels", "c", 1);
  cmd_options.addNaturalOption("year", "y", 2005);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  lengthopt = cmd_options.getRealOption("length");
  yearopt = cmd_options.getNaturalOption("year");
  gopt = cmd_options.getRealOption("gain");
  sropt = cmd_options.getRealOption("srate"); 
  copt = cmd_options.getNaturalOption("channels");
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
  recordNet->updctrl("AudioSource/asrc/mrs_natural/nChannels", copt);
  recordNet->updctrl("AudioSource/asrc/mrs_real/gain", gain);
  recordNet->updctrl("SoundFileSink/dest/mrs_string/filename", filename);
  
  mrs_real srate = recordNet->getctrl("AudioSource/asrc/mrs_real/israte")->toReal();
  mrs_natural nChannels = recordNet->getctrl("AudioSource/asrc/mrs_natural/nChannels")->toNatural();
  cout << "AudioSource srate =  " << srate << endl; 
  cout << "AudioSource nChannels = " << nChannels << endl;
  mrs_natural inSamples = recordNet->getctrl("AudioSource/asrc/mrs_natural/inSamples")->toNatural();
  

  mrs_natural iterations = (mrs_natural)((srate * length) / inSamples);



  
  for (mrs_natural t = 0; t < iterations; t++) 
    {
      recordNet->tick();
    }
  
  

  
}


void 
record_orcas(mrs_real length, mrs_natural year, 
	     string id1, string id2, string id3, string id4) 
{
  copt = 8;
  sropt = 44100.0;
  int bufferSize = 6144;
  

  MarSystemManager mng;
  
  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest1 = mng.create("SoundFileSink", "dest1");
  MarSystem* dest2 = mng.create("SoundFileSink", "dest2");
  MarSystem* dest3 = mng.create("SoundFileSink", "dest3");
  MarSystem* dest4 = mng.create("SoundFileSink", "dest4");


  ostringstream oss1;
  oss1 << "/Users/orcalab/orcaArchive/" << year << "/" << id1 << ".wav";
  ostringstream oss2;
  oss2 << "/Users/orcalab/orcaArchive/" << year << "/" << id2 << ".wav";
  ostringstream oss3;
  oss3 << "/Users/orcalab/orcaArchive/" << year << "/" << id3 << ".wav";
  ostringstream oss4;
  oss4 << "/Users/orcalab/orcaArchive/" << year << "/" << id4 << ".wav";
  
  string fname1 = oss1.str();
  string fname2 = oss2.str();
  string fname3 = oss3.str();
  string fname4 = oss4.str();

  
  dest1->updctrl("mrs_natural/inObservations", 2);
  dest1->updctrl("mrs_natural/inSamples", bufferSize);
  dest1->updctrl("mrs_real/israte", sropt);
  dest1->updctrl("mrs_string/filename", fname1);

  
  dest2->updctrl("mrs_natural/inObservations", 2);
  dest2->updctrl("mrs_natural/inSamples", bufferSize);
  dest2->updctrl("mrs_real/israte", sropt);
  dest2->updctrl("mrs_string/filename", "orca2.au");
  dest2->updctrl("mrs_string/filename", fname2);
  
  dest3->updctrl("mrs_natural/inObservations", 2);
  dest3->updctrl("mrs_natural/inSamples", bufferSize);
  dest3->updctrl("mrs_real/israte", sropt);
  dest3->updctrl("mrs_string/filename", fname3);

  
  dest4->updctrl("mrs_natural/inObservations", 2);
  dest4->updctrl("mrs_natural/inSamples", bufferSize);
  dest4->updctrl("mrs_real/israte", sropt);
  dest4->updctrl("mrs_string/filename", fname4);


  asrc->updctrl("mrs_natural/inSamples", bufferSize);
  asrc->updctrl("mrs_real/israte", sropt);
  asrc->updctrl("mrs_natural/nChannels", copt);
  // asrc->updctrl("mrs_real/gain", gain);
  
  mrs_real srate = asrc->getctrl("mrs_real/israte")->toReal();
  mrs_natural inSamples = asrc->getctrl("mrs_natural/inSamples")->toNatural();
  mrs_natural iterations = (mrs_natural)((srate * length * 60.0) / inSamples);

  realvec rin;
  realvec rout;
  realvec orca1;
  realvec orca2;
  realvec orca3;
  realvec orca4;
  

  rin.create(copt, bufferSize);
  rout.create(copt, bufferSize);
  
  orca1.create(2, bufferSize);
  orca2.create(2, bufferSize);
  orca3.create(2, bufferSize);
  orca4.create(2, bufferSize);

  cout << *asrc << endl;
  mrs_natural t;

  cout << "Recording " << length << " minutes to files: " << endl;
  cout << fname1 << endl;
  cout << fname2 << endl;
  cout << fname3 << endl;
  cout << fname4 << endl;
  
  mrs_natural minutes =0;
  

  for (mrs_natural i = 0; i < iterations; i++) 
    {
      if (((i % 430)==0)&&(i != 0))
	{
	  minutes ++;
	  cout << minutes << ":" << lengthopt << endl;
	}
      asrc->process(rin,rout);
      for (t=0; t < bufferSize; t++)
	{ 
	  orca1(0,t) = rout(0,t);
	  orca1(1,t) = rout(1,t);
	  orca2(0,t) = rout(2,t);
	  orca2(1,t) = rout(3,t);
	  orca3(0,t) = rout(4,t);
	  orca3(1,t) = rout(5,t);
	  orca4(0,t) = rout(6,t);
	  orca4(1,t) = rout(7,t);
	}
      dest1->process(orca1,orca1);
      dest2->process(orca2, orca2);
      dest3->process(orca3, orca3);
      dest4->process(orca4, orca4);
      
    }

  cout << "Recording complete" << endl;
  cout << "Recorded to files: " << endl;
  cout << fname1 << endl;
  cout << fname2 << endl;
  cout << fname3 << endl;
  cout << fname4 << endl;
  delete dest1;
  delete dest2;
  delete dest3;
  delete dest4;
  delete asrc;
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




  mrs_real srate = recordNet->getctrl("AudioSource/asrc/mrs_real/israte")->toReal();
  
  mrs_natural inSamples = recordNet->getctrl("AudioSource/asrc/mrs_natural/inSamples")->toNatural();
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
	   pnet->setctrl("Annotator/ann/mrs_natural/label", 1);
	 }
       /* else if (r > 61) 
	 {
	   cout << "middle" << endl;
	   pnet->setctrl("Annotator/ann/mrs_natural/label", 1);
	 }
       */ 
       else
	 {
	   pnet->setctrl("Annotator/ann/mrs_natural/label", 0);
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

  

  cout << "Recording to year" << yearopt << endl;
  string id1;
  string id2;
  string id3;
  string id4;
  id1 = soundfiles[0];
  id2 = soundfiles[1];
  id3 = soundfiles[2];
  id4 = soundfiles[3];

  // record_orcas(lengthopt, yearopt, id1, id2, id3, id4);

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {	
      cout << "Recording " << lengthopt << " seconds to file " << *sfi << endl;
      recordVirtualSensor(lengthopt,gopt,  *sfi);
      record(lengthopt,gopt,  *sfi);
    }
   

  
  exit(0);
}






	
