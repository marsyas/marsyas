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
    \class SineSource
    \brief SineSource generate a sine wave

*/

#include "SineSource.h"
using namespace std;

	
SineSource::SineSource(string name)
{
  type_ = "SineSource";
  name_ = name;
  addControls();
}


SineSource::~SineSource()
{
}


MarSystem* 
SineSource::clone() const 
{
  return new SineSource(*this);
}


void 
SineSource::addControls()
{
  addDefaultControls();
  addctrl("real/frequency", 440.0);
}




void
SineSource::update()
{

  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));

  wavetableSize_ = 8192;
  wavetable_.create((natural)wavetableSize_);
  
  real incr = TWOPI / wavetableSize_;
  for (t=0; t < wavetableSize_; t++)
    wavetable_(t) = (real)(0.5 * sin(incr * t));
  index_ = 0;
  defaultUpdate();
}




void 
SineSource::process(realvec &in, realvec &out)
{
  checkFlow(in,out);

  if (mute_) 
    {
      out.setval(0.0);
      return;
    }
  


  
  real incr = (getctrl("real/frequency").toReal() * wavetableSize_) / (getctrl("real/israte").toReal());

  
  
  
  natural inSamples = getctrl("natural/inSamples").toNatural();
  
  for (t=0; t < inSamples; t++)
    {
      out(0,t) = wavetable_((natural)index_);
      index_ += incr;
      while (index_ >= wavetableSize_)
	index_ -= wavetableSize_;
      while (index_ < 0)
	index_ += wavetableSize_;
    }
}




	
