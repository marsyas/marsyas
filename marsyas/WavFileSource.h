/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
   \class WavFileSource
   \brief SoundFileSource for .wav soundfiles
   
   SoundFileSource reader for .wav sound files.
*/


#ifndef MARSYAS_WAVFILESOURCE_H
#define MARSYAS_WAVFILESOURCE_H



#include "common.h"
#include "AbsSoundFileSource.h"


#include <string>
#include <stdio.h>


class WavFileSource: public AbsSoundFileSource
{
private:

  std::string tfname_;
  std::string filename_;
  
  short *sdata_;
  unsigned char *cdata_;
  FILE *sfp_;
  long sfp_begin_;
  
  natural sampleSize_;			// in bytes
  natural csize_;
  
  natural size_;  

  short sval_;
  bool advance_;
  natural cindex_;
  

  unsigned short bits_;

  void addControls();
  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);

  
  natural nChannels_;
  natural inSamples_;
  natural samplesToRead_;
  natural samplesRead_;
  natural samplesToWrite_;

  natural samplesOut_;
  
  real repetitions_;
  real duration_;
  
  
  
public:
  WavFileSource(std::string name);
  ~WavFileSource();
  MarSystem* clone() const;  

  natural getLinear16(realvec& win);
  natural getLinear8(natural c, realvec& win);

  void update();
  void getHeader(std::string filename);
  void process(realvec& in, realvec &out);
};




#endif     /* !MARSYAS_WAVFILESOURCE_H */ 

	
