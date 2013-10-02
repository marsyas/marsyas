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

#include "Rolloff.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Rolloff::Rolloff(mrs_string name):MarSystem("Rolloff",name)
{
  perc_ = 0.0;
  sum_ = 0.0;
  total_ = 0.0;

  addControls();
}

Rolloff::Rolloff(const Rolloff& a):MarSystem(a)
{
  ctrl_percentage_ = getctrl("mrs_real/percentage");
}

Rolloff::~Rolloff()
{
}

MarSystem*
Rolloff::clone() const
{
  return new Rolloff(*this);
}

void
Rolloff::addControls()
{
  addctrl("mrs_real/percentage", 0.9, ctrl_percentage_);
  setctrlState("mrs_real/percentage", true);
}

void
Rolloff::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Rolloff.cpp - Rolloff:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("Rolloff_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  sumWindow_.create(ctrl_inObservations_->to<mrs_natural>());

  perc_ = ctrl_percentage_->to<mrs_real>();
}

void
Rolloff::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // computer rolloff of observations for each time sample
  for (t = 0; t < inSamples_; t++)
  {
    sum_ = 0.0;
    sumWindow_.setval(0.0);
    for (o=0; o < inObservations_; o++)
    {
      sum_ += in(o,t);
      sumWindow_(o) = sum_;
    }
    total_ = sumWindow_(inObservations_-1);
    for (o=inObservations_-1; o>1; o--)
    {
      if (sumWindow_(o) < perc_ *total_)
      {
        out(0,t) = (mrs_real)o / inObservations_;
        return;
      }
    }
    out(0,t) = 1.0;			// default
  }
}
