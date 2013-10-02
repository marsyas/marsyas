// *************************************************************
//
// File: mixer.cpp
// Au: Ajay Kapur and Rich McWalter
// Date: Jan 26, 2005
//
// Mixer console for Intellitrance
//
// *************************************************************


#include <cstdio>
#include <sys/time.h>
#include <marsyas/system/MarSystemManager.h>
// #include "Messager.h"

#include <string>
#include <iostream>

using namespace std;
using namespace Marsyas;

double
currentTime(void)
/* It returns the current time as a real number: the whole part
 * is the number of seconds, the fractional part is the number of
 * microseconds since the epoch (1 jan 1970 at 00:00:00)
 */
{
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return (tval.tv_sec + tval.tv_usec/1000000.0);
}

void
sfmixer(string sfName1)
{

  // Initialize Messager
  // Messager* messager =0;
  // messager = new Messager(2,2001);

  // Set up mixer1 Series
  MarSystemManager mng;

  // Set up Channel 1 - 1
  MarSystem* Channel_1_1 = mng.create("Series", "Channel_1_1");
  Channel_1_1->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_1_1->addMarSystem(mng.create("Gain","gain"));
  Channel_1_1->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_1_1->updctrl("Gain/gain/mrs_real/gain", 0.0);


  // Set up Channel 2 - 1
  MarSystem* Channel_2_1 = mng.create("Series", "Channel_2_1");
  Channel_2_1->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_2_1->addMarSystem(mng.create("Gain","gain"));
  Channel_2_1->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_2_1->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 3 - 1
  MarSystem* Channel_3_1 = mng.create("Series", "Channel_3_1");
  Channel_3_1->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_3_1->addMarSystem(mng.create("Gain","gain"));
  Channel_3_1->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_3_1->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 4 - 1
  MarSystem* Channel_4_1 = mng.create("Series", "Channel_4_1");
  Channel_4_1->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_4_1->addMarSystem(mng.create("Gain","gain"));
  Channel_4_1->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_4_1->updctrl("Gain/gain/mrs_real/gain", 0.0);


  // Funnel all Channels together with a Fanin for Mix 1
  MarSystem* mix1 = mng.create("Fanin", "mix1");
  mix1->addMarSystem(Channel_1_1);
  mix1->addMarSystem(Channel_2_1);
  mix1->addMarSystem(Channel_3_1);
  mix1->addMarSystem(Channel_4_1);

  // Mixer1 for main Output 1
  MarSystem* mixer1 = mng.create("Series", "mixer1");
  mixer1->addMarSystem(mix1);
  mixer1->addMarSystem(mng.create("Gain","gain"));
  mixer1->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Mixer 1 for main Output 1 (CROSS FADE CHAIN)
  MarSystem* Cmixer1 = mng.create("Series", "Cmixer1");
  Cmixer1->addMarSystem(mixer1);
  Cmixer1->addMarSystem(mng.create("Gain","gain"));
  Cmixer1->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 1 - 2
  MarSystem* Channel_1_2 = mng.create("Series", "Channel_1_2");
  Channel_1_2->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_1_2->addMarSystem(mng.create("Gain","gain"));
  Channel_1_2->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_1_2->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 2 - 2
  MarSystem* Channel_2_2 = mng.create("Series", "Channel_2_2");
  Channel_2_2->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_2_2->addMarSystem(mng.create("Gain","gain"));
  Channel_2_2->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_2_2->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 3 - 2
  MarSystem* Channel_3_2 = mng.create("Series", "Channel_3_2");
  Channel_3_2->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_3_2->addMarSystem(mng.create("Gain","gain"));
  Channel_3_2->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_3_2->updctrl("Gain/gain/mrs_real/gain", 0.0);

  // Set up Channel 4 - 2
  MarSystem* Channel_4_2 = mng.create("Series", "Channel_4_2");
  Channel_4_2->addMarSystem(mng.create("SoundFileSource","src"));
  Channel_4_2->addMarSystem(mng.create("Gain","gain"));
  Channel_4_2->updctrl("SoundFileSource/src/mrs_string/filename",sfName1);
  Channel_4_2->updctrl("Gain/gain/mrs_real/gain", 1.0);


  // Funnel all Channels together with a Fanin for Mixer 2
  MarSystem* mix2 = mng.create("Fanin", "mix2");
  mix2->addMarSystem(Channel_1_2);
  mix2->addMarSystem(Channel_2_2);
  mix2->addMarSystem(Channel_3_2);
  mix2->addMarSystem(Channel_4_2);

  // Mixer 2 for main Output 2
  MarSystem* mixer2 = mng.create("Series", "mixer2");
  mixer2->addMarSystem(mix2);
  mixer2->addMarSystem(mng.create("Gain","gain"));
  mixer2->updctrl("Gain/gain/mrs_real/gain", 1.0);

  // Mixer 2 for main Output 2 (CROSS FADE CHAIN)
  MarSystem* Cmixer2 = mng.create("Series", "Cmixer2");
  Cmixer2->addMarSystem(mixer2);
  Cmixer2->addMarSystem(mng.create("Gain","gain"));
  Cmixer2->updctrl("Gain/gain/mrs_real/gain", 1.0);

  // Master Mixer using Fan Out
  MarSystem *mixMaster = mng.create("Series", "mixMaster");
  MarSystem *mixM = mng.create("Fanout", "mixM");
  mixM->addMarSystem(Cmixer2);
  mixM->addMarSystem(Cmixer1);
  mixMaster->addMarSystem(mixM);
  mixMaster->addMarSystem(mng.create("Sum", "sum"));
  mixMaster->addMarSystem(mng.create("Gain","gain"));
  mixMaster->addMarSystem(mng.create("AudioSink","dest"));
  mixMaster->updctrl("Gain/gain/mrs_real/gain", 1.0);

  // initialize controls for Series
  mixMaster->updctrl("mrs_natural/inSamples", 256);


  // create variables for messager
  string message;
  bool done = false;
  int type;
  string cname;
  mrs_real dur;

  // create variables for timing
  double start = -1.0;
  double bass[20];
  double snare[20];
  int ibass = 0;
  int isnare = 0;
  int i;

  // intialize timing variables
  for (i = 0; i < 20; i++)
  {
    bass[i] = -1.0;
    snare[i] = -1.0;
  }


  while (1)
  {
    // **********LOOP files********************
    //  mixer1->tick();
    mixMaster->tick();
    // mixMaster->tick();
    if (Channel_1_1->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_1_1->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer1->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_1_1->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_2_1->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_2_1->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer1->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_2_1->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_3_1->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_3_1->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer1->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_3_1->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_4_1->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_4_1->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer1->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_4_1->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_1_2->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_1_2->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer2->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_1_2->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_2_2->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_2_2->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer2->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_2_2->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_3_2->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_3_2->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer2->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_3_2->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }

    if (Channel_4_2->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() < Channel_4_2->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() + mixer2->getctrl("mrs_natural/inSamples")->to<mrs_natural>())
    {
      // reset to 0 for looping
      Channel_4_2->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
    }


    // ********* MESSAGER CONTROL ***************//

    // type = messager->nextMessage();
    if (type < 0)
      done = true;
    else
    {
      //   message = messager->getMessage();
      stringstream inss(message);
      inss >> cname;
      //cout << cname << endl;

      // MIXER SLIDER MESSAGES

      if (cname == "Gain/gt/mrs_real/gain1")
      {
        float x;
        inss >> dur >> x;
        Channel_1_1->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain2")
      {
        float x;
        inss >> dur >> x;
        Channel_2_1->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain3")
      {
        float x;
        inss >> dur >> x;
        Channel_3_1->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain4")
      {
        float x;
        inss >> dur >> x;
        Channel_4_1->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain5")
      {
        float x;
        inss >> dur >> x;
        Channel_1_2->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain6")
      {
        float x;
        inss >> dur >> x;
        Channel_2_2->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain7")
      {
        float x;
        inss >> dur >> x;
        Channel_3_2->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gain8")
      {
        float x;
        inss >> dur >> x;
        Channel_4_2->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gainmain")
      {
        float x;
        inss >> dur >> x;
        mixMaster->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gainleft")
      {
        float x;
        inss >> dur >> x;
        mixer1->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gainright")
      {
        float x;
        inss >> dur >> x;
        mixer2->updctrl("Gain/gain/mrs_real/gain", x);
      }
      else if (cname == "Gain/gt/mrs_real/gaincross")
      {
        float x;
        inss >> dur >> x;
        Cmixer1->updctrl("Gain/gain/mrs_real/gain", dur);
        Cmixer2->updctrl("Gain/gain/mrs_real/gain", x);
      }

      // LOAD COMMANDS
      else if (cname == "Audio/Track1/String/Filename")
      {
        string x;
        inss >> x;
        Channel_1_1->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track2/String/Filename")
      {
        string x;
        inss >> x;
        Channel_2_1->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track3/String/Filename")
      {
        string x;
        inss >> x;
        Channel_3_1->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track4/String/Filename")
      {
        string x;
        inss >> x;
        Channel_4_1->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track5/String/Filename")
      {
        string x;
        inss >> x;
        Channel_1_2->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track6/String/Filename")
      {
        string x;
        inss >> x;
        Channel_2_2->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track7/String/Filename")
      {
        string x;
        inss >> x;
        Channel_3_2->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }
      else if (cname == "Audio/Track8/String/Filename")
      {
        string x;
        inss >> x;
        Channel_4_2->updctrl("SoundFileSource/src/mrs_string/filename",x);
      }

      // MUTE COMMANDS
      else if (cname == "Channel/Track1/Mute/on")
      {

        Channel_1_1->updctrl("SoundFileSource/src/mrs_bool/mute",true);
      }
      else if (cname == "Channel/Track1/Mute/off")
      {
        Channel_1_1->updctrl("SoundFileSource/src/mrs_bool/mute",false);
      }



      // KEYBOARD MESSAGES

      else if (cname == "Interface/Keyboard/Event/BassHit")
      {
        cout << "BASS HIT " << ibass << endl;
        bass[ibass] = currentTime();
        ibass++;
      }
      else if (cname == "Interface/Keyboard/Event/SnareHit")
      {
        cout << "SNARE HIT " << isnare << endl;
        snare[isnare] = currentTime();
        isnare++;
      }

      else if (cname == "Interface/Keyboard/Transport/Record")
      {
        cout << "RECORD" << endl;
        start = currentTime();
        cout << start << endl;
      }
      else if (cname == "Interface/Keyboard/Transport/Stop")
      {
        cout << "STOP" << endl;

        // calculate timings
        for (i = 0; i < ibass; i++)
        {
          cout << "bass" << endl;
          bass[i] = bass[i]-start;
          cout << i << bass[i] << endl;
        }
        for (i = 0; i < isnare; i++)
        {
          cout << "snare" << endl;
          snare[i] = snare[i]-start;
          cout << i << snare[i] << endl;
        }

        // reinitialize timer variables
        ibass = 0;
        isnare = 0;
        for (i = 0; i < 20; i++)
        {
          bass[i] = -1.0;
          snare[i] = -1.0;
        }

      }
      else if (cname == "Interface/Keyboard/Transport/Play")
      {
        cout << "PLAY" << endl;
      }

    }

  } // end Main while loop

  // clean up
  //delete messager;
}

int main(int argc, char **argv)
{
  string sfName1;

  if (argc != 2)
  {
    cout << "Wrong number of arguments " << endl;
    exit(1);
  }
  else
  {
    sfName1 = argv[1];
  }
  sfmixer(sfName1);
}
