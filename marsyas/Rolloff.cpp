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
    \class Rolloff
    \brief Rolloff of each time slice of observations

    Rolloff computes the rolloff of the observations for each 
    time samle. It is defined as the frequency for which the 
    sum of magnitudes of its lower frequencies are equal to
    percentage of the sum of magnitudes of its higher frequencies. 

*/

#include "Rolloff.h"

using namespace std;
using namespace Marsyas;

Rolloff::Rolloff(string name):MarSystem("Rolloff",name)
{
  perc_ = 0.0;
  sum_ = 0.0;
  total_ = 0.0;
  
  addControls();
}

Rolloff::~Rolloff()
{
}

MarSystem* 
Rolloff::clone() const 
{
  return new Rolloff(*this);
}

void 
Rolloff::addControls()
{
  addctrl("mrs_real/percentage", 0.9);
  setctrlState("mrs_real/percentage", true);
}

void
Rolloff::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Rolloff.cpp - Rolloff:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());
  setctrl("mrs_string/onObsNames", "Rolloff,");
  sumWindow_.create(getctrl("mrs_natural/inObservations")->toNatural());

  perc_ = getctrl("mrs_real/percentage")->toReal();
}

void 
Rolloff::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  

  
  // computer rolloff of observations for each time sample 
  for (t = 0; t < inSamples_; t++)
    {
      sum_ = 0.0;
      sumWindow_.setval(0.0);
      for (o=0; o < inObservations_; o++)
	{
	  sum_ += in(o,t);
	  sumWindow_(o) = sum_;
	}
      total_ = sumWindow_(inObservations_-1);
      for (o=inObservations_-1; o>1; o--)
	{
	  if (sumWindow_(o) < perc_ *total_)
	    {
	      out(0,t) = (mrs_real)o / inObservations_;
	      return;
	    }
	}
      out(0,t) = 1.0;			// default 
    }
}

      
      







	
	
	
