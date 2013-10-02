/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include "AudioSinkBlocking.h"



#include "RtAudio3.h"




using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

AudioSinkBlocking::AudioSinkBlocking(mrs_string name):MarSystem("AudioSinkBlocking", name)
{
  bufferSize_ = 0;

  start_ = 0;
  end_ = 0;

  preservoirSize_ = 0;
  pnChannels_ = 1;

  data_ = NULL;
  audio_ = NULL;

  rtSrate_ = 0;
  bufferSize_ = 0;

  isInitialized_ = false;
  stopped_ = true;//lmartins

  rtSrate_ = 0;
  bufferSize_ = 0;
  rtChannels_ = 0;
  rtDevice_ = 0;

  addControls();
}

AudioSinkBlocking::~AudioSinkBlocking()
{
  delete audio_;
  data_ = NULL; // RtAudio deletes the buffer itself.
}

MarSystem*
AudioSinkBlocking::clone() const
{
  return new AudioSinkBlocking(*this);
}

void
AudioSinkBlocking::addControls()
{

#ifdef MARSYAS_MACOSX
  addctrl("mrs_natural/bufferSize", 512);
#else
  addctrl("mrs_natural/bufferSize", 256);
#endif

  addctrl("mrs_bool/initAudio", false);
  setctrlState("mrs_bool/initAudio", true);

  addctrl("mrs_natural/device", 0);

}

void
AudioSinkBlocking::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AudioSinkBlocking::myUpdate");

  MarSystem::myUpdate(sender);


  nChannels_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();//does nothing... [?]

  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
    initRtAudio();

  //Resize reservoir if necessary
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();



  if (inSamples_ < bufferSize_)
    reservoirSize_ = 2 * bufferSize_;
  else
  {
    if (2 * inSamples_ > preservoirSize_)
      reservoirSize_ = 2 * inSamples_;
  }


  if ((reservoirSize_ > preservoirSize_)||(nChannels_ != pnChannels_))
  {
    //cout << "NCHANNELS = " << nChannels_ << endl;
    reservoir_.stretch(nChannels_, reservoirSize_);
  } else {
    reservoirSize_ = preservoirSize_;
  }

  preservoirSize_ = reservoirSize_;
  pnChannels_ = nChannels_;

}

void
AudioSinkBlocking::initRtAudio()
{

  rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
  srate_ = rtSrate_;
  bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
  rtDevice_= (int)getctrl("mrs_natural/device")->to<mrs_natural>();

#ifdef MARSYAS_MACOSX
  if (rtSrate_ == 22050)
  {
    rtSrate_ = 44100;
    bufferSize_ = 2 * bufferSize_;
  }
#endif




  //marsyas represents audio data as float numbers
  RtAudio3Format rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO3_FLOAT64 : RTAUDIO3_FLOAT32;

  // hardwire channels to stereo playback even for mono
  int rtChannels = nChannels_;


  if (rtChannels == 1) 				// make mono playback in stereo
    rtChannels = 2;

  //create new RtAudio object (delete any existing one)
  if (audio_ != NULL)
  {
    audio_->stopStream();
    delete audio_;
  }

  try
  {
    audio_ = new RtAudio3(rtDevice_, rtChannels, 0, 0, rtFormat,
                          rtSrate_, &bufferSize_, 4);

    data_ = (mrs_real *) audio_->getStreamBuffer();
  }
  catch (RtError3 &error)
  {
    error.printMessage();
  }

  if (audio_ != NULL)
  {
    audio_->startStream();
  }

  if (rtDevice_ !=0) {
    RtAudio3DeviceInfo info;
    info = audio_->getDeviceInfo(rtDevice_);
    cout << "Using output device: " << info.name << endl;
  }


  //update bufferSize control which may have been changed
  //by RtAudio (see RtAudio documentation)
  setctrl("mrs_natural/bufferSize", (mrs_natural)bufferSize_);

  isInitialized_ = true;
  setctrl("mrs_bool/initAudio", false);

}

void
AudioSinkBlocking::start()
{
  if ( stopped_ && audio_) {
    audio_->startStream();
    stopped_ = false;
  }
}

void
AudioSinkBlocking::stop()
{
  if ( !stopped_ && audio_) {
    audio_->abortStream();
    stopped_ = true;
  }
}

void
AudioSinkBlocking::localActivate(bool state)
{
  if(state)
    start();
  else
    stop();
}

void
AudioSinkBlocking::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  // cout << "AudioSinkBlocking::myProcess start" << endl;
  // check MUTE
  if(ctrl_mute_->isTrue())
  {
    for (t=0; t < inSamples_; t++)
    {
      for (o=0; o < inObservations_; o++)
      {
        out(o,t) = in(o,t);
      }
    }

    if (audio_ != NULL)
    {
//			for (t=0; t < bufferSize_; t++)
//			{
//				data_[2*t] = 0.0;
//				data_[2*t+1] = 0.0;
//			}
//
//			try
//			{
//				audio_->tickStream();
//			}

      try
      {
        audio_->stopStream();
      }
      catch (RtError3 &error)
      {
        error.printMessage();
      }
    }

    return;
  }

  // copy to output and into reservoir



  for (t=0; t < inSamples_; t++)
  {
    for (o=0; o < inObservations_; o++)
    {
      reservoir_(o, end_) = in(o,t);
      out(o,t) = in(o,t);
    }
    end_ ++;
    if (end_ == reservoirSize_)
      end_ = 0;
  }



  //check if RtAudio is initialized
  if (!isInitialized_) {
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
#ifdef MARSYAS_MACOSX
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

// 	cout << "diff_=" << diff_ << " rsize_=" << rsize_ << " reservoirSize_=" << reservoirSize_ << " start_=" << start_ << " end_=" << end_ << endl;
  while (diff_ >= rsize_)
  {
// 	  cout << "diff_=" << diff_ << endl;
    for (t =0; t < rsize_; t++)
    {
      int rt = (start_ + t);

      while (rt >= reservoirSize_)
        rt -= reservoirSize_;
      while (rt < 0)
        rt += reservoirSize_;

      const int t2 = 2 * t;

      // What does this do? - LG
      // Why is this defined?? - <3 lg
//#ifndef MARSYAS_MACOSX
//
//			if (inObservations_ == 1)
//			{
//				for (int j=0; j < nChannels_; j++)
//				{
//					data_[t2+j] = reservoir_(0, rt);
//				}
//			}
//			else
//			{
//				for (int j=0; j < nChannels_; j++)
//				{
//					data_[t2+j] = reservoir_(0+j,   rt);
//				}
//
//			}
//
//#else
      const int t4 = 4 * t;
      if (srate_ == 22050)
      {

        if (inObservations_ == 1)
        {
          data_[t4] = reservoir_(0,rt);
          data_[t4+1] = reservoir_(0,rt);
          data_[t4+2] = reservoir_(0,rt);
          data_[t4+3] = reservoir_(0,rt);
        }
        else
        {
          for (int j=0; j < nChannels_; j++)
          {
            data_[t4] = reservoir_(0+j,rt);
            data_[t4+2+j] = reservoir_(0+j,rt);
          }
        }

      }
      else
      {
        if (inObservations_ == 1)
        {

          mrs_real foo = reservoir_(0, rt);
          data_[t2] = foo;
          data_[t2+1] = foo;
        }
        else
        {

          for (int j=0; j < nChannels_; j++)
          {
            data_[t2+j] = reservoir_(j,   rt);
          }

        }
      }
//#endif
    }

    //tick RtAudio
    try
    {
      audio_->tickStream();
    }
    catch (RtError3 &error)
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
//   cout << "AudioSinkBlocking::myProcess end" << endl;

}
