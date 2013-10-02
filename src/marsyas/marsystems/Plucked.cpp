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

#include "Plucked.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Plucked::Plucked(mrs_string name):MarSystem("Plucked",name)
{
  // Why do these variables exist?
  pointer1_ = 0;
  pointer2_ = 0;
  pointer3_ = 0;

  //
  a_ = 0;
  b_ = 0;
  noteon_ = false;
  delaylineSize_ = 0;
  gain_ = NULL;

  addControls();
}

Plucked::~Plucked()
{
  delete gain_;
}

MarSystem*
Plucked::clone() const
{
  return new Plucked(*this);
}

void Plucked::addControls()
{
  addctrl("mrs_real/frequency", 100.0);
  addctrl("mrs_real/pluckpos", 0.5);
  addctrl("mrs_real/nton", 0.5);
  addctrl("mrs_bool/noteon", false);
  addctrl("mrs_real/loss",1.0);
  addctrl("mrs_real/stretch",0.2);
  setctrlState("mrs_real/frequency", true);
  setctrlState("mrs_real/nton", true);
  setctrlState("mrs_bool/noteon", true);
  setctrlState("mrs_real/loss", true);
}

void Plucked::myUpdate(MarControlPtr sender)
{
  mrs_natural t;
  MRSDIAG("Plucked.cpp - Plucked:localUpdate");

  MarSystem::myUpdate(sender);

  gain_ = new Gain("pluckedGain");
  gain_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  gain_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  gain_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));
  gain_->updControl("mrs_real/gain", 1.3);

  gout_.create(gain_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
               gain_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  mrs_real freq = getctrl("mrs_real/frequency")->to<mrs_real>();
  mrs_real pos = getctrl("mrs_real/pluckpos")->to<mrs_real>();

  nton_ = getctrl("mrs_real/nton")->to<mrs_real>();
  noteon_ = getctrl("mrs_bool/noteon")->to<mrs_bool>();

  loss_ = getctrl("mrs_real/loss")->to<mrs_real>();

  s_ = getctrl("mrs_real/stretch")->to<mrs_real>();

  mrs_real israte = (getctrl("mrs_real/israte")->to<mrs_real>());

  // loweset frequency on a piano is 27.5Hz ... sample rate/27.5 for commute
  // this is the longest delay line required
  if (delaylineSize_ == 0)
  {
    delaylineSize_ = israte/27.5;
    noise_.create((mrs_natural)delaylineSize_);
    delayline1_.create((mrs_natural)delaylineSize_);
    pickDelayLine_.create((mrs_natural)delaylineSize_);

    for (t = 0; t < delaylineSize_; t++)
    {
      noise_(t) = (mrs_real)(rand() / (RAND_MAX + 1.0) -0.5);
    }
  }

  if (nton_ > 0 || noteon_)
  {
    a_ = 0;
    d_ = 2*israte/freq;
    N_ = (mrs_natural)floor(d_);
    g_ = -(-1+d_)/(-d_-1); //for all pass implementation
    picklength_ = (mrs_natural)floor(N_*pos); //for inverse comb implementation

    for (t = 0; t < N_; t++)
    {
      pickDelayLine_(0)=noise_(t);
      delayline1_(t) = noise_(t)+ (mrs_real)0.1 * pickDelayLine_(picklength_-1);

      //shift the pick delayline to the right 1 cell
      for(p=0; p<=picklength_-2; p++)
      {
        pickDelayLine_(picklength_-1-p) = pickDelayLine_(picklength_-1-p-1);
      }
    }
    wp_ = 1;
    wpp_ = 0;
    rp_ = N_-1;
  }
}


void Plucked::myProcess(realvec &in, realvec &out)
{
  (void)in;

  if (noteon_)
  {
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      // wp holds the current sample
      // wpp holds the previous sample
      a_ = delayline1_(wp_);
      b_ = delayline1_(wpp_);

      // rp_ holds the sample at delay N_
      delayline1_(rp_) = loss_*((1-s_)*a_ + s_* b_);

      rp_ = (rp_ + +1)  % N_;
      wp_ = (wp_ + 1)   % N_;
      wpp_ = (wpp_ + 1) % N_;
      gout_(0,t) = a_;
    }
  }
  gain_->process(gout_, out);
}
