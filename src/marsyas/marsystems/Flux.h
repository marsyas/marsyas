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

#ifndef MARSYAS_FLUX_H
#define MARSYAS_FLUX_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class Flux
    \ingroup Analysis
    \brief Flux calculate the flux between the current and prev. spectrum (e.g. output of PowerSpectrum)

    The flux is defined as the norm of the difference vector between
    two successive magnitue/power spectra, although different implementations are possible.

		Controls:
		- \b mrs_string/mode [w]: select from the different available implementations for Flux: "marsyas" and "DixonDAFX06"
		- \b mrs_bool/reset [rw] : clear and reset the memory buffer
*/

class Flux: public MarSystem
{
private:
  realvec prevWindow_;
  mrs_real diff_,flux_, max_;
  mrs_real logtmp_;
  mrs_bool reset_;

  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_reset_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Flux(std::string name);
  Flux(const Flux& a);
  ~Flux();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif






