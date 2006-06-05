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
  audio_fd_ = 0;
  sdata_ = 0;
}

AudioSink::AudioSink(string name)
{
  type_ = "AudioSink";
  name_ = name;
  audio_fd_ = 0;
  sdata_ = 0;
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
  update();
}



AudioSink::~AudioSink()
{
  delete sdata_;
  close(audio_fd_);
}

void 
AudioSink::update()
{
  MRSDIAG("AudioSink::update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  nChannels_ = getctrl("natural/nChannels").toNatural();
  if (audio_fd_ != 0) 
    close(audio_fd_);
  if (sdata_ != 0)
    delete sdata_;

  // extra room for stereo playback of mono 
  if (nChannels_ == 1)
    sdata_ = new short[2 * getctrl("natural/inSamples").toNatural() * nChannels_];
  else
    sdata_ = new short[getctrl("natural/inSamples").toNatural() * nChannels_];
  
  char DEVICE_NAME[100];
  int format;
  int stereo =0;			// for now only mono
  
  buf_size_ = getctrl("natural/inSamples").toNatural() * nChannels_;
  
  strcpy(DEVICE_NAME, "/dev/dsp");
  
  if ((audio_fd_ = open(DEVICE_NAME, O_WRONLY, 0)) == -1)
    {
      cerr << "Problem with opening audio device for playing" << endl;
      exit(1);
    }
  
  format = AFMT_S16_LE;
  if (ioctl(audio_fd_, SNDCTL_DSP_SETFMT, &format)==-1)
    { /* Fatal error */
      close(audio_fd_);
      cerr << "SNDCTL_DSP_SETFMT error" << endl;
      exit(0);
    }
  
  stereo = 1;  
  if (ioctl(audio_fd_, SNDCTL_DSP_STEREO, &stereo) == -1)
    {
      cerr << "Cannot set # of channels" << endl;
      exit(1);
    }
  
  int srate = (int)getctrl("real/israte").toReal();
  
  if (ioctl(audio_fd_, SNDCTL_DSP_SPEED, &srate) == -1)
    {
      cerr << "Cannot set sample rate" << endl;
      exit(1);
    }  

  defaultUpdate();
}






void 
AudioSink::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  bool mute = getctrl("bool/mute").toBool();

  
  natural c = getctrl("natural/channel").toNatural();
  setctrl("natural/channel", (c+1)%nChannels_);

  // copy to output 
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
      }
  if (mute) return;

  // write to audio device 
  if (nChannels_ == 1) // mono - playback stereo 
    {
      
      for (t=0; t < inSamples_; t++)
	{
	  sdata_[2 * t] = (short)(in(0,t) * MAXSHRT);
	  sdata_[2 * t+1] = (short)(in(0,t) * MAXSHRT);
	}
      write(audio_fd_, (void *)sdata_, 4 * nChannels_ * inSamples_);
    }
  else					// stereo or more 
    {
      for (t=0; t < inSamples_; t++)
	{
	  sdata_[nChannels_ * t + c] = (short)(in(0,t) * MAXSHRT);
	}
      
      // write when called for last channel 
      
      if (c == nChannels_ -1)
	{
	  write(audio_fd_, (void *)sdata_, 2 * nChannels_ * inSamples_);
	}
    }
}










