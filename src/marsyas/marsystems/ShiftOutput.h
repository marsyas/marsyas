
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

#ifndef MARSYAS_SHIFTOUTPUT_H
#define MARSYAS_SHIFTOUTPUT_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing Basic
	\brief Pass on N amount of samples from the input vector

	ShiftOutput outputs a slice of the input vector. It retains all
	observations, but only a limited amount of samples (starting from the
	first input sample).

	Controls:
	- \b mrs_natural/Interpolation [rw] : Amount of input samples to pass on,
	starting from the first input sample.
*/


class ShiftOutput: public MarSystem
{
private:
  mrs_natural interp_;

  MarControlPtr ctrl_Interpolation_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ShiftOutput(std::string name);
  ShiftOutput(const ShiftOutput& a);
  ~ShiftOutput();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif



