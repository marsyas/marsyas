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

//
// Brought to you by sness@sness.net
//

#include "Selector.h"
#include "../common_source.h"

#include <algorithm>

using namespace std;
using namespace Marsyas;

Selector::Selector(mrs_string name):MarSystem("Selector", name)
{
  addControls();
}

Selector::Selector(const Selector& a) : MarSystem(a)
{
  ctrl_enabled_ = getctrl("mrs_realvec/enabled");
}

Selector::~Selector()
{
}


MarSystem*
Selector::clone() const
{
  return new Selector(*this);
}

void
Selector::addControls()
{
  addctrl("mrs_natural/disable", -1);
  setctrlState("mrs_natural/disable", true);
  addctrl("mrs_natural/enable", -1);
  setctrlState("mrs_natural/enable", true);

  addControl("mrs_realvec/enableRange", realvec());
  setControlState("mrs_realvec/enableRange", true);
  addControl("mrs_realvec/disableRange", realvec());
  setControlState("mrs_realvec/disableRange", true);

  addctrl("mrs_realvec/enabled", realvec(), ctrl_enabled_);
}

void
Selector::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Selector.cpp - Selector:myUpdate");

  MarControlAccessor acc(ctrl_enabled_);
  mrs_realvec& mask = acc.to<mrs_realvec>();

  //
  // Disable any observations that the user asks to be disabled
  //
  mrs_natural input_to_disable = getctrl("mrs_natural/disable")->to<mrs_natural>();
  set_enabled(mask, input_to_disable, false);
  setctrl("mrs_natural/disable", -1);

  const realvec & range_to_disable = getControl("mrs_realvec/disableRange")->to<mrs_realvec>();
  if (range_to_disable.getSize() >= 2)
  {
    mrs_natural min = static_cast<mrs_natural>( range_to_disable(0) );
    mrs_natural max = static_cast<mrs_natural>( range_to_disable(1) );
    set_enabled_range(mask, min, max, false);
  }
  setControl("mrs_realvec/disableRange", realvec());

  //
  // Enable any observations that the user asks to be enabled
  //
  mrs_natural input_to_enable = getctrl("mrs_natural/enable")->to<mrs_natural>();
  set_enabled(mask, input_to_enable, true);
  setctrl("mrs_natural/enable", -1);

  const realvec & range_to_enable = getControl("mrs_realvec/enableRange")->to<mrs_realvec>();
  if (range_to_enable.getSize() >= 2)
  {
    mrs_natural min = static_cast<mrs_natural>( range_to_enable(0) );
    mrs_natural max = static_cast<mrs_natural>( range_to_enable(1) );
    set_enabled_range(mask, min, max, true);
  }
  setControl("mrs_realvec/enableRange", realvec());

  // Count how many of the input observations are enabled
  mrs_natural total_enabled = enabled_count(mask, inObservations_);

  // Set output format
  ctrl_onObservations_->setValue(total_enabled, NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
}

void
Selector::myProcess(realvec& in, realvec& out)
{
  const mrs_realvec & mask = ctrl_enabled_->to<realvec>();

  //
  // Copy all the input observations and samples to the output except
  // for any observations that are not enabled.
  //
  mrs_natural outIndex = 0;
  for (mrs_natural o=0; o < inObservations_; o++)
  {
    if (is_enabled(mask, o))
    {
      for (mrs_natural t = 0; t < inSamples_; t++)
      {
        out(outIndex,t) = in(o,t);
      }
      outIndex++;
    }
  }
}
