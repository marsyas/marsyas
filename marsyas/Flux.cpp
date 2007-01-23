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
    \class Flux
    \brief Flux calculate the flux between the current and prev. vector

    The flux is defined as the norm of the difference vector between 
    two succesive spectra. 
*/

#include "Flux.h"

using namespace std;
using namespace Marsyas;

Flux::Flux(string name):MarSystem("Flux",name)
{
  diff_ = 0.0;
  flux_ = 0.0;
  max_ = 0.0;
}


Flux::~Flux()
{
}


MarSystem* 
Flux::clone() const 
{
  return new Flux(*this);
}

void
Flux::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Flux.cpp - Flux:localUpdate");
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());
  setctrl("mrs_string/onObsNames", "Flux,");

  prevWindow_.create(getctrl("mrs_natural/inObservations")->toNatural(),
		     getctrl("mrs_natural/inSamples")->toNatural());
}

void 
Flux::myProcess(realvec& in, realvec& out)
{
  for (t = 0; t < inSamples_; t++)
    {
      flux_ = 0.0;
      diff_ = 0.0;
      max_ = 0.0;
      for(o = 1; o < inObservations_; ++o)
	{
	  diff_ = pow(log(in(o,t)+MINREAL) - log(prevWindow_(o,t)+MINREAL), 2.0);
	  if(diff_ > max_)
	    max_ = diff_;
	  flux_ += diff_;

	  prevWindow_(o,t) = in(o,t);
	}
		
      if(max_ != 0.0)
	flux_ /= (max_ * inObservations_);
      else
	flux_ = 0.0;

      out(0,t) = flux_;
    }
}














