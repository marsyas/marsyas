/*
** Copyright (C) 2000-2017 George Tzanetakis <gtzan@cs.uvic.ca>
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



#include <marsyas/system/MarSystemManager.h>
#include <marsyas/common_source.h>
#include <marsyas/CommandLineOptions.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>

using namespace Marsyas;
using namespace std;
CommandLineOptions cmd_options;

int helpopt;
int usageopt;
int verboseopt;
mrs_real ds_, dt_, db_;

void
printUsage(string progName)
{
  MRSDIAG("audioCompare.cpp - printUsage");
  cerr << "Usage : " << progName << "[-h help] [-u usage] [-dt threshold()] [-db threshold] [-ds threshold] audiofile1 audiofile2" << endl;
  cerr << endl;
  exit(0);
}

void
printHelp(string progName)
{
  MRSDIAG("bextract.cpp - printHelp");
  cerr << "bextract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u  --usage        : display short usage info" << endl;
  cerr << "-v  --verbose      : verbose output" << endl;
  cerr << "-c  --collection   : use files in this collection [only for MIREX 2007]" << endl;
  cerr << "-n  --normalize    : enable normalization" << endl;
  cerr << "-as --accSize      : accumulator size" << endl;
  cerr << "-cl --classifier   : classifier name" << endl;
  cerr << "-pr --predict      : predict class for files in collection" << endl;
  cerr << "-fe --featExtract  : only extract features" << endl;
  cerr << "-tc --test         : test collection" << endl;
  cerr << "-st --stereo       : use stereo feature extraction" << endl;
  cerr << "-ds --downsample   : downsampling factor" << endl;
  cerr << "-h  --help         : display this information " << endl;
  cerr << "-e  --extractor    : extractor " << endl;
  cerr << "-p  --plugin       : output plugin name " << endl;
  cerr << "-pm --pluginmute   : mute the plugin " << endl;
  cerr << "-csv --csvoutput   : output confidence values in sfplugin in csv format"<<endl;
  cerr << "-pb --playback     : playback during feature extraction " << endl;
  cerr << "-s  --start        : playback start offset in seconds " << endl;
  cerr << "-sh --shuffle      : shuffle collection file before processing" << endl;

  cerr << "-l  --length       : playback length in seconds " << endl;
  cerr << "-m  --memory       : memory size " << endl;
  cerr << "-w  --wekafile     : weka .arff filename " << endl;
  cerr << "-od --outputdir    : output directory for output of files" << endl;
  cerr << "-ws --windowsize   : analysis window size in samples " << endl;
  cerr << "-hp --hopsize      : analysis hop size in samples " << endl;
  cerr << "-t  --timeline     : flag 2nd input collection as timelines for the 1st collection"<<endl;
  cerr << "-os  --onlyStable  : only output 'stable' frames (silently omit the rest)"<<endl;
  cerr << "-rg  --regression  : print regression labels instead of classification labels"<<endl;
  cerr << endl;

  exit(0);
}



void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("delta_sample_threshold", "dt", 0.1);
  cmd_options.addRealOption("delta_rms_dB", "db", 0.0);
  cmd_options.addRealOption("delta_spectrum", "ds", 0.0);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  dt_ = cmd_options.getRealOption("delta_sample_threshold");
  db_ = cmd_options.getRealOption("delta_rms_dB");
  ds_ = cmd_options.getRealOption("delta_spectrum");
}



int
isClose(string infile1, string infile2)
{
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  MarSystem* invnet = mng.create("Series", "invnet");
  invnet->addMarSystem(mng.create("SoundFileSource", "src2"));
  invnet->updControl("SoundFileSource/src2/mrs_string/filename", infile2);
  invnet->addMarSystem(mng.create("Negative", "neg"));

  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
  fanout->updControl("SoundFileSource/src1/mrs_string/filename", infile1);
  fanout->addMarSystem(invnet);

  pnet->addMarSystem(fanout);
  pnet->addMarSystem(mng.create("Sum", "sum"));
  pnet->addMarSystem(mng.create("Rms", "rms"));
  pnet->linkControl("mrs_bool/hasData",
                    "Fanout/fanout/SoundFileSource/src1/mrs_bool/hasData");

  mrs_natural i;
  mrs_natural samples =
    pnet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural l = 0;
  mrs_bool diff_detected = false;
  mrs_natural num_of_different_windows = 0;
  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
  {
    pnet->tick();
    l++;
    const realvec& processedData =
      pnet->getctrl("Sum/sum/mrs_realvec/processedData")->to<mrs_realvec>();
    for (i=0; i<samples; ++i)
    {
      if (verboseopt)
	{
	  if (abs(processedData(i)) > ds_)
	    {
	      cout<<processedData(i)<< " > " << ds_;	  
	      cout<< " : " << l << "*512 + " << i << endl;
	    }
	}
      
      if ( abs(processedData(i)) > ds_)
      {
	diff_detected = true;
      }

    }

      const realvec& rmsData =
      	pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

      mrs_real rms_db = 20 * log10(rmsData(0,0) + 0.00001);
      if (verboseopt)
      	{
      	  // cout << "RMS in dB = " << rms_db << endl;
      	}
      if (rms_db > db_)
      	{
      	  diff_detected = true;
      	  cout << "RMS in dB(" << l <<  "*512)  = " << rms_db << "-" << db_ << endl;
      	}

      if (diff_detected)
	num_of_different_windows++;

    
  }
  delete pnet;
  cout << "Number of different windows = " << num_of_different_windows << endl;
  if (diff_detected)
    return 1;
  else
    return 0;
}


int
main(int argc, const char **argv)
{

  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining(); 
  string file1 = soundfiles[0];
  string file2 = soundfiles[1];
  mrs_bool is_close  = isClose(file1, file2);
  if (!is_close)
    cout << "Files are identical" << endl;
  else
    cout << "Files are different" << endl;
  return is_close;
}

