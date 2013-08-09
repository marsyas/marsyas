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

#include "AdditiveOsc.h"
#include "math.h"

using namespace Marsyas;

AdditiveOsc::AdditiveOsc(mrs_string name):MarSystem("AdditiveOsc", name)
{
  harmonics_ = 0; // How many harmonics our signal will have

  israte_ = 0; // Sampling rate of the system


  addControls();
}

AdditiveOsc::AdditiveOsc(const AdditiveOsc& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
}

AdditiveOsc::~AdditiveOsc()
{
}

MarSystem* AdditiveOsc::clone() const
{
  return new AdditiveOsc(*this);
}

void AdditiveOsc::addControls()
{
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_bool/noteon", false);

  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_bool/noteon", true);
}


void AdditiveOsc::myUpdate(MarControlPtr sender)
{
  mrs_real frequency = (getctrl("mrs_real/frequency")->to<mrs_real>());
  israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());


  // How many harmonics do we have before crossing the Nyquist threshold?
  for (harmonics_ = 1; harmonics_ * frequency * 2 <= israte_/2; harmonics_++) {};

  // Initialize our lists of coefficients and past values
  x1n1_.create((mrs_natural)harmonics_ + 1);
  x2n1_.create((mrs_natural)harmonics_ + 1);
  k_.create((mrs_natural)harmonics_ + 1);

  // Initialize our lists
  for (mrs_natural t = 1; t <= harmonics_; t++)
  {
    // The initial impulse of each wave guide
    x1n1_(t) = 0.95 * pow((mrs_real)(-1.0),t + 1);
    // Our last waveguide value is zero
    x2n1_(t) = 0;
    // The coefficient for the waveguide at a given harmonic
    k_(t) = cos((TWOPI * frequency * t)/israte_);
  }

  MarSystem::myUpdate(sender);
}

void AdditiveOsc::myProcess(realvec& in, realvec& out)
{
  (void) in;
  mrs_real y, x1, x2;

  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    y = 0;
    for (mrs_natural har = 1; har <= harmonics_; har++)
    {
      x1 = (2 * k_(har) * x1n1_(har)) - x2n1_(har);
      x2 = x1n1_(har);

      x1n1_(har) = x1;
      x2n1_(har) = x2;

      y += (x1 - x2);
      out(t) = (y * 0.9)/har;
    }
  }
}
