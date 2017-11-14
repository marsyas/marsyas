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
  MRSDIAG("audioCompare.cpp - printHelp");
  cerr << "audioCompare, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " file1 file2" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u  --usage        : display short usage info" << endl;
  cerr << "-v  --verbose      : verbose output" << endl;
  cerr << endl;
  exit(0);
}



void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("delta_sample_threshold", "dt", 0.4);
  cmd_options.addRealOption("delta_rms_dB", "db", -15.0);
  cmd_options.addRealOption("delta_spectrum_dB", "ds", -50.0);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  dt_ = cmd_options.getRealOption("delta_sample_threshold");
  db_ = cmd_options.getRealOption("delta_rms_dB");
  ds_ = cmd_options.getRealOption("delta_spectrum_dB");
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
    diff_detected = false;
    pnet->tick();
    l++;
    const realvec& processedData =
      pnet->getctrl("Sum/sum/mrs_realvec/processedData")->to<mrs_realvec>();
    for (i=0; i<samples; ++i)
      {
	if (verboseopt)
	  {
	    if (fabs(processedData(i)) > dt_)
	      {
		cout<<processedData(i)<< " > " << dt_;	  
		cout<< " : " << l << "*512 + " << i << endl;
	    }
	}
	
	if ( fabs(processedData(i)) > dt_)
	  {
	    diff_detected = true;
	  }
	
      }
    
    const realvec& rmsData =
      pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    
    mrs_real rms_db = 20 * log10(rmsData(0,0) + 0.00001);
    if (rms_db > db_)
      {
	diff_detected = true;
	if (verboseopt)
	  cout << "RMS in dB(" << l <<  "*512)  = " << rms_db << ":" << db_ << endl;
      }
    
    if (diff_detected)
      num_of_different_windows++;
  }
  delete pnet;
  cout << "Number of different windows = " << num_of_different_windows << endl;
  if (num_of_different_windows > 0)
    return 1;
  else
    return 0;
}



int
isCloseSpectral(string infile1, string infile2)
{
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  MarSystem* invnet = mng.create("Series", "invnet");
  invnet->addMarSystem(mng.create("SoundFileSource", "src2"));
  invnet->updControl("SoundFileSource/src2/mrs_string/filename", infile2);
  invnet->addMarSystem(mng.create("Spectrum", "spk2"));
  invnet->addMarSystem(mng.create("PowerSpectrum", "pspk2"));
  invnet->addMarSystem(mng.create("Transposer", "trsp2"));
  invnet->addMarSystem(mng.create("Negative", "neg"));

  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src1"));
  net-> updControl("SoundFileSource/src1/mrs_string/filename", infile1);
  net->addMarSystem(mng.create("Spectrum", "spk1"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspk1"));
  net->addMarSystem(mng.create("Transposer", "trsp1"));  

  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(net);
  fanout->addMarSystem(invnet);
  
  pnet->addMarSystem(fanout);
  pnet->addMarSystem(mng.create("Sum", "sum"));
  pnet->addMarSystem(mng.create("Rms", "rms"));
  pnet->linkControl("mrs_bool/hasData",
                    "Fanout/fanout/Series/net/SoundFileSource/src1/mrs_bool/hasData");

  mrs_natural i;
  mrs_natural samples =
    pnet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural l = 0;
  mrs_bool diff_detected = false;
  mrs_natural num_of_different_windows = 0;
  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
  {
    diff_detected = false;
    pnet->tick();
    l++;
    const realvec& processedData =
      pnet->getctrl("Sum/sum/mrs_realvec/processedData")->to<mrs_realvec>();
    const realvec& rmsData =
      pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    
    mrs_real rms_db = 20 * log10(rmsData(0,0) + 0.00001);
    if (rms_db > ds_)
      {
	diff_detected = true;
	if (verboseopt)
	  cout << "Spectral RMS in dB(" << l <<  "*512)  = " << rms_db << ":" << db_ << endl;
      }
    
    if (diff_detected)
      num_of_different_windows++;
  }
  delete pnet;
  cout << "Number of spectral different windows = " << num_of_different_windows << endl;
  if (num_of_different_windows > 0)
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


  cout << "Delta sample threshold (dt)  = " << dt_ << endl;
  cout << "Delta time-domain window RMS in dB (db) = " << db_ << endl;
  cout << "Delta spectral RMS in dB (ds) = " << ds_ << endl;
  
  mrs_bool is_close  = isClose(file1, file2);
  mrs_bool is_closeSpectral = isCloseSpectral(file1, file2);
  cout << is_close << endl;
  cout << is_closeSpectral << endl;
  if (!is_close && !is_closeSpectral)
    cout << "Files are identical" << endl;
  else
    cout << "Files are different" << endl;


  
  return is_close;
}

