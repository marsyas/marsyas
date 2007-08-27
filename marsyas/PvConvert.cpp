/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "PvConvert.h"

#include <algorithm>

using namespace std;
using namespace Marsyas;

PvConvert::PvConvert(string name):MarSystem("PvConvert",name)
{
  //type_ = "PvConvert";
  //name_ = name;
  
	psize_ = 0;
  size_ = 0;

	addControls();
}


PvConvert::~PvConvert()
{
}

MarSystem* 
PvConvert::clone() const 
{
  return new PvConvert(*this);
}


void 
PvConvert::addControls()
{
  addctrl("mrs_natural/Decimation",MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_natural/Sinusoids", 1);
  setctrlState("mrs_natural/Sinusoids", true);
}

void
PvConvert::myUpdate(MarControlPtr sender)
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() + 2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() * getctrl("mrs_natural/inObservations")->to<mrs_natural>());  

  //defaultUpdate(); [!]
	onObservations_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  
  size_ = onObservations_ /2 +1;
  
  if (size_ != psize_)
    {
      lastphase_.stretch(size_);
      phase_.stretch(size_);
      mag_.stretch(size_);
      sortedmags_.stretch(size_);
      sortedpos_.stretch(size_);
    }
  
  psize_ = size_;
  
  factor_ = ((getctrl("mrs_real/osrate")->to<mrs_real>()) / 
	     (mrs_real)( getctrl("mrs_natural/Decimation")->to<mrs_natural>()* TWOPI));
  fundamental_ = (mrs_real) (getctrl("mrs_real/osrate")->to<mrs_real>() / (mrs_real)getctrl("mrs_natural/inObservations")->to<mrs_natural>());
  kmax_ = getctrl("mrs_natural/Sinusoids")->to<mrs_natural>();

}





void 
PvConvert::process1(realvec& in, realvec& out)
{

  
  //checkFlow(in,out); 
  
  mrs_natural N2 = inObservations_/2;
  mrs_real a;
  mrs_real b;
  mrs_real phasediff;

  // handle amplitudes
  for (t=0; t <= N2; t++)
    {
      if (t==0)
	{
	  a = in(2*t,0);
	  b = 0.0;
	}
      else if (t == N2)
	{
	  a = in(1, 0);
	  b = 0.0;
	}
      else
	{
	  a = in(2*t, 0);
	  b = in(2*t+1, 0);
	}
      
      // computer magnitude value 
      mag_(t) = sqrt(a*a + b*b);
      // sortedmags_(t) = mag_(t);
      // compute phase
      phase_(t) = -atan2(b,a);
      
    }

  bool found = false;
  
  for (t=2; t <= N2; t++)
    {

      mrs_real val = mag_(t);
      if ((val > mag_(t-1)) && (val > mag_(t+1))) 
	found = true;
      else
	found = false;
      
      
      out(2*t,0) = 0.0;
      out(2*t+1,0) = t * fundamental_;
      
      if (found) 
	{
	  if (val == 0.0) 
	    phasediff = 0.0;
	  else 
	    {
	      out(2*t,0) = val;
	      phasediff = phase_(t) - lastphase_(t);
	      lastphase_(t) = phase_(t);	
	    }

	  
	  // phase unwrapping 
	  while (phasediff > PI) 
	    phasediff -= TWOPI;
	  while (phasediff < -PI) 
	    phasediff += TWOPI;
	  
	  
	  out(2*t+1, 0) = phasediff * factor_ + t * fundamental_;      
	}
    }
  
}



void 
PvConvert::myProcess(realvec& in, realvec& out)
{
  
  //checkFlow(in,out); 
  
  
  mrs_natural N2 = inObservations_/2;
  mrs_real a;
  mrs_real b;
  mrs_real phasediff;
  
  // handle amplitudes
  for (t=0; t <= N2; t++)
    {
      if (t==0)
	{
	  a = in(2*t,0);
	  b = 0.0;
	}
      else if (t == N2)
	{
	  a = in(1, 0);
	  b = 0.0;
	}
      else
	{
	  a = in(2*t, 0);
	  b = in(2*t+1, 0);
	}
      
      // computer magnitude value 
      mag_(t) = sqrt(a*a + b*b);
      sortedmags_(t) = mag_(t);
      // compute phase
      phase_(t) = -atan2(b,a);
      
    }
  
  
  mrs_real* data = sortedmags_.getData();
  sort(data, data+(N2+1), greater<mrs_real>());
  
  bool found;
  mrs_real val;
  
  
  for (t=0; t <= N2; t++)
    {
      found = false;
      val = mag_(t);
      
      for (c=0; c < kmax_; c++)
	{
	  if (val == sortedmags_(c))
	    {
	      found = true;
	      break;
	    }
	}

      out(2*t,0) = 0.0;
      out(2*t+1,0) = t * fundamental_;


      
      phasediff = phase_(t) - lastphase_(t);
      lastphase_(t) = phase_(t);	

      // phase unwrapping 
      while (phasediff > PI) 
	phasediff -= TWOPI;
      while (phasediff < -PI) 
	phasediff += TWOPI;      
      
      if (found) 
	{
	  if (val == 0.0) 
	    phasediff = 0.0;
	  else 
	    {
	      out(2*t,0) = val;
	    }
	  
	  out(2*t+1, 0) = phasediff * factor_ + t * fundamental_;      
	}
      else 
	{
	  out(2*t+1, 0) = phasediff * factor_ + t * fundamental_;      
	}
      
	
    }
}






	

	

	
	
      
