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

#include "../common_source.h"
#include "NormalizeAbs.h"

using std::ostringstream;
using namespace Marsyas;

NormalizeAbs::NormalizeAbs(mrs_string name) : MarSystem("NormalizeAbs", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

NormalizeAbs::NormalizeAbs(const NormalizeAbs& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_target_ = getctrl("mrs_real/target");
}


NormalizeAbs::~NormalizeAbs()
{
}

MarSystem*
NormalizeAbs::clone() const
{
  return new NormalizeAbs(*this);
}

void
NormalizeAbs::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  addctrl("mrs_real/target", 1.0, ctrl_target_);

}

void
NormalizeAbs::myUpdate(MarControlPtr sender)
{
  MRSDIAG("NormalizeAbs.cpp - NormalizeAbs:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
NormalizeAbs::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  const mrs_real& target = ctrl_target_->to<mrs_real>();

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    // find maximum value
    mrs_real max_val = 0.0;
    for (t = 0; t < inSamples_; t++)
      if (max_val < fabs(in(o,t)))
        max_val = fabs(in(o,t));
    mrs_real gain = 0.0;
    if (max_val > 0)
      gain = target / max_val;
    for (t = 0; t < inSamples_; t++)
      out(o, t) = gain * in(o, t);
  }
}
