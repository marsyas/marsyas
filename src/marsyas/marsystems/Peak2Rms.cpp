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

#include "Peak2Rms.h"

using std::ostringstream;
using namespace Marsyas;

Peak2Rms::Peak2Rms(mrs_string name):MarSystem("Peak2Rms", name)
{
}

Peak2Rms::Peak2Rms(const Peak2Rms& a) : MarSystem(a)
{
}

Peak2Rms::~Peak2Rms()
{
}

MarSystem*
Peak2Rms::clone() const
{
  return new Peak2Rms(*this);
}

void
Peak2Rms::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onObservations",1);
  setctrl("mrs_natural/onSamples",1);
  setctrl("mrs_real/osRate", israte_ / inSamples_);
}

void
Peak2Rms::myProcess(realvec& in, realvec& out)
{
  mrs_real peak = in.maxval();
  mrs_real rms = 0.0;
  mrs_natural t;

  for (t = 0; t < inSamples_; t++)
  {
    rms += in(0,t) * in(0,t);
  }
  rms /= inSamples_;
  rms = sqrt(rms);
  out(0) = peak/rms;
}
