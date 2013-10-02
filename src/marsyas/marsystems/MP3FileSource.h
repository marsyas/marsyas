/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.cmu.edu>
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

#ifndef MARSYAS_MP3SOURCE_H
#define MARSYAS_MP3SOURCE_H

#include <cstdio>

#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#include <marsyas/common_header.h>
#include "AbsSoundFileSource.h"

extern "C" {
#include "mad.h"
}

namespace Marsyas
{
/**
   \class MP3FileSource
   \ingroup Internal
   \brief MP3FileSource read mp3 files using libmad
   \author Stuart Bray

   This class reads an mp3 file using the mad mp3 decoder library.  Some
   of this code was inspired from Bertrand Petit's madlld example.  The
   code to resize the buffers was borrowed from Marsyas AudioSource.
*/


class MP3FileSource: public AbsSoundFileSource
{
private:
  MarControlPtr ctrl_pos_;
  void addControls();
  void myUpdate(MarControlPtr sender);
  mrs_natural getLinear16(realvec& slice);

  // MAD stuff
  inline signed int scale(mad_fixed_t sample);
  void madStructInitialize();
  void madStructFinish();

  void fillStream( long offset = 0 );
  void closeFile();

  void  PrintFrameInfo(struct mad_header *Header);

  // MAD stuff
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;

  mrs_natural fileSize_;
  mrs_natural frameSamples_;
  mrs_natural totalFrames_;
  mrs_natural frameCount_;
  unsigned char* ptr_;
  int fd;
  FILE* fp;
  struct stat myStat;

  // variables for buffer balancing
  int bufferSize_;
  mrs_natural currentPos_;
  realvec reservoir_;
  mrs_natural reservoirSize_;
  mrs_natural preservoirSize_;
  mrs_natural pnChannels;

  mrs_natural ri_;
  long offset;

  mrs_natural advance_;
  mrs_natural cindex_;

  mrs_real duration_;
  mrs_natural csize_;
  mrs_natural size_;
  mrs_natural samplesOut_;
  std::string filename_;
  mrs_string debug_filename;

  mrs_real repetitions_;

public:

  MP3FileSource(std::string name);
  MP3FileSource(const MP3FileSource& a);
  ~MP3FileSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  void getHeader(std::string filename);


};

}//namespace Marsyas

#endif
