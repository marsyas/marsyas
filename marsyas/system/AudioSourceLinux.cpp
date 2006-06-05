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
   \class AudioSource
   \brief Real-time Audio source for Linux

   Real-time Audio Source for Linux based on the OSS sound API. 
*/



#include "AudioSource.h"


AudioSource::AudioSource(unsigned int srate, unsigned int channels, 
			 unsigned int winSize):BaseAudioSource(srate, channels,
							       winSize)
{
  char DEVICE_NAME[100];
  int format;
  int stereo =0;			// for now only mono
  
  channels_ = channels;
  buf_size_ = winSize_;
  
  strcpy(DEVICE_NAME, "/dev/dsp");

  if (mode == MRS_AUDIO_PLAY)
    {
      if ((audio_fd_ = open(DEVICE_NAME, O_WRONLY, 0)) == -1)
	{
	  cerr << "Problem with opening audio device for playing" << endl;
	  exit(1);
	}
    }
  if (channels != 1)
    {
      cerr << "For now only one channel supported" << endl;
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

  if (ioctl(audio_fd_, SNDCTL_DSP_SPEED, &srate_) == -1)
    {
      cerr << "Cannot set sample rate" << endl;
      exit(1);
    }  
  sdata_ = new short[2*winSize_];
}

AudioSource::~AudioSource()
{
  close(audio_fd_);
}

unsigned long AudioSource::iterations()
{
  return 0;
}


void 
AudioSource::get(fvec& win, unsigned long i)
{
  if (win.size() != winSize_)
    {
      cerr << "AudioSource device set with different window size. No playback.\n" << endl;
      return;
    }
  int size = win.size();
  read(audio_fd_, (void *)sdata_, size * 2);
  for (int i=0; i<size; i++)
    win(i) = (float) sdata_[i] / FMAXSHRT;
  
}


	










