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

#include "ShiftOutput.h"
#include <algorithm>

using namespace std;
using namespace Marsyas;

ShiftOutput::ShiftOutput(mrs_string name):MarSystem("ShiftOutput",name)
{
  addControls();
}

ShiftOutput::ShiftOutput(const ShiftOutput& a):MarSystem(a)
{
  ctrl_Interpolation_ = getctrl("mrs_natural/Interpolation");
}

ShiftOutput::~ShiftOutput()
{
}

MarSystem*
ShiftOutput::clone() const
{
  return new ShiftOutput(*this);
}

void
ShiftOutput::addControls()
{
  addctrl("mrs_natural/Interpolation", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES / 2, ctrl_Interpolation_);
  setctrlState("mrs_natural/Interpolation", true);
}

void
ShiftOutput::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  interp_ = ctrl_Interpolation_->to<mrs_natural>();

  ctrl_onSamples_->setValue(ctrl_Interpolation_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}

void
ShiftOutput::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  if(interp_ > inSamples_)
    out.setval(0.0);

  for(o=0; o< inObservations_; ++o)
    for (t = 0; t < min(inSamples_,interp_); t++)
      out(o,t) = in(o,t);
}










