/*
 ** Copyright (C) 2010 Stefaan Lippens
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
#include "DelaySamples.h"

using std::ostringstream;
using namespace Marsyas;

DelaySamples::DelaySamples(mrs_string name) :
  MarSystem("DelaySamples", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

DelaySamples::DelaySamples(const DelaySamples& a) :
  MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_delay_ = getctrl("mrs_natural/delay");
}

DelaySamples::~DelaySamples()
{
}

MarSystem*
DelaySamples::clone() const
{
  return new DelaySamples(*this);
}

/// Add any specific controls needed by this MarSystem.
void DelaySamples::addControls()
{
  addctrl("mrs_natural/delay", 0, ctrl_delay_);
  setctrlState("mrs_natural/delay", true);
}

void DelaySamples::myUpdate(MarControlPtr sender)
{
  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Handle/cache delay value for in myProcess
  delay_ = ctrl_delay_->to<mrs_natural> ();
  if (delay_ < 0)
  {
    setctrl("mrs_natural/delay", 0);
    delay_ = 0;
  }

  // Prefix observation names with "DelayX".
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ostringstream oss;
  oss << "DelaySamples" << delay_ << "_";
  mrs_string onObsNames = obsNamesAddPrefix(inObsNames, oss.str());
  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

  // Allocate and initialize the buffers.
  this->memory_.stretch(inObservations_, delay_);
  this->memory_.setval(0.0);
}

void DelaySamples::myProcess(realvec& in, realvec& out)
{
  mrs_natural t, o;
  // The number of samples we'll fetch from and store to the memory buffer.
  mrs_natural memory_part = delay_ < inSamples_ ? delay_ : inSamples_;
  for (o = 0; o < inObservations_; o++)
  {
    // Fetch initial part from memory.
    for (t = 0; t < memory_part; t++)
    {
      out(o, t) = memory_(o, t);
    }
    // Copy from input if needed/possible (delay_ < inSamples_).
    for (t = delay_; t < inSamples_; t++)
    {
      out(o, t) = in(o, t - delay_);
    }
    // Shift memory if needed. (inSamples < delay_).
    for (t = 0; t < delay_ - inSamples_; t++)
    {
      memory_(o, t) = memory_(o, t + inSamples_);
    }
    // Put appropriate part of input in memory.
    for (t = 0; t < memory_part; t++)
    {
      memory_(o, delay_ - 1 - t) = in(o, inSamples_ - 1 - t);
    }
  }
}
