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

#ifndef MARSYAS_MP3FILESINK_H
#define MARSYAS_MP3FILESINK_H

#include <marsyas/common_header.h>
#include "SoundFileSink.h"
#include <marsyas/FileName.h>
#include "AbsSoundFileSink.h"

#include "lame/lame.h"

namespace Marsyas
{
/**
   \class MP3FileSink
   \ingroup Internal
   \brief MP3FileSink for .MP3 soundfiles

   SoundFileSink writer for .MP3 sound files.
*/

class MP3FileSink: public AbsSoundFileSink
{
private:
  unsigned long written_;
  long fpos_;

  lame_global_flags *gfp_;
  unsigned char *mp3Buffer_;
  short int *leftpcm_;
  short int *rightpcm_;

  std::string filename_;
  FILE *sfp_;
  long sfp_begin_;
  mrs_natural nChannels_;

  unsigned long ByteSwapLong(unsigned long nLongNumber);
  unsigned short ByteSwapShort (unsigned short nValue);

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  MP3FileSink(std::string name);
  ~MP3FileSink();
  MarSystem* clone() const;

  void putHeader(std::string filename);
  bool checkExtension(std::string filename);
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif    /* !MARSYAS_MP3FILESINK_H */





