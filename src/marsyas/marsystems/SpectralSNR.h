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

#ifndef MARSYAS_SPECTRALSNR_H
#define MARSYAS_SPECTRALSNR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Frequency domain signal-to-noise ratio.

	Takes as input two stacked PowerSpectrum-s.
*/

class SpectralSNR: public MarSystem
{
private:
  mrs_natural N2_;
  mrs_real orig_;
  mrs_real extr_;
  mrs_real sum_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  SpectralSNR(std::string name);
  SpectralSNR(const SpectralSNR&);
  ~SpectralSNR();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif








