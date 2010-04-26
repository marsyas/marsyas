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

#include "Delay.h"

using namespace std;
using namespace Marsyas;


Delay::Delay(string name):MarSystem("Delay",name)
{
  
  delay_		= 0;
  writeCursor_	= 0;
  readCursor_	= 0;

  cursorMask_	= 1;
  
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
  addctrl("mrs_real/gain", 0.0);   // direct gain
  addctrl("mrs_real/feedback", 0.0); // feedback gain
  addctrl("mrs_natural/delaySamples", 0); // delay in samples
  addctrl("mrs_real/delaySeconds", 0.0); // delay in seconds
  setctrlState("mrs_real/gain", true);
  setctrlState("mrs_real/feedback", true);
  setctrlState("mrs_real/delaySeconds", true);
  setctrlState("mrs_natural/delaySamples", true);
}


void
Delay::myUpdate(MarControlPtr sender)
{
	(void) sender;
  MRSDIAG("Delay.cpp - Delay:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
 
  // these controls aren't really used ????
  gain_ = getctrl("mrs_real/gain")->to<mrs_real>();
  feedback_ = getctrl("mrs_real/feedback")->to<mrs_real>();

  if(getctrl("mrs_natural/delaySamples")->to<mrs_natural>())
    delay_ = getctrl("mrs_natural/delaySamples")->to<mrs_natural>();
  
	// cout << getctrl("mrs_real/delaySeconds")->to<mrs_real>() << " " << endl;
  if(getctrl("mrs_real/delaySeconds")->to<mrs_real>() != 0.0)
    delay_ = (mrs_natural) ceil(getctrl("mrs_real/delaySeconds")->to<mrs_real>()*getctrl("mrs_real/osrate")->to<mrs_real>());
  
  readCursor_	= 0;
  writeCursor_	= delay_;	// difference between write pointer position and read pointer position should equal delay_
  cursorMask_	= nextPowOfTwo(delay_+1)-1;		// to ensure an efficient wrap around, buffer length will be a power of two

  buffer_.stretch(getctrl("mrs_natural/inObservations")->to<mrs_natural>(), cursorMask_+1);
	buffer_.setval(0);
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}


void 
Delay::myProcess(realvec& in, realvec& out)
{

	for (t = 0; t < inSamples_; t++)
	{
		for (o=0; o < inObservations_; o++)
		{
			// write new sample to buffer
			buffer_(o, writeCursor_)	= in(o,t);

			// read sample from buffer
			out(o,t)	= buffer_(o, readCursor_);
		}
		writeCursor_	= wrapCursor (++writeCursor_);
		readCursor_		= wrapCursor (++readCursor_);
	}

	/* previous code
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
	*/
}

mrs_natural	Delay::wrapCursor (mrs_natural unwrappedCursor)
{
	// add delay line length to have a sort-of dealing with negative indices as well; should be a while loop really
	return (unwrappedCursor + (cursorMask_+1)) & cursorMask_; 
}

mrs_natural	Delay::nextPowOfTwo (mrs_natural value)
{
	mrs_natural    order = 0;

	while (value>>order)
		order++;

	if (!order)
		return value;

	if (!(value%(1<<(order-1))))
		order--;

	order	= (order < 1)? 1 : order;

	return (1<<(order));
}

