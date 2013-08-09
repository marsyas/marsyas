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

#include "Rms.h"

using namespace std;
using namespace Marsyas;

Rms::Rms(mrs_string name):MarSystem("Rms",name)
{
  //type_ = "Rms";
  //name_ = name;
}

Rms::~Rms()
{
}

MarSystem*
Rms::clone() const
{
  return new Rms(*this);
}

void
Rms::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() / getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Rms_"), NOUPDATE);
}

void
Rms::myProcess(realvec& in, realvec& out)
{
  mrs_natural t;
  mrs_natural o;
  mrs_real val;
  mrs_real rmsEnergy;

  for (o = 0; o < inObservations_; o++) {
    rmsEnergy = 0.0;
    for (t = 0; t < inSamples_; t++) {
      val = in(o, t);
      rmsEnergy += (val * val);
    }
    if (rmsEnergy != 0.0)
    {
      rmsEnergy /= inSamples_;
      rmsEnergy = sqrt(rmsEnergy);
    }
    out(o, 0) = (mrs_real) rmsEnergy;
  }
}
