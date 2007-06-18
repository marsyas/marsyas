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
	\ingroup none
    \brief Rms energy of realvec

   Simple MarSystem example. Calculate the RMS energy of
a realvec
*/

#include "Rms.h"

using namespace std;
using namespace Marsyas;

Rms::Rms(string name):MarSystem("Rms",name)
{
  //type_ = "Rms";
	//name_ = name;  
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
Rms::myUpdate(MarControlPtr sender)
{
  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal() / getctrl("mrs_natural/inSamples")->toNatural());
}

void 
Rms::myProcess(realvec& in, realvec& out)
{
  mrs_real val;
  mrs_real rmsEnergy = 0.0;
  
  
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
  out(0) = (mrs_real)(rmsEnergy * 10000000.0);
}






	

	
