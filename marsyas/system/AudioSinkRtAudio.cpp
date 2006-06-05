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
  name_ = name;
  counter_ = 0;
  


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
  addctrl("bool/init", false);
  setctrlState("bool/init", true);
  update();
}



AudioSink::~AudioSink()
{
  delete audio_;
  data_ = 0; // RtAudio deletes the buffer itself.


}


void 
AudioSink::init()
{
  nChannels_ = getctrl("natural/nChannels").toNatural();
  real sampleRate = getctrl("real/israte").toReal();
  natural inSamples = getctrl("natural/inSamples").toNatural();
  int bufSize = (int)inSamples;

  RtAudio::RTAUDIO_FORMAT format = ( sizeof(real) == 8 ) ? RtAudio::RTAUDIO_FLOAT64 : RtAudio::RTAUDIO_FLOAT32;
  
  bufferSize_ = inSamples;      


  // Hardwire sampling rate and number of channels to stereo 44100 
  // deal with conversion in a hack kind of way in process 
  int rtSrate = 44100;
  int rtChannels = 2;

  bufferSize_ = 512;
  
  
  try {
    bufferSize_ =0; 
    audio_ = new RtAudio(&stream_, 0, rtChannels, 0, 0, format,
			 rtSrate, &bufferSize_, 4);
    data_ = (real *) audio_->getStreamBuffer(stream_);
  }
  catch (RtError &error) 
    {
      cout << "RT ERROR " << endl;
    }
  stopped_ = true;

}


void 
AudioSink::update()
{
  MRSDIAG("AudioSink::update");

  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  nChannels_ = getctrl("natural/nChannels").toNatural();
  sampleRate_ = getctrl("real/israte").toReal();
  natural inSamples = getctrl("natural/inSamples").toNatural();

  mute_ = getctrl("bool/mute").toBool();

  defaultUpdate();
  pwin_.create(inObservations_,inSamples_);
  din_.create(inObservations_, 2*inSamples_);
  pdin_.create(inObservations_,2*inSamples_);
 

  isInitialized_ = getctrl("bool/init").toBool(); 



}



void 
AudioSink::start()
{
  if ( stopped_ ) {
    audio_->startStream(stream_);
    stopped_ = false;
  }
}


void 
AudioSink::stop()
{
  if ( !stopped_ ) {
    audio_->stopStream(stream_);
    stopped_ = true;
  }
}





void 
AudioSink::process(realvec& in, realvec& out)
{
  if (!isInitialized_)
    {
      init();
      setctrl("bool/init", (MarControlValue)true);
      isInitialized_ = true;
    }
  checkFlow(in,out);

  
  natural nChannels = getctrl("natural/nChannels").toNatural();
  natural c = getctrl("natural/channel").toNatural();
  setctrl("natural/channel", (c+1)%nChannels);  

  // copy to output 
  din_.setval(0.0);
  pdin_.setval(0.0);
  
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
	din_(o,2*t) = in(o,t);
      }
  
  if (mute_) return;


  if ( stopped_ )
    start();
  
  
  if (inSamples_ < bufferSize_) 
    cout << "AudioSinkRtAudio can not handle inSamples " << inSamples_ << " less than the minimum buffer size which is " << bufferSize_ << endl;
  

  int i;

  if (c == nChannels -1) // HACK do everything when called for last channel
    {
      if (sampleRate_ == 44100.0)
	nBuffers_ = inSamples_ / bufferSize_;
      else if (sampleRate_ == 22050.0)
	nBuffers_ = 2 *inSamples_ / bufferSize_;
      
      for (i =0; i < nBuffers_; i++)
	{
	  if (nChannels == 1) 
	    {
	      if (sampleRate_ == 44100.0)
		for ( t=0; t<bufferSize_; t++ ) 
		  {
		    data_[2*t] = in(0,i*bufferSize_ +t);
		    data_[2*t+1] = in(0,i*bufferSize_+t);
		  }
	      else if (sampleRate_ == 22050.0)
		{
		  for ( t=0; t<bufferSize_; t++ ) 
		    {
		      data_[2*t] = din_(0,i*bufferSize_+t);
		      data_[2*t+1] = din_(0,i*bufferSize_+t);
		    }
		  
		}
	    }
	  else 
	    {
	      if (sampleRate_ == 44100.0)
		{
		  for ( t=0; t<bufferSize_; t++ ) 
		    {
		      data_[nChannels*t] = pwin_(0, i*bufferSize_ + t);
		      data_[nChannels*t+1] = in(0, i*bufferSize_ + t);
		    }
		}
	      else 
		if (sampleRate_ == 22050.0)
		  {
		    for ( t=0; t<bufferSize_; t++ ) 
		      {
			data_[2*t] = pdin_(0,i*bufferSize_ +t);
			data_[2*t+1] = din_(0,i*bufferSize_+t);
		      }		  
		  }
	    }

      
	  
	  
	  // else
	  // for ( t=0; t<bufferSize_; t++ ) 
	  // data_[nChannels*t+1] = in(0,i*bufferSize_+t);
	  
	  // data_[2*t+1] = in(0,i*bufferSize_+t);
	  // data_[nChannels *t + c] = in(0, i * (bufferSize_ / nChannels)+t);
	  
	  try {
	    audio_->tickStream(stream_);
	  }
	  catch (RtError &error) {
	    cout << "PROBLEM WITH RT AUDIO" << endl;
	  }
	}
      
    }
  
  
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	pwin_(o,t) = in(o,t);
	pdin_(o,2*t) = in(o,t);
      }
  
  
}











	
