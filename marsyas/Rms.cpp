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
    \class Rms
    \brief Rms energy of realvec

   Simple MarSystem example. Calculate the RMS energy of
a realvec
*/



#include "Rms.h"
using namespace std;


Rms::Rms()
{
  type_ = "Rms";
}


Rms::Rms(string name)
{
  type_ = "Rms";
  name_ = name;
  addControls();
  
}

Rms::~Rms()
{
}

MarSystem* 
Rms::clone() const 
{
  return new Rms(*this);
}


void 
Rms::addControls()
{
  addDefaultControls();
}


void 
Rms::update()
{
  setctrl("natural/onSamples", (natural)1);
  setctrl("natural/onObservations", (natural)1);
  setctrl("real/osrate", getctrl("real/israte").toReal() / getctrl("natural/inSamples").toNatural());
  defaultUpdate();
}




void 
Rms::process(realvec& in, realvec& out)
{
  real val;
  real rmsEnergy = 0.0;
  
  
  for (t=0; t < inSamples_; t++)
    {
      val = in(0,t);
      rmsEnergy += (val * val);
    }
  if (rmsEnergy != 0.0) 
    {
      rmsEnergy /= inSamples_;
      rmsEnergy = sqrt(rmsEnergy);
    }

  // SCALING SHOULD BE REMOVED 
  out(0) = (real)(rmsEnergy * 10000000.0);
}






	

	
