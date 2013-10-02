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

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/common_source.h>

using namespace std;
using namespace Marsyas;

int
main(int argc, const char **argv)
{
  (void) argc;  // tells the compiler that we know that we're not
  (void) argv;  // using these two variables
  MRSDIAG("helloWorld.cpp - main");

  // cout << "This is probably the simplest Marsyas example code: it simply
  // generates a sine wave with a frequency of 440Hz and send it to the audio
  // card output. Simple press CTRL+C to quit." << endl;

  //we usualy start by creating a MarSystem manager
  //to help us on MarSystem creation
  MarSystemManager mng;

  //create the network, which is a simple Series network with a sine wave
  //oscilator and a audio sink object to send the ausio data for playing
  //in the sound card
  MarSystem *network = mng.create("Series", "network");
  network->addMarSystem(mng.create("SineSource", "src"));
  network->addMarSystem(mng.create("AudioSink", "dest"));
  network->addMarSystem(mng.create("SoundFileSink", "dest2"));

  //set the window (i.e. audio frame) size (in samples). Let's say, 256 samples.
  //This is done in the outmost MarSystem (i.e. the Series/network) because flow
  //controls (as is the case of inSamples) are propagated through the network.
  //Check the Marsyas documentation for mode details.
  network->updControl("mrs_natural/inSamples", 4096);


  //set oscilator frequency to 440Hz
  network->updControl("SineSource/src/mrs_real/frequency", 440.0);

  // set the sampling to 44100  - a safe choice in most configurations
  network->updControl("mrs_real/israte", 44100.0);
  network->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  network->updControl("SoundFileSink/dest2/mrs_string/filename", "helloworld.wav");


  //now it's time for ticking the network,
  //ad aeternum (i.e. until the user quits by CTRL+C)
  while (1)
  {
    network->tick();
  }

  //ok, this is not really necessary because we are quiting by CTRL+C,
  //but it's a good habit anyway ;-)
  delete network;

  return(0);
}
