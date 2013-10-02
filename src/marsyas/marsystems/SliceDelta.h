/*
** Copyright (C) 2009 Stefaan Lippens
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

#ifndef MARSYAS_SLICEDELTA_H
#define MARSYAS_SLICEDELTA_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class SliceDelta
	\ingroup Processing
	\brief Takes the difference (delta) between successive input slices.

	Simple MarSystem that outputs the difference of the current input slice
	with the previous input slice. Unlike the Differentiator MarSystem, which
	takes differences at sample level, the difference is taken at slice level.

	This MarSystem has no extra controls.

	\see Differentiator
*/

class SliceDelta: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  /// Internal memory for the previous observed slice.
  realvec previousInputSlice_;

public:
  /// SliceDelta constructor.
  SliceDelta(std::string name) : MarSystem("SliceDelta", name) {};

  /// SliceDelta copy constructor.
  SliceDelta(const SliceDelta& a) : MarSystem(a) {};

  /// SliceDelta destructor.
  ~SliceDelta() {};

  /// Implementation of MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of MarSystem::myProcess.
  void myProcess(realvec& in, realvec& out);
};

}

#endif
