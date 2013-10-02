/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_CREST_H
#define MARSYAS_CREST_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis

	This calculates the PAR for a complete window of samples;
	it is similar to SCF (Spectral Crest Factor), however SCF
	calculates the PAR in various frequency bands of observations.

*/

class marsyas_EXPORT PowerToAverageRatio: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


public:
  /// PowerToAverageRatio constructor.
  PowerToAverageRatio(std::string name);

  /// PowerToAverageRatio copy constructor.
  PowerToAverageRatio(const PowerToAverageRatio& a);

  /// PowerToAverageRatio destructor.
  ~PowerToAverageRatio();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_CREST_H

