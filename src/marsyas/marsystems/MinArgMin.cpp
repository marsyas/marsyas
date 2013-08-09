/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "MinArgMin.h"

using std::ostringstream;
using namespace Marsyas;

MinArgMin::MinArgMin(mrs_string name):MarSystem("MinArgMin",name)
{
  //type_ = "MinArgMin";
  //name_ = name;

  addControls();
}


MinArgMin::~MinArgMin()
{
}


MarSystem*
MinArgMin::clone() const
{
  return new MinArgMin(*this);
}


void
MinArgMin::addControls()
{
  addctrl("mrs_natural/nMinimums", (mrs_natural)1);
}

void
MinArgMin::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_natural k = getctrl("mrs_natural/nMinimums")->to<mrs_natural>();

  setctrl("mrs_natural/onSamples",  2 * k);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}



void
MinArgMin::myProcess(realvec& in, realvec& out)
{
  out.setval(MAXREAL);
  mrs_natural k = getctrl("mrs_natural/nMinimums")->to<mrs_natural>();
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();


  for (mrs_natural t=0; t < inSamples; t++)
  {
    // for all the current minimums
    for (ki=0; ki < k; ++ki)
    {
      if (in(0,t) < out(0,2*ki))
      {
        out(0,2*ki) = in(t);
        out(0,2*ki+1) = (mrs_real)t;
        break;
      }
    }
  }
}











