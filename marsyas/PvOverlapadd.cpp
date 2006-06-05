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

/** 
    \class PvOverlapadd
    \brief PvOverlapadd

    Multiply with window (both length Nw) using modulus arithmetic;
fold and rotate windowed input into output array (FFT) (length N) 
according to current input time (t)
*/



#include "PvOverlapadd.h"
using namespace std;


PvOverlapadd::PvOverlapadd():MarSystem()
{
  type_ = "PvOverlapadd";
}


PvOverlapadd::PvOverlapadd(string name)
{
  type_ = "PvOverlapadd";
  name_ = name;
  addControls();
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
  addDefaultControls();
  addctrl("natural/Time",0);
  addctrl("natural/WindowSize", MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("natural/WindowSize", true);
  addctrl("natural/FFTSize", MRS_DEFAULT_SLICE_NSAMPLES);
  addctrl("natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES /4);
}



void
PvOverlapadd::update()
{
  setctrl("natural/onSamples", getctrl("natural/WindowSize"));
  setctrl("natural/onObservations", (natural)1);
  setctrl("real/osrate", getctrl("real/israte"));    

  natural N,Nw,I;
  N = getctrl("natural/inSamples").toNatural();
  Nw = getctrl("natural/onSamples").toNatural();
  I = getctrl("natural/Interpolation").toNatural();
  // create synthesis window 
  
  swin_.create(Nw);
  awin_.create(Nw);
  
  for (t=0; t < Nw; t++)
    {
      awin_(t) = (real)(0.54 - 0.46 * cos(TWOPI * t/(Nw-1)));
      swin_(t) = (real)(0.54 - 0.46 * cos(TWOPI * t/(Nw-1)));
    }
  /* when Nw > N also apply interpolating (sinc) windows 
   * to ensure that window are 0 at increments of N (the 
   * FFT length) aways from the center of the analysis
   * window 
   */ 
  if (Nw > N) 
    {
      real x;
      x = (real)(-(Nw -1) / 2.0);
      for (t=0; t < Nw; t++, x += 1.0)
	{
	  if (x != 0.0) 
	    {
	      awin_(t) *= N * sin (PI * x/N) / (PI *x);
	      swin_(t) *= I * sin (PI * x/I) / (PI *x);
	    }
	}
    }
  
  /* normalize window for unit gain */ 
  real sum = (real)0.0;
  
  for (t =0; t < Nw; t++)
    {
      sum += awin_(t);
    }
  
  real afac = (real)(2.0/ sum);
  real sfac = Nw > N ? (real)1.0 /afac : (real)afac;
  awin_ *= afac;
  swin_ *= sfac;
  
  if (Nw <= N)
    {
      sum = (real)0.0;
      
      for (t = 0; t < Nw; t+= I)
	{
	  sum += swin_(t) * swin_(t);
	}
      for (sum = (real)1.0/sum, t =0; t < Nw; t++)
	swin_(t) *= sum;
    }
}




void 
PvOverlapadd::process(realvec& in, realvec& out)
{

  
  // add assertions for sizes
  natural N,Nw;
  int n;
  
  N = getctrl("natural/inSamples").toNatural();
  Nw = getctrl("natural/onSamples").toNatural();
  n  = getctrl("natural/Time").toNatural();
  
  while (n < 0) 
    n += N;
  n %= N;


  for (t=0; t < Nw; t++)
    {
      out(0,t) += (in(0,n) * swin_(t));
      
      if (++n == N)
	n = 0;
    }
}






	

	

	

	
