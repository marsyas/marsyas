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

#include "WaveguideOsc.h"
#include "math.h"

using namespace Marsyas;

WaveguideOsc::WaveguideOsc(mrs_string name):MarSystem("WaveguideOsc", name)
{

  addControls();
}

WaveguideOsc::WaveguideOsc(const WaveguideOsc& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
}

WaveguideOsc::~WaveguideOsc()
{
}

MarSystem* WaveguideOsc::clone() const
{
  return new WaveguideOsc(*this);
}

void WaveguideOsc::addControls()
{
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_bool/noteon", false);

  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_bool/noteon", true);
}


void WaveguideOsc::myUpdate(MarControlPtr sender)
{
  // x1n1_ is initialized with an impluse
  x1n1_ = 0.95;
  x2n1_ = 0;

  frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
  israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());

  k_ = cos((TWOPI*frequency_)/israte_);
  MarSystem::myUpdate(sender);
}

void WaveguideOsc::myProcess(realvec& in, realvec& out)
{
  mrs_real x1, x2;

  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    k_ = cos((TWOPI*frequency_*(in(0,t) + 1))/israte_);
    x1 = (2* k_ * x1n1_) - x2n1_;
    x2 = x1n1_;
    x1n1_ = x1;
    x2n1_ = x2;

    out(0,t) = x1 - x2;
  }
}
