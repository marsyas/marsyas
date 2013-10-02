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

#ifndef MARSYAS_SPECTRALCENTROIDBANDNORM_H
#define MARSYAS_SPECTRALCENTROIDBANDNORM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis

	Controls:
	- \b mrs_real/expected_peak [w] : expected peak in signal. Default: 100.0.
*/

class marsyas_EXPORT SpectralCentroidBandNorm: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  mrs_real expected_peak_;


public:
  /// SpectralCentroidBandNorm constructor.
  SpectralCentroidBandNorm(std::string name);

  /// SpectralCentroidBandNorm copy constructor.
  SpectralCentroidBandNorm(const SpectralCentroidBandNorm& a);

  /// SpectralCentroidBandNorm destructor.
  ~SpectralCentroidBandNorm();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_SPECTRALCENTROIDBANDNORM_H

