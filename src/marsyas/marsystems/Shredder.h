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
** MERCHANTABILITY or FITNAESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_SHREDDER_H
#define MARSYAS_SHREDDER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Composites

	\brief Splits input and makes children process each slice separately
	(reverse of Accumulator).

	Splits input into time-slices, and makes children process the slices one after another.
	Typically used to restore the processing rate changed by an Accumulator.

	Controls:
	- \b mrs_natural/nTimes [rw] : Amount of slices to split input into.
	- \b mrs_bool/accumulate [w] : When true, the outputs of the child MarSystems are accumulated
	into the output.
*/

class Shredder: public MarSystem
{
private:
  mrs_natural nTimes_;
  realvec childIn_;
  realvec childOut_;
  mrs_natural childOnSamples_;

  MarControlPtr ctrl_nTimes_;
  MarControlPtr ctrl_accumulate_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Shredder(std::string name);
  Shredder(const Shredder& a);
  ~Shredder();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
