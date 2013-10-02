/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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



#include "../common_source.h"
#include "AudioSourceBlocking.h"


#include "RtAudio3.h"


using std::ostringstream;
using namespace Marsyas;

AudioSourceBlocking::AudioSourceBlocking(mrs_string name):MarSystem("AudioSourceBlocking", name)
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
  rtDevice_ = 0;
  nChannels_ = 0;

  addControls();
}

AudioSourceBlocking::~AudioSourceBlocking()
{
  delete audio_;
  data_ = NULL; // RtAudio deletes the buffer itself.
}


MarSystem*
AudioSourceBlocking::clone() const
{
  return new AudioSourceBlocking(*this);
}

void
AudioSourceBlocking::addControls()
{
  addctrl("mrs_natural/nChannels", 1);


#ifdef MARSYAS_MACOSX
  addctrl("mrs_natural/bufferSize", 512);
#else
  addctrl("mrs_natural/bufferSize", 256);
#endif

  addctrl("mrs_natural/nBuffers", 4);

  addctrl("mrs_bool/initAudio", false);
  setctrlState("mrs_bool/initAudio", true);

  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_real/gain", 1.0);

  addctrl("mrs_natural/device", 0);
}

void
AudioSourceBlocking::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AudioSourceBlocking::myUpdate");


  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
    initRtAudio();




  //set output controls
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/nChannels"));


  inObservations_ = ctrl_inObservations_->to<mrs_natural>();
  onObservations_ = ctrl_onObservations_->to<mrs_natural>();
  gain_ = getctrl("mrs_real/gain")->to<mrs_real>();

  //resize reservoir if necessary


  if (inSamples_ * onObservations_ < bufferSize_)
    reservoirSize_ = 2 * onObservations_ * bufferSize_;
  else
    reservoirSize_ = 2 * inSamples_ * onObservations_;

  if (reservoirSize_ > preservoirSize_)
  {
    reservoir_.stretch(reservoirSize_);
  }
  preservoirSize_ = reservoirSize_;
}


void
AudioSourceBlocking::initRtAudio()
{

  bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
  nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
  rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
  rtChannels_ = (int)getctrl("mrs_natural/nChannels")->to<mrs_natural>();
  nBuffers_ = (int)getctrl("mrs_natural/nBuffers")->to<mrs_natural>();
  rtDevice_ = (int)getctrl("mrs_natural/device")->to<mrs_natural>();



//marsyas represents audio data as float numbers
  RtAudio3Format rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO3_FLOAT64 : RTAUDIO3_FLOAT32;



  //Create new RtAudio object (delete any existing one)

  if (audio_ != NULL)
  {
    audio_->stopStream();
    delete audio_;
  }
  try
  {
    audio_ = new RtAudio3(0, 0, rtDevice_, rtChannels_, rtFormat,
                          rtSrate_, &bufferSize_, nBuffers_);
    data_ = (mrs_real *) audio_->getStreamBuffer();
  }
  catch (RtError3 &error)
  {
    error.printMessage();
  }

  //update bufferSize control which may have been changed
  //by RtAudio (see RtAudio documentation)
  setctrl("mrs_natural/bufferSize", (mrs_natural)bufferSize_);

  if (audio_ != NULL)
    audio_->stopStream();


  if (rtDevice_ !=0) {
    RtAudio3DeviceInfo info;
    info = audio_->getDeviceInfo(rtDevice_);
  }

  isInitialized_ = true;
  setctrl("mrs_bool/initAudio", false);
}

void
AudioSourceBlocking::start()
{
  if ( stopped_ && audio_) {
    audio_->startStream();
    stopped_ = false;
  }
}

void
AudioSourceBlocking::stop()
{
  if ( !stopped_ && audio_) {
    audio_->stopStream();
    stopped_ = true;
  }
}

void
AudioSourceBlocking::localActivate(bool state)
{
  if(state)
    start();
  else
    stop();
}

void
AudioSourceBlocking::myProcess(realvec& in, realvec& out)
{
  (void) in;

  mrs_natural t,o;

  //check if RtAudio is initialized
  if (!isInitialized_)
    return;

  //check MUTE
  if(ctrl_mute_->isTrue()) return;

  //assure that RtAudio thread is running
  //(this may be needed by if an explicit call to start()
  //is not done before ticking or calling process() )
  if ( stopped_ )
    start();

  int ssize = onSamples_ * onObservations_;

  //send audio to output
  while (ri_ < ssize)
  {
    try
    {

      audio_->tickStream();

    }
    catch (RtError3 &error)
    {
      error.printMessage();
    }

    for (t=0; t < onObservations_ * bufferSize_; t++)
    {
      reservoir_(ri_) = data_[t];
      ri_++;
    }
  }

  for (o=0; o < onObservations_; o++)
    for (t=0; t < onSamples_; t++)
    {
      out(o,t) = gain_ * reservoir_(onObservations_ * t + o);
    }

  for (t=ssize; t < ri_; t++)
    reservoir_(t-ssize) = reservoir_(t);

  ri_ = ri_ - ssize;

  /* MATLAB_PUT(out, "AudioSourceBlocking_out");
   MATLAB_EVAL("plot(AudioSourceBlocking_out)");*/
}
