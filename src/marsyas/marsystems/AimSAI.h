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

#ifndef MARSYAS_AIMSAI_H
#define MARSYAS_AIMSAI_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/StrobeList.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimSAI
	\ingroup Analysis
    \brief Stabilised auditory image

    Author : Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimSAI: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  // Reset all internal state variables to their initial values
  void ResetInternal();

  // Prepare the module
  void InitializeInternal();

  // Does the MarSystem need initialization?
  bool is_initialized;

  // What changes cause it to need initialization?
  mrs_real initialized_israte;
  mrs_natural initialized_inobservations;
  mrs_natural initialized_insamples;
  mrs_real initialized_frame_period_ms;
  mrs_real initialized_min_delay_ms;
  mrs_real initialized_max_delay_ms;
  mrs_real initialized_buffer_memory_decay;
  mrs_natural initialized_max_concurrent_strobes;
  mrs_real initialized_strobe_weight_alpha;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_real reseted_israte;
  mrs_natural reseted_inobservations;
  mrs_real reseted_frame_period_ms;

  // Does the MarSystem need to recalculate the centre frequencies
  bool is_centre_frequencies_calculated;

  // What causes it to need to recalculate the centre frequencies
  mrs_natural centre_frequencies_inobservations;
  mrs_natural centre_frequencies_max_frequency;
  mrs_natural centre_frequencies_min_frequency;

  //
  // MarControls
  //
  MarControlPtr ctrl_min_delay_ms_;
  MarControlPtr ctrl_max_delay_ms_;
  MarControlPtr ctrl_strobe_weight_alpha_;

  // Buffer decay parameter
  MarControlPtr ctrl_buffer_memory_decay_;

  // Period in milliseconds between output frames
  MarControlPtr ctrl_frame_period_ms_;

  // The maximum number strobes that can be active at the same time.
  MarControlPtr ctrl_max_concurrent_strobes_;

  // List of strobes for each channel
  std::vector<StrobeList> active_strobes_;

  // Sample index of minimum strobe delay
  int min_strobe_delay_idx_;

  // Sample index of maximum strobe delay
  int max_strobe_delay_idx_;

  // Factor with which the SAI should be decayed
  double sai_decay_factor_;

  // Precomputed 1/n^alpha values for strobe weighting
  std::vector<double> strobe_weights_;

  // Next Strobe for each channels
  std::vector<int> next_strobes_;

  // Temporary buffer for constructing the current SAI frame
  realvec sai_temp_;

  int fire_counter_;
  int frame_period_samples_;
  // int channel_count_;

  // The strobes for each observation
  std::vector<std::vector<int> > strobes_;
  void findStrobes(realvec& in);

  // // sness - From AimGammatone.  We need these in order to calculate
  // // the centre frequencies.
  // MarControlPtr ctrl_max_frequency_;
  // MarControlPtr ctrl_min_frequency_;
  std::vector<double> centre_frequencies_;
  // void CalculateCentreFrequencies();

  // The actual number of signal channels in the input.  The output
  // from PZFC + HCL + Localmax is a realvec with the first third of
  // observations being the signal, and the second third being the
  // channels and the last third being the strobes.
  mrs_natural channel_count_;


public:
  AimSAI(std::string name);

  ~AimSAI();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
