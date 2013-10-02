/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_WAVFILESOURCE_H
#define MARSYAS_WAVFILESOURCE_H

#include <marsyas/common_header.h>
#include "AbsSoundFileSource.h"

#include <string>
#include <cstdio>
#include <cstring>

namespace Marsyas
{
/**
	\ingroup Internal
	\brief Reads .wav soundfiles
*/


class WavFileSource: public AbsSoundFileSource
{
private:
  int *idata_;
  short *sdata_;
  unsigned char *cdata_;

  FILE *sfp_;
  long sfp_begin_;

  mrs_natural sampleSize_; //in bytes
  mrs_natural csize_;
  mrs_natural size_;
  int ival_;
  short sval_;
  unsigned short bits_;
  MarControlPtr ctrl_pos_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned int ByteSwapInt(unsigned int nInt);
  unsigned short ByteSwapShort (unsigned short nValue);

  mrs_natural nChannels_;
  mrs_natural inSamples_;
  mrs_natural samplesToRead_;
  mrs_natural samplesRead_;
  mrs_natural samplesToWrite_;

  mrs_natural nt_;

  mrs_natural samplesOut_;

  mrs_real repetitions_;
  mrs_real duration_;

  mrs_natural getLinear32(realvec& win);
  mrs_natural getLinear16(realvec& win);
  mrs_natural getLinear8(realvec& win);


public:
  WavFileSource(std::string name);
  WavFileSource(const WavFileSource& a);

  ~WavFileSource();
  MarSystem* clone() const;


  void getHeader(std::string filename);
  void myProcess(realvec& in, realvec &out);
};

}//namespace Marsyas


#endif     /* !MARSYAS_WAVFILESOURCE_H */
