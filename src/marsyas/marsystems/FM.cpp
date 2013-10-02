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

#include "FM.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

#define WAVETABLE_SIZE 16384*4

// create the wavetable.
FM::FM(mrs_string name) :
  MarSystem("FM",name),
  wavetable_(WAVETABLE_SIZE),
  wavetableSize_(WAVETABLE_SIZE)
{
  mrs_real incr = TWOPI / wavetableSize_;

  for (mrs_natural t=0; t < wavetableSize_; ++t)
  {
    wavetable_(t) = (mrs_real)(0.5 * sin(incr * t));
  }

  mIndex_ = 0;
  oIndex_ = 0;
  addControls();
}


FM :: ~FM()
{
}


MarSystem* FM::clone() const
{
  return new FM(*this);
}


void FM::addControls()
{
  addctrl("mrs_natural/nChannels",1);

  addctrl("mrs_real/mDepth", 15.0);						// modulator depth
  setctrlState("mrs_real/mDepth",true);

  addctrl("mrs_real/mSpeed", 6.0);						// modulator speed
  setctrlState("mrs_real/mSpeed", true);

  addctrl("mrs_real/cFrequency", 1000.0);			// carrier frequency
  setctrlState("mrs_real/cFrequency", true);

  addctrl("mrs_bool/noteon", false);
  setctrlState("mrs_bool/noteon", true);
}

void FM::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("FM.cpp - FM:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  // update the controls for the FM
  cFrequency_ = getctrl("mrs_real/cFrequency")->to<mrs_real>();
  isRate_ = getctrl("mrs_real/israte")->to<mrs_real>();
  mSpeed_ = getctrl("mrs_real/mSpeed")->to<mrs_real>();
  mDepth_ = getctrl("mrs_real/mDepth")->to<mrs_real>();
  mRate_ = (mSpeed_ * wavetableSize_) / getctrl("mrs_real/israte")->to<mrs_real>();
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
}

void FM::myProcess( realvec& in, realvec& out )
{
  (void) in;
  mrs_natural t;
  //checkFlow(in,out);


  register mrs_real mSample_;
  register mrs_real oFrequency_;

  if (getctrl("mrs_bool/noteon")->to<mrs_bool>() == false) {
    return;
  }

  for (t=0; t < inSamples_; t++)
  {

    // calculate the modulator output
    mSample_ = wavetable_((mrs_natural)mIndex_);
    mIndex_ += mRate_;
    mSample_ *= mDepth_;

    // calculate any FM and the new output rate
    oFrequency_ = cFrequency_ + mSample_;
    oRate_ = (oFrequency_ * wavetableSize_) / isRate_;

    out(0,t) = wavetable_((mrs_natural)oIndex_);

    // we are one sample behind in case this index goes off the map
    oIndex_ += oRate_;

    while (mIndex_ >= wavetableSize_)
    {
      mIndex_ -= wavetableSize_;
    }

    while (oIndex_ >= wavetableSize_)
    {
      oIndex_ -= wavetableSize_;
    }
  }
}
