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

#include "SineSource.h"

using namespace std;
using namespace Marsyas;
	
SineSource::SineSource(mrs_string name):MarSystem("SineSource",name)
{
	//type_ = "SineSource";
	//name_ = name;
	
	addControls();
	index_ = 0;
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
    addctrl("mrs_real/frequency", 440.0);
}

void
SineSource::myUpdate(MarControlPtr sender)
{

//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	MarSystem::myUpdate(sender);

	wavetableSize_ = 8192;
	wavetable_.create((mrs_natural)wavetableSize_);
	
	mrs_real incr = TWOPI / wavetableSize_;
	for (mrs_natural t=0; t < wavetableSize_; t++)
		wavetable_(t) = (mrs_real)(0.5 * sin(incr * t));
	
}

void 
SineSource::myProcess(realvec &in, realvec &out)
{
	(void) in;
	//checkFlow(in,out);
	
    //lmartins: if (mute_)
	if(getctrl("mrs_bool/mute")->to<mrs_bool>())
	{
		out.setval(0.0);
		return;
	}
	
	mrs_real incr = (getctrl("mrs_real/frequency")->to<mrs_real>() * wavetableSize_) / (getctrl("mrs_real/israte")->to<mrs_real>());
	mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	
	
	for (mrs_natural t=0; t < inSamples; t++)
	{
		out(0,t) = wavetable_((mrs_natural)index_);
		index_ += incr;
		while (index_ >= wavetableSize_)
			index_ -= wavetableSize_;
		while (index_ < 0)
			index_ += wavetableSize_;
	}
}




	
