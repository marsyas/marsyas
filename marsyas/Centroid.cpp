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
    \class Centroid
    \brief Centroid of each time slice of observations

    Centroid computes the centroid of the observations for each 
time samle. The center is defined as the normalized first moment 
(center of gravity) of the observation vector. 
*/

#include "Centroid.h"

using namespace std;
using namespace Marsyas;

Centroid::Centroid():MarSystem()
{
  type_ = "Centroid";
}

Centroid::Centroid(string name)
{
  type_ = "Centroid";
  name_ = name;
  addControls();
}


Centroid::~Centroid()
{
}


MarSystem* 
Centroid::clone() const 
{
  return new Centroid(*this);
}

void 
Centroid::addControls()
{
  addDefaultControls();
}


void
Centroid::update()
{
  MRSDIAG("Centroid.cpp - Centroid:update");
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte").toReal());
  setctrl("mrs_string/onObsNames", "Centroid,");
  
  defaultUpdate();
}


void 
Centroid::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  // computer centroid of observations for each time sample 
  // using zero and first-order moments 
  for (t = 0; t < inSamples_; t++)
    {
      m0_ = 0.0;
      m1_ = 0.0;
      for (o=0; o < inObservations_; o++)
	{
	  m1_ += o * in(o,t);
	  m0_ += in(o,t);
	}
      if (m0_ != 0.0) 
	out(0,t) = (m1_ / m0_) / inObservations_;
      else 
	out(0,t) = 0.5;
    }
  
}

      
      







	
	
