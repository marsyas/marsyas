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

#ifndef MARSYAS_AIMHCL2_H
#define MARSYAS_AIMHCL2_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimHCL2
	\ingroup Analysis
    \brief Halfwave rectification, compression and lowpass filtering

    Author Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>
	Made more Marsyas like by George Tzanetakis

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimHCL2: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  // Reset all internal state variables to their initial values
  void ResetInternal();

  // Prepare the module
  bool InitializeInternal();

  // Does the MarSystem need initialization?
  bool is_initialized;

  // What changes cause it to need initialization?
  mrs_real initialized_lowpass_cutoff;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_natural reseted_inobservations;
  mrs_real reseted_lowpass_order;

  MarControlPtr ctrl_do_lowpass_;     // Do lowpass filtering?
  MarControlPtr ctrl_do_log_;         // Do log compression?
  MarControlPtr ctrl_lowpass_cutoff_; // Cutoff frequency for lowpass filter
  MarControlPtr ctrl_lowpass_order_;  // Order of Lowpass Filter

  // int channel_count_;  // Internal record of the number of channels in the input
  double time_constant_;  // Time constant corresponsing to the lowpass filter cutoff freqency

  // Lowpass filter state variables
  double xn_;
  double yn_;
  std::vector<std::vector<double> > yns_;

public:
  AimHCL2(std::string name);

  ~AimHCL2();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
