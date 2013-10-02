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

#include "Limiter.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Limiter::Limiter(mrs_string name):MarSystem("Limiter",name)
{
  //type_ = "Limiter";
  //name_ = name;

  xdprev_ = 0.0;
  alpha_ = 0.0;

  addControls();
}


Limiter::~Limiter()
{
}


MarSystem*
Limiter::clone() const
{
  return new Limiter(*this);
}

void
Limiter::addControls()
{
  addctrl("mrs_real/thresh", 1.0);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  addctrl("mrs_real/slope", 1.0);
}


void
Limiter::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Limiter.cpp - Limiter:myUpdate");

  // setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  // setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  // setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  // setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
  MarSystem::myUpdate(sender); //default myUpdate as defined at parent MarSystem class...

  //defaultUpdate(); [!]
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  xd_.create(inSamples_);
  gains_.create(inSamples_);
}


void
Limiter::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_natural o,t;
  mrs_real thresh = getctrl("mrs_real/thresh")->to<mrs_real>();
  mrs_real at = getctrl("mrs_real/at")->to<mrs_real>();
  mrs_real rt = getctrl("mrs_real/rt")->to<mrs_real>();
  mrs_real slope  = getctrl("mrs_real/slope")->to<mrs_real>();

  // calculate at and rt time
  at = 1 - exp(-2.2/(22050*at));
  rt = 1 - exp(-2.2/(22050*rt));

  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      // Calculates the current amplitude of signal and incorporates
      // the at and rt times into xd(o,t)
      alpha_ = fabs(in(o,t)) - xdprev_;

      if (alpha_<0)
      {
        alpha_ = 0;
      }

      xd_(o,t)=xdprev_*(1-rt)+at*alpha_;
      xdprev_ = xd_(o,t);

      // Limiter
      if (xd_(o,t) > thresh)
      {
        gains_(o,t) = pow((mrs_real)10.0,-slope*(log10(xd_(o,t))-log10(thresh)));
        //  linear calculation of gains_ = 10^(-Limiter Slope * (current value - Limiter Threshold))
      }
      else
      {
        gains_(o,t) = 1;
      }

      out(o,t) =  gains_(o,t) * in(o,t);
    }
  }
}
