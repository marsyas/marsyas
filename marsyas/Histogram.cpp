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

#include "Histogram.h"

using namespace std;
using namespace Marsyas;


Histogram::Histogram(string name):MarSystem("Histogram",name)
{
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
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_bool/reset", false);
	setctrlState("mrs_bool/reset", true);
  addctrl("mrs_natural/startBin", 0);
  setctrlState("mrs_natural/startBin", true);
  addctrl("mrs_natural/endBin", 100);
  setctrlState("mrs_natural/endBin", true);
}

void
Histogram::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Histogram.cpp - Histogram:myUpdate");

  startBin_ = getctrl("mrs_natural/startBin")->toNatural();
  endBin_ = getctrl("mrs_natural/endBin")->toNatural();
  reset_ = getctrl("mrs_bool/reset")->toBool();  
  
  setctrl("mrs_natural/onSamples", endBin_ - startBin_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void 
Histogram::myProcess(realvec& in, realvec& out)
{
  
  //checkFlow(in,out);

  if (reset_) 
    {
      out.setval(0.0);
      reset_ = false;
      setctrl("mrs_bool/reset", false);
    }

  mrs_natural bin;
  mrs_real amp;
  
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_/2; t++)
      {
	bin = (mrs_natural)in(o,2*t+1);
	amp = in(o,2*t);

	
	if ((bin < endBin_ - startBin_)&&(bin > 0)) 
	  {
	    out(0,bin) += amp;
	  }
	

	/* 
	   mrs_real factor;	    
	   mrs_natural index;
	   
	   index = (mrs_natural) 2 * bin;
	   factor = 0.25;
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   index = (mrs_natural) round(0.5 * bin);
	   factor = 0.25;
	   
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   
	   index = 3 * bin;
	   factor = 0.15;
	   if (index < endBin_ - startBin_)
	   out(0,index) += factor * amp;
	   
	   
	   index = (mrs_natural) round(0.33333 * bin);
	   factor = 0.15;
	   
	   if (index < endBin_ - startBin_)
	   out(0,index) += (factor * amp);
	   
	    	     
	      }
	*/ 
	
	

      }
}







	

	
