/*
** Copyright (C) 2008 Soren Harward <stharward@gmail.com>
**  
** Marsyas object code copied from RawFileSource by George
** Tzanetakis <gtzan@cs.cmu.edu>.  GStreamer wrapper code adapted from
** code in Mirage project by Dominik Schnitzer <dominik@schnitzer.at>.
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

#include "config.h"



#ifndef MARSYAS_GSTREAMERSOURCE_H
#define MARSYAS_GSTREAMERSOURCE_H

#include "AbsSoundFileSource.h"
#include <sys/stat.h>

namespace Marsyas
{
/**
   \class GStreamerSource
	\ingroup Interal
   \brief SoundFileSource which uses GStreamer to decode an input file
   
    The GStreamer decoder always returns 44.1kHz, stereo, double-precision (ie, mrs_real) audio data
   
*/


class GStreamerSource : public AbsSoundFileSource
{
private: 
  //FILE *sfp_;
  mrs_real time_;
  mrs_real rate_;				// loop frequency
  
  //short *buffer_;
  
  
  mrs_real *data_;
  mrs_real phaseOffset_;
  
  mrs_natural fileSize_;
  bool byteSwap_;
  std::string filename_;
  mrs_natural ch_, pos_, nChannels_, sampleCount_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  
  void getHeader(std::string filename);

public:
  GStreamerSource(std::string name);
  ~GStreamerSource();
  
  MarSystem* clone() const;
  void myProcess(realvec& in,realvec &out);
  
};

}//namespace Marsyas

#endif    // !MARSYAS_GSTREAMERSOURCE_H



