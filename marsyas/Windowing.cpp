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
    \class Windowing
    \brief window the input signal

*/

#include "Windowing.h"

using namespace std;
using namespace Marsyas;

Windowing::Windowing(string name):MarSystem("Windowing",name)
{
 
	addcontrols();
}

Windowing::~Windowing()
{
}

MarSystem* 
Windowing::clone() const 
{
  return new Windowing(*this);
}

void
Windowing::addcontrols()
{
	addctrl("mrs_string/type", "Hamming");
	addctrl("mrs_natural/zeroPhasing", 0);
	addctrl("mrs_natural/size", 0);
}

void
Windowing::myUpdate()
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));  
	mrs_natural inSamples = getctrl("mrs_natural/inSamples")->toNatural();

	mrs_natural size = getctrl("mrs_natural/size")->toNatural();
	if(size)
	{
		setctrl("mrs_natural/onSamples", size);
	}


	tmp_.create(inSamples);
	envelope_.create(inSamples);


	string type = getctrl("mrs_string/type")->toString();
	// should be boolean [!]
	mrs_natural zeroPhase = getctrl("mrs_natural/zeroPhasing")->toNatural();
	if(zeroPhase == 1)
		delta_ = inSamples/2+1;
	else
		delta_=0;

	// default hamming
	mrs_real A = (mrs_real)0.54;
	mrs_real B = (mrs_real)0.46;

	if(type == "Hanning")
	{
		A = (mrs_real)0.5;
		B = (mrs_real)0.5;
	}
	// you can here add other window type for convenience

	mrs_real i;

	for (t=0; t < inSamples; t++)
	{
		i = 2*PI*t / (inSamples-1);
		envelope_(t) = A - B * cos(i);
	}
	norm_ = envelope_.mean();
}

void 
Windowing::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);
	//lmartins: if (mute_) return;
	if(getctrl("mrs_bool/mute")->toBool()) return;

	for (o=0; o < inObservations_; o++)
	{
		for (t = 0; t < inSamples_; t++)
		{
			tmp_(t) =  in(o,t)*envelope_(t); // /(norm_);     
		}
		for (t = 0; t < inSamples_/2; t++)
			out(o,t)=tmp_((t+delta_)%inSamples_);
		for (t = inSamples_/2; t < inSamples_; t++)
			out(o,t+(onSamples_-inSamples_))=tmp_((t+delta_)%inSamples_);
	}

	// MATLAB_PUT(out, "peaks");
	//MATLAB_EVAL("plot(peaks(1,:))");
}








