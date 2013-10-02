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

#ifndef MARSYAS_SPECTRALFLATNESSALLBANDS_H
#define MARSYAS_SPECTRALFLATNESSALLBANDS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Calculates a single spectral flatness value.

	Similar to SFM, but that marsystem splits the observations
	into bands, whereas this one uses the whole range of observations
	to generate one number.  It may be used to 'narrow in' on a
	specific range by putting a RemoveObservations marsystem before
	it.
*/

class marsyas_EXPORT SpectralFlatnessAllBands: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


public:
  /// SpectralFlatnessAllBands constructor.
  SpectralFlatnessAllBands(std::string name);

  /// SpectralFlatnessAllBands copy constructor.
  SpectralFlatnessAllBands(const SpectralFlatnessAllBands& a);

  /// SpectralFlatnessAllBands destructor.
  ~SpectralFlatnessAllBands();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_SPECTRALFLATNESSALLBANDS_H

