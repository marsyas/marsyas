/*
** Copyright (C) 2002 George Tzanetakis <gtzan@cs.cmu.edu>
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
   \class AudioSink
   \brief Real-time Audio source for Linux

   Real-time Audio Sink for Linux based on the OSS sound API. 
*/



#include "AudioSink.h"


AudioSink::AudioSink():SignalSink()
{
  type_ = "AudioSink";
  
}

AudioSink::AudioSink(string name)
{
  type_ = "AudioSink";
  addControls();
}




MarSystem* 
AudioSink::clone() const
{
  return new AudioSink(*this);
}



void 
AudioSink::addControls()
{
  addDefaultControls();
  addctrl("natural/channel", 0);
  addctrl("natural/nChannels",1);
  setctrlState("natural/nChannels", true);
  update();
}



AudioSink::~AudioSink()
{
}

void 
AudioSink::update()
{
  MRSDIAG("AudioSink::update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  // nChannels_ = getctrl("natural/nChannels").toNatural();
}






void 
AudioSink::process(realvec& in, realvec& out)
{
    checkFlow(in,out);
  bool mute = getctrl("bool/mute").toBool();

  
  natural nChannels = getctrl("natural/nChannels").toNatural();
  natural c = getctrl("natural/channel").toNatural();
  setctrl("natural/channel", (c+1)%nChannels);

  // copy to output 
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
      }
  if (mute) return;

}









