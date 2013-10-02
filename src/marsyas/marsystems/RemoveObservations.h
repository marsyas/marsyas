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

#ifndef MARSYAS_REMOVEOBSERVATIONS_H
#define MARSYAS_REMOVEOBSERVATIONS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Removes observations outside a given range.

	Mainly used to remove bins from a PowerSpectrum which fall
	outside the range of interest.  This MarSystem includes both
	cutoff values; it rounds down for the lowCutoff, and rounds up for
	the highCutoff.

	Controls:
	- \b mrs_real/lowCutoff [w] : the lowest observation to
	  pass to output, as a decimal percent of the total number
	  of observations (default 0.0).
	- \b mrs_real/highCutoff [w] : the highest observation to
	  pass to output, as a decimal percent of the total number
	  of observations (default 1.0).

*/

class marsyas_EXPORT RemoveObservations: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  mrs_natural lowestObs_, numObs_;

public:
  /// RemoveObservations constructor.
  RemoveObservations(std::string name);

  /// RemoveObservations copy constructor.
  RemoveObservations(const RemoveObservations& a);

  /// RemoveObservations destructor.
  ~RemoveObservations();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_REMOVEOBSERVATIONS_H

