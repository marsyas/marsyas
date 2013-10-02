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

#ifndef MARSYAS_DOWNSAMPLER_H
#define MARSYAS_DOWNSAMPLER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class DownSampler
	\ingroup Processing Basic
	\brief Downsample the input signal by dropping samples.

	Downsample the signal by an integer factor N. The output of the DownSampler
	will only reproduce the input samples at multiples of N.

	Note that the DownSampler does not take care of any low pass filtering to
	avoid frequency aliasing.

	Note that the DownSampler restarts counting and dropping samples for every
	given slice. Consequently, if the number of input samples is not divisible
	by the downsample factor, the sampling will be not uniform around the slice
	borders.

	Controls:
	- \b mrs_natural/factor [w] : factor to downsample by.
*/


class DownSampler: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  /// MarControl pointer for the downsample factor.
  MarControlPtr ctrl_factor_;

public:
  /// DownSampler constructor.
  DownSampler(std::string name);

  /// DownSampler copy constructor.
  DownSampler(const DownSampler& a);

  /// DownSampler destructor.
  ~DownSampler();

  /// DownSampler clone method.
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

