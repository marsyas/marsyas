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

#ifndef MARSYAS_DIFFERENTIATOR_H
#define MARSYAS_DIFFERENTIATOR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Differentiator
	\ingroup Processing Basic
	\brief Calculate the difference of successive input samples.

	Simple MarSystem that calculates the differences of successive input samples
	in the given slices. For the first sample of a slice, the last sample of
	the previous slice is used.

	This MarSystem has no extra controls.

	\see SliceDelta
*/

class Differentiator: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  /// Buffer for storing the last column of samples in a slice.
  realvec buffer_;

public:
  Differentiator(std::string name);
  Differentiator(const Differentiator& a);
  ~Differentiator();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

