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
#include "DPWOsc.h"
#include "math.h"

using std::ostringstream;
using namespace Marsyas;

DPWOsc::DPWOsc(mrs_string name) : MarSystem("DPWOsc", name)
{
  currentValue_ = 0;
  incr_  = 0;
  cyclicRate_ = 0;
  israte_ = 0;
  frequency_ = 0;
  type_ = 0;

  addControls();
}

DPWOsc::DPWOsc(const DPWOsc& a) : MarSystem(a)
{
  // IMPORTANT!
  // All member MarControlPtr have to be explicitly reassigned in
  // the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
}


DPWOsc::~DPWOsc()
{
}

MarSystem* DPWOsc::clone() const
{
  // Every MarSystem should do this.
  return new DPWOsc(*this);
}

void DPWOsc::addControls()
{
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_natural/type", 0);
  addctrl("mrs_real/cyclicrate", 0.0);
  addctrl("mrs_bool/cyclicin", false);

  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_natural/type", true);
  setctrlState("mrs_real/cyclicrate", true);
}

void DPWOsc::myUpdate(MarControlPtr sender)
{
  MRSDIAG("DPWOsc.cpp - DPWOsc:myUpdate");

  // Start the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // This sets the ouput channels to 1, as this is a mono oscillator
  ctrl_onObservations_->setValue(1, NOUPDATE);

  // Because our range is from -1 to 1, and frequency / israte is
  // for the range 0 to 1. We need to double the frequency to
  // accomedate the larger range.
  frequency_ = 2 * (getctrl("mrs_real/frequency")->to<mrs_real>());
  israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());
  cyclicIn_ = (getctrl("mrs_bool/cyclicin")->to<mrs_bool>());
  c_ = israte_/(4 * frequency_ * (1 - (frequency_/israte_)));

  incr_ = (frequency_ ) / israte_;
  type_ =  (getctrl("mrs_natural/type")->to<mrs_natural>());
  cyclicRate_ = (getctrl("mrs_real/cyclicrate")->to<mrs_real>());
}

void DPWOsc::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    incr_ = (frequency_ * (in(0,t) + 1) ) / israte_;

    currentValue_ += incr_;
    if (currentValue_ >= 1)
    {
      currentValue_ -= 2;
    }

    out(0,t) = c_ * df(currentValue_ * currentValue_);
  }
}
