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

#include "WaveletBands.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

WaveletBands::WaveletBands(mrs_string name):MarSystem("WaveletBands",name)
{
  iwvpt_ = NULL;

  addControls();
}

WaveletBands::~WaveletBands()
{
  delete iwvpt_;
}


// copy constructor
WaveletBands::WaveletBands(const WaveletBands& a):MarSystem(a)
{
  iwvpt_ = NULL;
}



MarSystem*
WaveletBands::clone() const
{
  return new WaveletBands(*this);
}

void
WaveletBands::addControls()
{
  addctrl("mrs_natural/nBands", 6);
  setctrlState("mrs_natural/nBands", true);
  addctrl("mrs_natural/startBand", 2);
  setctrlState("mrs_natural/startBand", true);
}

void
WaveletBands::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  MRSDIAG("WaveletBands.cpp - WaveletBands:myUpdate");
  mrs_natural nBands = getctrl("mrs_natural/nBands")->to<mrs_natural>();

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() * nBands);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));


  // Set up the inverse wavelet transform object.
  if (!iwvpt_)
  {
    // TODO: why is this done here and not in the contructors? Please explain.
    iwvpt_ = new WaveletPyramid("iwvpt");
  }

  iwvpt_->setctrl("mrs_bool/forward", false);
  iwvpt_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  iwvpt_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  iwvpt_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));

  // Allocate the band and tband realvecs.
  band_.create(getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
               getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  tband_.create(getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                getctrl("mrs_natural/inSamples")->to<mrs_natural>());

}


void
WaveletBands::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_natural level;
  mrs_natural hlevel, llevel;
  mrs_natural base = getctrl("mrs_natural/startBand")->to<mrs_natural>();

  for (o = 0; o < onObservations_; o++)
  {
    // Copy the input for one observation channel to band_.
    for (t=0; t < inSamples_; t++)
    {
      band_(t) = in(0, t);
    }

    // Get the ranges to set to zero.
    level =  7+o;
    hlevel = base << level;
    llevel = base << (level -1);

    // Set the desired parts of band_ to zero.
    band_.setval(hlevel, inSamples_, 0.0);
    band_.setval(0, llevel, 0.0);

    // Do the inverse wavelet transform.
    iwvpt_->process(band_, tband_);

    // Copy the calculations to the output.
    for (t=0; t < inSamples_; t++)
    {
      out(o,t) = tband_(t);
    }
  }

}
