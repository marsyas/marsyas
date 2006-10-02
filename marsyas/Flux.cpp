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
  //type_ = "Flux";
  //name_ = name;
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
Flux::localUpdate()
{
  MRSDIAG("Flux.cpp - Flux:localUpdate");
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte").toReal());
  setctrl("mrs_string/onObsNames", "Flux,");
  
  prevWindow_.create(getctrl("mrs_natural/inObservations").toNatural(),
		     getctrl("mrs_natural/inSamples").toNatural());
  prevWindow_.setval(0.0);
}

void 
Flux::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  // computer flux of observations for each time sample 


  for (t = 0; t < inSamples_; t++)
    {
      flux_ = 0.0;
      max_ = 0.0;

      for (o=0; o < inObservations_; o++)
	{
	  diff_ = in(o,t)*in(o,t) - prevWindow_(o,t)*prevWindow_(o,t); // classic
	  // diff_ = log(in(o,t) + 0.000001) - log(prevWindow_(o,t) + 0.000001); log
	  // diff_ = in(o,t) - prevWindow_(o,t);
	  
	  if (in(o,t) > max_) 
	    max_ = in(o,t);
	  // flux_ += sqrt(fabs(diff_));
	  // flux_ += fabs(diff_); log 
	  
	  flux_ += (diff_ * diff_); // classic
	  
	  prevWindow_(o,t) = in(o,t);
	}
      
      if (flux_ != 0.0) // classic
	flux_ = sqrt(flux_); // classic
      
      
      if (max_ != 0.0)
	// flux_ = flux_ / (max_ * inObservations_);
	flux_ = flux_ / (sqrt(1.0 * inObservations_) * max_); // classic
     

	// flux_ = flux_ / inObservations_;   logflux 
      
      
      
      out(0,t) = flux_;
    }

}

      
      







	
	
	
	
