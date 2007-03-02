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
    \class WaveletStep
    \brief One step of Discrete Wavelet Transform (DWT)

   One step of the Discrete Wavelet Transform (DWT) 
pyramid algorithm. Called by WaveletPyramid. The code is 
is based on the Numerical Recipies wavelet code. 
*/

#include "WaveletStep.h"

using namespace std;
using namespace Marsyas;

WaveletStep::WaveletStep(string name):MarSystem("WaveletStep",name)
{
  //type_ = "WaveletStep";
  //name_ = name;
}


WaveletStep::~WaveletStep()
{
}

MarSystem* 
WaveletStep::clone() const 
{
  return new WaveletStep(*this);
}

void 
WaveletStep::myProcess(realvec& in, realvec& out) 
{
  //checkFlow(in,out);
  
  // copy to output 
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
      }
  
}


	
