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

#ifndef MARSYAS_AIMPZFC2_H
#define MARSYAS_AIMPZFC2_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimPZFC2
	\ingroup Analysis
    \brief Time-domain AimPZFC2

    Dick Lyon's Pole-Zero Filter Cascade - implemented in C++ by Tom
    Walters from the AIM-MAT module based on Dick Lyon's code.

    Ported to Marsyas from AIM-C by Steven Ness (sness@sness.net).

	AimPZFC2 is a re-write of the AimPZFC to be more Marsyas like.

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimPZFC2: public MarSystem
{
private:
  mrs_real zcrs_;

  void myUpdate(MarControlPtr sender);
  void addControls();

  // Reset all internal state variables to their initial values
  void ResetInternal();

  // Prepare the module
  bool InitializeInternal();

  // Does the MarSystem need initialization?
  bool is_initialized;

  // What changes cause it to need initialization?
  mrs_real initialized_israte;
  mrs_real initialized_inobservations;
  mrs_real initialized_mindamp;
  mrs_real initialized_maxdamp;
  mrs_real initialized_cf_max;
  mrs_real initialized_cf_min;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_natural reseted_inobservations;
  mrs_natural reseted_agc_factor;

  // Set the filterbank parameters according to a fit matrix from Unoki
  // bool SetPZBankCoeffsERB();
  bool SetPZBankCoeffsERBFitted();
  bool SetPZBankCoeffsOrig();

  // Sets the general filterbank coefficients
  bool SetPZBankCoeffs();

  // Automatic Gain Control
  void AGCDampStep();


  // Detector function - halfwave rectification etc. Used internally,
  // but not applied to the output.
  double DetectFun(double fIN);

  // Minimum
  inline double Minimum(double a, double b);

  int channel_count_;
  // int buffer_length_;
  int agc_stage_count_;
  // double sample_rate_;
  double last_input_;

  double agc_factor_;
  double offset_;


  // Parameters
  // User-settable scalars
  MarControlPtr ctrl_pole_damping_;
  MarControlPtr ctrl_zero_damping_;
  MarControlPtr ctrl_zero_factor_;
  MarControlPtr ctrl_step_factor_;
  MarControlPtr ctrl_bandwidth_over_cf_;
  MarControlPtr ctrl_min_bandwidth_hz_;
  MarControlPtr ctrl_agc_factor_;
  MarControlPtr ctrl_cf_max_;
  MarControlPtr ctrl_cf_min_;
  MarControlPtr ctrl_mindamp_;
  MarControlPtr ctrl_maxdamp_;
  MarControlPtr ctrl_do_agc_step_;
  MarControlPtr ctrl_use_fit_;

  // Internal Buffers
  // Initialised once
  //

  mrs_realvec pole_dampings_;
  mrs_realvec agc_epsilons_;
  mrs_realvec agc_gains_;
  mrs_realvec pole_frequencies_;



  std::vector<double> za0_;
  std::vector<double> za1_;
  std::vector<double> za2_;
  std::vector<double> rmin_;
  std::vector<double> rmax_;
  std::vector<double> xmin_;
  std::vector<double> xmax_;

  // Modified by algorithm at each time step
  std::vector<double> detect_;
  std::vector<std::vector<double> > agc_state_;
  std::vector<double> state_1_;
  std::vector<double> state_2_;
  // std::vector<double> previous_out_;
  mrs_realvec previous_out_;

  std::vector<double> pole_damps_mod_;
  mrs_realvec inputs_;

  // Hold the centre frequencies, which we will output to the second half
  // of the observations
  std::vector<double> centre_frequencies_;

public:
  AimPZFC2(std::string name);
  AimPZFC2(const AimPZFC2& a);

  ~AimPZFC2();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
