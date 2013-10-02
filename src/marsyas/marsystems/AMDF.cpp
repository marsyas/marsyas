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

#include "AMDF.h"
#include "../common_source.h"


using std::ostringstream;

using namespace Marsyas;

AMDF::AMDF(mrs_string name):MarSystem("AMDF", name)
{
  //type_ = "AMDF";
  //name_ = name;
}


AMDF::~AMDF()
{
}


MarSystem*
AMDF::clone() const
{
  return new AMDF(*this);
}

void
AMDF::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AMDF.cpp - AMDF:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}

void
AMDF::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  //checkFlow(in,out);

  // FIXME This value is defined but (possibly) not used
  // mrs_real gain = getctrl("mrs_real/gain")->to<mrs_real>();
  mrs_natural i,k;
  mrs_real temp;

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) = 0.0;
      for (i=t, k=0; i < inSamples_; ++i, k++)
      {
        temp = in(o,i) - in(o,k);
        if (temp < 0)
          out(o,t) -= temp;
        else
          out(o,t) += temp;
      }
    }

}









