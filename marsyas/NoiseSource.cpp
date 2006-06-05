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


#include "NoiseSource.h"
using namespace std;

	
NoiseSource::NoiseSource(string name)
{
  type_ = "NoiseSource";
  name_ = name;
  addControls();
}


NoiseSource::~NoiseSource()
{
}


MarSystem* 
NoiseSource::clone() const 
{
  return new NoiseSource(*this);
}


void 
NoiseSource::addControls()
{
  addDefaultControls();
}




void
NoiseSource::update()
{

  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));

  wavetableSize_ = 8*8192;  // 8192
  wavetable_.create((natural)wavetableSize_);
  
  for (t=0; t < wavetableSize_; t++)
    // wavetable_(t) = (real)(0.5 * sin(incr * t));
	// Random Generator
    wavetable_(t) = (real)(2.0 * rand() / (RAND_MAX + 1.0) );
    index_ = 0;
  defaultUpdate();
}




void 
NoiseSource::process(realvec &in, realvec &out)
{
  checkFlow(in,out);
  
  real incr = (440.0 * wavetableSize_) / (getctrl("real/israte").toReal());
  
  
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




	
