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

#include "NoiseGate.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

NoiseGate::NoiseGate(mrs_string name):MarSystem("NoiseGate",name)
{
  //type_ = "NoiseGate";
  //name_ = name;

  state_ = 1.0;
  xdprev_ = 0.0;
  alpha_ = 0.0;
  gainsprev_ = 1.0;

  addControls();
}


NoiseGate::~NoiseGate()
{
}


MarSystem*
NoiseGate::clone() const
{
  return new NoiseGate(*this);
}

void
NoiseGate::addControls()
{
  addctrl("mrs_real/thresh", 0.1);
  addctrl("mrs_real/release", 0.5);
  addctrl("mrs_real/rolloff", .130);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  addctrl("mrs_real/slope", 1.0);
}

void
NoiseGate::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("NoiseGate.cpp - NoiseGate:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  //defaultUpdate(); [!]
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  xd_.create(inSamples_);
  gains_.create(inSamples_);
}


void
NoiseGate::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //checkFlow(in,out);

  mrs_real thresh = getctrl("mrs_real/thresh")->to<mrs_real>();
  mrs_real release = getctrl("mrs_real/release")->to<mrs_real>();
  mrs_real rolloff = getctrl("mrs_real/rolloff")->to<mrs_real>();
  mrs_real at = getctrl("mrs_real/at")->to<mrs_real>();
  mrs_real rt = getctrl("mrs_real/rt")->to<mrs_real>();
  // FIXME This variable is defined but unused.
  // mrs_real slope = getctrl("mrs_real/slope")->to<mrs_real>();

  // calculate rolloff, at and rt time
  at = 1 - exp(-2.2/(22050*at));
  rt = 1 - exp(-2.2/(22050*rt));

  for (o = 0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      // Calculates the current amplitude of signal and incorporates
      // the at and rt times into xd(o,t)
      alpha_ = fabs(in(o,t)) - xdprev_;
      if (alpha_ < 0)
      {
        alpha_ = 0;
      }
      xdprev_=xdprev_*(1-rt)+at*alpha_;

      if (state_ == 1.0)
      {
        if (xdprev_ < thresh)
        {
          gains_(o,t) = gainsprev_*rolloff;
          state_ = 0.0;
        }
        else
        {
          gains_(o,t) = 1;
        }
      }
      else
      {
        if (xdprev_ < release)
        {
          gains_(o,t) = gainsprev_*rolloff;
          // rolloff time
        }
        else if (xdprev_ > release)
        {
          gains_(o,t) = 1.0;
          state_ = 1.0;
        }
        else
        {
          gains_(o,t) = 0.0;
        }
      }

      gainsprev_ = gains_(o,t);
      out(o,t) =  gainsprev_ * in(o,t);

    }
}








