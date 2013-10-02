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

#ifndef MARSYAS_AIMBOXCUTTER_H
#define MARSYAS_AIMBOXCUTTER_H

#include <marsyas/system/MarSystem.h>

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
    \class AimBoxes
	\ingroup Analysis
    \brief 'Box-cutting' routine to generate dense features

    Author : Thomas Walters <tom@acousticscale.org>

    Ported to Marsyas by Steven Ness <sness@sness.net>

	The original source code for these functions in AIM-C can be found at:
	http://code.google.com/p/aimc/

*/


class AimBoxes: public MarSystem
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
  mrs_natural initialized_box_size_spectral;
  mrs_natural initialized_box_size_temporal;

  // Does the MarSystem need reset?
  bool is_reset;

  // What changes cause it to need a reset?
  mrs_natural reset_inobservations;

  // User-settable parameters
  MarControlPtr ctrl_box_size_spectral_;
  MarControlPtr ctrl_box_size_temporal_;

  // float sample_rate_;
  // int buffer_length_;
  // int channel_count_;
  std::vector<int> box_limits_time_;
  std::vector<std::pair<int, int> > box_limits_channels_;
  int box_count_;
  int feature_size_;

public:
  AimBoxes(std::string name);
  AimBoxes(const AimBoxes& a);

  ~AimBoxes();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
