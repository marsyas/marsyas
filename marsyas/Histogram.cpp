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
    \class Histogram
    \brief Histogram

    Calculate histogram
*/




#include "Histogram.h"
using namespace std;



Histogram::Histogram(string name)
{
  type_ = "Histogram";
  name_ = name;
  addControls();
}


Histogram::~Histogram()
{
}


MarSystem* 
Histogram::clone() const 
{
  return new Histogram(*this);
}

void 
Histogram::addControls()
{
  addDefaultControls();
  addctrl("real/gain", 1.0);
  addctrl("bool/reset", false);
  addctrl("natural/startBin", 0);
  setctrlState("natural/startBin", true);
  addctrl("natural/endBin", 100);
  setctrlState("natural/endBin", true);
  addctrl("bool/reset", true);
  setctrlState("bool/reset", true);

}


void
Histogram::update()
{
  MRSDIAG("Histogram.cpp - Histogram:update");

  

  startBin_ = getctrl("natural/startBin").toNatural();
  endBin_ = getctrl("natural/endBin").toNatural();
  reset_ = getctrl("bool/reset").toBool();  
  
  setctrl("natural/onSamples", endBin_ - startBin_);
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));


  defaultUpdate();
}


void 
Histogram::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);

  if (reset_) 
    {
      out.setval(0.0);
      reset_ = false;
      setctrl("bool/reset", (MarControlValue)false);
    }

  natural bin;
  real amp;
  
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_/2; t++)
      {
	bin = (natural)in(o,2*t+1);
	amp = in(o,2*t);

	
	if ((bin < endBin_ - startBin_)&&(bin > 0)) 
	  {
	    out(0,bin) += amp;
	  }
	

	/* 
	   real factor;	    
	   natural index;
	   
	   index = (natural) 2 * bin;
	   factor = 0.25;
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   index = (natural) round(0.5 * bin);
	   factor = 0.25;
	   
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   
	   index = 3 * bin;
	   factor = 0.15;
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   
	   index = (natural) round(0.33333 * bin);
	   factor = 0.15;
	   
	   if (index < endBin_ - startBin_)
	   out(0,index) += (factor * amp);
	   
	    	     
	      }
	*/ 
	
	

      }
}







	

	
