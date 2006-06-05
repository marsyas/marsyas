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
    \class MaxArgMax
    \brief Calculate k maximums and their positions

    The output is : max1, argmax1, max2, argmax2, .... 
*/


#include "MaxArgMax.h"
using namespace std;


MaxArgMax::MaxArgMax(string name)
{
  type_ = "MaxArgMax";
  name_ = name;
  addControls();
}


MaxArgMax::~MaxArgMax()
{
}


MarSystem* 
MaxArgMax::clone() const 
{
  return new MaxArgMax(*this);
}


void 
MaxArgMax::addControls()
{
  addDefaultControls();
  addctrl("natural/nMaximums", (natural)1);
  setctrlState("natural/nMaximums", true);
}


void
MaxArgMax::update()
{
  natural k = getctrl("natural/nMaximums").toNatural();
  
  setctrl("natural/onSamples",  2 * k);
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));  

  defaultUpdate();
}



void 
MaxArgMax::process(realvec& in, realvec& out)
{
  checkFlow(in,out);


  
  out.setval(0.0);
  natural k = getctrl("natural/nMaximums").toNatural();

  

    for (o=0; o < inObservations_; o++)
      for (t=0; t < inSamples_; t++)
	{
	  real newmax = in(0,t);
	  real newmax_i = t;
	  for (ki=0; ki < k; ki++)
	    {
	      if (newmax > out(0, 2*ki))
		{
		  real oldmax = out(0, 2*ki);
		  real oldmax_i = out(0,2*ki+1);
		  out(0,2*ki) = newmax;
		  out(0,2*ki+1) = newmax_i;
		  newmax = oldmax;
		  newmax_i = oldmax_i;
		}
	    }
	}
    
  
  
}







	

	

	
