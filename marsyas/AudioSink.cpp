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
  bufferSize_ = 0;
  
  start_ = 0;
  end_ = 0;
  
  preservoirSize_ = 0;

  data_ = NULL;
  audio_ = NULL;

  rtSrate_ = 0;
  bufferSize_ = 0;
  
  isInitialized_ = false;
  stopped_ = true;//lmartins
  
  rtSrate_ = 0;
  bufferSize_ = 0;
  rtChannels_ = 0;
  


  addControls();
}

AudioSink::~AudioSink()
{
  delete audio_;
  data_ = NULL; // RtAudio deletes the buffer itself.
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
  
#ifdef __OS_MACOSX__
  addctrl("mrs_natural/bufferSize", 1024);
#else
  addctrl("mrs_natural/bufferSize", 512);
#endif

  addctrl("mrs_bool/initAudio", false);
  setctrlState("mrs_bool/initAudio", true);
  

}

void 
AudioSink::myUpdate()
{
  MRSDIAG("AudioSink::myUpdate");


  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
    initRtAudio();
  


  
  nChannels_ = getctrl("mrs_natural/nChannels")->toNatural();//does nothing... [?]
  
  //Resize reservoir if necessary
  inSamples_ = getctrl("mrs_natural/inSamples")->toNatural();
  if (inSamples_ < bufferSize_) 
    reservoirSize_ = 2 * bufferSize_;
  else 
    reservoirSize_ = 2 * inSamples_;
  
  if (reservoirSize_ > preservoirSize_)
    reservoir_.stretch(reservoirSize_);
  
  preservoirSize_ = reservoirSize_;
  
}

void 
AudioSink::initRtAudio()
{
  cout << "initRtAudio" << endl;

  rtSrate_ = (int)getctrl("mrs_real/israte")->toReal();
  srate_ = rtSrate_;
  bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->toNatural();

#ifdef __OS_MACOSX__
  if (rtSrate_ == 22050) 
    {
      rtSrate_ = 44100;
      bufferSize_ = 2 * bufferSize_;
    }
#endif	




  //marsyas represents audio data as float numbers
  RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  
  // hardwire channels to stereo playback even for mono
  int rtChannels = 2;
  
  //create new RtAudio object (delete any existing one)
  delete audio_;
  try 
    {
      audio_ = new RtAudio(0, rtChannels, 0, 0, rtFormat,
			   rtSrate_, &bufferSize_, 4);

      data_ = (mrs_real *) audio_->getStreamBuffer();
    }
  catch (RtError &error) 
    {
      error.printMessage();
    }
  
  //update bufferSize control which may have been changed
  //by RtAudio (see RtAudio documentation)
  setctrl("mrs_natural/bufferSize", (mrs_natural)bufferSize_);
  
  isInitialized_ = true;
  setctrl("mrs_bool/initAudio", false);
}

void 
AudioSink::start()
{
  if ( stopped_ && audio_) {
    audio_->startStream();
    stopped_ = false;
  }
}

void 
AudioSink::stop()
{
  if ( !stopped_ && audio_) {
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
AudioSink::myProcess(realvec& in, realvec& out)
{
  
  
  // copy to output and into reservoir
  for (t=0; t < inSamples_; t++)
    {
      reservoir_(end_) = in(0,t);
      end_ = (end_ + 1) % reservoirSize_;
      
      for (o=0; o < inObservations_; o++)
	out(o,t) = in(o,t);
    }
  
  //check if RtAudio is initialized
  if (!isInitialized_)
    return;
  
  //check MUTE
  if(getctrl("mrs_bool/mute")->isTrue())
    {
      for (t=0; t < rsize_; t++) 
	{
	  data_[2*t] = 0.0;
	  data_[2*t+1] = 0.0;
	}
      
      return;
    }
  
  
  //assure that RtAudio thread is running
  //(this may be needed by if an explicit call to start()
  //is not done before ticking or calling process() )
  if ( stopped_ )
    {
      start();
    }
  
  //update reservoir pointers 
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
  
  //send audio data in reservoir to RtAudio
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
      
      //tick RtAudio
      try 
	{
	  audio_->tickStream();
	}
      catch (RtError &error) 
	{
	  error.printMessage();
	}
      
      //update reservoir pointers
      start_ = (start_ + rsize_) % reservoirSize_;
      if (end_ >= start_) 
	diff_ = end_ - start_;
      else 
	diff_ = reservoirSize_ - (start_ - end_);
    }
}

 









	
