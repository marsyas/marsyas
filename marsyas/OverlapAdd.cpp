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
    \class OverlapAdd
    \brief OverlapAdd

   perform an overlap-add operation of the input buffer
*/

#include "OverlapAdd.h"

using namespace std;
using namespace Marsyas;

OverlapAdd::OverlapAdd(string name):MarSystem("OverlapAdd",name)
{
  //type_ = "OverlapAdd";
  //name_ = name;

}


OverlapAdd::~OverlapAdd()
{
}

MarSystem* 
OverlapAdd::clone() const 
{
  return new OverlapAdd(*this);
}


void
OverlapAdd::myUpdate(MarControlPtr sender)
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples")->toNatural()/2);
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->toNatural());
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());    

	mrs_natural N;
	N = getctrl("mrs_natural/onSamples")->toNatural();

	back_.stretch(N);
	back_.setval(0);
	// create synthesis window 
	win_.stretch(N*2);

	for (t=0; t < win_.getSize(); t++)
    {
			mrs_real i = 2*PI*t / (win_.getSize());
      win_(t) = .5 - .5 * cos(i);
    }
}

void 
OverlapAdd::myProcess(realvec& in, realvec& out)
{
	mrs_natural N;
	int i;

	N= out.getSize();

	in*=win_;

	for(i=0;i<N;i++)
		out(i) = back_(i)+in(i);

	//MATLAB_PUT(tmp_, "vec");
	//MATLAB_PUT(back_, "vec1");
	//MATLAB_PUT(out, "vec2");
	//MATLAB_EVAL("figure(1);clf;plot(vec1, 'r'); hold ; plot(vec) ; hold");

	for(i=0;i<N;i++)
		back_(i) = in(i+N);
}






	

	

	

	
