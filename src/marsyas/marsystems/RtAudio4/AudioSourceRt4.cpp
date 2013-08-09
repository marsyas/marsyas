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

#include "AudioSourceRt4.h"

using namespace std;
using namespace Marsyas;

AudioSourceRt4::AudioSourceRt4(string name):MarSystem("AudioSource", name)
{

#ifdef MARSYAS_AUDIOIO
  audio_ = NULL;
#endif


  ri_ = 0;
  pringBufferSize_ = 0;

  isInitialized_ = false;
  stopped_ = true;

  rtSrate_ = 0;
  bufferSize_ = 0;
  rtChannels_ = 0;
  nChannels_ = 0;
  pnChannels_ = 0;

  addControls();
}

AudioSourceRt4::~AudioSourceRt4()
{
#ifdef MARSYAS_AUDIOIO
  delete audio_;
#endif

}


MarSystem*
AudioSourceRt4::clone() const
{
  return new AudioSourceRt4(*this);
}

void
AudioSourceRt4::addControls()
{
  addctrl("mrs_natural/nChannels", 1);


#ifdef MARSYAS_MACOSX
  addctrl("mrs_natural/bufferSize", 1024);
#else
  addctrl("mrs_natural/bufferSize", 256);
#endif


  addctrl("mrs_bool/initAudio", false);
  setctrlState("mrs_bool/initAudio", true);

  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_real/gain", 1.0);
}




int
AudioSourceRt4::recordCallback(void *outputBuffer, void *inputBuffer,
                               unsigned int nBufferFrames,
                               double streamTime, unsigned int status,
                               void *userData)
{

  unsigned int drain_count = 0;
  mrs_real* data = (mrs_real*)inputBuffer;
  InputData *iData = (InputData *)userData;
  AudioSourceRt4* mythis = (AudioSourceRt4 *)iData->myself;
  realvec& ringBuffer = *(iData->ringBuffer);
  unsigned int t;
  unsigned int spaceAvailable;

  for (t=0; t < nBufferFrames; t++)
  {
    if (iData->samplesInBuffer <= iData->high_watermark)
    {
      ringBuffer(0,iData->wp) = data[2*t];
      ringBuffer(1,iData->wp) = data[2*t+1];

      iData->wp = ++ (iData->wp) % iData->ringBufferSize;
      if (iData->wp >= iData->rp)
        iData->samplesInBuffer = iData->wp - iData->rp;
      else
        iData->samplesInBuffer = iData->ringBufferSize - (iData->rp - iData->wp);
    }
    else
    {
      while (iData->samplesInBuffer > iData->high_watermark)
      {
        SLEEP(1);
        drain_count++;
        if (drain_count == 1000)
          return 1;
      }
    }
  }
  return 0;
}



void
AudioSourceRt4::myUpdate(MarControlPtr sender)
{
  (void) sender;
  MRSDIAG("AudioSourceRt4::myUpdate");

  //set output controls
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/nChannels"));


  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  inObservations_ = ctrl_inObservations_->to<mrs_natural>();
  gain_ = getctrl("mrs_real/gain")->to<mrs_real>();



  //resize ringBuffer if necessary
  if (inSamples_ < bufferSize_)
    ringBufferSize_ =  8 * bufferSize_;
  else
  {
    ringBufferSize_ =  8 * inSamples_;
  }


  idata.ringBufferSize = ringBufferSize_;
  idata.high_watermark = ringBufferSize_/4;
  idata.low_watermark = ringBufferSize_/8;


  nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
  if ((ringBufferSize_ > pringBufferSize_)||(nChannels_ != pnChannels_))
  {
    ringBuffer_.stretch(nChannels_, ringBufferSize_);
  }
  pringBufferSize_ = ringBufferSize_;
  pnChannels_ = nChannels_;





  if (getctrl("mrs_bool/initAudio")->to<mrs_bool>())
    initRtAudio();
}


void
AudioSourceRt4::initRtAudio()
{

  bufferSize_ = (int)getctrl("mrs_natural/bufferSize")->to<mrs_natural>();
  nChannels_ = getctrl("mrs_natural/nChannels")->to<mrs_natural>();
  rtSrate_ = (int)getctrl("mrs_real/israte")->to<mrs_real>();
  rtChannels_ = (int)getctrl("mrs_natural/nChannels")->to<mrs_natural>();


  //marsyas represents audio data as float numbers
#ifdef MARSYAS_AUDIOIO
  if (audio_ == NULL)
    audio_ = new RtAudio();


  unsigned int bufferFrames = bufferSize_;


  RtAudio::StreamParameters iParams;
  iParams.deviceId = 1;
  iParams.nChannels = rtChannels_;
  iParams.firstChannel = 0;

  idata.ringBuffer = &ringBuffer_;
  idata.wp = 0;
  idata.rp = 0;
  idata.ringBufferSize = ringBufferSize_;
  idata.myself = this;





  RtAudioFormat rtFormat = (sizeof(mrs_real) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;


  try
  {
    audio_->openStream(NULL, &iParams, rtFormat, rtSrate_,
                       &bufferFrames, &recordCallback, (void *)&idata, NULL);
  }
  catch (RtError& e)
  {
    e.printMessage();
    exit(0);
  }


  //update bufferSize control which may have been changed
  //by RtAudio (see RtAudio documentation)
  setctrl("mrs_natural/bufferSize", (mrs_natural)bufferFrames);

#endif

  isInitialized_ = true;
  setctrl("mrs_bool/initAudio", false);
}

void
AudioSourceRt4::start()
{
#ifdef MARSYAS_AUDIOIO
  if ( stopped_) {
    audio_->startStream();
    stopped_ = false;
  }
#endif
}

void
AudioSourceRt4::stop()
{
#ifdef MARSYAS_AUDIOIO
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
#endif
}

void
AudioSourceRt4::localActivate(bool state)
{
  if(state)
    start();
  else
    stop();
}


unsigned int
AudioSourceRt4::getSpaceAvailable()
{
  unsigned int free = (idata.rp - idata.wp -1 + idata.ringBufferSize) % idata.ringBufferSize;
  unsigned int underMark = idata.high_watermark - getSamplesAvailable();

  return(min(underMark, free));
}


unsigned int
AudioSourceRt4::getSamplesAvailable()
{
  unsigned int samplesAvailable = (idata.wp - idata.rp +idata.ringBufferSize) % idata.ringBufferSize;
  return samplesAvailable;
}


void
AudioSourceRt4::myProcess(realvec& in, realvec& out)
{


  (void) in;

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

  for (t=0; t < onSamples_; t++)
  {
    if (getSamplesAvailable())
    {
      for (o=0; o < onObservations_; o++)
        out(o,t) = ringBuffer_(o,idata.rp);
      idata.rp = ++(idata.rp) % idata.ringBufferSize;
      if (idata.rp >= idata.rp)
        idata.samplesInBuffer = idata.wp - idata.rp;
      else
        idata.samplesInBuffer = idata.ringBufferSize - (idata.rp - idata.wp);

    }
  }

  while (idata.samplesInBuffer < idata.low_watermark)
  {
    SLEEP(1);
  }




}

