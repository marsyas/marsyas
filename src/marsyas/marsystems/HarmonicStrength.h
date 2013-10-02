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

#ifndef MARSYAS_HARMONICSTRENGTH_H
#define MARSYAS_HARMONICSTRENGTH_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class HarmonicStrength
	\ingroup Processing
	\brief Calculates the relative strength of harmonics

	Calculates the strength of multiples of the base_frequency
compared to the overall spectrum rms.

	Controls:
	- \b mrs_real/base_frequency [w] : sets the fundamental
	  frequency.
	- \b mrs_realvec/harmonics [w] : which harmonics to examine.
	  Need not be integer multiples (e.g. 0.5, 1.5, is ok)
	- \b mrs_realvec/harmonicsSize [w] : how many harmonics
          (long story, it's a workaround for some weird memory thing)
	- \b mrs_real/harmonicsWidth [w] : percent of frequency range to
      search for a peak.
    - \b mrs_natural/type [w] : 0 = output harmonic strength
      relative to spectrum rms; 1 = output absolute (interpolated)
      power of harmonic strength; 2 = log of #1
    -b \b mrs_real/inharmonicity_B : string stiffness
*/

class marsyas_EXPORT HarmonicStrength: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  MarControlPtr ctrl_base_frequency_;
  MarControlPtr ctrl_harmonics_;
  MarControlPtr ctrl_harmonicsSize_;
  MarControlPtr ctrl_harmonicsWidth_;
  MarControlPtr ctrl_inharmonicity_B_;

  mrs_real find_peak_magnitude(mrs_real central_bin,
                               mrs_realvec& in, mrs_natural t,
                               mrs_real low, mrs_real high);
  mrs_real quadratic_interpolation(mrs_real best_bin,
                                   mrs_realvec& in, mrs_natural t);

public:
  /// HarmonicStrength constructor.
  HarmonicStrength(std::string name);

  /// HarmonicStrength copy constructor.
  HarmonicStrength(const HarmonicStrength& a);

  /// HarmonicStrength destructor.
  ~HarmonicStrength();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_HARMONICSTRENGTH_H

