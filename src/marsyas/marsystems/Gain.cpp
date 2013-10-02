/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Gain.h"
#include "../common_source.h"

using namespace Marsyas;

Gain::Gain(mrs_string name):MarSystem("Gain", name)
{
  //Add any specific controls needed by Gain
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

Gain::Gain(const Gain& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_gain_ = getctrl("mrs_real/gain");
}

Gain::~Gain()
{
}

MarSystem*
Gain::clone() const
{
  return new Gain(*this);
}

void
Gain::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/gain", 1.0, ctrl_gain_);
}


void
Gain::myUpdate(MarControlPtr sender)
{
  // no change to network flow
  MarSystem::myUpdate(sender);
}

void
Gain::myProcess(realvec& in, realvec& out)
{
  mrs_real gainValue = ctrl_gain_->to<mrs_real>();
  MRSDIAG(type_ << "/" << name_ << "/mrs_real/gain = " << gainValue);

  // It is important to loop over both observations
  // and channels so that for example a gain can be
  // applied to multi-channel signals
  for (mrs_natural o=0; o < inObservations_; o++)
  {
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      //apply gain to all channels
      out(o,t) = gainValue * in(o,t);
    }
  }
}
