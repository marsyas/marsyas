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
    extract: batch feature extraction 
*/

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "CommandLineOptions.h"

#include <string> 
#include <iostream>

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
mrs_real lengthopt;

#define EMPTYSTRING "MARSYAS_EMPTY" 

CommandLineOptions cmd_options;



void 
printUsage(string progName)
{
  MRSDIAG("drumExtract.cpp - printUsage");
  cerr << "Usage : " << progName << " [-l length(seconds)] [-h help] [-u usage] soundfile1 soundfile2 soundfile3" << endl;
  cerr << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("drumExtract.cpp - printHelp");
  cerr << "drumExtract, MARSYAS, Copyright George Tzanetakis " << endl;
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
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  lengthopt = cmd_options.getRealOption("length");
}



void 
drumExtract(vector<Collection> cls, string classNames)
{
  MarSystemManager mng;
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_natural/inSamples", 4096);
  
  
  mrs_natural inObservations = src->getctrl("mrs_natural/inObservations")->toNatural();
  mrs_natural inSamples = src->getctrl("mrs_natural/inSamples")->toNatural();  
  
  realvec in(inObservations, inSamples);
  realvec out(inObservations, inSamples);

  mrs_natural cj,i;
  mrs_natural win = 0;
  mrs_natural startPos = 0;
  mrs_natural endPos = 0;
  mrs_natural startWin = 0;
  mrs_natural endWin = 0;

  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(src);

  MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
  spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
  spectimeFanout->addMarSystem(mng.create("Rms", "rms"));
  
  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Hamming", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  // featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  // featureFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
  // featureFanout->addMarSystem(mng.create("Skewness", "skewness"));
  spectralNet->addMarSystem(featureFanout);

  spectimeFanout->addMarSystem(spectralNet);
  
  extractNet->addMarSystem(spectimeFanout);
  
  extractNet->addMarSystem(mng.create("Annotator", "ann"));
  extractNet->addMarSystem(mng.create("WekaSink",  "wsink"));
  extractNet->addMarSystem(mng.create("GaussianClassifier", "classifier"));  

  extractNet->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
  extractNet->updctrl("WekaSink/wsink/mrs_string/labelNames",classNames);  
  extractNet->updctrl("WekaSink/wsink/mrs_string/filename", "art.arff");

  
  extractNet->updctrl("GaussianClassifier/classifier/mrs_natural/nLabels", (mrs_natural)cls.size());
  extractNet->updctrl("GaussianClassifier/classifier/mrs_string/mode","train");     


  for (cj=0; cj < (mrs_natural)cls.size(); cj++)
    {
      Collection l = cls[cj];
      extractNet->updctrl("Annotator/ann/mrs_natural/label", cj);
      
      for (i=0; i < l.size(); i++)
	{ 
	  win = 0;
	  startPos = 0;
	  endPos = 0;
	  startWin = 0;
	  endWin = 0;
	  src->updctrl("mrs_string/filename", l.entry(i));
	  cout << "Processing " << l.entry(i) << endl;
	  
	  src->updctrl("mrs_natural/inSamples", 4096);
	  
	  while(src->getctrl("mrs_bool/notEmpty")->toBool()) 
	    {
	      src->process(in,out);
	      
	      for (mrs_natural t = 0; t < inSamples; t++)
		{
		  if ((fabs(out(0,t)) > 0.1)&&(startPos == 0))
		    {
		      
		      startPos = t;
		      startWin = win;
		    }
		  if ((fabs(out(0,t)) > 0.999)&&(endPos == 0))
		    {
		      endPos = t;
		      endWin = win;
		    }
		  
		}      
	      win++;
	    }
	  endPos = startPos + 512;
	  
	  extractNet->updctrl("SoundFileSource/src/mrs_natural/inSamples", 
			      endPos - startPos);
	  extractNet->updctrl("SoundFileSource/src/mrs_natural/pos", startPos);
	  extractNet->tick();
	  


	}
    }
  
  
  extractNet->updctrl("GaussianClassifier/classifier/mrs_bool/done", true);
  extractNet->updctrl("GaussianClassifier/classifier/mrs_string/mode","predict");   	  
  extractNet->tick();  
  
  cout << (*extractNet) << endl;
  

  cout << "Wrote " << extractNet->getctrl("WekaSink/wsink/mrs_string/filename")->toString() << endl;

  
  return;
  


  

  /* src->updctrl("mrs_string/filename", filename);
  src->updctrl("mrs_natural/inSamples", 4096);
  


  
	    
  

  
  




  
  extractNet->tick();
  */ 
}


void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
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

  
  if (attempts == 1) 
    {
      string warn;
      warn += "Problem reading collection ";
      warn += name; 
      warn += " - tried both default mf directory and current working directory";
      MRSWARN(warn);
      exit(1);

    }
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

  vector<Collection> cls;
  string classNames = "";
  
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {	
      string sfname = *sfi;
      
      Collection l;
      readCollection(l,sfname);
      l.write("patata.mf");
      
      classNames += (l.name()+',');
      cls.push_back(l);
      
    }
  drumExtract(cls, classNames);
  
  exit(0);
}






	
	
