/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "StandardDeviation.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

StandardDeviation::StandardDeviation(mrs_string name): MarSystem("StandardDeviation",name)
{
}

StandardDeviation::~StandardDeviation()
{
}

MarSystem*
StandardDeviation::clone() const
{
  return new StandardDeviation(*this);
}

void
StandardDeviation::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("StandardDeviation.cpp - StandardDeviation:myUpdate");

  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  obsrow_.create(ctrl_inSamples_->to<mrs_natural>());

  inObservations_ = ctrl_inObservations_->to<mrs_natural>();

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Std_"), NOUPDATE);

}

void
StandardDeviation::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //checkFlow(in,out);
  out.setval(0.0);
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      // Calculate mean
      obsrow_(t) = in(o,t);
    }
    out(o,0) = obsrow_.std();
  }

  // VERY INEFFICIENT - LOTS OF MEMORY ALLOCATION AND COPYING
  // out = in.stdObs();
}
