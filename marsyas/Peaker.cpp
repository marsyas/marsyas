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
    \class Peaker
    \brief Pick peaks out of signal 

   Peaker is used to select peaks(or valleys) from the input vector. Various 
parameters of the peak selection process can be adjusted.
*/

#include "Peaker.h"

using namespace std;
using namespace Marsyas;

Peaker::Peaker(string name):MarSystem("Peaker",name)
{
  //type_ = "Peaker";
  //name_ = name;

	addControls();
}


Peaker::~Peaker()
{
}

MarSystem* 
Peaker::clone() const 
{
  return new Peaker(*this);
}

void
Peaker::addControls()
{
  addctrl("mrs_real/peakSpacing", 0.0);
  addctrl("mrs_real/peakStrength", 0.0);
  addctrl("mrs_natural/peakStart", (mrs_natural)0);
  addctrl("mrs_natural/peakEnd", (mrs_natural)0);
  addctrl("mrs_real/peakGain", 1.0);
}

void 
Peaker::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  
  mrs_real peakSpacing;
  mrs_real peakStrength;
  mrs_real peakGain;
  
  mrs_natural peakStart;
  mrs_natural peakEnd;
  

  

  peakSpacing = getctrl("mrs_real/peakSpacing").toReal();
  peakStrength = getctrl("mrs_real/peakStrength").toReal();
  peakStart = getctrl("mrs_natural/peakStart").toNatural();
  peakEnd = getctrl("mrs_natural/peakEnd").toNatural();
  peakGain = getctrl("mrs_real/peakGain").toReal();
  mrs_real srate = getctrl("mrs_real/israte").toReal();
  

  out.setval(0.0);

  for (o = 0; o < inObservations_; o++)
    {
      rms_ = 0.0;
      peakSpacing = (mrs_real)(peakSpacing * inSamples_);
      for (t=peakStart+1; t < peakEnd-1; t++)
	{
	  rms_ += in(o,t) * in(o,t);
	} 
      if (rms_ != 0.0) 
	rms_ /= (peakEnd - peakStart);
      rms_ = sqrt(rms_);
      
      mrs_real max;
      mrs_natural maxIndex;
      
      bool peakFound = false;



      
      for (t=peakStart+1; t < peakEnd-1; t++)
	{
	  // peak has to be larger than neighbors 
	  if ((in(o,t-1) < in(o,t)) 
	      && (in(o,t+1) < in(o,t))
	      && (in(o,t) > 0.0)
	      && (in(o,t) > (peakStrength * rms_)))
	    {
	      // check for another peak in the peakSpacing area
	      max = in(o,t);
	      maxIndex = t;
	      
	      

	      for (int j=0; j < (mrs_natural)peakSpacing; j++)
		{
		  if (t+j < peakEnd-1)
		    if (in(o,t+j) > max)
		      {
			max = in(o,t+j);
			maxIndex = t+j;
		      }
		}
	      
	      t += (mrs_natural)peakSpacing;
	      
	      out(o,maxIndex) = in(o,maxIndex);

	      /* twice_ = 2 * maxIndex;
	      half_ = (mrs_natural) (0.5 * maxIndex);
	      triple_ = 3 * maxIndex;
	      third_ = (mrs_natural) (0.33 * maxIndex);
	      
	      if (twice_ < (peakEnd - peakStart))
		{
		  out(o, maxIndex) += in(o, twice_);
		}

	      if (half_ < (peakEnd - peakStart))
		{
		  out(o, maxIndex) += in(o, half_);
		}

	      if (triple_ < (peakEnd - peakStart))
		{
		  out(o, maxIndex) += in(o, triple_);
		}

	      if (third_ < (peakEnd - peakStart))
		{
		  out(o, maxIndex) += in(o, third_);
		}

	      */ 
	      
	      
	      
	      
	      peakFound = true;
	    }
	}

      
    }


}






	

	
