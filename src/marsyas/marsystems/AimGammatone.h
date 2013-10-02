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

#ifndef MARSYAS_AIMGAMMATONE_H
#define MARSYAS_AIMGAMMATONE_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
   \class AimGammatone
   \ingroup Analysis
   \brief Slaney's gammatone filterbank

   Author : Thomas Walters <tom@acousticscale.org>

   Ported to Marsyas by Steven Ness <sness@sness.net>

   This is the version of the IIR gammatone used in Slaney's Auditory toolbox.
   The original verison as described in Apple Tech. Report #35 has a problem
   with the high-order coefficients at low centre frequencies and high sample
   rates. Since it is important that AIM-C can deal with these cases (for
   example for the Gaussian features), I've reiplemeted Slaney's alternative
   version which uses a cascade of four second-order filters in place of the
   eighth-order filter.

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class marsyas_EXPORT AimGammatone: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  // Parameters
  MarControlPtr ctrl_num_channels_;
  MarControlPtr ctrl_max_frequency_;
  MarControlPtr ctrl_min_frequency_;

  // Filter coefficients
  std::vector<std::vector<double> > b1_;
  std::vector<std::vector<double> > b2_;
  std::vector<std::vector<double> > b3_;
  std::vector<std::vector<double> > b4_;
  std::vector<std::vector<double> > a_;

  std::vector<std::vector<double> > state_1_;
  std::vector<std::vector<double> > state_2_;
  std::vector<std::vector<double> > state_3_;
  std::vector<std::vector<double> > state_4_;

  std::vector<double> centre_frequencies_;

  // Reset all internal state variables to their initial values
  void ResetInternal();

  // Prepare the module
  bool InitializeInternal();

  // Does the MarSystem need initialization?
  bool is_initialized;

  // What changes cause it to need initialization?
  mrs_natural initialized_num_channels;
  mrs_real initialized_min_frequency;
  mrs_real initialized_max_frequency;
  mrs_real initialized_israte;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_natural reset_num_channels;

public:
  AimGammatone(std::string name);
  AimGammatone(const AimGammatone& a);

  ~AimGammatone();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
