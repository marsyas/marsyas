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


#include "Centroid.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Centroid::Centroid(mrs_string name):MarSystem("Centroid", name)
{
  m0_ = 0.0;
  m1_ = 0.0;
}

Centroid::~Centroid()
{
}

MarSystem*
Centroid::clone() const
{
  return new Centroid(*this);
}

void
Centroid::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  MRSDIAG("Centroid.cpp - Centroid:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("Centroid_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}

void
Centroid::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // computer centroid of observations for each time sample
  // using zero and first-order moments
  for (t = 0; t < inSamples_; t++)
  {
    m0_ = 0.0;
    m1_ = 0.0;
    for (o=0; o < inObservations_; o++)
    {
      m1_ += o * in(o,t);
      m0_ += in(o,t);
    }
    if (m0_ != 0.0)
      out(0,t) = (m1_ / m0_) / inObservations_;
    else
      out(0,t) = 0.5;
  }

}
