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

#ifndef MARSYAS_ONEPOLE_H
#define MARSYAS_ONEPOLE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class OnePole
	\ingroup Processing
	\brief Simple one-pole digital filter.

	OnePole implements the digital filter described by equation
		\f[ y(t) = (1 - \alpha) x(t) + \alpha  y(t - 1) \f]
	This filter has one pole at \f$ z = \alpha \f$.
	If \f$ \alpha > 0 \f$, the filter has a low-pass characteristic,
	the closer to 1 the smaller the pass band.
	If \f$ \alpha < 0 \f$, it has a high-pass characteristic,
	the closer to -1, the smaller the pass band.

	Note that the coefficient of \f$ x(t) \f$ is set to \f$ (1-\alpha) \f$
	to have unity gain for the DC component (both in the low pass and high
	pass case) as a sort of normalization.

	Controls:
	- \b mrs_real/alpha [w]: the \f$ \alpha \f$ parameter in the filter equation.

	\see Filter
*/


class OnePole: public MarSystem
{
private:

  /// Cache for the alpha value.
  mrs_real alpha_;

  /// Cache for the gain value.
  mrs_real gain_;

  /// Internal memory for the last samples of the previous observed slice.
  realvec previousOutputSamples_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  OnePole(std::string name);
  ~OnePole();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

