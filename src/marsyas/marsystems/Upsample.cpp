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

#include "Upsample.h"
#include <iostream>

using namespace std;
using namespace Marsyas;

Upsample::Upsample(mrs_string name):MarSystem("Upsample",name)
{
  addControls();
}

Upsample::~Upsample()
{
}

MarSystem*
Upsample::clone() const
{
  return new Upsample(*this);
}

void
Upsample::addControls()
{
  addctrl("mrs_real/default", 0.0);
  addctrl("mrs_natural/factor", 2);
  addctrl("mrs_string/interpolation", "none");
  getControl("mrs_natural/factor")->setState(true);
}



void
Upsample::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_natural factor = getctrl("mrs_natural/factor")->to<mrs_natural>();
  mrs_natural onSamples = (mrs_natural)(mrs_real)(ctrl_inSamples_->to<mrs_natural>() * factor);
  ctrl_onSamples_->setValue(onSamples, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / factor, NOUPDATE);
}

void
Upsample::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_natural factor = getctrl("mrs_natural/factor")->to<mrs_natural>();
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_string interpolation = getctrl("mrs_string/interpolation")->to<mrs_string>();
  //updControl("mrs_natural/osRate", getctrl("mrs_natural/isRate")/factor);
  //if (getctrl("mrs_natural/onSamples")->to<mrs_natural>() !=(inSamples*factor)) {
  //	updControl("mrs_natural/onSamples", inSamples*factor);
  //}

  // Set default values
  if (getctrl("mrs_real/default")->to<mrs_real>() != 0.0) {
    for (mrs_natural t=0; t < inSamples*factor; t++)
    {
      out(0,t)=getctrl("mrs_real/default")->to<mrs_real>();;
    }
  }

  // Get actual values
  for (mrs_natural t=0; t < inSamples; t++)
  {
    // no interpolation
    if (interpolation=="none") {
      out(0,t*factor) = in(0,t);
    }
    if (interpolation=="repeat") {
      for (mrs_natural k=(t*factor); k<((t+1)*factor); k++) {
        out(0,k)=in(0,t);
      }
    }
  }
}

