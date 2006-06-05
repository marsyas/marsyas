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



#ifndef MARSYAS_AUDIOSOURCE_H
#define MARSYAS_AUDIOSOURCE_H




#include "BaseAudio.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>


class AudioSource: public BaseAudioSource
{
private:
  int audio_fd_;
  int buf_size_;
  
public:
  AudioSource(unsigned int srate, unsigned int channels, unsigned int winSize);
  ~AudioSource();
  void get(fvec& vec);
};


#endif

	

	
