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


#include <cstdio>
#include <cstdlib>
#include <string>

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Conversions.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif

using namespace std;
using namespace Marsyas;

string fileName = EMPTYSTRING;
string pluginName = EMPTYSTRING;

// Global variables for command-line options
bool helpopt_ = 0;
bool usageopt_ =0;
bool quietopt_ =0;
int fftSize_ = 512;
int winSize_ = 512;
int dopt = 64;
int iopt = 64;
int sopt = 80;
int bopt = 128;
int vopt_ = 1;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;
bool oscbank_ = false;

mrs_real popt = 1.0;
bool auto_ = false;
mrs_natural midi_ = -1;
bool microphone_ = false;
mrs_string convertmode_ = "sorted";
mrs_string onsetsfile_ = "";
mrs_string unconvertmode_ = "classic";
mrs_bool multires_ = false;
mrs_string multiresMode_ = "transient_switch";


CommandLineOptions cmd_options;

int
printUsage(string progName)
{
  MRSDIAG("phasevocoder.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
  return 1;
}

int
printHelp(string progName)
{
  MRSDIAG("phasevocoder.cpp - printHelp");
  cerr << "phasevocoder, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-d --decimation      : decimation size " << endl;
  cerr << "-i --interpolatation : interpolation size" << endl;
  cerr << "-p --pitchshift      : pitch shift" << endl;
  cerr << "-s --sinusoids       : number of sinusoids" << endl;
  cerr << "-v --voices          : number of voices" << endl;
  cerr << "-ucm --unconvertmode : synthesis back-end mode" << endl;

  /* TODO: Add short description

     cerr << "-a --auto            : " << endl;

  */

  cerr << "-e --epochHeterophonics : heterophonics epoch" << endl;
  cerr << "-m --midi            : midi input port number" << endl;
  cerr << "-b --bufferSize      : audio buffer size" << endl;
  cerr << "-g --gain            : gain (0.0-1.0) " << endl;
  cerr << "-f --filename        : output filename" << endl;
  cerr << "-q --quit            : don't display console output" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;
  cerr << "-ob --oscbank        : oscbank resynthesis instead of IFFT " << endl;
  cerr << "-cm --convertmode    : mode for which frequencies/bins to resynthesize " << endl;
  cerr << "-on --onsets         : file with onsets " << endl;
  return(1);
}







void
phasevocoder(string sfName, mrs_natural N, mrs_natural Nw,
             mrs_natural D, mrs_natural I, mrs_real P,
             string outsfname)
{
  if (!quietopt_)
    cout << "Marsyas Phasevocoder" << endl;

  vector<int> onsets;
  if (onsetsfile_ != "")
  {
    cout << "ONSETS FILE IS " << onsetsfile_ << endl;
    ifstream infile(onsetsfile_.c_str());
    int onset_index;
    while (!infile.eof())
    {
      infile >> onset_index;
      onsets.push_back(onset_index);
    }
    // convert to analysis frame rate
    for (size_t j=0; j < onsets.size(); j++)
    {
      onsets[j] /= D;
      cout << "on = " << onsets[j] << endl;
    }
  }

  MarSystemManager mng;

  // create the phasevocoder network
  MarSystem* pvseries = mng.create("Series", "pvseries");


  if (microphone_)
    pvseries->addMarSystem(mng.create("AudioSource", "src"));
  else
    pvseries->addMarSystem(mng.create("SoundFileSource", "src"));



  if (oscbank_)
  {
    pvseries->addMarSystem(mng.create("PhaseVocoderOscBank", "pvoc"));
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_natural/winSize", Nw);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_natural/FFTSize", N);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_natural/Interpolation", I);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_natural/Decimation", D);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_natural/Sinusoids",
                         (mrs_natural)sopt);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_string/convertMode", convertmode_);
    pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_real/PitchShift", P);
  }

  else
  {
    pvseries->addMarSystem(mng.create("PhaseVocoder", "pvoc"));
    pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/winSize", Nw);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/FFTSize", N);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/Interpolation", I);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/Decimation", D);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/Sinusoids",
                         (mrs_natural)sopt);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_string/convertMode", convertmode_);
    pvseries->updControl("PhaseVocoder/pvoc/mrs_string/unconvertMode",
                         unconvertmode_);
  }


  if (outsfname == EMPTYSTRING)
  {
    pvseries->addMarSystem(mng.create("AudioSink", "dest"));
    pvseries->updControl("AudioSink/dest/mrs_natural/bufferSize", bopt);
  }
  else
  {
    pvseries->addMarSystem(mng.create("SoundFileSink", "dest"));
  }


  if (microphone_)
  {
    pvseries->updControl("AudioSource/src/mrs_real/israte", 44100.0);
    pvseries->updControl("AudioSource/src/mrs_bool/initAudio", true);
  }
  else
  {
    pvseries->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  }

  pvseries->updControl("mrs_natural/inSamples", D);


  pvseries->updControl("SoundFileSink/dest/mrs_string/filename", outsfname);

  if (outsfname == EMPTYSTRING)
  {
    pvseries->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  }



  if (!quietopt_)
    cout << *pvseries << endl;


  int numticks = 0;
  int onset_counter = 5;

  while(pvseries->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    // initialize synthesis phases to analysis phases
    if ((numticks == 0)&&(oscbank_ == false))
      pvseries->updControl("PhaseVocoder/pvoc/mrs_bool/phaselock", true);

    if ((numticks == 0)&&(oscbank_ == true))
    {
      cout << "PvOscBank phaselock" << endl;
      pvseries->updControl("PhaseVocoderOscBank/pvoc/mrs_bool/phaselock",
                           true);
    }


    pvseries->tick();

    numticks++;


    mrs_bool onset_found = false;

    if (onsetsfile_ != "")
    {
      for (size_t j=0; j < onsets.size(); j++)
      {
        if (numticks == onsets[j])
        {
          onset_found = true;
          break;
        }
      }
      if (onset_found)
      {
        onset_counter = 0;
        cout << "ONSET" << endl;
      }


      onset_counter++;

      if (onset_counter ==2)
      {
        // initialize synthesis phases to analysis phases
        pvseries->updControl("PhaseVocoder/pvoc/mrs_bool/phaselock", true);
        pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/Interpolation", D);
        cout << "onset_counter = " << onset_counter << endl;
      }
      else
      {
        pvseries->updControl("PhaseVocoder/pvoc/mrs_natural/Interpolation", I);
      }
    }
  }
  delete pvseries;
}







// original monophonic phasevocoder
void
phasevocSeriesOld(string sfName, mrs_natural N, mrs_natural Nw,
                  mrs_natural D, mrs_natural I, mrs_real P,
                  string outsfname)
{
  if (!quietopt_)
    cout << "phasevocSeries" << endl;

  vector<int> onsets;
  if (onsetsfile_ != "")
  {
    cout << "ONSETS FILE IS " << onsetsfile_ << endl;



    ifstream infile(onsetsfile_.c_str());
    int onset_index;
    while (!infile.eof())
    {
      infile >> onset_index;
      onsets.push_back(onset_index);
    }

    for (size_t j=0; j < onsets.size(); j++)
    {
      onsets[j] /= D;
      cout << "on = " << onsets[j] << endl;
    }
  }







  MarSystemManager mng;

  // create the phasevocoder network
  MarSystem* pvseries = mng.create("Series", "pvseries");
  // oscbank_ = false;


  if (microphone_)
    pvseries->addMarSystem(mng.create("AudioSource", "src"));
  else
    pvseries->addMarSystem(mng.create("SoundFileSource", "src"));
  pvseries->addMarSystem(mng.create("Stereo2Mono", "s2m"));

  pvseries->addMarSystem(mng.create("ShiftInput", "si"));
  pvseries->addMarSystem(mng.create("PvFold", "fo"));

  pvseries->addMarSystem(mng.create("Spectrum", "spk"));
  pvseries->addMarSystem(mng.create("PvConvert", "conv"));
  if (oscbank_)
    pvseries->addMarSystem(mng.create("PvOscBank", "ob"));
  else
  {
    pvseries->addMarSystem(mng.create("PvUnconvert", "uconv"));
    pvseries->addMarSystem(mng.create("InvSpectrum", "ispectrum"));
    pvseries->addMarSystem(mng.create("PvOverlapadd", "pover"));
  }

  pvseries->addMarSystem(mng.create("ShiftOutput", "so"));
  pvseries->addMarSystem(mng.create("Gain", "gain"));


  MarSystem *dest;
  if (outsfname == EMPTYSTRING)
    dest = mng.create("AudioSink", "dest");
  else
  {
    dest = mng.create("SoundFileSink", "dest");
    //dest->updControl("mrs_string/filename", outsfname);
  }
  pvseries->addMarSystem(dest);

  if (outsfname == EMPTYSTRING)
    pvseries->updControl("AudioSink/dest/mrs_natural/bufferSize", bopt);

  // update the controls
  if (microphone_)
  {
    pvseries->updControl("mrs_natural/inSamples", D);
    pvseries->updControl("mrs_natural/inObservations", 1);
  }
  else
  {
    pvseries->updControl("SoundFileSource/src/mrs_string/filename", sfName);
    pvseries->updControl("mrs_natural/inSamples", D);
    pvseries->updControl("mrs_natural/inObservations", 1);

    // if audio output loop to infinity and beyond
    if (outsfname == EMPTYSTRING)
      pvseries->updControl("SoundFileSource/src/mrs_real/repetitions", -1.0);
  }


  pvseries->updControl("ShiftInput/si/mrs_natural/winSize", Nw);
  pvseries->updControl("PvFold/fo/mrs_natural/FFTSize", N);
  pvseries->updControl("PvConvert/conv/mrs_natural/Decimation",D);
  pvseries->updControl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);
  pvseries->updControl("PvConvert/conv/mrs_string/mode", convertmode_);


  if (oscbank_)
  {
    pvseries->updControl("PvOscBank/ob/mrs_natural/Interpolation", I);
    pvseries->updControl("PvOscBank/ob/mrs_real/PitchShift", P);
    pvseries->updControl("PvOscBank/ob/mrs_natural/winSize", Nw);
  }
  else
  {
    pvseries->updControl("PvUnconvert/uconv/mrs_natural/Interpolation", I);
    pvseries->updControl("PvUnconvert/uconv/mrs_natural/Decimation", D);
    pvseries->updControl("PvUnconvert/uconv/mrs_string/mode",unconvertmode_);
    pvseries->updControl("PvOverlapadd/pover/mrs_natural/FFTSize", N);
    pvseries->updControl("PvOverlapadd/pover/mrs_natural/winSize", Nw);
    pvseries->updControl("PvOverlapadd/pover/mrs_natural/Interpolation", I);
    pvseries->updControl("PvOverlapadd/pover/mrs_natural/Decimation",D);
  }

  pvseries->updControl("ShiftOutput/so/mrs_natural/Interpolation", I);
  pvseries->updControl("Gain/gain/mrs_real/gain", 2.0);

  pvseries->linkControl("PvConvert/conv/mrs_realvec/phases",
                        "PvUnconvert/uconv/mrs_realvec/analysisphases");

  pvseries->linkControl("PvUnconvert/uconv/mrs_realvec/regions",
                        "PvConvert/conv/mrs_realvec/regions");


  if (!quietopt_)
    cout << *pvseries << endl;

  if (outsfname == EMPTYSTRING)
    pvseries->updControl("AudioSink/dest/mrs_bool/initAudio", true);



#ifdef MARSYAS_MIDIIO

  int type;
  int byte2, byte3;
  //double stamp;
  mrs_real diff;
  RtMidiIn *midiin = NULL;

  // open midi if midiPort is specified
  if (midi_ != -1)
  {
    try {
      midiin = new RtMidiIn();
    }
    catch (RtError &error) {
      error.printMessage();
      exit(1);
    }
    try {
      midiin->openPort(midi_);
    }
    catch (RtError &error) {
      error.printMessage();
      exit(1);

    }
  }
#endif

  // midi message
  std::vector<unsigned char> message;


  if (outsfname != EMPTYSTRING)
    dest->updControl("mrs_string/filename", outsfname);

  int numticks = 0;

  //int onset_counter;
  //onset_counter = 20;



  while(1)
  {
#ifdef MARSYAS_MIDIIO
    if (midi_ != -1)
    {
      //stamp = midiin->getMessage( &message );
      size_t nBytes = message.size();
      if (nBytes >2)
      {
        byte3 = message[2];
        byte2 = message[1];
        type = message[0];

        if ((byte3 != 0)&&(type == 144))
        {
          diff = byte2 - 60.0;
          pvseries->updControl("PvOscBank/ob/mrs_real/PitchShift", pow((double)1.06, (double)diff));
          mrs_real gain = byte3 * 1.0 / 128.0;
          pvseries->updControl("Gain/gain/mrs_real/gain", gain);
        }
        if (type == 224)
        {
          mrs_real pitchShift = byte3 * 1.0/ 64.0;
          pvseries->updControl("PvOscBank/ob/mrs_real/PitchShift", pitchShift);
        }
        if ((type == 176)&&(byte2 == 100))
        {
          pvseries->updControl("PvConvert/conv/mrs_natural/Sinusoids", byte3);
        }
        if ((type == 176) && (byte2 == 101)&& (byte3 > 4))
        {
          pvseries->updControl("mrs_natural/inSamples", byte3);

          pvseries->updControl("PvConvert/conv/mrs_natural/Decimation", byte3 );
        }
      }
    }
#endif //MARSYAS_MIDIIO

    if (numticks == 0)
    {

      pvseries->updControl("PvUnconvert/uconv/mrs_bool/phaselock", true);
      if (oscbank_)
      {
        cout << "Oscbank phaselock" << endl;

        pvseries->updControl("PvOscBank/ob/mrs_bool/phaselock", true);
      }

      else
        pvseries->updControl("PvUnconvert/uconv/mrs_bool/phaselock", true);
    }


    pvseries->tick();

    numticks++;

    if (!microphone_)
      if (pvseries->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() == false)
        break;


    mrs_bool onset_found = false;

    if (onsetsfile_ != "")
    {
      for (size_t j=0; j < onsets.size(); j++)
      {

        if (numticks == onsets[j])
        {
          onset_found = true;
          break;
        }
      }

      if (onset_found)
      {
        pvseries->updControl("PvUnconvert/uconv/mrs_bool/phaselock", true);
        pvseries->updControl("PvUnconvert/uconv/mrs_natural/Interpolation",D);
        pvseries->updControl("PvOverlapadd/pover/mrs_natural/Interpolation",D);
        pvseries->updControl("ShiftOutput/so/mrs_natural/Interpolation", D);
      }
      else
      {
        pvseries->updControl("PvUnconvert/uconv/mrs_natural/Interpolation", I);
        pvseries->updControl("PvOverlapadd/pover/mrs_natural/Interpolation",I);
        pvseries->updControl("ShiftOutput/so/mrs_natural/Interpolation", I);
      }
    }
  }

  // MATLAB_CLOSE();

}

void
phasevocPoly(string sfName, mrs_natural N, mrs_natural Nw,
             mrs_natural D, mrs_natural I, mrs_real P,
             string outsfname)
{
  cout << "Phasevocoder::polyphonic" << endl;

  MarSystemManager mng;

  MarSystem* total = mng.create("Series", "total");
  MarSystem* mixer = mng.create("Fanout", "mixer");

  // Use the prototype for the Composite Phasevocoder
  // Check phasevocSeries for the components
  // or look at the MarSystemManager code

  // vector of phasevocoders
  vector<MarSystem*> pvoices;
  for (int i=0; i < vopt_; ++i)
  {
    ostringstream oss;
    oss << "pvseries" << i;
    pvoices.push_back(mng.create("PhaseVocoder", oss.str()));

    // one-to-many mapping that under the hood
    // utilizes linkControl when the phasevocoder
    // prototype is added to the MarSystemManager
    pvoices[i]->updControl("mrs_natural/Decimation", D);
    pvoices[i]->updControl("mrs_natural/winSize", Nw);
    pvoices[i]->updControl("mrs_natural/FFTSize", N);
    pvoices[i]->updControl("mrs_natural/Interpolation", I);
    pvoices[i]->updControl("mrs_real/PitchShift", P);
    pvoices[i]->updControl("mrs_natural/Sinusoids", sopt);
    pvoices[i]->updControl("mrs_real/gain", gopt_);
    mixer->addMarSystem(pvoices[i]);
  }

  if (microphone_)
    total->addMarSystem(mng.create("AudioSource", "src"));
  else
    total->addMarSystem(mng.create("SoundFileSource", "src"));

  total->addMarSystem(mixer);
  total->addMarSystem(mng.create("Sum", "sum"));

  if (outsfname == EMPTYSTRING)
  {
    total->addMarSystem(mng.create("AudioSink", "dest"));
    total->updControl("AudioSink/dest/mrs_natural/bufferSize", bopt);
  }
  else
  {
    total->addMarSystem(mng.create("SoundFileSink","dest"));
    total->updControl("SoundFileSink/dest/mrs_string/filename", outsfname);
  }

  if (microphone_)
  {
    total->updControl("mrs_natural/inSamples", D);
    total->updControl("mrs_natural/inObservations", 1);
  }
  else
  {
    total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
    total->updControl("mrs_natural/inSamples", D);
    total->updControl("mrs_natural/inObservations", 1);

    // if audio output loop to infinity and beyond
    if (outsfname == EMPTYSTRING)
      total->updControl("SoundFileSource/src/mrs_real/repetitions", -1.0);
  }

  //int type;
  string cname;

#ifdef MARSYAS_MIDIIO
  RtMidiIn *midiin = NULL;
  //double stamp;
#endif

  int byte2, byte3;
  mrs_real diff;

  // used to keep track of polyphony
  vector<int> voices;
  for (int i=0; i < vopt_; ++i)
  {
    voices.push_back(60);
  }

#ifdef MARSYAS_MIDIIO
  if (midi_ != -1)
  {
    try {
      midiin = new RtMidiIn();
    }
    catch (RtError &error) {
      error.printMessage();
      exit(1);
    }
    try {
      midiin->openPort(midi_);
    }
    catch (RtError &error) {
      error.printMessage();
      exit(1);
    }
  }
#endif

  std::vector<unsigned char> message;
  int voiceCount =0;

  while(1)
  {
    if (!auto_)
    {
      if (midi_ != -1)
      {
#ifdef MARSYAS_MIDIIO
        //stamp = midiin->getMessage( &message );
#endif
        size_t nBytes = message.size();
        if (nBytes >2)
        {
          byte3 = message[2];
          byte2 = message[1];
          //type = message[0];

          if (byte3 != 0)
          {
            voices[voiceCount] = byte2;
            voiceCount = (voiceCount + 1) % vopt_;

            for (int i=0; i < vopt_; ++i)
            {
              diff = voices[i] - 60.0;
              if (voices[i] != 0)
                pvoices[i]->updControl("mrs_real/PitchShift",
                                       pow((double)1.06, (double)diff));
              // cout << pow((double) 1.06, (double) diff) << endl;
            }
          }
        }
      }
    }
    total->tick();
  }
}

void
phasevocCrossSynth(string sfName, string sfName2, mrs_natural N, mrs_natural Nw,
                   mrs_natural D, mrs_natural I, mrs_real P,
                   string outsfname)
{
  cout << "PHASE-VOCODER CROSS-SYNTHESIS MAG/PHASE" << endl;
  MarSystemManager mng;

  // build cross-synthesis network
  MarSystem* total = mng.create("Series", "total");
  MarSystem* pvfan = mng.create("Fanout", "pvfan");

  MarSystem* branch1 = mng.create("Series", "branch1");
  branch1->addMarSystem(mng.create("SoundFileSource","src1"));
  branch1->addMarSystem(mng.create("ShiftInput", "si1"));
  branch1->addMarSystem(mng.create("PvFold", "fo1"));
  branch1->addMarSystem(mng.create("Spectrum", "spk1"));
  branch1->addMarSystem(mng.create("Gain", "gain1"));
  branch1->updControl("Gain/gain/mrs_real/gain1", 1.0);

  MarSystem* branch2 = mng.create("Series", "branch2");
  branch2->addMarSystem(mng.create("SoundFileSource","src2"));
  branch2->addMarSystem(mng.create("ShiftInput", "si2"));
  branch2->addMarSystem(mng.create("PvFold", "fo2"));
  branch2->addMarSystem(mng.create("Spectrum", "spk2"));
  branch1->addMarSystem(mng.create("Gain", "gain2"));
  branch1->updControl("Gain/gain/mrs_real/gain2", 1.0);

  pvfan->addMarSystem(branch1);
  pvfan->addMarSystem(branch2);

  total->addMarSystem(pvfan);
  total->addMarSystem(mng.create("PvConvolve", "pvconv"));
  total->addMarSystem(mng.create("PvConvert",  "conv"));
  total->addMarSystem(mng.create("PvOscBank",  "ob"));
  total->addMarSystem(mng.create("ShiftOutput", "so"));
  total->addMarSystem(mng.create("SoundFileSink", "sdest"));
  total->addMarSystem(mng.create("Gain", "destgain"));
  total->addMarSystem(mng.create("AudioSink", "dest"));

  // link controls
  total->linkControl("mrs_string/filename1",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_string/filename");

  total->linkControl("mrs_string/filename2",
                     "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_string/filename");

  total->linkControl("mrs_real/repetitions",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_real/repetitions");

  total->linkControl("mrs_real/repetitions",
                     "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_real/repetitions");

  total->linkControl("mrs_natural/inSamples1",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_natural/inSamples");

  total->linkControl("mrs_natural/inSamples2",
                     "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_natural/inSamples");





  total->linkControl("mrs_natural/Decimation",
                     "PvConvert/conv/mrs_natural/Decimation");
  total->linkControl("mrs_natural/Sinusoids",
                     "PvConvert/conv/mrs_natural/Sinusoids");

  total->linkControl("mrs_natural/FFTSize",
                     "Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/FFTSize");
  total->linkControl("mrs_natural/FFTSize",
                     "Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/FFTSize");

  total->linkControl("mrs_natural/winSize",
                     "Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/winSize");
  total->linkControl("mrs_natural/winSize",
                     "Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/winSize");

  total->linkControl("mrs_natural/Interpolation",
                     "ShiftOutput/so/mrs_natural/Interpolation");

  total->linkControl("mrs_natural/Interpolation",
                     "PvOscBank/ob/mrs_natural/Interpolation");

  total->linkControl("mrs_real/PitchShift",
                     "PvOscBank/ob/mrs_real/PitchShift");

  // update controls
  total->updControl("mrs_string/filename1", sfName);
  total->updControl("mrs_string/filename2", sfName2);
  total->updControl("mrs_real/repetitions", -1.0);
  total->updControl("mrs_natural/inSamples1", D);
  total->updControl("mrs_natural/inSamples2", D);
  total->updControl("mrs_natural/Decimation", D);
  total->updControl("mrs_natural/winSize", Nw);
  total->updControl("mrs_natural/FFTSize", N);
  total->updControl("mrs_natural/Interpolation", I);
  total->updControl("mrs_real/PitchShift", P);
  total->updControl("mrs_natural/Sinusoids", sopt);
  total->updControl("SoundFileSink/sdest/mrs_string/filename", outsfname);
  total->updControl("Gain/destgain/mrs_real/gain", gopt_);

  while(1)
  {
    total->tick();
  }

}

void
phasevocConvolve(string sfName, mrs_natural N, mrs_natural Nw,
                 mrs_natural D, mrs_natural I, mrs_real P,
                 string outsfname)
{
  cout << "PHASE-VOCODER CROSS-SYNTHESIS WITH CONVOLUTION" << endl;
  MarSystemManager mng;

  // build cross-synthesis network
  MarSystem* total = mng.create("Series", "total");
  MarSystem* pvfan = mng.create("Fanout", "pvfan");

  MarSystem* branch1 = mng.create("Series", "branch1");
  branch1->addMarSystem(mng.create("SoundFileSource","src1"));
  branch1->addMarSystem(mng.create("ShiftInput", "si1"));
  branch1->addMarSystem(mng.create("PvFold", "fo1"));
  branch1->addMarSystem(mng.create("Spectrum", "spk1"));
  branch1->addMarSystem(mng.create("Gain", "gain"));
  branch1->updControl("Gain/gain/mrs_real/gain", 0.25);


  MarSystem* branch2 = mng.create("Series", "branch2");
  // branch2->addMarSystem(mng.create("SoundFileSource","src2"));
  branch2->addMarSystem(mng.create("AudioSource","src2"));

  branch2->addMarSystem(mng.create("ShiftInput", "si2"));
  branch2->addMarSystem(mng.create("PvFold", "fo2"));
  branch2->addMarSystem(mng.create("Spectrum", "spk2"));

  pvfan->addMarSystem(branch1);
  pvfan->addMarSystem(branch2);

  total->addMarSystem(pvfan);
  total->addMarSystem(mng.create("PvConvolve", "pvconv"));
  total->addMarSystem(mng.create("PvConvert",  "conv"));
  total->addMarSystem(mng.create("PvOscBank",  "ob"));
  total->addMarSystem(mng.create("ShiftOutput", "so"));
  total->addMarSystem(mng.create("SoundFileSink", "sdest"));
  total->addMarSystem(mng.create("Gain", "destgain"));
  total->addMarSystem(mng.create("AudioSink", "dest"));

  // link controls
  total->linkControl("mrs_string/filename1",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_string/filename");

  // total->linkControl("mrs_string/filename2",
  // "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_string/filename");

  total->linkControl("mrs_real/repetitions",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_real/repetitions");

  // total->linkControl("mrs_real/repetitions",
  // "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_real/repetitions");

  total->linkControl("mrs_natural/inSamples1",
                     "Fanout/pvfan/Series/branch1/SoundFileSource/src1/mrs_natural/inSamples");

  // total->linkControl("mrs_natural/inSamples2",
  // "Fanout/pvfan/Series/branch2/SoundFileSource/src2/mrs_natural/inSamples");

  total->linkControl("mrs_natural/inSamples2",
                     "Fanout/pvfan/Series/branch2/AudioSource/src2/mrs_natural/inSamples");


  total->linkControl("mrs_natural/Decimation",
                     "Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/Decimation");


  total->linkControl("mrs_natural/Decimation",
                     "Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/Decimation");

  total->linkControl("mrs_natural/Decimation",
                     "PvConvert/conv/mrs_natural/Decimation");

  total->linkControl("mrs_natural/Sinusoids",
                     "PvConvert/conv/mrs_natural/Sinusoids");

  total->linkControl("mrs_natural/FFTSize",
                     "Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/FFTSize");

  total->linkControl("mrs_natural/FFTSize",
                     "Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/FFTSize");

  total->linkControl("mrs_natural/winSize",
                     "Fanout/pvfan/Series/branch1/PvFold/fo1/mrs_natural/winSize");

  total->linkControl("mrs_natural/winSize",
                     "Fanout/pvfan/Series/branch2/PvFold/fo2/mrs_natural/winSize");

  total->linkControl("mrs_natural/Interpolation",
                     "ShiftOutput/so/mrs_natural/Interpolation");

  total->linkControl("mrs_natural/Interpolation",
                     "PvOscBank/ob/mrs_natural/Interpolation");

  total->linkControl("mrs_real/PitchShift",
                     "PvOscBank/ob/mrs_real/PitchShift");

  // update controls
  total->updControl("mrs_string/filename1", sfName);
  // total->updControl("mrs_string/filename2", "/home/gtzan/data/sound/music_speech/music/gravity.au");
  total->updControl("mrs_real/repetitions", -1.0);
  total->updControl("mrs_natural/inSamples1", D);
  total->updControl("mrs_natural/inSamples2", D);
  total->updControl("mrs_natural/Decimation", D);
  total->updControl("mrs_natural/winSize", Nw);
  total->updControl("mrs_natural/FFTSize", N);
  total->updControl("mrs_natural/Interpolation", I);
  total->updControl("mrs_real/PitchShift", P);
  total->updControl("mrs_natural/Sinusoids", sopt);
  total->updControl("SoundFileSink/sdest/mrs_string/filename", outsfname);
  total->updControl("Gain/destgain/mrs_real/gain", gopt_);

  while(1)
  {
    total->tick();
  }
}

// phasevocoder variant for Jan 2006 event
// at Royal BC Museum with Andy playing the radio drum

void
phasevocHeterophonicsRadioDrum(string sfName1, string sfName2, mrs_natural N,
                               mrs_natural Nw,
                               mrs_natural D, mrs_natural I, mrs_real P,
                               string outsfname)
{
  (void) P;

  cout << "HETEROPHONICS - RADIODRUM " << endl;

  vopt_ = 2;
  cout << "sfName1 = " << sfName1 << endl;
  cout << "sfName2 = " << sfName2 << endl;

  MarSystemManager mng;
  MarSystem* total = mng.create("Series", "total");

  //MarSystem* mic;
  //mic = mng.create("SoundFileSource", "mic");
  // total->addMarSystem(mic);
  // total->updControl("SoundFileSource/mic/mrs_string/filename", sfName);

  MarSystem* mixer = mng.create("Fanout", "mixer");

  MarSystem *bpvoc0 = mng.create("PhaseVocoder", "bpvoc0");
  MarSystem* bpvoc1 = mng.create("PhaseVocoder", "bpvoc1");

  // vector of voices
  vector<MarSystem*> pvoices;
  for (int i=0; i < vopt_; ++i)
  {
    if (i == 0)
    {
      pvoices.push_back(mng.create("Series", "background0"));
      pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_string/filename", sfName1);
      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);

      pvoices[i]->addMarSystem(mng.create("Gain", "bgain0"));
      pvoices[i]->addMarSystem(bpvoc0);
      pvoices[i]->updControl("Gain/bgain0/mrs_real/gain", 1.0);
      bpvoc0->updControl("mrs_natural/Decimation", D);
      bpvoc0->updControl("mrs_natural/winSize", Nw);
      bpvoc0->updControl("mrs_natural/FFTSize", N);
      bpvoc0->updControl("mrs_natural/Interpolation", I);
      bpvoc0->updControl("mrs_real/PitchShift", 1.0);
      bpvoc0->updControl("mrs_natural/Sinusoids", sopt);
      mixer->addMarSystem(pvoices[i]);
    }
    else if (i==1)
    {
      pvoices.push_back(mng.create("Series", "background1"));
      pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_string/filename", sfName2);

      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);

      pvoices[i]->addMarSystem(mng.create("Gain", "bgain1"));
      pvoices[i]->addMarSystem(bpvoc1);
      pvoices[i]->updControl("Gain/bgain1/mrs_real/gain", 1.0);
      bpvoc1->updControl("mrs_natural/Decimation", D);
      bpvoc1->updControl("mrs_natural/winSize", Nw);
      bpvoc1->updControl("mrs_natural/FFTSize", N);
      bpvoc1->updControl("mrs_natural/Interpolation", I);
      bpvoc1->updControl("mrs_real/PitchShift", 1.0);
      bpvoc1->updControl("mrs_natural/Sinusoids", sopt);
      mixer->addMarSystem(pvoices[i]);
    }
  }

  total->addMarSystem(mixer);
  total->addMarSystem(mng.create("Sum", "sum"));

  if (outsfname == EMPTYSTRING)
  {
    total->addMarSystem(mng.create("AudioSink", "dest"));
    total->updControl("AudioSink/dest/mrs_natural/bufferSize", bopt);
  }
  else
  {

    total->addMarSystem(mng.create("SoundFileSink","dest"));
    total->updControl("SoundFileSink/dest/mrs_string/filename", outsfname);
  }

  int type;
  string cname;

  //mrs_real diff;
  //mrs_natural fc;
  //fc = 0;
  //mrs_real time;
  //time = 0.0;
  //mrs_real epsilon;
  //epsilon = 0.0029024;

  //diff = 0.0;

  //bool trigger;
  //trigger = true;

  total->updControl("mrs_natural/inSamples", D);

  cout << "Ready to start processing " << endl;

#ifdef MARSYAS_MIDIIO
  RtMidiIn *midiin = NULL;
  // open midi if midiPort is specified
  if (midi_ != -1)
  {
    try
    {
      midiin = new RtMidiIn();
    }
    catch (RtError &error)
    {
      error.printMessage();
      exit(1);
    }
    try
    {
      midiin->openPort(midi_);
    }
    catch (RtError &error)
    {
      error.printMessage();
      exit(1);
    }
  }
#endif

  // midi message
  std::vector<unsigned char> message;
  int byte2, byte3;
  double s1x;
  double s1y;
  double s1z;

  double s2x;
  double s2y;
  double s2z;

  while(1)
  {
    if (midi_ != -1)
    {
      size_t nBytes = message.size();
      if (nBytes >2)
      {
        byte3 = message[2];
        byte2 = message[1];
        type = message[0];

        if ((type == 160)&&(byte2 == 1))
        {
          pvoices[0]->updControl("SoundFileSource/mixsrc/mrs_bool/advance", true);

          cout << "s1 whack" << endl;

        }
        if ((type == 160)&&(byte2 == 8))
        {
          s1x = byte3 / 128.0;

          bpvoc0->updControl("mrs_natural/Sinusoids", (mrs_natural) (s1x * sopt)+1);
        }
        if  ((type == 160)&&(byte2 == 9))
        {
          s1y = byte3 / 128.0;
          bpvoc0->updControl("mrs_real/PitchShift", (mrs_real) 0.5  + s1y * 1.5);
        }
        if  ((type == 160)&&(byte2 == 10))
        {
          s1z = (byte3-14.0) / 128.0;
          pvoices[0]->updControl("Gain/bgain0/mrs_real/gain", s1z);
        }
        if ((type == 160)&&(byte2 == 2))
        {
          cout << "s2 whack" << endl;
          pvoices[1]->updControl("SoundFileSource/mixsrc/mrs_bool/advance", true);
        }
        if ((type == 160)&&(byte2 == 11))
        {
          s2x = byte3 / 128.0;
          bpvoc1->updControl("mrs_natural/Sinusoids", (mrs_natural) (s2x * sopt)+1);
        }
        if  ((type == 160)&&(byte2 == 12))
        {
          s2y = byte3 / 128.0;
          bpvoc1->updControl("mrs_real/PitchShift", (mrs_real) 0.5  + s2y * 1.5);
        }
        if  ((type == 160)&&(byte2 == 13))
        {
          s2z = (byte3 -14) / 128.0;
          pvoices[1]->updControl("Gain/bgain1/mrs_real/gain", s2z);
        }
      }
    }
    // play the sound
    total->tick();
  }
}

// phasevocoder variant for November 2005 concert
// at UVic

void
phasevocHeterophonics(string sfName, mrs_natural N, mrs_natural Nw,
                      mrs_natural D, mrs_natural I, mrs_real P,
                      string outsfname)
{

  cout << "HETEROPHONICS " << endl;

  // Heterophonics contains piece-specific code
  // for a piece performed in November 2005 at UVic
  // It combines a polyphonic phasevocoder
  // with real-time pitch tracking

  // hardwire 3 voices
  // the first two a pitch shifted version of the input
  // the third is pitch shifted background material
  // load from various soundfiles
  vopt_ = 4;

  MarSystemManager mng;
  MarSystem* total = mng.create("Series", "total");
  MarSystem* mixer = mng.create("Fanout", "mixer");
  MarSystem* bpvoc = mng.create("PhaseVocoder", "bpvoc");

  // vector of voices
  vector<MarSystem*> pvoices;
  for (int i=0; i < vopt_; ++i)
  {
    if (i < 2)
    {
      ostringstream oss;
      oss << "pvseries" << i;
      pvoices.push_back(mng.create("PhaseVocoder", oss.str()));
      pvoices[i]->updControl("mrs_natural/Decimation", D);
      pvoices[i]->updControl("mrs_natural/winSize", Nw);
      pvoices[i]->updControl("mrs_natural/FFTSize", N);
      pvoices[i]->updControl("mrs_natural/Interpolation", I);
      pvoices[i]->updControl("mrs_real/PitchShift", P);
      pvoices[i]->updControl("mrs_natural/Sinusoids", sopt);
      pvoices[i]->updControl("mrs_real/gain", gopt_);
      mixer->addMarSystem(pvoices[i]);
    }
    else if (i==2)
    {
      pvoices.push_back(mng.create("Series", "background"));
      pvoices[i]->addMarSystem(mng.create("SoundFileSource", "mixsrc"));
      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_string/filename", sfName);
      pvoices[i]->updControl("SoundFileSource/mixsrc/mrs_real/repetitions", -1.0);
      bpvoc = mng.create("PhaseVocoder", "bpvoc");
      pvoices[i]->addMarSystem(mng.create("Gain", "bgain"));
      pvoices[i]->addMarSystem(bpvoc);
      pvoices[i]->updControl("Gain/bgain/mrs_real/gain", 1.0);
      bpvoc->updControl("mrs_natural/Decimation", D);
      bpvoc->updControl("mrs_natural/winSize", Nw);
      bpvoc->updControl("mrs_natural/FFTSize", N);
      bpvoc->updControl("mrs_natural/Interpolation", I);
      bpvoc->updControl("mrs_real/PitchShift", 1.0);
      bpvoc->updControl("mrs_natural/Sinusoids", sopt);
      mixer->addMarSystem(pvoices[i]);
    }
    else if (i==3)
    {
      pvoices.push_back(mng.create("SoundFileSource", "osrc"));
      pvoices[i]->updControl("mrs_string/filename", sfName);
      mixer->addMarSystem(pvoices[i]);
    }
  }

  MarSystem* mic;
  mic = mng.create("AudioSource", "mic");
  total->addMarSystem(mixer);
  total->addMarSystem(mng.create("Sum", "sum"));

  if (outsfname == EMPTYSTRING)
  {
    total->addMarSystem(mng.create("AudioSink", "dest"));
    total->updControl("AudioSink/dest/mrs_natural/bufferSize", bopt);
  }
  else
  {

    total->addMarSystem(mng.create("SoundFileSink","dest"));
    total->updControl("SoundFileSink/dest/mrs_string/filename", outsfname);
  }

  // vectors used for sharing between phasevocoder
  // network and pitch extraction network
  realvec in, min, out, pin;

  mic->updControl("mrs_natural/inSamples", D);
  mic->updControl("mrs_natural/inObservations", 1);
  total->updControl("mrs_natural/inSamples", D);
  total->updControl("mrs_natural/inObservations", 1);

  in.create( (long)1, (long)D);
  min.create((long)1, (long)D);
  out.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
             total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  pin.create((long)1, (long)512);

  string cname;
  mrs_real diff;
  mrs_natural fc = 0;

  // Build the pitch extractor network
  MarSystem* pitchExtractor = mng.create("Series", "pitchExtractor");
  pitchExtractor->addMarSystem(mng.create("AutoCorrelation", "acr"));
  pitchExtractor->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.67);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));

  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Gain", "id1"));
  fanout->addMarSystem(mng.create("TimeStretch", "tsc"));
  pitchExtractor->addMarSystem(fanout);

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("Gain", "id2"));
  fanin->addMarSystem(mng.create("Negative", "nid"));

  pitchExtractor->addMarSystem(fanin);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  pitchExtractor->addMarSystem(mng.create("Peaker", "pkr"));
  pitchExtractor->addMarSystem(mng.create("MaxArgMax", "mxr"));

  // update controls
  pitchExtractor->updControl("mrs_natural/inSamples", 512);
  pitchExtractor->updControl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5);

  // Convert pitch bounds to samples
  mrs_natural lowPitch = 32;
  mrs_natural highPitch = 100;

  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);
  mrs_natural lowSamples =
    hertz2samples(highFreq, 22050.0);
  mrs_natural highSamples =
    hertz2samples(lowFreq, 22050.0);
  pitchExtractor->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
  pitchExtractor->updControl("Peaker/pkr/mrs_real/peakStrength", 0.5);
  pitchExtractor->updControl("Peaker/pkr/mrs_natural/peakStart", lowSamples);
  pitchExtractor->updControl("Peaker/pkr/mrs_natural/peakEnd", highSamples);
  pitchExtractor->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);

  MarSystem* gainExtractor = mng.create("MaxArgMax", "mgain");
  gainExtractor->updControl("mrs_natural/inSamples", 512);

  realvec pitchres(pitchExtractor->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), pitchExtractor->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  realvec mgres(gainExtractor->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                gainExtractor->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_real pitch = 0.0;
  //mrs_real prev_pitch = 0.0;

  //mrs_real glide0 = 0.0;
  mrs_real glide1 = 0.0;

  mrs_real time = 0.0;
  //mrs_real epsilon;
  //epsilon  = 0.0029024;

  mrs_natural epoch =0;
  diff = 0.0;

  //bool trigger;
  //trigger = true;

  pvoices[0]->updControl("mrs_real/gain", 0.0);
  pvoices[1]->updControl("mrs_real/gain", 0.0);

  vector<string> tablas;
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Ge2.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Ge3_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/ke1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na10_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Dhi1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na10_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/tun1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/na7.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/te10.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/Dha12_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/tun1_s1.wav");
  tablas.push_back("/home/gtzan/data/sound/Nov2005Concert/tabla/te12_s1.wav");

  mrs_natural note, prev_note = 0;

  mrs_natural noteCount = 0;
  mrs_natural sealCount = 0;

  while(1)
  {
    time += 0.0029024;

    if ((pitch >= 1000) && (pitch<= 1050) &&
        (pitchres(0) > 0.5) &&
        (epoch == 0))
    {
      cout << "Epoch " << epoch << ": Whole-tone descending - Middle C"  << endl;
      epoch++;
    }

    if ((pitch >= 460.0) &&
        (pitch <= 480.0) &&
        (pitchres(0) > 0.5) &&
        (epoch == 1))
    {
      cout << "Epoch " << epoch << ": Harmonizer - Ab" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.30);
      pvoices[1]->updControl("mrs_real/gain", 0.30);
      pvoices[0]->updControl("mrs_real/PitchShift", 0.25);
      pvoices[1]->updControl("mrs_real/PitchShift", pow((double)1.06, (double)-17.0));
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
      pvoices[3]->updControl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    if ((pitch >= 750.0) &&
        (pitch <= 770.0) &&
        (pitchres(0) > 0.5) &&
        (epoch == 2))
    {
      cout << "Epoch " << epoch << ": Speech manipulation high D" << endl;
      epoch ++;

      //glide0 = 0.25;
      glide1 = 1.0;
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/rainSS.wav");
    }

    if ((pitch >= 1000) && (pitch<= 1050) &&
        (pitchres(0) > 0.5) &&
        (epoch == 3))
    {
      cout << "Epoch " << epoch << ": Greek Folk solo middle B" << endl;
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);
      epoch ++;
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    if ((pitch >= 435) && (pitch<= 455) &&
        (pitchres(0) > 0.5) &&
        (epoch == 4))
    {
      cout << "Epoch " << epoch << ": Greek Folk solo + tabla : Middle Bb" << endl;
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);
      epoch++;
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    if ((pitch >= 415) && (pitch<= 430) &&
        (pitchres(0) > 0.5) &&
        (epoch == 5))
    {
      cout << "Epoch " << epoch << ": Seashell calls - Low F#" << endl;
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);

      epoch ++;
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    if ((pitch >= 320) && (pitch<= 330) &&
        (pitchres(0) > 0.5) &&
        (epoch == 6))

    {
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);
      cout << "Epoch " << epoch << ": Orchestra manipulation : High D" << endl;
      epoch ++;
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/storm1.wav");
      bpvoc->updControl("mrs_real/PitchShift", 1.0);
    }

    if ((pitch >= 1000) && (pitch<= 1050) &&
        (pitchres(0) > 0.5) &&
        (epoch == 7))
    {
      cout << "Epoch " << epoch << ": Glissanti : Low Eb" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.20);
      pvoices[1]->updControl("mrs_real/gain", 0.20);
      glide1 = 1.0;
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    if ((pitch >= 270) && (pitch<= 280) &&
        (pitchres(0) > 0.5) &&
        (epoch == 8))
    {
      cout << "Epoch " << epoch << ": Seals High D" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.75);
      pvoices[1]->updControl("mrs_real/gain", 0.75);
      pvoices[0]->updControl("mrs_real/PitchShift", 0.558395);
      pvoices[1]->updControl("mrs_real/PitchShift", 0.747259);
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/silence.wav");
      pvoices[3]->updControl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/seal.wav");
      sealCount = 0;
    }

    if ((pitch >= 1000) && (pitch<= 1050) &&
        (pitchres(0) > 0.5) &&
        (sealCount > 1000) &&
        (epoch == 9))
    {
      cout << "Epoch " << epoch << ": Storm : Low Eb" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.75);
      pvoices[1]->updControl("mrs_real/gain", 0.75);
      pvoices[0]->updControl("mrs_real/PitchShift", 0.558395);
      pvoices[1]->updControl("mrs_real/PitchShift", 0.747259);
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
      bpvoc->updControl("mrs_real/PitchShift", 1.0);
      pvoices[2]->updControl("Gain/bgain/mrs_real/gain", 1.0);
      pvoices[3]->updControl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/storm2.wav");
      pvoices[3]->updControl("mrs_real/repetitions", -1.0);
    }

    if ((pitch >= 270) && (pitch<= 280) &&
        (pitchres(0) > 0.5) &&
        (epoch == 10))
    {
      cout << "Epoch " << epoch << ": Drips : Solo sax F# Low" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.5);
      pvoices[1]->updControl("mrs_real/gain", 0.5);
      pvoices[0]->updControl("mrs_real/PitchShift", pow((double)1.06, (double)7.04));
      pvoices[1]->updControl("mrs_real/PitchShift", pow((double)1.06, (double)10.0));
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/drip03.wav");
      bpvoc->updControl("mrs_real/PitchShift", 1.0);
      pvoices[2]->updControl("Gain/bgain/mrs_real/gain", 1.0);
      pvoices[3]->updControl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/howl2.wav");
    }

    if ((pitch >= 320) && (pitch<= 330) &&
        (pitchres(0) > 0.5) &&
        (epoch == 11))
    {
      cout << "Epoch " << epoch << ": Finale" << endl;
      epoch ++;
      pvoices[0]->updControl("mrs_real/gain", 0.0);
      pvoices[1]->updControl("mrs_real/gain", 0.0);
      pvoices[0]->updControl("mrs_real/PitchShift", 1.0);
      pvoices[1]->updControl("mrs_real/PitchShift", 1.0);
      pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
      bpvoc->updControl("mrs_real/PitchShift", 1.0);
      pvoices[2]->updControl("Gain/bgain/mrs_real/gain", 0.0);
      pvoices[3]->updControl("mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/silence.wav");
    }

    // read input for microphone 64 samples
    mic->process(in, min);

    // accumulte in buffers of 512 for pitch extraction
    for (int i=0; i < D; ++i)
      pin(0, fc * D + i) = min(0,i);

    // play the sound
    total->process(min, out);

    if (fc == 7)
    {
      // extract pitch
      pitchExtractor->process(pin, pitchres);
      // extract gain
      gainExtractor->process(pin, mgres);

      pitch = samples2hertz((mrs_natural)pitchres(1), 22050.0);
      note = (mrs_natural)hertz2pitch(pitch);

      if (epoch == 3)
      {
        diff = hertz2pitch(pitch) - 79.0;
        bpvoc->updControl("mrs_real/PitchShift", pow((double)1.06, (double)diff));
        pvoices[2]->updControl("Gain/bgain/mrs_real/gain", 2.5 * mgres(0));
      }

      if (epoch == 5)
      {
        if ((note != prev_note)&&(pitchres(0) > 0.5)&&(noteCount > 4))
        {
          mrs_natural tablaIndex = note % 12;
          pvoices[3]->updControl("mrs_string/filename", tablas[tablaIndex]);
          noteCount = 0;
        }

      }

      if (epoch == 7)
      {
        if (pitchres(0) > 0.5)
        {
          diff = hertz2pitch(pitch) - 64.0;
          bpvoc->updControl("mrs_real/PitchShift", pow((double)1.06, (double)diff));
        }
        pvoices[2]->updControl("Gain/bgain/mrs_real/gain", 2.0 * mgres(0));
      }

      if (epoch == 8)
      {
        if ((note != prev_note)&&(pitchres(0) > 0.5))
        {
          glide1 = 1.0;
        }

        diff = hertz2pitch(pitch) - 79.0;
        pvoices[0]->updControl("mrs_real/PitchShift", 0.25);
        pvoices[1]->updControl("mrs_real/gain", 1.0);
        pvoices[1]->updControl("mrs_real/PitchShift", glide1);
        glide1 -= 0.01;
        if (glide1 < 0.125)
          glide1 = 1.0;
      }

      if (epoch == 9)
      {
        sealCount++;
        if (sealCount == 2000)
          cout << "Ready for high D" << endl;

      }

      if (epoch == 11)
      {

        if ((note != prev_note)&&(pitchres(0) > 0.5))
        {
          pvoices[2]->updControl("SoundFileSource/mixsrc/mrs_string/filename", "/home/gtzan/data/sound/Nov2005Concert/drip03.wav");

        }
        pvoices[2]->updControl("Gain/bgain/mrs_real/gain", mgres(0));
      }

      //prev_pitch = pitch;
      prev_note = note;
      noteCount++;
    }
    fc = (fc + 1) % 8;
  }
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("quiet", "q", false);
  cmd_options.addNaturalOption("voices", "v", 1);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addNaturalOption("winsize", "w", winSize_);
  cmd_options.addNaturalOption("fftsize", "n", fftSize_);
  cmd_options.addNaturalOption("decimation", "d", dopt);
  cmd_options.addNaturalOption("interpolation", "i", iopt);
  cmd_options.addNaturalOption("sinusoids", "s", sopt);
  cmd_options.addNaturalOption("bufferSize", "b", bopt);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("pitchshift", "p", 1.0);
  cmd_options.addBoolOption("auto", "a", auto_);
  cmd_options.addNaturalOption("midi", "m", midi_);
  cmd_options.addNaturalOption("epochHeterophonics", "e", eopt_);
  cmd_options.addBoolOption("oscbank", "ob", oscbank_);
  cmd_options.addStringOption("convertmode", "cm", convertmode_);
  cmd_options.addStringOption("onsets", "on", onsetsfile_);
  cmd_options.addStringOption("unconvertmode", "ucm", unconvertmode_);
  // cmd_options.addBoolOption("multires", "mr", multires_);
  // cmd_options.addStringOption("multiresMode", "mrm", multiresMode_);

}

void
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  quietopt_ = cmd_options.getBoolOption("quiet");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
  winSize_ = cmd_options.getNaturalOption("winsize");
  fftSize_ = cmd_options.getNaturalOption("fftsize");
  dopt = cmd_options.getNaturalOption("decimation");
  iopt = cmd_options.getNaturalOption("interpolation");
  sopt = cmd_options.getNaturalOption("sinusoids");
  bopt = cmd_options.getNaturalOption("bufferSize");
  popt = cmd_options.getRealOption("pitchshift");
  auto_ = cmd_options.getBoolOption("auto");
  oscbank_ = cmd_options.getBoolOption("oscbank");
  midi_ = cmd_options.getNaturalOption("midi");
  vopt_ = cmd_options.getNaturalOption("voices");
  gopt_ = cmd_options.getRealOption("gain");
  eopt_ = cmd_options.getNaturalOption("epochHeterophonics");
  convertmode_ = cmd_options.getStringOption("convertmode");
  unconvertmode_ = cmd_options.getStringOption("unconvertmode");
  onsetsfile_ = cmd_options.getStringOption("onsets");
  // multires_ = cmd_options.getBoolOption("multires");
  // multiresMode_ = cmd_options.getStringOption("multiresMode");
}

int
main(int argc, const char **argv)
{
  MRSDIAG("sftransform.cpp - main");

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  string progName = argv[0];

  if (helpopt_)
    return printHelp(progName);

  if (usageopt_)
    return printUsage(progName);

  if (!quietopt_)
  {
    cerr << "Phasevocoder configuration (-h show the options): " << endl;
    cerr << "fft size (-n)      = " << fftSize_ << endl;
    cerr << "win size (-w)      = " << winSize_ << endl;
    cerr << "decimation (-d)    = " << dopt << endl;
    cerr << "interpolation (-i) = " << iopt << endl;
    cerr << "pitch shift (-p)   = " << popt << endl;
    cerr << "sinusoids (-s)     = " << sopt << endl;
    cerr << "voices (-v)        = " << vopt_ << endl;
    cerr << "midiPort (-m)      = " << midi_ << endl;
    cerr << "outFile  (-f)      = " << fileName << endl;
  }

  int i =0;
  // soundfile input
  string sfname;
  string sfname2;
  if (soundfiles.size() != 0)
  {
    sfname = soundfiles[0];
    if (soundfiles.size() != 1)
    {
      sfname2 = soundfiles[1];
    }
    i = 1;
  }

  if (!quietopt_)
    cout << "Phasevocoding " << sfname << endl;

  if(i == 1)//sound file input
  {
    if (!quietopt_)
      cout << "Using sound file input" << endl;
    microphone_ = false;
    if (vopt_ == 1)
    {
      phasevocoder(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
    }
    else
    {
      if (eopt_ == 0)
        phasevocPoly(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
      else if (eopt_ == 1)
        phasevocHeterophonics(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
      else if (eopt_ == 2)
        phasevocConvolve(sfname, fftSize_, winSize_, dopt, iopt, popt, fileName);
      else if (eopt_ == 3)
        phasevocCrossSynth(sfname, sfname2, fftSize_, winSize_, dopt, iopt, popt, fileName);
      else if (eopt_ == 4)
      {
        string sfname1 = soundfiles[0];
        string sfname2 = soundfiles[1];
        phasevocHeterophonicsRadioDrum(sfname1, sfname2, fftSize_, winSize_, dopt, iopt, popt, fileName);
      }
      else
        cout << "Not supported heterophonics epoch" << endl;
    }

  }
  if (i == 0) //micophone input
  {
    if (!quietopt_)
      cout << "Using live microphone input" << endl;
    microphone_ = true;
    if (vopt_ == 1)
      phasevocSeriesOld("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else if (eopt_ ==0)
      phasevocPoly("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else if (eopt_ == 1)
      phasevocHeterophonics("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else if (eopt_ == 2)
      phasevocConvolve("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else if (eopt_ == 3)
      phasevocCrossSynth("microphone", "", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else if (eopt_ == 4)
      phasevocHeterophonicsRadioDrum("microphone", "microphone", fftSize_, winSize_, dopt, iopt, popt, fileName);
    else
      cout << "Not supported heterophonics epoch" << endl;
  }

  return (0);
}


