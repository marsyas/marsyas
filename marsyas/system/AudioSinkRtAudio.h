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
   \brief Real-time Audio sink using RtAudio

   Real-time Audio Sink using RtAudio
*/



#ifndef MARSYAS_AUDIOSINK_H
#define MARSYAS_AUDIOSINK_H


#include "SignalSink.h"
#include "RtAudio.h"
#include "TimeStretch.h"

#include <stdio.h>



class AudioSink: public SignalSink
{
private:
  RtAudio *audio_;
  bool stopped_;
  int stream_;
  int bufferSize_;
  int counter_;
  int nBuffers_;
  bool isInitialized_;
  
  
  natural nChannels_;
  real *data_;  
  realvec pwin_;
  realvec din_;
  realvec pdin_;
  
  
  real sampleRate_;
  
  
  void addControls();
  void AudioSink::start();
  void AudioSink::stop();
  
  
public:
  AudioSink();
  AudioSink(string name);

  ~AudioSink();
  MarSystem* clone() const;  
  void init();
  void update();
  void process(realvec& in, realvec& out);
};


#endif

	

	
	
