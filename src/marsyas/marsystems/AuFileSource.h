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

#ifndef MARSYAS_AUFILESOURCE_H
#define MARSYAS_AUFILESOURCE_H

#include "AbsSoundFileSource.h"


#include <string>
#include <vector>

namespace Marsyas
{
/**
   \class AuFileSource
   \ingroup Internal
   \brief SoundFileSource for .au (.snd) soundfiles

   SoundFileSource reader for .au (.snd) sound files.
   (Next, Sun audio format).
*/


struct snd_header;


class AuFileSource: public AbsSoundFileSource
{

private:
  snd_header* hdr_;
  std::string tfname_;
  std::string filename_;

  short *sdata_;
  short sval_;
  unsigned short usval_;

  unsigned char *cdata_;
  FILE *sfp_;
  long sfp_begin_;

  mrs_natural sampleSize_;// in bytes
  mrs_natural size_;
  mrs_natural csize_;

  mrs_natural samplesOut_;

  std::vector<std::string> sndFormats_;
  std::vector<int>    sndFormatSizes_;

  mrs_natural nChannels_;
  mrs_natural samplesToRead_;
  mrs_natural samplesToWrite_;
  mrs_natural samplesRead_;

  mrs_real duration_;
  mrs_natural advance_;
  mrs_natural cindex_;
  mrs_natural nt_;

  mrs_real repetitions_;

  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);

  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_size_;
  MarControlPtr ctrl_pos_;


public:
  AuFileSource(std::string name);
  AuFileSource(const AuFileSource& a);
  ~AuFileSource();
  MarSystem* clone() const;

  void getHeader(std::string filename);
  mrs_natural getLinear16(realvec& win);

  void myProcess(realvec& in,realvec &out);

};

}//namespace Marsyas

#endif    /* !MARSYAS_AUFILESOURCE_H */


