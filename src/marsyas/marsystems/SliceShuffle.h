/*
** Copyright (C) 2010 Stefaan Lippens
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

#ifndef MARSYAS_SLICESHUFFLE_H
#define MARSYAS_SLICESHUFFLE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class SliceShuffle
	\ingroup Processing
	\brief Shuffle the incoming slices randomly

	Incoming slices are stored in a buffer and on each process call
	a random one is drawn as output.

	Controls:
	- \b mrs_natural/bufferSize [w] : set the buffer size in number of slices.
*/

class SliceShuffle: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// MarControlPtr for the buffer size (number of slices that fit in buffer).
  MarControlPtr ctrl_bufferSize_;

  /// Caching variable for the buffer size
  mrs_natural bufferSize_;

  /// Caching variable for the number of samples per slice
  mrs_natural nSamples_;

  /// Internal memory for the previous observed slice.
  realvec sliceBuffer_;


public:
  /// SliceShuffle constructor.
  SliceShuffle(std::string name);

  /// SliceShuffle copy constructor.
  SliceShuffle(const SliceShuffle& a);

  /// SliceShuffle destructor.
  ~SliceShuffle();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_SLICESHUFFLE_H

