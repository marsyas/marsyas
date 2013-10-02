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

#ifndef MARSYAS_SUBTRACT_H
#define MARSYAS_SUBTRACT_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing Basic
	\brief Subtracts one observation from another.

	Each sample of the second input observation gets subtracted from the
	corresponding sample of	the first input observation, to form a single
	output observation.	Other input observations are discarded.

	No controls.
*/

class Subtract: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_gain_EXAMPLE_;

public:
  Subtract(std::string name);
  Subtract(const Subtract& a);
  ~Subtract();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

