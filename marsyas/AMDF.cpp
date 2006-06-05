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
    \class AMDF
    \brief Average Magnitude Difference Function

    Computer the average magnitude difference function 
which can then be used for pitch detection by detecting 
the location of valleys. 
*/


#include "AMDF.h"
using namespace std;



AMDF::AMDF(string name)
{
  type_ = "AMDF";
  name_ = name;
  addControls();
}


AMDF::~AMDF()
{
}


MarSystem* 
AMDF::clone() const 
{
  return new AMDF(*this);
}

void 
AMDF::addControls()
{
  addDefaultControls();
}


void
AMDF::update()
{
  MRSDIAG("AMDF.cpp - AMDF:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));


  
  setctrl("string/onObsNames", getctrl("string/inObsNames"));
  defaultUpdate();  
}


void 
AMDF::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  real gain = getctrl("real/gain").toReal();
  natural i,k;
  real temp;
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) = 0.0;
	for (i=t, k=0; i < inSamples_; i++, k++)
	  {
	    temp = in(o,i) - in(o,k);
	    if (temp < 0) 
	      out(o,t) -= temp;
	    else 
	      out(o,t) += temp;
	  }
      }
  
}







	
	
