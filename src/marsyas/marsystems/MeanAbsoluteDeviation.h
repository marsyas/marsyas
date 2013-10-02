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

#ifndef MARSYAS_MEANABSOLUTEDEVIATION_H
#define MARSYAS_MEANABSOLUTEDEVIATION_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class MeanAbsoluteDeviation
	\ingroup Analysis
	\brief Calculates the mean absolute deviation

*/

class marsyas_EXPORT MeanAbsoluteDeviation: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


public:
  /// MeanAbsoluteDeviation constructor.
  MeanAbsoluteDeviation(std::string name);

  /// MeanAbsoluteDeviation copy constructor.
  MeanAbsoluteDeviation(const MeanAbsoluteDeviation& a);

  /// MeanAbsoluteDeviation destructor.
  ~MeanAbsoluteDeviation();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_MEANABSOLUTEDEVIATION_H

