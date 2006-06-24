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
    \class PvUnconvert
    \brief PvUnconvert

    PvUnconvert N real and imaginary spectrum values to 
N/2+1 pairs of magnitude and phase values. The phases 
are unwrapped and successive phase differences are 
used to compute estimates of the instantaneous frequencies 
for each phase vocoder analysis channel; decimation rate 
and sampling rate are used to render these frequencies 
directly in Hz. 
*/

#include "PvUnconvert.h"

using namespace std;
using namespace Marsyas;

PvUnconvert::PvUnconvert():MarSystem()
{
  type_ = "PvUnconvert";
}


PvUnconvert::PvUnconvert(string name)
{
  type_ = "PvUnconvert";
  name_ = name;
  addControls();
}


PvUnconvert::~PvUnconvert()
{
  
}


MarSystem* 
PvUnconvert::clone() const 
{
  return new PvUnconvert(*this);
}


void 
PvUnconvert::addControls()
{
  addDefaultControls();
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
}



void
PvUnconvert::update()
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations").toNatural() - 2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  
  
  mrs_natural inObservations = getctrl("mrs_natural/inObservations").toNatural();
  mrs_natural onObservations = getctrl("mrs_natural/onObservations").toNatural();
  mrs_real israte = getctrl("mrs_real/israte").toReal();
  
  N2_ = onObservations/2;
  lastphase_.create(N2_+1);
  
  fundamental_ = (mrs_real) (israte  / inObservations);
  factor_ = (((getctrl("mrs_natural/Interpolation").toNatural()* TWOPI)/(israte * onObservations)));
  
}




void 
PvUnconvert::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  mrs_real phase;
  mrs_natural re, amp, im, freq;
  mrs_real mag;
  

  for (t=0; t <= N2_; t++)
    {
      re = amp = 2*t;
      im = freq = 2*t+1;
      if (t== N2_)
	{
	  re = 1;
	}
      mag = in(re, 0);
      lastphase_(t) += in(freq) - t * fundamental_;
      phase = lastphase_(t) * factor_;
      out(re,0) = mag * cos(phase);
      if (t != N2_)
	out(im,0) = -mag * sin(phase);
    }
}

 





	

	

	
	

	
