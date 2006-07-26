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
   \brief Real-time Audio sink using RtAudio

   Real-time Audio Sink using RtAudio
*/


#ifndef MARSYAS_AUDIOSINK_H
#define MARSYAS_AUDIOSINK_H

#include "RtAudio.h"
#include "MarSystem.h"

namespace Marsyas
{

class AudioSink:public MarSystem
{
private:
  RtAudio *audio_;
  bool stopped_;
  int stream_;
  int bufferSize_;
  int counter_;
  int nBuffers_;
  bool isInitialized_;
  int rtSrate_;
  int srate_;
    
  mrs_natural nChannels_;
  mrs_real *data_;  
  realvec reservoir_;
  mrs_natural reservoirSize_;
  mrs_natural preservoirSize_;
  
  mrs_natural rstart_;
  mrs_natural ri_;
  
  mrs_natural start_;
  mrs_natural end_;
  mrs_natural diff_;
  mrs_natural rsize_;
  
  realvec pwin_;
  realvec din_;
  realvec pdin_;
   
  void addControls();
	void localUpdate();

	void initRtAudio();
	
  void start();
  void stop();

	void localActivate(bool state);
    
public:
  AudioSink(std::string name);
  ~AudioSink();
  MarSystem* clone() const;  

  void process(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	

	
	
