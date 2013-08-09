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

#include "PvOverlapadd.h"

using std::ostringstream;
using namespace Marsyas;

PvOverlapadd::PvOverlapadd(mrs_string name):MarSystem("PvOverlapadd",name)
{
  //type_ = "PvOverlapadd";
  //name_ = name;
  n_ = 0;

  addControls();
}


PvOverlapadd::PvOverlapadd(const PvOverlapadd& a):MarSystem(a)
{
  ctrl_rmsIn_ = getctrl("mrs_real/rmsIn");
}


PvOverlapadd::~PvOverlapadd()
{
}

MarSystem*
PvOverlapadd::clone() const
{
  return new PvOverlapadd(*this);
}


void
PvOverlapadd::addControls()
{
  addctrl("mrs_natural/Time",0);
  addctrl("mrs_natural/winSize", MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/winSize", true);
  addctrl("mrs_natural/FFTSize", MRS_DEFAULT_SLICE_NSAMPLES);
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES /4);
  addctrl("mrs_natural/Decimation", MRS_DEFAULT_SLICE_NSAMPLES /4);
  addctrl("mrs_real/rmsIn", 0.0, ctrl_rmsIn_);
}

void
PvOverlapadd::myUpdate(MarControlPtr sender)
{
  mrs_natural t;
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/winSize"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural N,Nw;
  N = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  Nw = getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  I_ = getctrl("mrs_natural/Interpolation")->to<mrs_natural>();
  D_ = getctrl("mrs_natural/Decimation")->to<mrs_natural>();

  n_ = - (Nw * I_) / D_;


  // create synthesis window
  swin_.create(Nw);
  awin_.create(Nw);
  temp_.stretch(N);
  tin_.create(N);

  for ( t=0; t < Nw; t++)
  {
    awin_(t) = (mrs_real)(0.5 * (1  - cos(TWOPI * t/(Nw-1))));
    swin_(t) = (mrs_real)(0.5 * (1  - cos(TWOPI * t/(Nw-1))));

  }
  /* when Nw > N also apply interpolating (sinc) windows
   * to ensure that window are 0 at increments of N (the
   * FFT length) aways from the center of the analysis
   * window
   */
  /* if (Nw > N)
     {
     mrs_real x;
     x = (mrs_real)(-(Nw -1) / 2.0);
     for (t=0; t < Nw; t++, x += 1.0)
     {
     if (x != 0.0)
     {
     awin_(t) *= N * sin (PI * x/N) / (PI *x);
     swin_(t) *= I_ * sin (PI * x/I_) / (PI *x);
     }
     }
     }
  */

  /* normalize window for unit gain */
  mrs_real sum = (mrs_real)0.0;

  for (t =0; t < Nw; t++)
  {
    sum += awin_(t);
  }

  mrs_real afac = (mrs_real)(2.0/ sum);
  mrs_real sfac = Nw > N ? (mrs_real)1.0 /afac : (mrs_real)afac;
  awin_ *= afac;
  swin_ *= sfac;


  if (Nw <= N)
  {
    sum = (mrs_real)0.0;

    for (t = 0; t < Nw; t+= I_)
    {
      sum += swin_(t) * swin_(t);
    }
    for (sum = (mrs_real)1.0/sum, t =0; t < Nw; t++)
      swin_(t) *= sum;
  }
}


void
PvOverlapadd::myProcess(realvec& in, realvec& out)
{
  mrs_natural t;

  // add assertions for sizes
  mrs_natural N,Nw;

  N = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  Nw = getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  n_ += I_;

  mrs_natural n;
  n  = n_;


  while (n < 0)
    n += N;
  n %= N;


  for (t=0; t < Nw; t++)
  {
    tin_(t) = in(0, t);
  }


  // undo circular shift
  int half_Nw_ = Nw/2;
  mrs_real tmp;
  for (t=0; t < half_Nw_; t++)
  {
    tmp = tin_(t);
    tin_(t) = tin_(t+half_Nw_);
    tin_(t+half_Nw_) = tmp;
  }

  mrs_real rmsOut = 0.0;

  for (t=0; t < Nw; t++)
  {
    temp_(t) += (tin_(t) * swin_(t));
  }


  for (t=0; t < N; t++)
  {
    out(0,t) = temp_(t);
    rmsOut += out(0,t) * out(0,t);
  }

  rmsOut /= Nw;
  rmsOut = sqrt(rmsOut);



  /* out *= 0.75 * (rmsIn / rmsOut); */


  for  (t=0; t < N-I_; t++)
    temp_(t) = temp_(t+I_);
  for (t=N-I_; t<N; t++)
    temp_(t) = 0.0;




}














