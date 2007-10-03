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

#include "PeakSynthOsc.h"
#include "peakView.h"

using namespace std;
using namespace Marsyas;

PeakSynthOsc::PeakSynthOsc(string name):MarSystem("PeakSynthOsc",name)
{
	addControls();
}

PeakSynthOsc::PeakSynthOsc(const PeakSynthOsc& a):MarSystem(a)
{
	ctrl_harmonize_ = getctrl("mrs_realvec/harmonize");
}


PeakSynthOsc::~PeakSynthOsc()
{
}

MarSystem* 
PeakSynthOsc::clone() const 
{
	return new PeakSynthOsc(*this);
}

void 
PeakSynthOsc::addControls()
{
	addctrl("mrs_natural/synSize", MRS_DEFAULT_SLICE_NSAMPLES);
	addctrl("mrs_natural/delay", MRS_DEFAULT_SLICE_NSAMPLES /2);
	//addctrl("mrs_natural/nbSinusoids", 0);
	addctrl("mrs_realvec/harmonize", realvec(), ctrl_harmonize_);
}

void
PeakSynthOsc::myUpdate(MarControlPtr sender)
{
	(void) sender;
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/synSize"));
	setctrl("mrs_natural/onObservations", (mrs_natural)1);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>()); //HACK!! [!] - see PeakConvert::myUpdate()

	delay_ = getctrl("mrs_natural/delay")->to<mrs_natural>();

	mrs_real fs = getctrl("mrs_real/osrate")->to<mrs_real>();
	factor_ = TWOPI/fs;
}

void 
PeakSynthOsc::sine(realvec& out, mrs_real f, mrs_real a, mrs_real p)
{
	int i;
	int N = out.getSize();
	if(f > 0.0 && a)
	{
		for (i=0 ; i<N ; i++)
			out(i) += a*cos(factor_*f*(i-delay_)+p); // consider -fftSize/2 for synth in phase
		//	 cout << f << " " << a << " " << p << endl;
	}
}

void 
PeakSynthOsc::myProcess(realvec& in, realvec& out)
{
	mrs_natural N, Nb, nbH;
	int i;

	Nb = in.getSize()/peakView::nbPkParameters ; //getctrl("mrs_natural/nbSinusoids")->to<mrs_natural>();
	N= out.getSize();

	out.setval(0);
	//cout << in;
	nbH = ctrl_harmonize_->to<mrs_realvec>().getSize();
	if(nbH)
		for(mrs_natural j=0 ; j<(nbH-1)/2 ; j++)
		{
			mrs_real mulF = ctrl_harmonize_->to<mrs_realvec>()(1+j*2); 
			mrs_real mulA = ctrl_harmonize_->to<mrs_realvec>()(2+j*2);
			//	cout << "mulF" << mulF << "mulA" << mulA << endl;
			for (i=0; i < Nb; i++)
			{
				if(in(i+peakView::pkGroup*Nb) > -1)
					sine(out, in(i)*mulF, in(i+Nb)*mulA, in(i+2*Nb));
			}
		}
	else
		for (i=0; i < Nb; i++)
		{
			if(in(i+peakView::pkGroup*Nb) > -1)
				sine(out, in(i), in(i+Nb), in(i+2*Nb));
			//		       	cout << "truc" << in(i) << " " << in(i+Nb) << "  " << in(i+2*Nb) << endl;
		}
}













