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
\class Hamming
\brief Multiply input realvec with gain

Simple MarSystem example. Just multiply the values of the input realvec
with gain and put them in the output vector. 
*/

#include "Hamming.h"

using namespace std;
using namespace Marsyas;

Hamming::Hamming(string name):MarSystem("Hamming",name)
{
	//type_ = "Hamming";
	//name_ = name;
}

Hamming::~Hamming()
{
}

MarSystem* 
Hamming::clone() const 
{
	return new Hamming(*this);
}

void
Hamming::myUpdate(MarControlPtr sender)
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));  
	mrs_natural inSamples = getctrl("mrs_natural/inSamples")->toNatural();
	envelope_.create(inSamples);

	mrs_real A = (mrs_real)0.54;
	mrs_real B = (mrs_real)0.46;
	mrs_real i;
	for (t=0; t < inSamples; t++)
	{
		i = 2*PI*t / (inSamples-1);
		envelope_(t) = A - B * cos(i);
	}
}

void 
Hamming::myProcess(realvec& in, realvec& out)
{

	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_; t++)
		{
			out(o,t) = envelope_(t) * in(o,t);
		}
/*
	MATLAB_PUT(in, "Hamming_in");
	MATLAB_PUT(out, "Hamming_out");
	MATLAB_PUT(envelope_, "Hamming_envelope");
	MATLAB_EVAL("plot(hamming(length(Hamming_envelope)), 'g')");
	MATLAB_EVAL("hold on; plot(Hamming_envelope,'r'); hold off");
*/
}








