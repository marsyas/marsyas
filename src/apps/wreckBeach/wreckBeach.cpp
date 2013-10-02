/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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



// wreckBeach is a container executable for various simple
// functions/applications that typically test drive
// a single MarSystem or type of processing.

// It is a place for the often-times ugly, unobscured source code.
// No pretence of polite society is here!  Be bold, be
// adventurous.  Let it all hang out.  Have no shame!


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>


#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;
string toy_with;
int helpopt;
int usageopt;
int verboseopt;

void
printUsage(string progName)
{
  MRSDIAG("wreckBeach - printUsage");
  cerr << "Usage : " << progName << " -t toy_with file1 [file2]" << endl;
  cerr << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("wreckBeach.cpp - printHelp");
  cerr << "wreckBeach, MARSYAS, Copyright Graham Percival " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Various tests " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " -t toy_with file1 file2 file3" << endl;
  cerr << endl;
  cerr << "Supported toy_with:" << endl;
  cerr << "null            : example" << endl;
  cerr << "harmonics       : play with HarmonicStrength" << endl;
  cerr << "spectral_single : play with single SCF and SFM" << endl;
  cerr << "csv_input       : play with input csv file" << endl;
  cerr << "arff_in_out     : should pass an arff file untouched" << endl;
  cerr << "mel             : spectrum2mel" << endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("toy_with", "t", EMPTYSTRING);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  toy_with = cmd_options.getStringOption("toy_with");
}


// ********************** start toys **********
void
toy_with_harmonicStrength(mrs_string sfname)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  net->addMarSystem(mng.create("Spectrum", "spec"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
  //net->addMarSystem(mng.create("RemoveObservations", "ro"));
  net->addMarSystem(mng.create("HarmonicStrength", "harm"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", 2048);
  net->updControl("ShiftInput/si/mrs_natural/winSize", 4096);
  net->updControl("Windowing/win/mrs_string/type", "Hanning");
  //net->updControl("RemoveObservations/ro/mrs_real/highCutoff", 0.25);

  mrs_natural num_harmonics = 5;
  realvec harmonics(num_harmonics);
  /*
  cout<<"---------------------------"<<endl;
  cout<<"Relative harmonic strengths"<<endl;
  for (mrs_natural h = 0; h<num_harmonics; ++h)
  {
  	if (h==num_harmonics-1)
  	{
  		cout<<"0.5"<<"\t";
  		harmonics(h) = 0.5;
  	}
  	else
  {
  	cout<<h<<"\t";
  	harmonics(h) = h+1;
  }
  }
  cout<<endl;
  */

  //net->updControl("HarmonicStrength/harm/mrs_realvec/harmonics", harmonics);
  net->updControl("HarmonicStrength/harm/mrs_natural/harmonicsSize", num_harmonics);
  net->updControl("HarmonicStrength/harm/mrs_real/harmonicsWidth", 0.001);
  net->updControl("HarmonicStrength/harm/mrs_natural/type", 0);
  net->updControl("HarmonicStrength/harm/mrs_real/base_frequency", 1000.0);
  //net->updControl("HarmonicStrength/harm/mrs_real/base_frequency", 65.0);
  // typical value for piano strings
  net->updControl("HarmonicStrength/harm/mrs_real/inharmonicity_B", 2e-5);

  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    net->tick();
    mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (mrs_natural h = 0; h<num_harmonics; ++h)
    {
      //printf("%.3E\t", v(h));
      printf("%.2f\t", v(h));
    }
    cout<<endl;
  }
  delete net;
}

void
toy_with_spectral_single(mrs_string sfname)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  net->addMarSystem(mng.create("Spectrum", "spec"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
  net->updControl("SoundFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", 1024);
  net->updControl("ShiftInput/si/mrs_natural/winSize", 1024);
  net->updControl("Windowing/win/mrs_string/type", "Hanning");

  MarSystem *fan = mng.create("Fanout", "fan");
  net->addMarSystem(fan);
  fan->addMarSystem(mng.create("SpectralFlatnessAllBands", "sfab"));

  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    net->tick();
    mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (mrs_natural i = 0; i<v.getSize(); ++i)
    {
      printf("%.5g\t", v(i));
    }
    cout<<endl;
  }
  delete net;
}

void
toy_with_csv_input(mrs_string sfname)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("CsvFileSource", "src"));

  net->updControl("CsvFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", 1);

  while ( net->getctrl("CsvFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    net->tick();
    mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (mrs_natural i = 0; i<v.getSize(); ++i)
    {
      printf("%.5g\t", v(i));
    }
    cout<<endl;
  }
  delete net;
}

void
toy_with_arff_in_out(mrs_string in_name, mrs_string out_name)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "src"));
  net->addMarSystem(mng.create("WekaSink", "dest"));

  net->updControl("WekaSource/src/mrs_string/filename", in_name);

  net->updControl("WekaSink/dest/mrs_natural/nLabels",
                  net->getControl("WekaSource/src/mrs_natural/nClasses"));
  net->updControl("WekaSink/dest/mrs_string/labelNames",
                  net->getControl("WekaSource/src/mrs_string/classNames"));
  net->updControl("WekaSink/dest/mrs_bool/regression",
                  net->getControl("WekaSource/src/mrs_bool/regression"));

  // must happen after setting the above controls
  net->updControl("WekaSink/dest/mrs_string/filename", out_name);

  while ( !net->getctrl("WekaSource/src/mrs_bool/done")->to<mrs_bool>() )
  {
    net->tick();
  }
  delete net;
}

void
toy_with_mel(mrs_string sfname)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  // you probably want to add more here
  net->addMarSystem(mng.create("Spectrum", "spec"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
  net->addMarSystem(mng.create("Spectrum2Mel", "s2m"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", 512);
  net->updControl("mrs_natural/inSamples", 1024);
  net->updControl("ShiftInput/si/mrs_natural/winSize", 512);
  net->updControl("Windowing/win/mrs_string/type", "Hanning");

  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    net->tick();
#if 1
    mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout<<"--------------"<<endl;
    for (mrs_natural i = 0; i<v.getSize(); ++i)
    {
      printf("%.5g\t", v(i));
    }
    cout<<endl;
#endif
  }
  delete net;
}



// please keep this at the end of all the toy_with_ functions.
// to use, copy this and paste it above, then modify as needed
void
toy_with_null(mrs_string sfname)
{
  MarSystemManager mng;

  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  // you probably want to add more here
  net->addMarSystem(mng.create("Spectrum", "spec"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
  net->addMarSystem(mng.create("Spectrum2Chroma", "s2c"));
  net->addMarSystem(mng.create("PlotSink", "ps"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", 512);
  net->updControl("mrs_natural/inSamples", 1024);
  net->updControl("ShiftInput/si/mrs_natural/winSize", 512);

  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    net->tick();
#if 0
    mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (mrs_natural i = 0; i<v.getSize(); ++i)
    {
      printf("%.5g\t", v(i));
    }
    cout<<endl;
#endif
  }
  delete net;
}



// ********************** end toys **********

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

  string fname0 = EMPTYSTRING;
  string fname1 = EMPTYSTRING;
  string fname2 = EMPTYSTRING;
  string fname3 = EMPTYSTRING;
  string fname4 = EMPTYSTRING;
  string fname5 = EMPTYSTRING;

  if (soundfiles.size() > 0)
    fname0 = soundfiles[0];
  if (soundfiles.size() > 1)
    fname1 = soundfiles[1];
  if (soundfiles.size() > 2)
    fname2 = soundfiles[2];
  if (soundfiles.size() > 3)
    fname3 = soundfiles[3];
  if (soundfiles.size() > 4)
    fname4 = soundfiles[4];
  if (soundfiles.size() > 5)
    fname5 = soundfiles[5];

  cout << "Marsyas toy_with name = " << toy_with << endl;
  cout << "fname0 = " << fname0 << endl;
  cout << "fname1 = " << fname1 << endl;


  if (toy_with == "null")
    toy_with_null(fname0);
  else if (toy_with == "harmonics")
    toy_with_harmonicStrength(fname0);
  else if (toy_with == "spectral_single")
    toy_with_spectral_single(fname0);
  else if (toy_with == "csv_input")
    toy_with_csv_input(fname0);
  else if (toy_with == "arff_in_out")
    toy_with_arff_in_out(fname0, fname1);
  else if (toy_with == "mel")
    toy_with_mel(fname0);
  else
  {
    cout << "Unsupported toy_with " << endl;
    printHelp(progName);
  }

}
