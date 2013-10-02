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

#include "ZeroCrossings.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

ZeroCrossings::ZeroCrossings(mrs_string name):MarSystem("ZeroCrossings",name)
{
}


ZeroCrossings::~ZeroCrossings()
{
}


MarSystem*
ZeroCrossings::clone() const
{
  return new ZeroCrossings(*this);
}

void
ZeroCrossings::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("ZeroCrossings.cpp - ZeroCrossings:myUpdate");
  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>());
  ctrl_onObsNames_->setValue("ZeroCrossings_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}




void
ZeroCrossings::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  for (o=0; o < inObservations_; o++)
  {
    zcrs_ = 1.0;
    for (t = 1; t < inSamples_; t++)
    {
      if (((in(o, t-1) > 0) && (in(o,t) < 0)) ||
          ((in(o, t-1) < 0) && (in(o,t) > 0)))
      {
        zcrs_++;
      }
    }
    out(o,0) = zcrs_ / inSamples_;
  }
}

