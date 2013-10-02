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
#include "MeanAbsoluteDeviation.h"

using std::ostringstream;
using namespace Marsyas;

MeanAbsoluteDeviation::MeanAbsoluteDeviation(mrs_string name) : MarSystem("MeanAbsoluteDeviation", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

MeanAbsoluteDeviation::MeanAbsoluteDeviation(const MeanAbsoluteDeviation& a) : MarSystem(a)
{
}


MeanAbsoluteDeviation::~MeanAbsoluteDeviation()
{
}

MarSystem*
MeanAbsoluteDeviation::clone() const
{
  return new MeanAbsoluteDeviation(*this);
}

void
MeanAbsoluteDeviation::addControls()
{
}

void
MeanAbsoluteDeviation::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MeanAbsoluteDeviation.cpp - MeanAbsoluteDeviation:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames,
                             "MeanAbsoluteDeviation_"), NOUPDATE);
}

void
MeanAbsoluteDeviation::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    mrs_real sum = 0.0;
    for (t = 0; t < inSamples_; t++)
    {
      sum += fabs( in(o,t) );
    }
    out(o, 0) = sum / inSamples_;
  }
}
