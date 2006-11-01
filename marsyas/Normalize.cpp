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
    \class Normalize
    \brief Normalize my mapping min/max range to user specified range
*/

#include "Normalize.h"

using namespace std;
using namespace Marsyas;


Normalize::Normalize(string name):MarSystem("Normalize",name)
{
  //type_ = "Normalize";
  //name_ = name;
}

Normalize::~Normalize()
{
}

MarSystem* 
Normalize::clone() const 
{
  return new Normalize(*this);
}

void 
Normalize::myProcess(realvec& in, realvec& out)
{
  
  checkFlow(in,out);

  mrs_real rms = 0.0;

  for (t = 0; t < inSamples_; t++)  
    {
      rms = 0.0;

      // calculate 2-norm 
      for (o=0; o < inObservations_; o++)
		{
		  rms += (in(o,t) * in(o,t));
		}
      rms = sqrt(rms);

      // normalize 
      for (o=0; o < inObservations_; o++)
		{
		  out(o,t) = in(o,t) / rms;
		}
 
    }
  
}







	
	

	
