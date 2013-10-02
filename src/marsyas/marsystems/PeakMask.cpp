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

#include "PeakMask.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

PeakMask::PeakMask(mrs_string name):MarSystem("PeakMask", name)
{
  addControls();
}

PeakMask::PeakMask(const PeakMask& a) : MarSystem(a)
{
  ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


PeakMask::~PeakMask()
{
}

MarSystem*
PeakMask::clone() const
{
  return new PeakMask(*this);
}

void
PeakMask::addControls()
{

  addctrl("mrs_bool/dummyEXAMPLE", false);
  setctrlState("mrs_bool/dummyEXAMPLE", true);

  addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);

}

void
PeakMask::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PeakMask.cpp - PeakMask:myUpdate");

  MarSystem::myUpdate(sender);
}

void
PeakMask::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  const mrs_real& gainValueEXAMPLE = ctrl_gain_EXAMPLE_->to<mrs_real>();

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      out(o,t) = gainValueEXAMPLE * in(o,t);
}
