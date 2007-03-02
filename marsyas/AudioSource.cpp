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
  data_ = NULL;
  audio_ = NULL;

  ri_ = 0;
  preservoirSize_ = 0;

  isInitialized_ = false;
  stopped_ = true;

  rtSrate_ = 0;
  bufferSize_ = 0;
  rtChannels_ = 0;
  nChannels_ = 0;

  addControls();
}

AudioSource::~AudioSource()
{
  delete audio_;
  data_ = NULL; // RtAudio deletes the buffer itself.
}


MarSystem* 
AudioSource::clone() const
{
  return new AudioSource(*this);
}

void 
AudioSource::addControls()
{
  addctrl("mrs_natural/nChannels", 1);

  
#ifdef MARSYAS_MACOSX
  addctrl("mrs_natural/bufferSize", 512);
#else
  addctrl("mrs_natural/bufferSize", 256);
#endif
  
  addctrl("mrs_bool/initAudio", false);
  setctrlState("mrs_bool/initAudio", true);
  
  addctrl("mrs_bool/notEmpty", true);
  addctrl("mrs_real/gain", 1.0); 
}

void 
AudioSource::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AudioSource::myUpdate");


  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>()) 
    initRtAudio();

  
  
  
  //set output controls
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_natural/inObservations", nChannels_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  
  
  inObservations_ = ctrl_inObservations_->to<mrs_natural>();
  
  gain_ = getctrl("mrs_real/gain")->toReal();
  
  //resize reservoir if necessary
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
AudioSource::initRtAudio()
{
  bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->toNatural();
  nChannels_ = getctrl("mrs_natural/nChannels")->toNatural();
  rtSrate_ = (int)getctrl("mrs_real/israte")->toReal();
  rtChannels_ = (int)getctrl("mrs_natural/nChannels")->toNatural();



  
  //marsyas represents audio data as float numbers
  RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;



  
  //Create new RtAudio object (delete any existing one)


  if (audio_ != NULL) 
    {
      audio_->stopStream(); 
      delete audio_;
    } 
  try 
    {
      audio_ = new RtAudio(0, 0, 0, rtChannels_, rtFormat,
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

  if (audio_ != NULL)
      audio_->stopStream(); 

  
  isInitialized_ = true;
  setctrl("mrs_bool/initAudio", false);
}

void 
AudioSource::start()
{
  if ( stopped_ && audio_) {
    audio_->startStream();
    stopped_ = false;
  }
}

void 
AudioSource::stop()
{
  if ( !stopped_ && audio_) {
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
AudioSource::myProcess(realvec& in, realvec& out)
{

  //check if RtAudio is initialized
  if (!isInitialized_)
    return;
  
  //check MUTE
  if(getctrl("mrs_bool/mute")->isTrue()) return;
  
  //assure that RtAudio thread is running
  //(this may be needed by if an explicit call to start()
  //is not done before ticking or calling process() )
  if ( stopped_ )
    start();
  
  
  
  //send audio to output
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
  
  MATLAB_PUT(out, "AudioSource_out");
  MATLAB_EVAL("plot(AudioSource_out)");
}










