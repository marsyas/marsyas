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



AudioSink::AudioSink(string name)
{
  type_ = "AudioSink";
  name_ = name;
  counter_ = 0;
  bufferSize_ = 0;

  addControls();
  ri_ = 0;
  start_ = 0;
  end_ = 0;
  
  rstart_ = 0;
  preservoirSize_ = 0;
  isInitialized_ = 0;
  data_ = NULL;
  audio_ = NULL;

  
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
  addDefaultControls();
  addctrl("natural/nChannels",1);
  setctrlState("natural/nChannels", true);
  addctrl("bool/init", false);
  setctrlState("bool/init", true);

#ifdef __OS_MACOSX__
  addctrl("natural/bufferSize", 1024);
#else
  addctrl("natural/bufferSize", 512);
#endif

  setctrlState("natural/bufferSize", true);
}





void 
AudioSink::init()
{
  nChannels_ = getctrl("natural/nChannels").toNatural();
  
  RtAudioFormat format = ( sizeof(real) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  
  // hardwire channels to stereo playback even for mono
  int rtChannels = 2;

  delete audio_;
  
  try {
    audio_ = new RtAudio(0, rtChannels, 0, 0, format,
			 rtSrate_, &bufferSize_, 4);

    data_ = (real *) audio_->getStreamBuffer();
  }
  catch (RtError &error) 
    {
      error.printMessage();
    }
  stopped_ = true;

}


void 
AudioSink::update()
{
  MRSDIAG("AudioSink::update");
  
  rtSrate_ = (natural)getctrl("real/israte").toReal();
  srate_ = rtSrate_;
  
  bufferSize_ = (natural)getctrl("natural/bufferSize").toNatural();

#ifdef __OS_MACOSX__
  if (rtSrate_ == 22050) 
    {
      rtSrate_ = 44100;
      bufferSize_ = 2 * bufferSize_;
    }
#endif	
  

  setctrl("string/onObsNames", getctrl("string/inObsNames"));  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  nChannels_ = getctrl("natural/nChannels").toNatural();


  if (getctrl("real/israte").toReal() != sampleRate_)
    {
      sampleRate_ = getctrl("real/israte").toReal();
      isInitialized_ = false;
    }
  


  mute_ = getctrl("bool/mute").toBool();
  
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
AudioSink::process(realvec& in, realvec& out)
{
  if (!isInitialized_)
    {
      init();
      isInitialized_ = true;
    }
  
  checkFlow(in,out);
  

  
  // copy to output and into reservoir

  for (t=0; t < inSamples_; t++)
    {
      
      reservoir_(end_) = in(0,t);
      end_ = (end_ + 1) % reservoirSize_;
      
      for (o=0; o < inObservations_; o++)
	out(o,t) = in(o,t);
    }
  
  
  
  if (mute_) return;
  

  if ( stopped_ )
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

      

      try {
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

 









	
