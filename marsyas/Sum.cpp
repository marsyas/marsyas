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
    \class Sum
    \brief Sum the input rows of observations into one row 

    Sum the input rows of observation into one row. Frequently 
used for mixing audio signals before SoudFileSink or AudioSink. 
*/

#include "Sum.h"

using namespace std;
using namespace Marsyas;

Sum::Sum(string name):MarSystem("Sum",name)
{
}


Sum::~Sum()
{
}


MarSystem* 
Sum::clone() const 
{
  return new Sum(*this);
}

void
Sum::localUpdate()
{
  MRSDIAG("Sum.cpp - Sum:localUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (MarControlValue)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void 
Sum::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  out.setval(0.0);
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
		out(0,t) += in(o,t);
      }
}







	
	
