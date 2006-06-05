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
    \class Delay
    \brief Delay

   Simple Delay example
*/



#include "Delay.h"
using namespace std;



Delay::Delay(string name)
{
  type_ = "Delay";
  name_ = name;
  delay_ = 0.0;
  cursor_ = 0;
  addControls();
}


Delay::~Delay()
{
}


MarSystem* 
Delay::clone() const 
{
  return new Delay(*this);
}

void 
Delay::addControls()
{
  addDefaultControls();
  addctrl("real/gain", 1.0);
  addctrl("real/feedback", 0.3);
  addctrl("real/delay", 0.2);
  setctrlState("real/gain", true);
  setctrlState("real/feedback", true);
  setctrlState("real/delay", true);
}


void
Delay::update()
{
  MRSDIAG("Delay.cpp - Delay:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  gain = getctrl("real/gain").toReal();
  feedback = getctrl("real/feedback").toReal();
  delay = getctrl("real/delay").toReal();
  
  bufferSize_ = 22050;
  cursor_ = (natural)floor(bufferSize_*delay+0.5);
  buffer_.create((natural)bufferSize_);
  
  setctrl("string/onObsNames", getctrl("string/inObsNames"));
  defaultUpdate();  
}


void 
Delay::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	if (cursor_ >= bufferSize_)
	  cursor_ = 0; 
	delay_ = buffer_(cursor_);
	buffer_(cursor_++) = in(o,t) + delay_*feedback;
	out(o,t) =  gain*delay_;
      }
}







	
