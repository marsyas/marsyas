/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_STEREOSPECTRUM_H
#define MARSYAS_STEREOSPECTRUM_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/fft.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Computes the panning index for each spectrum bin of a stereo input.

	Input is expected to be the output of a parallel of two Spectrum MarSystems,
	one for each stereo channel).
*/

class StereoSpectrum: public MarSystem
{
private:
  mrs_natural N4_, N2_;
  mrs_real rel_, iml_, rer_, imr_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  StereoSpectrum(std::string name);
  StereoSpectrum(const StereoSpectrum&);
  ~StereoSpectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif






