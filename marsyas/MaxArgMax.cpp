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

#include "MaxArgMax.h"

using namespace std;
using namespace Marsyas;

MaxArgMax::MaxArgMax(string name):MarSystem("MaxArgMax",name)
{
	//type_ = "MaxArgMax";
	//name_ = name;

	addControls();
}


MaxArgMax::~MaxArgMax()
{
}


MarSystem* 
MaxArgMax::clone() const 
{
	return new MaxArgMax(*this);
}


void 
MaxArgMax::addControls()
{
	addctrl("mrs_natural/nMaximums", (mrs_natural)1);
	addctrl("mrs_natural/interpolation", (mrs_natural)0);
	setctrlState("mrs_natural/nMaximums", true);
}


void
MaxArgMax::myUpdate(MarControlPtr sender)
{
	mrs_natural k = getctrl("mrs_natural/nMaximums")->to<mrs_natural>();

	setctrl("mrs_natural/onSamples",  2 * k);
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  
}

void quadraticInterpolation(mrs_real *ix, mrs_real *iy, realvec& data)
{
	mrs_natural index = (mrs_natural) *ix;
    mrs_real d = (data(index-1)-data(index+1))/(2*(-2*data(index)+data(index-1)+data(index+1))); 
	*ix += d;
	*iy -= d*(data(index-1)-data(index+1))/4;
}


void 
MaxArgMax::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);

	//cout << in;

	out.setval(0.0);
	mrs_natural k = getctrl("mrs_natural/nMaximums")->to<mrs_natural>();
	mrs_natural interpolationMode = getctrl("mrs_natural/interpolation")->to<mrs_natural>();


	// ML should replace 0 by o [?]
	for (o=0; o < inObservations_; o++)
	{
		for (t=0; t < inSamples_; t++)
		{
			mrs_real newmax = in(0,t);
			mrs_real newmax_i = t;
			for (ki=0; ki < k; ki++)
			{
				if (newmax > out(0, 2*ki))
				{
					mrs_real oldmax = out(0, 2*ki);
					mrs_real oldmax_i = out(0,2*ki+1);
					out(0,2*ki) = newmax;
					out(0,2*ki+1) = newmax_i;
					newmax = oldmax;
					newmax_i = oldmax_i;
				}
			}
		}
		if(interpolationMode)
			for (ki=0; ki < k; ki++)
			{
				mrs_real ix =  out(0,2*ki+1), iy =  out(0,2*ki);
				quadraticInterpolation(&ix, &iy, in);
				out(0,2*ki) = iy;
				out(0,2*ki+1) = ix;
			}
	}
}












