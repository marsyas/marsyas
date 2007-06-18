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
	\class NoiseSource
	\ingroup Synthesis
	\brief Creates noise

	Controls:
	- \b mrs_string/mode	: dunno DDD
*/


#include "NoiseSource.h"

using namespace std;
using namespace Marsyas;
	
NoiseSource::NoiseSource(string name):MarSystem("NoiseSource",name)
{
  //type_ = "NoiseSource";
  //name_ = name;
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
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_string/mode", "wavetable");
	setctrlState("mrs_string/mode", true);
}



void
NoiseSource::myUpdate(MarControlPtr sender)
{
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	MarSystem::myUpdate(sender);

	string mode = getctrl("mrs_string/mode")->toString();

	if(mode == "wavetable")
	{
  wavetableSize_ = 8*8192;  // 8192
  wavetable_.create((mrs_natural)wavetableSize_);
  
  for (t=0; t < wavetableSize_; t++)
    // wavetable_(t) = (mrs_real)(0.5 * sin(incr * t));
	// Random Generator
    wavetable_(t) = (mrs_real)(2.0 * rand() / (RAND_MAX + 1.0) );
    index_ = 0;
		mode_=0;
	}
	else
		mode_ = 1;
}

void 
NoiseSource::myProcess(realvec &in, realvec &out)
{
	//checkFlow(in,out);

	mrs_real incr = (440.0 * wavetableSize_) / (getctrl("mrs_real/israte")->toReal());


	mrs_natural inSamples = getctrl("mrs_natural/inSamples")->toNatural();

	if(mode_ == 1)
	{
		for (t=0; t < inSamples; t++)
			out(t) = (mrs_real)(2.0 * rand() / (RAND_MAX + 1.0) );
	}
	else
		for (t=0; t < inSamples; t++)
		{
			out(0,t) = wavetable_((mrs_natural)index_);
			index_ += incr;
			while (index_ >= wavetableSize_)
				index_ -= wavetableSize_;
			while (index_ < 0)
				index_ += wavetableSize_;
		}

}




	
