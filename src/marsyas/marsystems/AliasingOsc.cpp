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

#include "../common_source.h"
#include "AliasingOsc.h"

using std::ostringstream;
using namespace Marsyas;

AliasingOsc::AliasingOsc(mrs_string name) : MarSystem("AliasingOsc", name)
{
  currentValue_ = 0;
  incr_  = 0;
  cyclicRate_ = 0;
  israte_ = 0;
  frequency_ = 0;
  type_ = 0;

  addControls();
}

AliasingOsc::AliasingOsc(const AliasingOsc& a) : MarSystem(a)
{
  // IMPORTANT!
  // All member MarControlPtr have to be explicitly reassigned in
  // the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
}


AliasingOsc::~AliasingOsc()
{
}

MarSystem* AliasingOsc::clone() const
{
  // Every MarSystem should do this.
  return new AliasingOsc(*this);
}

void AliasingOsc::addControls()
{
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_natural/type", 0);
  addctrl("mrs_real/cyclicrate", 0.0);
  addctrl("mrs_bool/cyclicin", false);

  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_natural/type", true);
  setctrlState("mrs_real/cyclicrate", true);
}

void AliasingOsc::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AliasingOsc.cpp - AliasingOsc:myUpdate");

  // Start the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // This sets the ouput channels to 1, as this is a mono oscillator
  ctrl_onObservations_->setValue(1, NOUPDATE);

  // Because our range is from -1 to 1, and frequency / israte is
  // for the range 0 to 1. We need to double the frequency to
  // accomedate the larger range.
  frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
  israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());
  cyclicIn_ = (getctrl("mrs_bool/cyclicin")->to<mrs_bool>());

  type_ =  (getctrl("mrs_natural/type")->to<mrs_natural>());
  cyclicRate_ = (getctrl("mrs_real/cyclicrate")->to<mrs_real>());
}

void AliasingOsc::myProcess(realvec& in, realvec& out)
{
  mrs_natural t;

  for (t = 0; t < inSamples_; t++)
  {

    incr_ = (frequency_ * (in(0,t) + 1) ) / israte_;

    currentValue_ += incr_;
    if (currentValue_ >= 1)
    {
      currentValue_ -= 2;
    }

    // Logic for different oscillator types
    switch(type_)
    {
    case 0: // Saw
      out(0,t) = currentValue_;
      break;
    case 1: // PWM
      if (currentValue_ >= (cyclicIn_ ? in(1,t) : cyclicRate_))
      {
        out(0,t) = 0.9;
      }
      else
      {
        out(0,t) = -0.9;
      }
      break;
    }
  }
}
