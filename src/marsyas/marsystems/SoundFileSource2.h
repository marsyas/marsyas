/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_SOUNDFILESOURCE2_H
#define MARSYAS_SOUNDFILESOURCE2_H

#include <string>

#include <marsyas/realvec.h>

#include "AbsSoundFileSource2.h"
#include "WavFileSource2.h"

namespace Marsyas
{
/**
\class SoundFileSource2
\ingroup Experimental
\brief Interface for sound file sources
\todo Merge this with SoundFileSource (and all the other soundfileXXX2 stuff)

Abstract base class for any type of sound file input.
Specific IO classes for various formats like AuFileSource
are children of this class.
*/

class SoundFileSource2: public MarSystem
{
private:
  std::string filename_;
  AbsSoundFileSource2* src_;

  void addControls();

  void myUpdate(MarControlPtr sender);

  bool checkType();
  void fileReady(bool ready);

public:
  SoundFileSource2(std::string name);
  SoundFileSource2(const SoundFileSource2& a);
  ~SoundFileSource2();
  MarSystem* clone() const;

  realvec& getAudioRegion(mrs_natural startSample, mrs_natural endSample);

  virtual void myProcess(realvec& in,realvec& out);

};

}//namespace Marsyas

#endif /* MARSYAS_SOUNDFILESOURCE2_H */
