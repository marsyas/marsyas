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

#ifndef MARSYAS_PeakDistanceHorizontality_H
#define MARSYAS_PeakDistanceHorizontality_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Analysis
\brief compute a weight depending on how "horizontal" the distance between two peak entries is

Controls:
- \b mrs_realvec/inpIsHorizontal [w] : defines an input to be a horizontal or vertical distance measure
- \b mrs_realvec/weights [rw] : weight of combinations default [1 1 1 ... 1] with the length of the number of systems
- \b mrs_natural/numInputs [w] : number of parallels.
- \b mrs_bool/bypass [w] : set all weights to 1 if true
- \b mrs_real/rangeX [w] : upper - lower bound for abscissa
- \b mrs_real/rangeY [w] : upper - lower bound for ordinate
*/

class marsyas_EXPORT PeakDistanceHorizontality: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  mrs_real ComputeHorizontality(mrs_real diffX, mrs_real diffY);
  //mrs_real ComputeHorizontality(mrs_real freq1, mrs_real freq2);

  inline mrs_real sigmoid (mrs_real val)
  {
    return (1. / (1. + exp(sigSteepness_ * (val - sigCutOff_))));
  }

  inline mrs_real gaussian (mrs_real x)
  {
    return exp (-(x*x)/(2*(sigCutOff_*sigCutOff_)));// / sqrt (TWOPI*std);
  }

  /// MarControlPtr for the gain control
  MarControlPtr	ctrl_horizvert_,
                ctrl_rangeX_,
                ctrl_rangeY_;

  mrs_realvec		weights_;
  mrs_real		sigSteepness_,
              sigCutOff_;

public:
  /// PeakDistanceHorizontality constructor.
  PeakDistanceHorizontality(std::string name);

  /// PeakDistanceHorizontality copy constructor.
  PeakDistanceHorizontality(const PeakDistanceHorizontality& a);

  /// PeakDistanceHorizontality destructor.
  ~PeakDistanceHorizontality();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_PeakDistanceHorizontality_H

