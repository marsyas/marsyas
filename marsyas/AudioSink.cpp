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
   \class AudioSink
   \brief Real-time Audio source for Linux

   Real-time Audio Sink for Linux based on the OSS sound API. 
*/

#include "AudioSink.h"

using namespace std;
using namespace Marsyas;

AudioSink::AudioSink(string name):MarSystem("AudioSink", name)
{
  //type_ = "AudioSink";
  //name_ = name;
  
  counter_ = 0;
  bufferSize_ = 0;
  
  ri_ = 0;
  start_ = 0;
  end_ = 0;
  
  rstart_ = 0;
  preservoirSize_ = 0;
  isInitialized_ = 0;
  data_ = NULL;
  audio_ = NULL;
  sampleRate_ = 0.0;
  
	addControls();
}

AudioSink::~AudioSink()
{
  delete audio_;
  data_ = 0; // RtAudio deletes the buffer itself.
}

MarSystem* 
AudioSink::clone() const
{
  return new AudioSink(*this);
}

void 
AudioSink::addControls()
{
  addctrl("mrs_natural/nChannels",1);
  setctrlState("mrs_natural/nChannels", true);
  addctrl("mrs_bool/init", false);
  setctrlState("mrs_bool/init", true);

#ifdef __OS_MACOSX__
  addctrl("mrs_natural/bufferSize", 1024);
#else
  addctrl("mrs_natural/bufferSize", 512);
#endif

  setctrlState("mrs_natural/bufferSize", true);
}

void 
AudioSink::init()
{
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();
  
  RtAudioFormat format = ( sizeof(mrs_real) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  
  // hardwire channels to stereo playback even for mono
  int rtChannels = 2;

  delete audio_;
  
  try {
    audio_ = new RtAudio(0, rtChannels, 0, 0, format,
			 rtSrate_, &bufferSize_, 4);

    data_ = (mrs_real *) audio_->getStreamBuffer();
  }
  catch (RtError &error) 
    {
      error.printMessage();
    }
  stopped_ = true;
}


void 
AudioSink::localUpdate()
{
  MRSDIAG("AudioSink::localUpdate");
  
  rtSrate_ = (mrs_natural)getctrl("mrs_real/israte").toReal();
  srate_ = rtSrate_;
  
  bufferSize_ = (mrs_natural)getctrl("mrs_natural/bufferSize").toNatural();

#ifdef __OS_MACOSX__
  if (rtSrate_ == 22050) 
    {
      rtSrate_ = 44100;
      bufferSize_ = 2 * bufferSize_;
    }
#endif	

  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  nChannels_ = getctrl("mrs_natural/nChannels").toNatural();
  
  if (getctrl("mrs_real/israte").toReal() != sampleRate_)
    {
      sampleRate_ = getctrl("mrs_real/israte").toReal();
      isInitialized_ = false;
    }
 
  //lmartins: mute_ = getctrl("mrs_bool/mute").toBool();
  
  //defaultUpdate();
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
   
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
AudioSink::start()
{
  if ( stopped_ ) {
    audio_->startStream();
    stopped_ = false;
  }
}

void 
AudioSink::stop()
{
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
}

void
AudioSink::localActivate(bool state)
{
	if(state)
		start();
	else
		stop();
}

void 
AudioSink::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
	
	if (!isInitialized_)
  {
    init();
    isInitialized_ = true;
  }
  
  // copy to output and into reservoir
  for (t=0; t < inSamples_; t++)
  {
    reservoir_(end_) = in(0,t);
    end_ = (end_ + 1) % reservoirSize_;
    
    for (o=0; o < inObservations_; o++)
			out(o,t) = in(o,t);
  }
  
  //check MUTE
	//lmartins: if (mute_) return;
	if(getctrl("mrs_bool/mute").toBool()) return;

  if ( stopped_ )//[?]
  {
    start();
  }
 
  rsize_ = bufferSize_;
	#ifdef __OS_MACOSX__ 
  if (srate_ == 22050)
    rsize_ = bufferSize_/2;		// upsample to 44100
  else 
    rsize_ = bufferSize_;
	#endif 
  
  if (end_ >= start_) 
    diff_ = end_ - start_;
  else 
    diff_ = reservoirSize_ - (start_ - end_);

  while (diff_ >= rsize_)  
  {
    for (t=0; t < rsize_; t++) 
		{
			#ifndef __OS_MACOSX__
			data_[2*t] = reservoir_((start_+t)%reservoirSize_);
			data_[2*t+1] = reservoir_((start_+t)%reservoirSize_);
			#else
			if (srate_ == 22050)
				{
					data_[4*t] = reservoir_((start_+t) % reservoirSize_);
					data_[4*t+1] = reservoir_((start_+t)%reservoirSize_);
					data_[4*t+2] = reservoir_((start_+t) % reservoirSize_);
					data_[4*t+3] = reservoir_((start_+t) % reservoirSize_);
				}
			else
				{
					data_[2*t] = reservoir_((start_+t)%reservoirSize_);
					data_[2*t+1] = reservoir_((start_+t)%reservoirSize_);
				}
		  
			#endif 
		}

		try 
		{
			audio_->tickStream();
    }
    catch (RtError &error) 
		{
			error.printMessage();
		}
  
    start_ = (start_ + rsize_) % reservoirSize_;
    
    if (end_ >= start_) 
			diff_ = end_ - start_;
    else 
			diff_ = reservoirSize_ - (start_ - end_);
  }
}

 









	
