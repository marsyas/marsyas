/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_SPECTRALTRANSFORMATIONS_H
#define MARSYAS_SPECTRALTRANSFORMATIONS_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Various spectral transformations. Input and output are complex spectrums.
	Mainly used to demonstrate how spectral analyis works.


	Controls:
	- \b mrs_real/gain [w]   : sets the gain multiplier.
	- \b mrs_string/mode [w] : sets the specific transformation to be used (for example PhaseRandomize or SingleBin).

*/

class SpectralTransformations: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_gain_;
  MarControlPtr ctrl_mode_;

  mrs_real N2_, re_, im_, mag_, phs_;

public:
  SpectralTransformations(std::string name);
  SpectralTransformations(const SpectralTransformations& a);
  ~SpectralTransformations();
  MarSystem* clone() const;
  void phaseRandomize(realvec& in, realvec& out);
  void singlebin(realvec& in, realvec& out);
  void three_peaks(realvec& in, realvec& out);
  void compress_magnitude(realvec& in, realvec& out);

  void myProcess(realvec& in, realvec& out);
};

}

#endif
