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
#include "Histogram.h"

using std::ostringstream;
using namespace Marsyas;

Histogram::Histogram(mrs_string name) : MarSystem("Histogram", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

Histogram::Histogram(const Histogram& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


Histogram::~Histogram()
{
}

MarSystem*
Histogram::clone() const
{
  return new Histogram(*this);
}

void
Histogram::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  addctrl("mrs_bool/dummyEXAMPLE", false);
  setctrlState("mrs_bool/dummyEXAMPLE", true);

  addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);

}

void
Histogram::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Histogram.cpp - Histogram:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
Histogram::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  const mrs_real& gainValueEXAMPLE = ctrl_gain_EXAMPLE_->to<mrs_real>();

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = gainValueEXAMPLE * in(o, t);
    }
  }
}
