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

#include "PvFold.h"

using std::ostringstream;
using namespace Marsyas;

PvFold::PvFold(mrs_string name):MarSystem("PvFold",name)
{
  //type_ = "PvFold";
  //name_ = name;

  N_ = 0;
  Nw_ = 0;
  PNw_ = 0;
  PN_ = 0;

  addControls();
}

PvFold::PvFold(const PvFold& a):MarSystem(a)
{
  N_ = 0;
  Nw_ = 0;
  PNw_ = 0;
  PN_ = 0;
  ctrl_rmsIn_ = getctrl("mrs_real/rmsIn");
}



PvFold::~PvFold()
{
}

MarSystem*
PvFold::clone() const
{
  return new PvFold(*this);
}


void
PvFold::addControls()
{
  addctrl("mrs_natural/FFTSize", MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/FFTSize", true);
  addctrl("mrs_real/rmsIn", 0.0, ctrl_rmsIn_);
  n_ = 0;
}



void
PvFold::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_natural t;
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/FFTSize"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  N_ = getctrl("mrs_natural/onSamples")->to<mrs_natural>();

  Nw_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  // create analysis window if necessary
  if ((Nw_ != PNw_)||(N_ != PN_))
  {
    n_ = -Nw_;
    awin_.stretch(Nw_);



    for (t=0; t < Nw_; t++)
    {
      awin_(t) = (mrs_real)(0.5 * (1 - cos(TWOPI * t/(Nw_-1))));
    }
    /* when Nw_ > N also apply interpolating (sinc) windows
     * to ensure that window are 0 at increments of N (the
     * FFT length) aways from the center of the analysis
     * window
     */
    /* if (Nw_ > N_)
    {

    mrs_real x;
    x = (mrs_real)(-(Nw_ -1) / 2.0);
    for (t=0; t < Nw_; t++, x += 1.0)
    {
      if (x != 0.0)
    	  awin_(t) *= N_ * sin (PI * x/N_) / (PI *x);
    }
    }
    */
    /* normalize window for unit gain */

    mrs_real sum = 0.0;
    for (t =0; t < Nw_; t++)
    {
      sum += awin_(t);
    }

    mrs_real afac = (mrs_real)(2.0/ sum);

    awin_ *= afac;
  }






  PNw_ = Nw_;
  PN_ = N_;

}

void
PvFold::myProcess(realvec& in, realvec& out)
{

  mrs_natural t;

  for (t=0; t < Nw_; t++)
  {
    out(0,t) = in(0,t)*awin_(t);
    // rmsIn += in(0,t) * in(0,t);
  }
  // rmsIn /= Nw_;
  // rmsIn = sqrt(rmsIn);
  // ctrl_rmsIn_->setValue(rmsIn );


  // circular shift
  int half_Nw_ = Nw_/2;
  mrs_real tmp;
  for (t=0; t < half_Nw_; t++)
  {
    tmp = out(0,t);
    out(0,t) = out(0, t+half_Nw_);
    out(0,t+half_Nw_) = tmp;
  }



}











