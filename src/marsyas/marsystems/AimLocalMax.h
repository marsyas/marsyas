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

#ifndef MARSYAS_AIMLOCALMAX_H
#define MARSYAS_AIMLOCALMAX_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimLocalMax
	\ingroup Analysis
    \brief Local maximum strobe criterion: decaying threshold with timeout

    Author : Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimLocalMax: public MarSystem
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
  mrs_real initialized_israte;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_natural reset_inobservations;

  MarControlPtr ctrl_decay_time_ms_;
  MarControlPtr ctrl_timeout_ms_;

  int strobe_timeout_samples_;
  int strobe_decay_samples_;

  std::vector<double> threshold_;
  std::vector<double> decay_constant_;

  std::vector<double> prev_sample_;
  std::vector<double> curr_sample_;
  std::vector<double> next_sample_;

  std::vector<double> strobe_count_;
  std::vector<int> last_strobe_;

  // The actual number of signal channels in the input.  The output
  // from PZFC (and HCL) is a realvec with the first half of
  // observations being the signal, and the second half being the
  // channels.
  mrs_natural channel_count_;

public:
  AimLocalMax(std::string name);
  AimLocalMax(const AimLocalMax& a);

  ~AimLocalMax();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
