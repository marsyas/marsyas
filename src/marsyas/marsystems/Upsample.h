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

#ifndef MARSYAS_UPSAMPLE_H
#define MARSYAS_UPSAMPLE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup none
	\brief Upsamples input by integer factor using chosen interpolation.
	Frame size is also increased by the same factor.

	Upsamples a signal by a factor n so that `out[t]=in[n*t]`.
	Also runs some kind of interpolation to find out the value for unknown samples
	(see description of the **mrs_string/mode** control).

	 Controls:
	 - \b mrs_natural/factor [w] : Adjust the upsampling factor.
	 - \b mrs_string/mode [w] : Change the interpolation mode
		- "none": unknown samples are unchanged.
		- "repeat": unknown sample [t] equals known sample [t+1]
	 - \b mrs_float/default [w] : Unchanged values are set to this value.
*/


class Upsample: public MarSystem
{
private:
  void addControls();

public:
  Upsample(std::string name);
  ~Upsample();
  MarSystem* clone() const;
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


