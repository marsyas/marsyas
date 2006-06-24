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
    \class Gain
    \brief Multiply input realvec with gain

   Simple MarSystem example. Just multiply the values of the input realvec
with gain and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

#include "Gain.h"

using namespace std;
using namespace Marsyas;

Gain::Gain(string name)
{
  type_ = "Gain";
  name_ = name;
  addControls();
}


Gain::~Gain()
{
}


MarSystem* 
Gain::clone() const 
{
  return new Gain(*this);
}

void 
Gain::addControls()
{
  addDefaultControls();
  addctrl("mrs_real/gain", 1.0);
}


void
Gain::update()
{
  MRSDIAG("Gain.cpp - Gain:update");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
  defaultUpdate();  
}


void 
Gain::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  mrs_real gain = getctrl("mrs_real/gain").toReal();
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
		out(o,t) =  gain * in(o,t);
      }
}







	
