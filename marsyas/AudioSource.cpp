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

AudioSource::AudioSource(string name):MarSystem("AudioSource", name)
{
  //type_ = "AudioSource";
  //name_ = name;
  
  counter_ = 0;
  
  ri_ = 0;
  rstart_ = 0;
  preservoirSize_ = 0;
  isInitialized_ = false;
  data_ = NULL;
  audio_ = NULL;
  bufferSize_ = 256;

	#ifdef __OS_MACOSX__
  bufferSize_ = 2048;
	#endif

	addControls();
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
  bufferSize_ = 2048;
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
AudioSource::localUpdate()
{
  MRSDIAG("AudioSource::localUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();
  setctrl("mrs_natural/inObservations", nChannels_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));

  sampleRate_ = getctrl("mrs_real/israte").toReal();

  //lmartins: mute_ = getctrl("mrs_bool/mute").toBool();
  gain_ = getctrl("mrs_real/gain").toReal();
  
  //defaultUpdate(); [!]
	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
  
  if (inSamples_ * inObservations_ < bufferSize_) 
    reservoirSize_ = 2 * inObservations_ * bufferSize_;
  else 
    reservoirSize_ = 2 * inSamples_ * inObservations_;
  
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
AudioSource::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}

void 
AudioSource::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
	
	if (!isInitialized_)
  {
    init();
    isInitialized_ = true;
  }
  
  mrs_natural nChannels = getctrl("mrs_natural/nChannels").toNatural();
  
  //check MUTE
	//lmartins: if (mute_) return;
	if(getctrl("mrs_bool/mute").toBool()) return;
  
  if ( stopped_ )//[?]
	  start();
  
  while (ri_ < inSamples_ * inObservations_)
  {
    
    try 
		{
			audio_->tickStream();
    }
    catch (RtError &error) 
		{
			error.printMessage();
		}

    for (t=0; t < inObservations_ * bufferSize_; t++)
		{
			reservoir_(ri_) = data_[t];
			ri_++;
		}
  }
  
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
				out(o,t) = gain_ * reservoir_(inObservations_ * t + o);
      }

  for (t=inSamples_*inObservations_; t < ri_; t++)
    reservoir_(t-inSamples_ * inObservations_) = reservoir_(t);
  
  ri_ = ri_ - inSamples_ * inObservations_;
}










