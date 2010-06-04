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

#ifndef MARSYAS_AIMHCL_H
#define MARSYAS_AIMHCL_H

#include "MarSystem.h"

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimHCL
	\ingroup Analysis
    \brief Halfwave rectification, compression and lowpass filtering

    Author Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>
*/


class marsyas_EXPORT AimHCL: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  // Reset all internal state variables to their initial values
  void ResetInternal();

  // Prepare the module
  bool InitializeInternal();

  bool initialized;

  MarControlPtr ctrl_do_lowpass_;     // Do lowpass filtering?
  MarControlPtr ctrl_do_log_;         // Do log compression?
  MarControlPtr ctrl_lowpass_cutoff_; // Cutoff frequency for lowpass filter
  MarControlPtr ctrl_lowpass_order_;  // Order of Lowpass Filter

  // int channel_count_;  // Internal record of the number of channels in the input
  float time_constant_;  // Time constant corresponsing to the lowpass filter cutoff freqency

  // Lowpass filter state variables
  float xn_;
  float yn_;
  std::vector<std::vector<float> > yns_;

public:
  AimHCL(std::string name);

  ~AimHCL();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
