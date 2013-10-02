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

#ifndef MARSYAS_NORM_H
#define MARSYAS_NORM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Norm
	\ingroup Processing
	\brief Normalize each observation channel separately per slice.

	Normalize each observation channel by subtracting the observation's mean
	and dividing by observation's standard deviation.
	The mean and standard deviation are (re)calculated per slice.

	\see NormMaxMin, NormMatrix
*/


class Norm: public MarSystem
{
public:
  Norm(std::string name);
  ~Norm();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
