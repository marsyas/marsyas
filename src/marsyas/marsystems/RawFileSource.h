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

#ifndef MARSYAS_RAWFILESOURCE_H
#define MARSYAS_RAWFILESOURCE_H

#include "AbsSoundFileSource.h"
#include <sys/stat.h>

namespace Marsyas
{
/**
   \class RawFileSource
   \ingroup Interal
   \brief SoundFileSource for .raw wavetable files

   STK rawwave files have no header and are assumed to contain a
   monophonic stream of 16-bit signed integers in big-endian byte
   order with a sample rate of 22050 Hz.

*/


class RawFileSource : public AbsSoundFileSource
{
private:
  FILE *sfp_;
  mrs_real time_;
  mrs_real rate_;				// loop frequency

  short *buffer_;


  realvec data_;
  mrs_real phaseOffset_;

  unsigned long fileSize_;
  unsigned long bufferSize_;
  bool byteSwap_;
  mrs_natural ch_, pos_, nChannels_;

  void addControls();
  void myUpdate(MarControlPtr sender);
  void swap16(unsigned char *ptr);

  void readData( unsigned long index );
  bool getRawInfo( const char *fileName );
  void getHeader(std::string filename);

public:
  RawFileSource(std::string name);
  ~RawFileSource();

  MarSystem* clone() const;
  void myProcess(realvec& in,realvec &out);


  // helpers for synthesis routines
  void setFrequency(mrs_real frequency);
  void openFile(std::string filename); //[?] not coherent with interface defined by AbsSoundFileSource!
};

}//namespace Marsyas

#endif    /* !MARSYAS_RAWFILESOURCE_H */
