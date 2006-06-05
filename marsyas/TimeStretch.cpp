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
    \class TimeStretch
    \brief TimeStretch input realvec by factor 


*/




#include "TimeStretch.h"
using namespace std;


TimeStretch::TimeStretch(string name)
{
  type_ = "TimeStretch";
  name_ = name;
  addControls();
}



TimeStretch::~TimeStretch()
{
}

MarSystem* 
TimeStretch::clone() const 
{
  return new TimeStretch(*this);
}


void 
TimeStretch::addControls()
{
  addDefaultControls();
  addctrl("real/factor", 1.0);
}



void
TimeStretch::update()
{
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));  
  defaultUpdate();
}



void 
TimeStretch::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  real factor = getctrl("real/factor").toReal();
  natural inSamples = getctrl("natural/inSamples").toNatural();
  
  for (t=0; t < inSamples; t++)
    {
      // linear interpolation 
      ni = t* factor;
      li = ((unsigned long)ni) % inSamples;
      ri = li + 1;
      
      w_ = ni - li;
      out(0,t) = in(li) + w_ * (in(0,ri) - in(0,li));
    }
}






	

	
