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
using namespace Marsyas;


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
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_real/feedback", 0.3);
  addctrl("mrs_real/delay", 0.2);
  setctrlState("mrs_real/gain", true);
  setctrlState("mrs_real/feedback", true);
  setctrlState("mrs_real/delay", true);
}


void
Delay::update()
{
  MRSDIAG("Delay.cpp - Delay:update");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  gain = getctrl("mrs_real/gain").toReal();
  feedback = getctrl("mrs_real/feedback").toReal();
  delay = getctrl("mrs_real/delay").toReal();
  
  bufferSize_ = 22050;
  cursor_ = (mrs_natural)floor(bufferSize_*delay+0.5);
  buffer_.create((mrs_natural)bufferSize_);
  
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
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







	
