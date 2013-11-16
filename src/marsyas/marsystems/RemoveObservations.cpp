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
#include "RemoveObservations.h"

using std::ostringstream;
using namespace Marsyas;

RemoveObservations::RemoveObservations(mrs_string name) : MarSystem("RemoveObservations", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
  lowestObs_ = 0;
  numObs_ = inObservations_;
}

RemoveObservations::RemoveObservations(const RemoveObservations& a) : MarSystem(a)
{
  lowestObs_ = 0;
  numObs_ = inObservations_;
}


RemoveObservations::~RemoveObservations()
{
}

MarSystem*
RemoveObservations::clone() const
{
  return new RemoveObservations(*this);
}

void
RemoveObservations::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  addctrl("mrs_real/lowCutoff", 0.0);
  addctrl("mrs_real/highCutoff", 1.0);

}

void
RemoveObservations::myUpdate(MarControlPtr sender)
{
  MRSDIAG("RemoveObservations.cpp - RemoveObservations:myUpdate");

  MarSystem::myUpdate(sender);

  // round down is the default with C math
  lowestObs_ = (mrs_natural) (inObservations_
                              * getctrl("mrs_real/lowCutoff")->to<mrs_real>());
  // round up with ceil()
  numObs_ = (mrs_natural) (ceil( inObservations_
                                 * getctrl("mrs_real/highCutoff")->to<mrs_real>()
                               ) - lowestObs_);

  ctrl_onObservations_->setValue(numObs_, NOUPDATE);

  // remove unused observation names, so they don't
  // overwrite other observations from other parts
  // of the network
  mrs_string names = ctrl_inObsNames_->to<mrs_string>();
  std::string::size_type lowNamePos = 0;
  std::string::size_type highNamePos = 0;
  for (int i=0; i < lowestObs_; i++) {
    lowNamePos = names.find(",", lowNamePos) + 1;
  }
  for (int i=0; i < numObs_; i++) {
    highNamePos = names.find(",", highNamePos) + 1;
  }
  if (highNamePos > 0) {
    names = names.substr(lowNamePos, highNamePos);
    ctrl_onObsNames_->setValue(names, NOUPDATE);
  }
}

void
RemoveObservations::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < numObs_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = in(o+lowestObs_, t);
    }
  }
}
