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

#include "Energy.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Energy::Energy(mrs_string name):MarSystem("Energy",name)
{
  addControls();
}

Energy::Energy(const Energy& a) : MarSystem(a)
{
}

Energy::~Energy()
{
}


MarSystem*
Energy::clone() const
{
  return new Energy(*this);
}

void
Energy::addControls()
{

}

void
Energy::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Energy.cpp - Energy:myUpdate");

  setctrl("mrs_natural/onSamples", 1);
  setctrl("mrs_natural/onObservations",
          getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate",
          getctrl("mrs_real/israte"));
  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Power_"), NOUPDATE);
}


void
Energy::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //checkFlow(in,out);

  out.setval(0.0);
  for (o=0; o < inObservations_; o++)
  {
    mrs_real tmp=0;
    for (t = 0; t < inSamples_; t++)
    {
      tmp+= in(o,t)*in(o,t);
    }
    out(o,0) = tmp;
  }
}









