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


Delay::Delay(string name):MarSystem("Delay",name)
{
  //type_ = "Delay";
  //name_ = name;
  
	delay_ = 0;
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
  addctrl("mrs_real/gain", 0);   // direct gain
  addctrl("mrs_real/feedback", 0); // feedback gain
  addctrl("mrs_natural/delay", 0); // delay in samples
  setctrlState("mrs_real/gain", true);
  setctrlState("mrs_real/feedback", true);
  setctrlState("mrs_real/delay", true);
}

void
Delay::myUpdate()
{
  MRSDIAG("Delay.cpp - Delay:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  gain_ = getctrl("mrs_real/gain")->toReal();
  feedback_ = getctrl("mrs_real/feedback")->toReal();
  delay_ = getctrl("mrs_natural/delay")->toNatural();
  
  cursor_ = 0;
  buffer_.stretch(delay_);
	buffer_.setval(0);
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}


void 
Delay::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

	if(delay_ < onSamples_)
	{
		for (t = 0; t < delay_ ; t++)
			out(t) = buffer_(t);
		for (t = 0; t < onSamples_-delay_ ; t++)
			out(t+delay_) = in(t);
		for (t = 0; t < delay_ ; t++)
			buffer_(t) = in(t+onSamples_-delay_);
	}
	else
	{
		for (t = 0; t < onSamples_ ; t++)
			out(t) = buffer_(t);
		for (t = 0; t < delay_-onSamples_ ; t++)
			buffer_(t) = buffer_(t+onSamples_);
		for (t = 0; t < onSamples_ ; t++)
			buffer_(t+delay_-onSamples_) = in(t);
	}
}







	
