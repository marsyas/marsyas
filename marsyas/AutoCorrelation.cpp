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

#include "AutoCorrelation.h"
#include "Windowing.h"

using namespace std;
using namespace Marsyas;

AutoCorrelation::AutoCorrelation(string name):MarSystem("AutoCorrelation",name)
{
	myfft_ = NULL;

	addControls();
}


AutoCorrelation::~AutoCorrelation()
{
	delete myfft_;
}

// copy constructor 
AutoCorrelation::AutoCorrelation(const AutoCorrelation& a):MarSystem(a)
{
	// 	type_ = a.type_;
	// 	name_ = a.name_;
	// 	ncontrols_ = a.ncontrols_; 		
	// 
	// 	inSamples_ = a.inSamples_;
	// 	inObservations_ = a.inObservations_;
	// 	onSamples_ = a.onSamples_;
	// 	onObservations_ = a.onObservations_;
	// 	dbg_ = a.dbg_;
	// 	mute_ = a.mute_;
	myfft_ = NULL;
}

void
AutoCorrelation::addControls()
{
	addctrl("mrs_real/magcompress", 2.0);
	addctrl("mrs_natural/normalize", 0);
	addctrl("mrs_real/octaveCost", 0.0);
	addctrl("mrs_real/voicingThreshold", 0.1);
	setctrlState("mrs_natural/normalize", true);
	setctrlState("mrs_real/octaveCost", true);
	setctrlState("mrs_real/voicingThreshold", true);
	//delete myfft_; //[?]
	//myfft_ = new fft();//[?]
}

MarSystem*
AutoCorrelation::clone() const
{
	return new AutoCorrelation(*this);
}

void
AutoCorrelation::myUpdate(MarControlPtr sender)
{
	delete myfft_; //[!]
	myfft_ = new fft();//[!]

	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  

	scratch_.create(2*getctrl("mrs_natural/onSamples")->toNatural());

	// only working for hanning window
	normalize_ = 0;
	if(getctrl("mrs_natural/normalize")->toNatural())
	{
		realvec tmp(getctrl("mrs_natural/onSamples")->toNatural());
		normalize_ = 1;
		norm_.create(getctrl("mrs_natural/onSamples")->toNatural());
		norm_.setval(1);
		Windowing win("Windowing");
		win.updctrl("mrs_string/type", "Hanning");
		win.updctrl("mrs_natural/inSamples", norm_.getCols());
		win.updctrl("mrs_natural/inObservations", norm_.getRows());
		win.process(norm_, tmp);

		AutoCorrelation autocorr("Autocorrelation");
		autocorr.updctrl("mrs_natural/inSamples", norm_.getCols());
		autocorr.updctrl("mrs_natural/inObservations", norm_.getRows());
		autocorr.update();
		autocorr.process(tmp, norm_);
		
	
		for (mrs_natural i = 0 ; i < norm_.getSize() ; i++)
			norm_(i) = 1/norm_(i);
	}


	octaveCost_ = getctrl("mrs_real/octaveCost")->toReal();
	voicing_ = getctrl("mrs_real/voicingThreshold")->toReal();

	if(octaveCost_)
	{
		octaveCost_ *= octaveCost_;
		octaveMax_ = octaveCost_*log(36.0*inSamples_);
	}
}

void 
AutoCorrelation::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);
	mrs_real re,im,am;
	mrs_real k;
	k = getctrl("mrs_real/magcompress")->toReal();

	// Copy to output to perform inplace fft and zeropad to double size

	for (o=0; o < inObservations_; o++)
	{
		for (t=0; t < inSamples_; t++)
			scratch_(t) = in(o,t); 

		mrs_real *tmp = scratch_.getData();
		myfft_->rfft(tmp, inSamples_/2, FFT_FORWARD);

		// Special case for zero and Nyquist/2 only real part 
		if (k == 2.0)
		{
			re = tmp[0];
			tmp[0] = re * re;
			re = tmp[1];
			tmp[1] = re * re;
		}
		else 
		{
			re = tmp[0];
			re = sqrt(re * re);
			tmp[0] = pow(re, k);
			re = tmp[1];
			re = sqrt(re * re);
			tmp[1] = pow(re, k);
		}

		// Compress the magnitude spectrum and zero 
		// the imaginary part.   
		for (t=1; t < inSamples_/2; t++)
		{
			re = tmp[2*t];
			im = tmp[2*t+1];
			if (k == 2.0)
				am = re * re + im * im;
			else
			{
				am = sqrt(re * re + im * im);
				am = pow(am, k);
			}
			tmp[2*t] = am;
			tmp[2*t+1] = 0;
		}

		// Take the inverse Fourier Transform 
		myfft_->rfft(tmp, inSamples_/2, FFT_INVERSE);

		// Copy to output 
		if(normalize_) 
		{
			for (t=0; t < inSamples_; t++) 
			{
				out(o,t) = scratch_(t)*norm_(t);
			}
		}

		else
			for (t=0; t < inSamples_; t++)  
				out(o,t) = scratch_(t);
	}

	for (o=0; o < inObservations_; o++)
	{

		if(octaveCost_)
		{
			mrs_real maxOut = 0;
			for (t=1 ; t<inSamples_/2 ; t++)
				if (out(o, t)> out(o, t+1) && out(o, t) > out(o, t-1) && out(o, t)>maxOut)
					maxOut = out(o, t) ;
			//	cout << maxOut/out(o, 0)<< " " << 1+voicing_ << << endl;

			if(maxOut && maxOut/out(o, 0) > 1-voicing_)
				for (t=1; t < inSamples_; t++) 
					out(o, t) += octaveMax_-octaveCost_*log(36.0*t);
			else
				out.setval(0);

		}
	}

	/*MATLAB_PUT(out, "corr");
	MATLAB_EVAL("plot(corr)");*/
}









