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

#ifndef MARSYAS_OGGSOURCE_H
#define MARSYAS_OGGSOURCE_H

#include <marsyas/common_header.h>
#include "AbsSoundFileSource.h"

// So we don't need to include vorbis headers,
// or create #ifdefs to hide them.
struct OggVorbis_File;
struct vorbis_info;

namespace Marsyas
{
/**
   \class OggFileSource
   \ingroup Internal
   \brief OggFileSource read ogg files using vorbisfile
   \author Taras Glek

   This class reads an ogg file using the vorbis ogg decoder library.
*/


class OggFileSource: public AbsSoundFileSource
{
private:

  void addControls();
  void myUpdate(MarControlPtr sender);
  void closeFile();

  OggVorbis_File *vf;
  vorbis_info *vi;

public:

  OggFileSource(std::string name);
  OggFileSource(const OggFileSource& a);
  ~OggFileSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  void getHeader(std::string filename);

};

}//namespace Marsyas

#endif
