/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "Vibrato.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Vibrato::Vibrato(mrs_string name):MarSystem("Vibrato",name)
{
  addControls();
}

Vibrato::~Vibrato()
{
}

MarSystem*
Vibrato::clone() const
{
  return new Vibrato(*this);
}

void
Vibrato::addControls()
{
  addctrl("mrs_real/mod_freq", 5.0);       // 5 Hz
  addctrl("mrs_real/width", 0.005);               // 5 ms
  setctrlState("mrs_real/mod_freq", true);
  setctrlState("mrs_real/width", true);
}

void
Vibrato::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Vibrato.cpp - Vibrato:localuUpdate");

  MarSystem::myUpdate(sender);

  mrs_real mod_freq;
  mod_freq = getctrl("mrs_real/mod_freq")->to<mrs_real>();
  width_ = getctrl("mrs_real/width")->to<mrs_real>();

  delay_ = floor(width_ * israte_);
  width_ = floor(width_ * israte_);
  mod_freq = mod_freq / israte_;     // mod_freq in samples

  mrs_natural L = mrs_natural(2 + delay_ + width_ * 2);
//  cout << "L = " << L << endl;

  if (delaylineSize_ == 0)
  {
    delaylineSize_ = L;
    delayline_.create((mrs_natural)delaylineSize_);
    wp_ = 0;			// write pointer for delay line
    rp_ = 0;			// read pointer for fractional delay
    rpp_ = 0;			// read pointer for fractional delay
  }

  tmod_ = 0;

}


void
Vibrato::myProcess(realvec &in, realvec &out)
{
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    mrs_real M = getctrl("mrs_real/mod_freq")->to<mrs_real>();
    M = M / israte_;
    mrs_real MOD = sin(M* 2 * PI * tmod_);
    tmod_ ++ ;

    mrs_real Z = 1 + delay_ + width_ * MOD;
    mrs_natural i = (mrs_natural)floor(Z);
    mrs_real frac = Z - i;

    // put samples in delay line
    delayline_(wp_) = in(0,t);

    // advance read/write pointers in circular buffer
    wp_ = (wp_+1) % delaylineSize_;
    rp_ = (wp_ + i + 1) % delaylineSize_;
    rpp_ = (wp_ + i) % delaylineSize_;

    // write to output
    out(0,t) = delayline_(rp_)* frac + delayline_(rpp_)   * (1 -frac);

  }

}








