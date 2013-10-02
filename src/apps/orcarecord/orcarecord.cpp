/*
 ** Copyright (C) 2007 George Tzanetakis <gtzan@cs.princeton.edu>
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
orca_record: multichannel->stereo recording for the Orchive project
 */

#include <cstdio>
#include <cstdlib>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/Accumulator.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

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
mrs_bool datopt;

CommandLineOptions cmd_options;



void printUsage(string progName)
{
  MRSDIAG("record.cpp - printUsage");
  cerr << "Usage : " << progName << " [-l length(seconds)] [-s samplerate] [-c channels] [-d dat] [-g gain] soundfile" << endl;
  cerr << endl;
  exit(1);
}

void printHelp(string progName)
{
  MRSDIAG("record.cpp - printHelp");
  cerr << "record, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Records 4 stereo channels in parallel using a TASCAP audio device to a sound file " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " [-l length(seconds)] [-s samplerate] [-c channels] [-d dat] [-g gain] soundfile" << endl;
  cerr << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose output" << endl;
  cerr << "-l --length     : record length in seconds " << endl;
  cerr << "-s --srate      : samping rate " << endl;
  cerr << "-y --year       : record to year" << endl;
  cerr << "-c --channels   : number of channels to record " << endl;
  cerr << "-d --dat        : Record dat from the TASCAM FW-1804 " << endl;
  cerr << endl;
  exit(1);
}




void initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("length", "l", 48.0);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("srate", "s", 44100.0);
  cmd_options.addNaturalOption("channels", "c", 1);
  cmd_options.addNaturalOption("year", "y", 2005);
  cmd_options.addBoolOption("dat", "d", false);
}


void loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  lengthopt = cmd_options.getRealOption("length");
  yearopt = cmd_options.getNaturalOption("year");
  gopt = cmd_options.getRealOption("gain");
  sropt = cmd_options.getRealOption("srate");
  copt = cmd_options.getNaturalOption("channels");
  datopt = cmd_options.getBoolOption("dat");
}



void record_orcas(mrs_real length, mrs_natural year,
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


  dest1->updControl("mrs_natural/inObservations", 2);
  dest1->updControl("mrs_natural/inSamples", bufferSize);
  dest1->updControl("mrs_real/israte", sropt);
  dest1->updControl("mrs_string/filename", fname1);


  dest2->updControl("mrs_natural/inObservations", 2);
  dest2->updControl("mrs_natural/inSamples", bufferSize);
  dest2->updControl("mrs_real/israte", sropt);
  dest2->updControl("mrs_string/filename", fname2);

  dest3->updControl("mrs_natural/inObservations", 2);
  dest3->updControl("mrs_natural/inSamples", bufferSize);
  dest3->updControl("mrs_real/israte", sropt);
  dest3->updControl("mrs_string/filename", fname3);


  dest4->updControl("mrs_natural/inObservations", 2);
  dest4->updControl("mrs_natural/inSamples", bufferSize);
  dest4->updControl("mrs_real/israte", sropt);
  dest4->updControl("mrs_string/filename", fname4);

  asrc->setctrl("mrs_natural/nChannels", copt);
  asrc->setctrl("mrs_natural/inSamples", bufferSize);
  asrc->setctrl("mrs_natural/bufferSize", bufferSize);
  asrc->setctrl("mrs_real/israte", sropt);
  asrc->updControl("mrs_bool/initAudio", true);
  asrc->update();



  mrs_real srate = asrc->getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural inSamples = asrc->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
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

  mrs_natural t;

  cout << "Recording " << length << " minutes to files: " << endl;
  cout << fname1 << endl;
  cout << fname2 << endl;
  cout << fname3 << endl;
  cout << fname4 << endl;

  mrs_natural minutes =0;


  for (mrs_natural i = 0; i < iterations; ++i)
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

void record_orcas_dat(mrs_real length, mrs_natural year, string id1)
{

  copt = 17;
  sropt = 44100.0;
  int bufferSize = 6144;


  MarSystemManager mng;

  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest1 = mng.create("SoundFileSink", "dest1");

  ostringstream oss1;
  oss1 << "/Users/orcalab/orcaArchive/" << year << "/" << id1 << ".wav";

  string fname1 = oss1.str();

  dest1->updControl("mrs_natural/inObservations", 2);
  dest1->updControl("mrs_natural/inSamples", bufferSize);
  dest1->updControl("mrs_real/israte", sropt);
  dest1->updControl("mrs_string/filename", fname1);

  asrc->setctrl("mrs_natural/nChannels", copt);
  asrc->setctrl("mrs_natural/inSamples", bufferSize);
  asrc->setctrl("mrs_natural/bufferSize", bufferSize);
  asrc->setctrl("mrs_real/israte", sropt);
  asrc->updControl("mrs_bool/initAudio", true);
  asrc->update();

  mrs_real srate = asrc->getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural inSamples = asrc->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural iterations = (mrs_natural)((srate * length * 60.0) / inSamples);

  realvec rin;
  realvec rout;
  realvec orca1;

  rin.create(copt, bufferSize);
  rout.create(copt, bufferSize);

  orca1.create(2, bufferSize);

  mrs_natural t;

  cout << "Recording " << length << " minutes to files: " << endl;
  cout << fname1 << endl;

  mrs_natural minutes =0;

  for (mrs_natural i = 0; i < iterations; ++i)
  {
    if (((i % 430)==0)&&(i != 0))
    {
      minutes ++;
      cout << minutes << ":" << lengthopt << endl;
    }
    asrc->process(rin,rout);
    for (t=0; t < bufferSize; t++)
    {
      orca1(16,t) = rout(0,t);
      orca1(17,t) = rout(1,t);
    }
    dest1->process(orca1,orca1);

  }

  cout << "Recording complete" << endl;
  cout << "Recorded to file: " << endl;
  cout << fname1 << endl;
  delete dest1;
  delete asrc;
}


int
main(int argc, const char **argv)
{
  MRSDIAG("record.cpp - main");

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

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  string id1;
  string id2;
  string id3;
  string id4;

  if (!datopt) {
    cout << "Recording to year" << yearopt << endl;
    id1 = soundfiles[0];
    id2 = soundfiles[1];
    id3 = soundfiles[2];
    id4 = soundfiles[3];
    record_orcas(lengthopt, yearopt, id1, id2, id3, id4);
  } else {
    cout << "Recording DAT to year" << yearopt << endl;
    id1 = soundfiles[0];
    record_orcas_dat(lengthopt, yearopt, id1);
  }

  return 0;
}
