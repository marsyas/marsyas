/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BlitOsc.h"
#include "math.h"

using namespace Marsyas;

BlitOsc::BlitOsc(mrs_string name):MarSystem("BlitOsc", name)
{
  addControls();
}

BlitOsc::BlitOsc(const BlitOsc& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
}

BlitOsc::~BlitOsc()
{
}

MarSystem*
BlitOsc::clone() const
{
  return new BlitOsc(*this);
}

void BlitOsc::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_bool/noteon", false);
  addctrl("mrs_natural/type", 0);

  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_natural/type", true);
  setctrlState("mrs_bool/noteon", true);
}


void BlitOsc::myUpdate(MarControlPtr sender)
{
  ap1.set_delay(1.9);
  ap2.set_delay(1.3);

  frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
  type_ = (getctrl("mrs_natural/type")->to<mrs_natural>());
  noteon_ = (getctrl("mrs_bool/noteon")->to<mrs_bool>());
  israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());

  phase_ = 0;
  inv_ = 1;

  switch (type_)
  {
  case 0: // Saw
    //std::cout << frequency_ << std::endl;
    dc_ = frequency_/israte_;
    break;
  case 1: // Square
    // The frequency has to be doubled to compensate for
    // the frequency be being halved by the square wave
    // being bipolar
    frequency_ *= 2;
    dc_ = 0;
    break;
  }

  // d is how many samples to delay
  // because it is possible a fractional amount we split it
  // into the integer part, and the fractional part.
  mrs_real d = israte_/frequency_;
  // N_ is the integer part.
  N_ = (mrs_natural)floor(d);
  // frac_ is the fractional part
  frac_ = (d - N_);
  // delay is used as an overflow counter for the
  // fractional part.
  delay_ = frac_;

  // no change to network flow
  MarSystem::myUpdate(sender);
}

void BlitOsc::myProcess(realvec& in, realvec& out)
{
  (void) in;
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    if (phase_ >= N_ - 1)
    {
      phase_ = 0;

      // The amount of delay is incresed by 1 to optimize the allpass filter.
      // N_ is compensated accordingly for this delay at the top of this if
      // statement.
      ap1.set_delay(delay_ + 1);

      switch (type_)
      {
      case 0: // Saw
        out(0,t) = le(ap2(ap1(1)) - dc_);
        break;
      case 1: // Square
        out(0,t) = le(ap2(ap1(1 * inv_)));
        inv_ = (-inv_);
        break;
      }

      // This is the logic to tune the fractional part of the period.
      delay_ += frac_;
      if (delay_ >= 1)
      {
        delay_ -= 1;
        phase_ = -1;
      }
    }
    else
    {
      phase_++;
      out(0,t) = le(ap2(ap1(0)) - dc_);
    }
  }
}
