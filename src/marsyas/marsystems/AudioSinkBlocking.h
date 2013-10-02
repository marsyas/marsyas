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


#ifndef MARSYAS_AUDIOSINK_BLOCKING_H
#define MARSYAS_AUDIOSINK_BLOCKING_H


#include <marsyas/common_header.h>

#include <marsyas/system/MarSystem.h>




class RtAudio3;



namespace Marsyas
{


/**
   \class AudioSinkBlocking
   \ingroup IO
   \brief Real-time Audio source for Linux

   Real-time Audio Sink based on RtAudio.

   Controls:
   - \b mrs_natural/bufferSize [rw] : size of audio buffer (in samples)
   - \b mrs_bool/initAudio [w] : initialize audio (this should be \em true)
*/






class AudioSinkBlocking:public MarSystem
{
private:
  RtAudio3 *audio_;

  int bufferSize_;
  int rtSrate_;
  int rtChannels_;
  int srate_;
  unsigned int rtDevice_;

  bool isInitialized_;
  bool stopped_;

  mrs_natural nChannels_;
  mrs_real *data_;
  realvec reservoir_;
  mrs_natural reservoirSize_;
  mrs_natural preservoirSize_;
  mrs_natural pnChannels_;

  mrs_natural start_;
  mrs_natural end_;
  mrs_natural diff_;
  mrs_natural rsize_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  void initRtAudio();

  void start();
  void stop();

  void localActivate(bool state);

public:
  AudioSinkBlocking(std::string name);
  ~AudioSinkBlocking();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif





