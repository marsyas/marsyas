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

#include "Skewness.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Skewness::Skewness(mrs_string name):MarSystem("Skewness",name)
{
  //type_ = "Skewness";
  //name_ = name;
}


Skewness::~Skewness()
{
}


MarSystem*
Skewness::clone() const
{
  return new Skewness(*this);
}


void
Skewness::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Skewness.cpp - Skewness:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_string/onObsNames", "Skewness,");

  //defaultUpdate();[!]
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  obsrow_.create(inObservations_);
}


void
Skewness::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  for (t = 0; t < inSamples_; t++)
  {

    for (o=0; o < inObservations_; o++)
    {
      obsrow_(o) = in(o,t);
    }
    z_ = 0.0;
    for (o=0; o < inObservations_; o++)
    {
      b_ =  obsrow_(o) - obsrow_.mean() ;

      // take x - mean to the third power into the sum
      z_ += (b_ * b_ * b_);
    }
    if (z_ > 1.0e-45)
      z_ /= inObservations_;
    else
      z_ = 0.0;


    // standard deviation to the fourth power
    q_ = pow(obsrow_.var(), (mrs_real)1.5);

    if ((q_ < 1.0e-45)||(z_== 1.0e-45))
      out(0,t) = 0.5;
    else
      out(0,t) = (z_ / q_)  / inObservations_;
  }

}
