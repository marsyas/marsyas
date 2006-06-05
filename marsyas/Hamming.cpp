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
    \class Hamming
    \brief Multiply input realvec with gain

   Simple MarSystem example. Just multiply the values of the input realvec
with gain and put them in the output vector. 
*/



#include "Hamming.h"
using namespace std;

Hamming::Hamming()
{
  type_ = "Hamming";
}


Hamming::Hamming(string name)
{
  type_ = "Hamming";
  name_ = name;
  addControls();
}


Hamming::~Hamming()
{
}

MarSystem* 
Hamming::clone() const 
{
  return new Hamming(*this);
}

void 
Hamming::addControls()
{
  addDefaultControls();
}


void
Hamming::update()
{
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  setctrl("string/onObsNames", getctrl("string/inObsNames"));  
  natural inSamples = getctrl("natural/inSamples").toNatural();
  envelope_.create(inSamples);
  
  real A = (real)0.54;
  real B = (real)0.46;
  real i;
  for (t=0; t < inSamples; t++)
    {
      i = 2*PI*t / (inSamples-1);
      envelope_(t) = A - B * cos(i);
    }
  defaultUpdate();
}




void 
Hamming::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  if (mute_) return;
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) = envelope_(t) * in(o,t);
      }
  
}






	
	
