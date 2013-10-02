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



#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#else

#endif

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Conversions.h>
#include <marsyas/CommandLineOptions.h>

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;

int helpopt;
int usageopt;
mrs_natural bufferSizeopt;
mrs_natural inputSizeopt;
string backgroundopt;
bool verbose_opt;
string mappingopt;
string deviceopt;
mrs_natural portopt;

vector<string> tempo_map[11][11];



void midiInfo()
{
  cout << "MIDI INFO port = " << portopt << endl;

  std::vector<unsigned char> message;
  double stamp;
  int nBytes;
  int i;

#ifdef MARSYAS_MIDIIO
  RtMidiIn *midiin = NULL;
  try {
    midiin = new RtMidiIn();
  }
  catch (RtError3 &error) {
    error.printMessage();
    exit(1);
  }


  try {
    midiin->openPort(portopt);
  }
  catch (RtError3 &error) {
    error.printMessage();
    exit(1);

  }

  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );


  while(1)

  {

    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    if (nBytes >0)
    {

      for ( i=0; i<nBytes; i++ )
        std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
      if ( nBytes > 0 )
        std::cout << "stamp = " << stamp << '\n';
    }

    usleep(10);

  }

#endif

}

void newMidiInfo()
{
  /*
  This function describes using the MidiInput object and MarControlPointers
  to read midi controls in a loop. The disadvantage of this approach is that
  MidiINput will only return the last midi value between buffers. Change the
  buffer size you would like more precision and throughput for midi messages.
  */
  MarSystemManager mng;
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("AudioSource","src"));
  series->addMarSystem(mng.create("MidiInput","midiin"));

  series->updctrl("mrs_real/israte", 44100.0);
  series->updctrl("mrs_real/osrate", 44100.0);
  // to change how often marsyas grabs messages change the buffersize
  series->updctrl("AudioSource/src/mrs_natural/bufferSize", 64);
  series->updctrl("AudioSource/src/mrs_bool/initAudio",true);
  series->updctrl("MidiInput/midiin/mrs_bool/initmidi",true);

  MarControlPtr b1 = series->getctrl("MidiInput/midiin/mrs_natural/byte1");
  MarControlPtr b2 = series->getctrl("MidiInput/midiin/mrs_natural/byte2");
  MarControlPtr b3 = series->getctrl("MidiInput/midiin/mrs_natural/byte3");

  while(1)
  {
    const  mrs_natural& byte1 = b1->to<mrs_natural>();
    const  mrs_natural& byte2 = b2->to<mrs_natural>();
    const  mrs_natural& byte3 = b3->to<mrs_natural>();

    std::cout <<  "Byte 1: " << byte1 << "    Byte 2: " << byte2 << "    Byte 3: " << byte3 << endl;

    series->tick();
  }
}


//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void PluckLive(string deviceopt, mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret)
{



#ifdef MARSYAS_MIDIIO
  RtMidiIn *midiin = 0;
  std::vector<unsigned char> message;
  double stamp;
  int nBytes;
  int i;

  // initialize RtMidi
  try {
    midiin = new RtMidiIn();
  }
  catch (RtError3 &error) {
    error.printMessage();
    exit(1);
  }

  // find input midi port
  try {
    midiin->openPort(portopt);
  }
  catch (RtError3 &error) {
    error.printMessage();
    exit(1);

  }
  MarSystemManager mng;
  MarSystem* series = mng.create("Series", "series");


  // create 16 plucked Karplus-Strong strings
  MarSystem* mix  = mng.create("Fanout", "mix");
  for (mrs_natural i = 0; i < 16; i++)
  {
    ostringstream oss;
    oss << "src" << i;
    mix->addMarSystem(mng.create("Plucked", oss.str()));
  }

  series->addMarSystem(mix);
  series->addMarSystem(mng.create("Sum", "sum"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->update();


  series->updctrl("Gain/gain/mrs_real/gain", 0.10);
  series->updctrl("AudioSink/dest/mrs_real/israte",
                  series->getctrl("Fanout/mix/Plucked/src0/mrs_real/osrate"));

  series->updctrl("AudioSink/dest/mrs_natural/bufferSize", 128);
  series->updctrl("Fanout/mix/Plucked/src0/mrs_real/frequency",fre);
  series->updctrl("Fanout/mix/Plucked/src0/mrs_real/pluckpos",pos);
  series->updctrl("Fanout/mix/Plucked/src0/mrs_real/loss",loz);
  series->updctrl("mrs_natural/inSamples", 64);
  series->update();


  // initially only play one string
  for (int i=1; i < 16; i++)
  {
    ostringstream oss1;
    oss1 << "Fanout/mix/Plucked/src"
         << i << "/mrs_real/nton";
    series->updctrl(oss1.str(), 0.0);
  }



  mrs_natural t=0;

  int channel, type, byte2, byte3;
  int mes_count = 0;
  mrs_real freq;
  int p0byte3, p1byte3, p2byte3;


  // used to keep track of polyphony
  vector<int> voices;
  for (int i=0; i < 16; i++)
  {
    voices.push_back(0);
  }




  while(1)
  {

    // for (int i=0; i < 4; i++)
    // {
    stamp = midiin->getMessage( &message );
    // }

    nBytes = message.size();

    if (nBytes > 0)
    {


      byte3 = message[2];
      byte2 = message[1];
      type = message[0];

      if (deviceopt == "Keyboard")
      {

        if (type == 144)
        {

          // allocate voice
          for (int i=0; i < 16; i++)
          {
            if (voices[i] == 0)
            {
              voices[i] = byte2;
              break;
            }
          }
          // free voice if velocity is 0 (implicit noteoff)
          for (int i=0; i < 16; i++)
          {
            if ((byte3 == 0)&&(voices[i] == byte2))
            {
              voices[i] = 0;
              break;
            }
          }

          for (int i=0; i < 16; i++)
          {

            ostringstream oss, oss1;
            oss << "Fanout/mix/Plucked/src"
                << i << "/mrs_real/frequency";

            oss1 << "Fanout/mix/Plucked/src"
                 << i << "/mrs_real/nton";

            if (voices[i] != 0)
            {
              freq =  220.0 * pow( 2.0,(voices[i] - 57.0) / 12.0 );

              series->updctrl(oss1.str(), 1.0);

              series->updctrl(oss.str(),freq);
              // series->update();

            }
          }

        }

        if (type == 128)
        {
          // free voice if noteoff
          for (int i=0; i < 16; i++)
          {
            if (voices[i] == byte2)
            {

              ostringstream oss, oss1;

              oss1 << "Fanout/mix/Plucked/src"
                   << i << "/mrs_real/nton";


              series->updctrl(oss1.str(), 0.0);
              voices[i] = 0;
              break;
            }
          }
        }

      }


      if (deviceopt == "KiomB")
      {
        if (byte2 == 0)
        {
          freq =  220.0 * pow( 2.0, (byte3 - 57.0) / 12.0 );

          if ((byte3 > 25)&&(abs(byte3 - p0byte3) > 2))
          {
            series->updctrl("Fanout/mix/Plucked/src0/mrs_real/frequency",freq);
          }

        }
        p0byte3 = byte3;
      }



      if (byte2 == 1)
      {
        freq = 220.0 * pow( 2.0, (byte3 - 60.0) / 12.0 );
        if ((byte3 > 25)&&(abs(byte3 - p1byte3) > 2))
          series->updctrl("Fanout/mix/Plucked/src1/mrs_real/frequency",freq);

        p1byte3 = byte3;

      }


      if (byte2 == 2)
      {
        freq = 220.0 * pow( 2.0, (byte3 - 62.0) / 12.0 );
        if ((byte3 > 25)&&(abs(byte3 - p2byte3) > 2))
          series->updctrl("Fanout/mix/Plucked/src2/mrs_real/frequency",freq);

        p2byte3 = byte3;

      }
    }


    series->tick();
    t++;
  }


#endif


}


void midiBoomChickAnnotate(mrs_natural bufferSize,
                           mrs_natural inputSize,
                           string backgroundopt)
{

  /* RtMidi* rtmidi = NULL;


     try {
     rtmidi = new RtMidi();
     }
     catch (RtError3 &error) {
     exit(1);
     }
  */

  MarSystemManager mng;
  MarSystem* total = mng.create("Series", "total");

  MarSystem* pnet = mng.create("Series","pnet");
  MarSystem* oscbank = mng.create("Fanout", "oscbank");
  oscbank->addMarSystem(mng.create("SoundFileSource", "src3"));
  oscbank->addMarSystem(mng.create("SoundFileSource", "src4"));

  pnet->addMarSystem(oscbank);
  pnet->addMarSystem(mng.create("Sum", "sum"));
  pnet->addMarSystem(mng.create("SoundFileSink", "dest"));


  MarSystem* bbank = mng.create("Fanout", "bbank");
  bbank->addMarSystem(pnet);
  if (backgroundopt != EMPTYSTRING)
    bbank->addMarSystem(mng.create("SoundFileSource", "src5"));


  total->addMarSystem(bbank);
  total->addMarSystem(mng.create("Sum", "tsum"));
  total->addMarSystem(mng.create("AudioSink", "dest"));

  // output file
  pnet->updctrl("SoundFileSink/dest/mrs_string/filename", "drum.wav");


  int byte2, byte3;
  int channel;
  int type;
  int prev_pitch;

  // bass drum and snare drum sounds
  pnet->updctrl("Fanout/oscbank/SoundFileSource/src3/mrs_string/filename", "../rawwaves/sd22k.wav");
  pnet->updctrl("Fanout/oscbank/SoundFileSource/src4/mrs_string/filename", "../rawwaves/bd22k.wav");
  if (backgroundopt != EMPTYSTRING)
    bbank->updctrl("SoundFileSource/src5/mrs_string/filename", backgroundopt);


  total->updctrl("AudioSink/dest/mrs_natural/bufferSize", bufferSize);
  total->updctrl("mrs_natural/inSamples", inputSize);
  pnet->linkctrl("mrs_natural/pos3", "Fanout/oscbank/SoundFileSource/src3/mrs_natural/pos");
  pnet->linkctrl("mrs_natural/pos4", "Fanout/oscbank/SoundFileSource/src4/mrs_natural/pos");



  /* while(1)
     {
     if (rtmidi->nextMessage() > 0)
     {
     byte3 = rtmidi->getByteThree();
     byte2 = rtmidi->getByteTwo();
     channel = rtmidi->getChannel();
     type = rtmidi->getType();

   // STC1000 NoteOn's
   if ((type == 144) && (byte3 != 0))
   {
   // rewind the files
   if (byte2 == 44)
   pnet->updctrl("mrs_natural/pos3", 0);

   if (byte2 == 53)
   pnet->updctrl("mrs_natural/pos4", 0);

   }

   // Radio Drum stick 1
   if ((type == 160) && (byte2 == 15))
   {

   if ((byte3 >= 40) &&
   (byte3 <= 100))
   {
   pnet->updctrl("mrs_natural/pos3", 0);
   }
   else
   {
   pnet->updctrl("mrs_natural/pos4", 0);
   }
   }

   // Radio Drum stick 2
   if ((type == 160) && (byte2 == 17))
   {
   if ((byte3 >= 40) &&
   (byte3 <= 100))
   {
   pnet->updctrl("mrs_natural/pos3", 0);
   }
   else
   {
   pnet->updctrl("mrs_natural/pos4", 0);
   }
   }
   }

   total->tick();



   }
  */

}




/*
   void midiBrowse()
   {
   RtMidi* rtmidi = NULL;


   try {
   rtmidi = new RtMidi();
   }
   catch (RtError3 &error) {
   exit(1);
   }


   int byte2, byte3;
   int xtemp, ytemp;
   int channel;
   int type;
   int prev_pitch;

   string sfName;


   MarSystemManager mng;

// Create a series Composite
MarSystem* series = mng.create("Series", "series");
series->addMarSystem(mng.create("SoundFileSource", "src"));
series->addMarSystem(mng.create("AudioSink", "dest"));

// only update controls from Composite level
series->updctrl("mrs_natural/inSamples", 128);
vector<string> retrievedFiles = tempo_map[1][1];
sfName = retrievedFiles[0];
series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);



int si = 0;
int ti = 0;
int pindex = 0;
int titemp = 0;
int sitemp = 0;



while(1)

{
if (rtmidi->nextMessage() > 0)
{
byte3 = rtmidi->getByteThree();
byte2 = rtmidi->getByteTwo();

channel = rtmidi->getChannel();
type = rtmidi->getType();

// store x and y data from Radio Drum
if ((type == 160))
{
if (byte2 == 9)
{
sitemp = si;
xtemp = floor(byte3/12.7);
si = xtemp;
if (si != sitemp)
pindex = 0;

}
if (byte2 == 8)
{
titemp = ti;
ytemp = floor(byte3/12.7);
ti = ytemp;
if (ti != titemp)
pindex = 0;
}
}


// Recieve files code

cout << "si = " << si << endl;
cout << "ti = " << ti << endl;
cout << "pindex = " << pindex << endl;

vector<string> retrievedFiles = tempo_map[si][ti];
cout << "Retrieved " << retrievedFiles.size() << " files" << endl;

if (retrievedFiles.size() !=  0)
{
sfName = retrievedFiles[pindex];
series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
if (byte2 == 2)
{
int zgrid;

if (retrievedFiles.size() > 10)
zgrid = 10;
else
zgrid = retrievedFiles.size();
pindex = floor(byte3/127.0 * zgrid);
pindex = (pindex+1) % (retrievedFiles.size());
}

}
else
{
cout << "No files" << endl;
}


}
series->tick();
}

delete series;
}

*/

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addNaturalOption("outputSize", "o", 128);
  cmd_options.addNaturalOption("inputSize", "i", 64);
  cmd_options.addStringOption("background", "b", EMPTYSTRING);
  cmd_options.addStringOption("mapping", "m", EMPTYSTRING);
  cmd_options.addStringOption("device", "d", EMPTYSTRING);
  cmd_options.addNaturalOption("port", "p", 0);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  bufferSizeopt = cmd_options.getNaturalOption("outputSize");
  inputSizeopt = cmd_options.getNaturalOption("inputSize");
  backgroundopt = cmd_options.getStringOption("background");
  verbose_opt = cmd_options.getBoolOption("verbose");
  mappingopt = cmd_options.getStringOption("mapping");
  deviceopt = cmd_options.getStringOption("device");
  portopt = cmd_options.getNaturalOption("port");
}

// take advantage of MarSystemManager
void
tempotest_sfplay(string sfName)
{
  cout << "Playing " << sfName << endl;

  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("AudioSink", "dest"));

  // only update controls from Composite level
  series->updctrl("mrs_natural/inSamples", 128);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    series->tick();

  delete series;
}



void
tempo_test(string fname, int tempo, int rank)
{
  ifstream from(fname.c_str());

  string name;
  int itempo;
  float strength;



  vector<string> names;
  vector<float>  tempos;
  vector<float>  strengths;
  int count = 0;




  float min_strength = 1000.0;
  float max_strength = 0.0;
  float min_tempo = 1000.0;
  float max_tempo = 0.0;

  while(!from.eof() )
  {
    from >> name;
    from >> itempo;
    from >> strength;

    names.push_back(name);
    tempos.push_back(itempo);
    strengths.push_back(strength);

    if (strength > max_strength)
      max_strength = strength;
    if (strength < min_strength)
      min_strength = strength;


    if (itempo > max_tempo)
      max_tempo = itempo;
    if (itempo < min_tempo)
      min_tempo = itempo;

    count++;
  }
  cout << "Min strength = " << min_strength << endl;
  cout << "Max strength = " << max_strength << endl;

  cout << "Min tempo = " << min_tempo << endl;
  cout << "Max temp = " << max_tempo << endl;


  for (int i=0; i < count; i++)
  {
    strengths[i] = floor(((strengths[i] - min_strength) / (max_strength - min_strength)) * 10.0 + 0.5);

    tempos[i] = floor(((tempos[i] - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);

    /* cout << "Name = " << names[i] << endl;
       cout << "Tempo = " << tempos[i] << endl;
       cout << "Strength = " << strengths[i] << endl;
    */

    int si = (int)strengths[i];
    int ti = (int)tempos[i];

    /* cout << "si = " << si << endl;
       cout << "ti = " << ti << endl;
    */

    tempo_map[si][ti].push_back(names[i]);
  }








  for (int ti = 0; ti < 11; ti++)
    for (int si = 0; si < 11; si++)
    {
      cout << "Tempo = " << ti << " - " << "Strength = " << si << endl;
      vector<string> retrievedFiles = tempo_map[si][ti];

      vector<string>::iterator vi;

      for (vi = retrievedFiles.begin(); vi != retrievedFiles.end(); ++vi)
      {
        cout << (*vi) << endl;
      }



    }


  /* tempo = floor(((tempo - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);
     vector<string> retrievedFiles = tempo_map[rank][tempo];

     if (retrievedFiles.size()  == 0)
     cout << "No file for these specs" << endl;
     else
     {
     cout << "Playing " << retrievedFiles[0] << endl;

     tempotest_sfplay(retrievedFiles[0]);
     }
  */



  return;

}


int main(int argc, const char **argv)
{

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  // default options
  if (mappingopt == EMPTYSTRING)
    mappingopt = "BoomChick";

  if (deviceopt == EMPTYSTRING)
    deviceopt = "Keyboard";


  if (verbose_opt)
    newMidiInfo();
  else if (mappingopt == "BoomChick")
    midiBoomChickAnnotate(bufferSizeopt, inputSizeopt,backgroundopt);
  else if (mappingopt == "TempoBrowse")
  {
    tempo_test("mp3libtempo.txt", 100, 0);
    // midiBrowse();
  }
  else if (mappingopt == "KarplusStrong")
  {
    PluckLive(deviceopt, 0,100,1.0,0.5);
  }


  else
    cout << "Unrecognized mapping: " << mappingopt << endl;


}
