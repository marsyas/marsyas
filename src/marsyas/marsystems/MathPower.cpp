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

#include "MathPower.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

MathPower::MathPower(mrs_string name) : MarSystem("MathPower", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

MathPower::MathPower(const MathPower& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_exponent_ = getctrl("mrs_real/exponent");
}


MathPower::~MathPower()
{
}

MarSystem*
MathPower::clone() const
{
  return new MathPower(*this);
}

void
MathPower::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  addctrl("mrs_real/exponent", 1.0, ctrl_exponent_);

}

void
MathPower::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MathPower.cpp - MathPower:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames,
                             "MathPower_"), NOUPDATE);

  exponent_ = ctrl_exponent_->to<mrs_real>();
}

void
MathPower::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = pow(in(o, t), exponent_);
    }
  }
}
