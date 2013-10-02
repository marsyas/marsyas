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

#ifndef MARSYAS_SHIFTER_H
#define MARSYAS_SHIFTER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Shifter
	\ingroup Processing Basic
	\brief Shifts the input and outputs the original and shifted version.

	Shifts the input by \<shift\> samples and send to output the original
	and shifted version as different observations.

	\author lmartins@inescporto.pt

	Controls:
	- \b mrs_natural/shift [rw] : number of samples to shift by.
*/


class Shifter: public MarSystem
{
private:
  mrs_natural shift_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Shifter(std::string name);
  ~Shifter();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif //MARSYAS_SHIFTER_H
