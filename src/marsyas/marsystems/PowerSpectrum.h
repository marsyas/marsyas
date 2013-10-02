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

#ifndef MARSYAS_POWERSPECTRUM_H
#define MARSYAS_POWERSPECTRUM_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Magnitude/power of complex spectrum

	Computes the magnitude/power/decibels/powerdensity of a complex spectrum (as output from the
	Spectrum MarSystem - see its documentation for info about the spectrum
	format used in Marsyas). PowerSpectrum takes N/2+1 complex spectrum bins
	and computes the corresponding N/2+1 power/magnitude/decibels/powerdensity real values.

	Controls:
	- \b mrs_string/spectrumType [w] : choose between "power", "magnitude", "decibels",
	"logmagnitude" (for 1+log(magnitude*1000), "logmagnitude2" (for 1+log10(magnitude))
	and "powerdensity" computations

	\see Spectrum
*/


class PowerSpectrum: public MarSystem
{
private:
  std::string stype_;

  mrs_natural ntype_;
  mrs_natural N2_;
  mrs_real re_, im_;
  mrs_real dB_, pwr_;
  MarControlPtr ctrl_spectrumType_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PowerSpectrum(std::string name);
  PowerSpectrum(const PowerSpectrum&);
  ~PowerSpectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif




