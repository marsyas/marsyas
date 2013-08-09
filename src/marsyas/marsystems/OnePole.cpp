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

#include "OnePole.h"

using std::ostringstream;
using namespace Marsyas;


OnePole::OnePole(mrs_string name) : MarSystem("OnePole", name)
{
  addControls();
}


OnePole::~OnePole()
{
}


MarSystem*
OnePole::clone() const
{
  return new OnePole(*this);
}

void
OnePole::addControls()
{
  addControl("mrs_real/alpha", 0.9);
  setControlState("mrs_real/alpha", true);
}


void
OnePole::myUpdate(MarControlPtr sender)
{
  // Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Cache the alpha and gain value.
  alpha_ = getControl("mrs_real/alpha")->to<mrs_real>();
  gain_ = 1.0 - alpha_;

  // Allocate and initialize the buffer for previous output samples.
  mrs_natural rows = ctrl_inObservations_->to<mrs_natural>();
  previousOutputSamples_.stretch(rows, 1);
  previousOutputSamples_.setval(0.0);
}

void
OnePole::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (o = 0; o < inObservations_; o++)
  {
    // Use the last sample from the previous slice for the first sample of
    // this slice.
    t = 0;
    out(o, t) = gain_ * in(o, t) + alpha_ * previousOutputSamples_(o, 0);

    // Do the remaining samples.
    for (t = 1; t < inSamples_; t++)
    {
      out(o, t) = gain_ * in(o, t) + alpha_ * out(o, t - 1);
    }

    // Store the last sample for usage in next process() call.
    previousOutputSamples_(o, 0) = out(o, inSamples_ - 1);
  }
}
