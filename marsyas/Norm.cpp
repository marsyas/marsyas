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
    \class Norm
    \brief Norm: normalize input row vectors

    Normalize by subtracting mean and dividing by standard deviation
*/

#include "Norm.h"

using namespace std;
using namespace Marsyas;

Norm::Norm(string name):MarSystem("Norm",name)
{
  //type_ = "Norm";
  //name_ = name;
}


Norm::~Norm()
{
}


MarSystem* 
Norm::clone() const 
{
  return new Norm(*this);
}

void 
Norm::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);
  realvec row(inSamples_);
  mrs_real mean;
  mrs_real std;
  

  for (o=0; o < inObservations_; o++)
    {
      // calculate the mean and standard deviation 
      // of each row 
      for (t = 0; t < inSamples_; t++)
	row(t) = in(o,t);
      mean = row.mean();
      std =  row.std();
      
      for (t = 0; t < inSamples_; t++)
	{
	  // Scale to -1.0 1.0 audio range
	  out(o,t) = (mrs_real)(0.05 * ((in(o,t) - mean) / std)); 
	  
	}
    }
  
  
}







	

	
