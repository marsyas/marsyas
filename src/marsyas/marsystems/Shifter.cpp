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

#include "Shifter.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Shifter::Shifter(mrs_string name):MarSystem("Shifter", name)
{
  addControls();
}


Shifter::~Shifter()
{
}

MarSystem*
Shifter::clone() const
{
  return new Shifter(*this);
}

void
Shifter::addControls()
{
  addctrl("mrs_natural/shift", 0);
  setctrlState("mrs_natural/shift", true);
}

void
Shifter::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Shifter.cpp - Shifter:myUpdate");

  shift_ = getctrl("mrs_natural/shift")->to<mrs_natural>();

  mrs_natural onsamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>()- shift_;
  //for too big shifts (> inSamples), do a zero shift
  if(onsamples < 0)
    onsamples = inSamples_;

  setctrl("mrs_natural/onSamples", onsamples);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>()*2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}

void
Shifter::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  int delta = 0;
  for (o=0; o < onObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      out(o,t) =  in(0,t+delta);
    }
    delta +=shift_;
  }

  //MATLAB_PUT(out, "in");
  //MATLAB_EVAL("figure(1);plot(mag)");
  //MATLAB_EAVL("figure(2);plot(peaks(1:2:end))");
}
