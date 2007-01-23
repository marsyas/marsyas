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
    \class PeOverlapadd
    \brief PeOverlapadd

    Multiply with window (both length Nw) using modulus arithmetic;
fold and rotate windowed input into output array (FFT) (length N) 
according to current input time (t)
*/

#include "PeOverlapadd.h"

using namespace std;
using namespace Marsyas;

PeOverlapadd::PeOverlapadd(string name):MarSystem("PeOverlapadd",name)
{
  //type_ = "PeOverlapadd";
  //name_ = name;

	addControls();
}


PeOverlapadd::~PeOverlapadd()
{
}

MarSystem* 
PeOverlapadd::clone() const 
{
  return new PeOverlapadd(*this);
}


void 
PeOverlapadd::addControls()
{
  addctrl("mrs_natural/hopSize", MRS_DEFAULT_SLICE_NSAMPLES /2);
	addctrl("mrs_natural/delay", MRS_DEFAULT_SLICE_NSAMPLES /2);
	addctrl("mrs_natural/nbSinusoids", 0);
}

void
PeOverlapadd::myUpdate(MarControlPtr sender)
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/hopSize"));
	setctrl("mrs_natural/onObservations", (mrs_natural)1);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());    

	mrs_natural N;
	N = getctrl("mrs_natural/onSamples")->toNatural();
	delay_ = getctrl("mrs_natural/delay")->toNatural();

	mrs_real fs = getctrl("mrs_real/osrate")->toReal();
	factor_ = TWOPI/fs;

	tmp_.stretch(N*2);
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
PeOverlapadd::sine(realvec& out, mrs_real f, mrs_real a, mrs_real p)
{
	int i;
	int N2 = out.getSize()/2;
	if(f > 0.0)
	{
		for (i=0 ; i<2*N2 ; i++)
			out(i) += a*cos(factor_*f*(i-delay_)+p); // consider -fftSize/2 for synth in phase
//	 cout << f << " " << a << " " << p << endl;
	}
	}

void 
PeOverlapadd::myProcess(realvec& in, realvec& out)
{
	mrs_natural N, Nb;
	int i;

	Nb = getctrl("mrs_natural/nbSinusoids")->toNatural();
	N= out.getSize();

	tmp_.setval(0);
//cout << in;
	for (i=0; i < Nb; i++)
	{
		if(in(i+6*Nb) > -1)
		sine(tmp_, in(i), in(i+Nb), in(i+2*Nb));
	/*	else
			cout << "truc" << endl;*/
	}
//	tmp_.setval(1);
	tmp_*=win_;

	for(i=0;i<N;i++)
		out(i) = back_(i)+tmp_(i);

	//MATLAB_PUT(tmp_, "vec");
	//MATLAB_PUT(back_, "vec1");
	//MATLAB_PUT(out, "vec2");
	//MATLAB_EVAL("figure(1);clf;plot(vec1, 'r'); hold ; plot(vec) ; hold");

	for(i=0;i<N;i++)
		back_(i) = tmp_(i+N);

}






	

	

	

	
