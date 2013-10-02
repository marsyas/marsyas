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

#ifndef MARSYAS_AIMSSI_H
#define MARSYAS_AIMSSI_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimSSI
	\ingroup Analysis
    \brief Size-shape image (aka the 'sscAI')

    Author : Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimSSI: public MarSystem
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

  int ExtractPitchIndex(realvec& in) const;

  // The MarControls
  MarControlPtr ctrl_do_pitch_cutoff_;
  MarControlPtr ctrl_weight_by_cutoff_;
  MarControlPtr ctrl_weight_by_scaling_;
  MarControlPtr ctrl_log_cycles_axis_;
  MarControlPtr ctrl_pitch_search_start_ms_;
  MarControlPtr ctrl_ssi_width_cycles_;
  MarControlPtr ctrl_pivot_cf_;

  // sness - From AimGammatone.  We need these in order to calculate
  // the centre frequencies.
  MarControlPtr ctrl_max_frequency_;
  MarControlPtr ctrl_min_frequency_;
  bool is_centre_frequencies_calculated;
  std::vector<double> centre_frequencies_;
  void CalculateCentreFrequencies();

  // double sample_rate_;
  // int buffer_length_;
  // int channel_count_;
  int ssi_width_samples_;

  double Log2( double n );

public:
  AimSSI(std::string name);

  ~AimSSI();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
