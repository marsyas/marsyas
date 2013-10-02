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

#ifndef MARSYAS_PLUCKED_H
#define MARSYAS_PLUCKED_H


#include <marsyas/system/MarSystem.h>
#include "Gain.h"

#include <cstdlib>

namespace Marsyas
{
/**
	\ingroup Synthesis
	\brief The Karplus-Strong model.

	Implementation of the Karplus_Strong 1D Digital Waveguide Model.
	http://ccrma.stanford.edu/~jos/SimpleStrings/Karplus_Strong_Algorithm.html
	With extensions proposed by Jaffe and Smith: Blend Factor, Decay, and Stretch Factor.
*/


class Plucked: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec delayline1_;
  realvec pickDelayLine_;
  realvec noise_;

  mrs_real delaylineSize_;
  mrs_real a_;
  mrs_real b_;
  mrs_real blend_;
  mrs_real loss_;
  mrs_real s_;
  mrs_real d_; //the delay of the all pass filter dependant on the freq
  mrs_real g_; //coefficient of all pass filter g_=-(1-d)/d+1)
  mrs_real c;  //output of inverse comb filt (for pick pos)

  mrs_real nton_;
  mrs_bool noteon_;

  mrs_natural wp_;
  mrs_natural wpp_;

  mrs_natural rp_;

  // Why do these 'pointers' exist
  mrs_natural pointer1_;
  mrs_natural pointer2_;
  mrs_natural pointer3_;

  mrs_natural picklength_;
  mrs_natural N_;
  mrs_natural p;
  MarSystem* gain_;
  realvec gout_;

public:
  Plucked(std::string name);
  ~Plucked();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
