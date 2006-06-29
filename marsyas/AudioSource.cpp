/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.cmu.edu>
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
   \class AudioSource
   \brief Real-time audio source 

   Real-time Audio Source based on RtAudio
*/

#include "AudioSource.h"

using namespace std;
using namespace Marsyas;

AudioSource::AudioSource(string name)
{
  type_ = "AudioSource";
  name_ = name;
  
  counter_ = 0;
  addControls();
  
  ri_ = 0;
  rstart_ = 0;
  preservoirSize_ = 0;
  isInitialized_ = false;
  data_ = NULL;
  audio_ = NULL;
  bufferSize_ = 256;

#ifdef __OS_MACOSX__
  bufferSize_ = 1024;
#endif	
}



AudioSource::~AudioSource()
{
  delete audio_;
  data_ = 0; // RtAudio deletes the buffer itself.
}


MarSystem* 
AudioSource::clone() const
{
  return new AudioSource(*this);
}


void 
AudioSource::addControls()
{
  addDefaultControls();
  addctrl("mrs_natural/nChannels",1);
  addctrl("mrs_real/gain", 1.05);
  setctrlState("mrs_real/gain", true);
  setctrlState("mrs_natural/nChannels", true);
  addctrl("mrs_bool/init", false);
  setctrlState("mrs_bool/init", true);
}


void 
AudioSource::init()
{
  
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();
  
  RtAudioFormat format = ( sizeof(mrs_real) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  
  int rtSrate = (mrs_natural)getctrl("mrs_real/israte").toReal();
  bufferSize_ = 256;

#ifdef __OS_MACOSX__
  rtSrate = 44100;
  bufferSize_ = 1024;
#endif	

  
  delete audio_;
  
  int rtChannels = (mrs_natural)getctrl("mrs_natural/nChannels").toNatural();
  
  try {
    audio_ = new RtAudio(0, 0, 0, rtChannels, format,
			 rtSrate, &bufferSize_, 4);
    data_ = (mrs_real *) audio_->getStreamBuffer();
  }
  catch (RtError &error) 
    {
      error.printMessage();
    }
  stopped_ = true;

}

void 
AudioSource::update()
{
  MRSDIAG("AudioSource::update");

  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();
  sampleRate_ = getctrl("mrs_real/israte").toReal();

  mute_ = getctrl("mrs_bool/mute").toBool();
  gain_ = getctrl("mrs_real/gain").toReal();
  
  defaultUpdate();
  
  if (inSamples_ < bufferSize_) 
    reservoirSize_ = 2 * bufferSize_;
  else 
    reservoirSize_ = 2 * inSamples_;
  
  if (reservoirSize_ > preservoirSize_)
    {
      reservoir_.stretch(reservoirSize_);
    }
  preservoirSize_ = reservoirSize_;
}



void 
AudioSource::start()
{
  if ( stopped_ ) {
    audio_->startStream();
    stopped_ = false;
  }
}


void 
AudioSource::stop()
{
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
}





void 
AudioSource::process(realvec& in, realvec& out)
{

  
  if (!isInitialized_)
    {
      init();
      isInitialized_ = true;
    }
  checkFlow(in,out);

  mrs_natural nChannels = getctrl("mrs_natural/nChannels").toNatural();
  
  if (mute_) return;


  
  if ( stopped_ )
    start();


  
  
  while (ri_ < inSamples_)
    {
      
      try {
	audio_->tickStream();
      }
      catch (RtError &error) 
	{
	  error.printMessage();
	}
      for (t=0; t < bufferSize_; t++)
	{
	  reservoir_(ri_) = data_[t];
	  ri_++;
	}
    }

  
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	out(o,t) = gain_ * reservoir_(t);
      }

  for (t=inSamples_; t < ri_; t++)
    reservoir_(t-inSamples_) = reservoir_(t);
  

  ri_ = ri_ - inSamples_;
  
  
}










