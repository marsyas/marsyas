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
    \class AbsMax
    \brief Calculate the maximum of the absolute values of the signal 
    
Calculates the maximum of the absolute values of 
the singal. Used for sending waveform plots to editor. 
*/

#include "AbsMax.h"
using namespace std;
using namespace Marsyas;


AbsMax::AbsMax(string name)
{
  type_ = "AbsMax";
  name_ = name;
  addControls();
}


AbsMax::~AbsMax()
{
}


MarSystem* 
AbsMax::clone() const 
{
  return new AbsMax(*this);
}


void 
AbsMax::addControls()
{
  addDefaultControls();
}


void
AbsMax::update()
{
  setctrl("mrs_natural/onSamples",  (mrs_natural)1);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte").toReal() / (mrs_real)getctrl("mrs_natural/inSamples").toNatural());  
  defaultUpdate();
}



void 
AbsMax::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  for (o=0; o < inObservations_; o++)
    {
      max_ = 0.0;
      for (t=0; t < inSamples_; t++)
	{
	  if (fabs(in(o,t)) > max_) 
	    max_ = fabs(in(o,t));
	}
      out(o,0) = max_;
    }
  
}






	

	

	
	
