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

#include "Transposer.h"

using namespace std;
using namespace Marsyas;

Transposer::Transposer(mrs_string name):MarSystem("Transposer", name)
{
}

Transposer::Transposer(const Transposer& a) : MarSystem(a)
{
}

Transposer::~Transposer()
{
}

MarSystem*
Transposer::clone() const
{
  return new Transposer(*this);
}

void
Transposer::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

  if (getctrl("mrs_natural/inSamples")->to<mrs_natural>() != getctrl("mrs_natural/onObservations")->to<mrs_natural>())
    updControl("mrs_natural/onObservations", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  if (getctrl("mrs_natural/onSamples")->to<mrs_natural>() != getctrl("mrs_natural/inObservations")->to<mrs_natural>())
    updControl("mrs_natural/onSamples", getctrl("mrs_natural/inObservations")->to<mrs_natural>());
}


void
Transposer::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // copy all values but switch the dimensions
  // if input has [m,n] then output has [n,m]
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      // transpose observations in channels and vice versa
      out(t,o) = in(o,t);
    }
  }
}
