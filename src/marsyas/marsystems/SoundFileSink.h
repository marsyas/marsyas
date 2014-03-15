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

#ifndef MARSYAS_SOUNDFILESINK_H
#define MARSYAS_SOUNDFILESINK_H

#include <marsyas/realvec.h>
#include "AuFileSink.h"
#include "WavFileSink.h"

namespace Marsyas
{
/**
	\ingroup IO
	\brief Abstract interface for sound file sinks.

	Abstract base class for any type of sound file input.
	Specific IO classes for various formats like AuFileSink
	are children of this class.

	Controls:
	- \b mrs_string/filename [w] : name of the audio file to write

*/


class SoundFileSink: public MarSystem
{
private:
  virtual void addControls();
  virtual void myUpdate(MarControlPtr sender);

  std::string filename_;
  AbsSoundFileSink* backend_;

public:
  SoundFileSink(std::string name);
  SoundFileSink(const SoundFileSink& a);
  ~SoundFileSink();
  MarSystem* clone() const;
  bool updateBackend();

  virtual void myProcess(realvec& in, realvec& out);

  virtual void putHeader();

  // void putLinear16(mrs_natural c, realvec& win);
  // void putLinear16Swap(mrs_natural c, realvec& win);
  void putFloat(mrs_natural c, realvec& win);

};

}//namespace Marsyas

#endif /* !MARSYAS_SOUNDFILESINK_H */


