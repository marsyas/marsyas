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

#ifndef MARSYAS_WAVFILESINK_H
#define MARSYAS_WAVFILESINK_H

#include <marsyas/common_header.h>
#include "SoundFileSink.h"
#include "WavFileSource.h"
#include <marsyas/FileName.h>
#include "AbsSoundFileSink.h"

namespace Marsyas
{
struct wavhdr {
  char riff[4];           // "RIFF"
  signed int file_size;      // in bytes

  char wave[4];           // "WAVE"

  char fmt[4];            // "fmt "
  signed int chunk_size;     // in bytes (16 for PCM)
  signed short format_tag;     // 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
  signed short num_chans;      // 1=mono, 2=stereo
  signed int sample_rate;
  signed int bytes_per_sec;
  signed short bytes_per_samp; // 2=16-bit mono, 4=16-bit stereo
  signed short bits_per_samp;
  char data[4];           // "data"
  signed int data_length;    // in bytes
};

/**
\ingroup Internal
\brief Writes to .wav soundfiles.
*/
class WavFileSink: public AbsSoundFileSink
{
private:
  wavhdr hdr_;
  unsigned long written_;
  long fpos_;

  short * sdata_;
  unsigned char *cdata_;

  std::string filename_;
  FILE *sfp_;
  long sfp_begin_;
  mrs_natural nChannels_;

  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  WavFileSink(std::string name);
  ~WavFileSink();
  MarSystem* clone() const;

  void putLinear16Swap(realvec& slice);
  void putHeader(std::string filename);
  bool checkExtension(std::string filename);
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif    /* !MARSYAS_WAVFILESINK_H */





