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

#ifndef MARSYAS_WAVELETBANDS_H
#define MARSYAS_WAVELETBANDS_H

#include <marsyas/system/MarSystem.h>
#include "WaveletPyramid.h"

namespace Marsyas
{
/**
	\class WaveletBands
	\ingroup none
	\brief Calculate time-domain wavelet-based bands

	Calculate time-domain wavelet-based bands by
	zeroing out all the coefficients except for the band
	we are interested in and then performing the Inverse
	Wavelet Transform to get back to time domain.
*/


class WaveletBands: public MarSystem
{
private:
  realvec band_;
  realvec tband_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  /// Inverse wavelet transform object.
  WaveletPyramid *iwvpt_;

public:
  WaveletBands(std::string name);
  ~WaveletBands();
  WaveletBands(const WaveletBands& a);

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif


