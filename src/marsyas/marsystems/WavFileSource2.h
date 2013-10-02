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

#ifndef MARSYAS_WAVFILESOURCE2_H
#define MARSYAS_WAVFILESOURCE2_H

#include <marsyas/common_header.h>
#include "AbsSoundFileSource2.h"

#include <string>
#include <cstring>
#include <cstdio>

namespace Marsyas
{
/**
	\ingroup Experimental
	\brief Reads .wav soundfiles
*/


class WavFileSource2: public AbsSoundFileSource2
{
private:
  short *sdata_;
  unsigned char *cdata_;

  FILE *sfp_;
  long sfp_begin_;

  mrs_natural sampleSize_; //in bytes
  short sval_;
  unsigned short bits_;

  mrs_natural samplesToRead_;
  mrs_natural samplesRead_;
  mrs_natural samplesToWrite_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);

  //mrs_natural getLinear8(mrs_natural c, realvec& win);
  mrs_natural getLinear16(realvec& win);

  void hdrError();
  bool getHeader();

public:
  WavFileSource2(std::string name);
  WavFileSource2(const WavFileSource2& a); //[!][?]
  ~WavFileSource2();
  MarSystem* clone() const;

  realvec& getAudioRegion(mrs_natural startSample, mrs_natural endSample);

  void myProcess(realvec& in, realvec &out);
};

}//namespace Marsyas


#endif     /* MARSYAS_WAVFILESOURCE2_H */


