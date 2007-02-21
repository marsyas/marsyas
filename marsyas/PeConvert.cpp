/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
\class PeConvert
\brief PeConvert

PeConvert N real and imaginary spectrum values to 
to a fixed numer of peaks.
Peaks have several fields interlieved: frequency, amplitude, phase, vf, va
*/

#include "PeConvert.h"
#include "Peaker.h"
#include "MaxArgMax.h"
#include "PeUtilities.h"

#include <algorithm>

using namespace std;
using namespace Marsyas;

PeConvert::PeConvert(string name):MarSystem("PeConvert",name)
{
	psize_ = 0;
	size_ = 0;
	nbParameters_ = nbPkParameters; // f, a, p, df, da, t, g  // should be set as a control [!]
	time_ = 0;
	skip_=0;

	fundamental_ = 0.0;
	factor_ = 0.0;
	cuttingFrequency_ = 0.0;
	nbPeaks_ = 0;
	kmax_  = 0;


	addControls();

}


PeConvert::~PeConvert()
{
}

MarSystem* 
PeConvert::clone() const 
{
	return new PeConvert(*this);
}


void 
PeConvert::addControls()
{
	addctrl("mrs_natural/Decimation",MRS_DEFAULT_SLICE_NSAMPLES/4);
	setctrlState("mrs_natural/Decimation", true);
	addctrl("mrs_natural/Sinusoids", 1);
	setctrlState("mrs_natural/Sinusoids", true);
	addctrl("mrs_real/cuttingFrequency", 0.0);
	setctrlState("mrs_real/cuttingFrequency", true);
	addctrl("mrs_natural/nbFramesSkipped", 0);
	setctrlState("mrs_natural/nbFramesSkipped", true);
}

void
PeConvert::myUpdate(MarControlPtr sender)
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/Sinusoids")->toNatural()*nbParameters_);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal() * getctrl("mrs_natural/inObservations")->toNatural()/2);  

	//defaultUpdate(); [!]
	inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();

	size_ = inObservations_ /4 +1;

	if (size_ != psize_)
	{
		lastphase_.stretch(size_);
		phase_.stretch(size_);
		mag_.stretch(size_);
		magCorr_.stretch(size_);
		frequency_.stretch(size_);
		lastmag_.stretch(size_);
		lastfrequency_.stretch(size_);
		deltamag_.stretch(size_);
		deltafrequency_.stretch(size_);
	}

	psize_ = size_;

	factor_ = getctrl("mrs_real/osrate")->toReal();
	factor_ /= TWOPI;
	fundamental_ = (mrs_real) (getctrl("mrs_real/osrate")->toReal() / (mrs_real)getctrl("mrs_natural/inObservations")->toNatural()*2);
	kmax_ = getctrl("mrs_natural/Sinusoids")->toNatural();
	skip_ = getctrl("mrs_natural/nbFramesSkipped")->toNatural();
	cuttingFrequency_ = getctrl("mrs_real/cuttingFrequency")->toReal();
}


double lobe_value_compute (double f, int type, int size)
{
	double re ;

	// size par size-2 !!!
	switch (type)
	{
	case 1 :
		{
			re= fabs (0.5*lobe_value_compute(f, 0, size)+
				0.25*lobe_value_compute(f-2.*PI/size, 0, size)+
				0.25*lobe_value_compute(f+2.*PI/size, 0, size))/size ;
			return fabs(re);
		}
	case 0 :
		return (double) (f == 0) ? size : (sin(f*0.5*(size))/sin(f*0.5));

	default :
		{
			//  assert (0) ;
			return 0 ;
		}
	}
}

void
PeConvert::getBinInterval(realvec& interval, realvec& index, realvec& mag)
{
	mrs_natural i, k=0, start=0, nbP = index.getSize();
	mrs_natural minIndex = 0;

	//cout << index;
	// getting rid of padding zeros
	while(start<index.getSize() && !index(start)){start++;}

	for(i=start ; i<nbP ; i++, k++)
	{
		interval(2*k) = index(i)-1;
		interval(2*k+1) = index(i);
	}
	//cout << interval;
}


void
PeConvert::getShortBinInterval(realvec& interval, realvec& index, realvec& mag)
{
	mrs_natural i, j, k=0, start=0, nbP = index.getSize();
	mrs_natural minIndex = 0;

	//cout << index;
	// getting rid of padding zeros
	while(start<index.getSize() && !index(start)){start++;}

	for(i=start ; i<nbP ; i++, k++)
	{
		minIndex = 0;
		// look for the next valley location upward
		for (j= (mrs_natural) index(i) ; j<mag.getSize()-1 ; j++)
		{
			if(mag(j) < mag(j+1))
			{
				minIndex = j;
				break;
			}
		}
		if(!minIndex)
		{
			cout << "pb while looking for bin intervals" << endl;
		}
		interval(2*k+1) = minIndex;
		// look for the next valley location downward
		for (j= (mrs_natural) index(i) ; j>1 ; j--)
		{
			if(mag(j) < mag(j-1))
			{
				minIndex = j;
				break;
			}
		}
		if(!minIndex)
		{
			cout << "pb while looking for bin intervals" << endl;
		}
		interval(2*k) = minIndex;
	}
	//cout << interval;
}


void
PeConvert::getLargeBinInterval(realvec& interval, realvec& index, realvec& mag)
{
	mrs_natural i, j, k=0, start=0, nbP = index.getSize();

	// handling the first case
	mrs_real minVal = HUGE_VAL;
	mrs_natural minIndex = 0;
	//cout << index;
	// getting rid of padding zeros
	while(!index(start)){start++;}

	for (j= 0 ; j<index(start) ; j++)
		if(minVal > mag(j))
		{
			minVal = mag(j);
			minIndex = j;
		}
		if(!minIndex)
		{
			cout << "pb while looking for minimal bin intervals" << endl;
		}
		interval(0) = minIndex;

		for(i=start ; i<nbP-1 ; i++, k++)
		{
			minVal = HUGE_VAL;
			minIndex = 0;
			// look for the minimal value among successive peaks
			for (j= (mrs_natural) index(i) ; j<index(i+1) ; j++)
				if(minVal > mag(j))
				{
					minVal = mag(j);
					minIndex = j;
				}

				if(!minIndex)
				{
					cout << "pb while looking for bin intervals" << endl;
				}
				interval(2*k+1) = minIndex-1;
				interval(2*(k+1)) = minIndex;
		}
		// handling the last case
		minVal = HUGE_VAL;
		minIndex = 0;
		for (j= (mrs_natural) index(nbP-1) ; j<mag.getSize()-1 ; j++)
		{
			if(minVal > mag(j))
			{
				minVal = mag(j);
				minIndex = j;
			}
			// consider stopping the search at the first valley
			if(minVal<mag(j+1))
				break;
		}
		if(!minIndex)
		{
			cout << "pb while looking for maximal bin intervals" << endl;
		}
		interval(2*(k)+1) = minIndex;
	//	cout << interval;
}

void 
PeConvert::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out); 

	mrs_natural N2 = inObservations_/4;
	mrs_real a, c;
	mrs_real b, d;
	mrs_real phasediff;

	if(skip_<=time_)
	{
		// handle amplitudes
		for (t=0; t <= N2; t++)
		{
			if (t==0)
			{
				a = in(2*t,0);
				b = 0.0;
				c = in(2*N2+2*t, 0);
				d = 0.0;
			}
			else if (t == N2)
			{
				a = in(1, 0);
				b = 0.0;
				c = in(2*N2+1, 0);
				d = 0.0;
			}
			else
			{
				a = in(2*t, 0);
				b = in(2*t+1, 0);
				c = in(2*N2+2*t, 0);
				d = in(2*N2+2*t+1, 0);
			}

			// computer magnitude value 
			//mrs_real par = lobe_value_compute (0, 1, 2048);
			// compute phase
			phase_(t) = atan2(b,a);

			// compute precise frequency using the phase difference
			lastphase_(t)= atan2(d,c);
			if(phase_(t) >= lastphase_(t))
				phasediff = phase_(t) - lastphase_(t);
			else
				phasediff = phase_(t) - lastphase_(t)+TWOPI;
			frequency_(t) = phasediff * factor_ ;

			// compute precise amplitude
			mag_(t) = sqrt((a*a + b*b))*2;//*4/0.884624;//*50/3); // [!!!!!!!!!!!]
			mrs_real mag = lobe_value_compute ((t * fundamental_-frequency_(t))/factor_, 1, N2*2);
			magCorr_(t) = mag_(t)/mag;


			// computing precise frequency using the derivative method // use at your own risk	
			/*	mrs_real lastmag = sqrt(c*c + d*d);
			mrs_real rap = (mag_(t)-lastmag)/(lastmag*2);
			f=asin(rap);
			f *= (getctrl("mrs_real/osrate")->toReal())/PI;
			*/
			// rough frequency
			// 	frequency_(t) = t * fundamental_;


			if(lastfrequency_(t) != 0.0)
				deltafrequency_(t) = (frequency_(t)-lastfrequency_(t))/(frequency_(t)+lastfrequency_(t));
			deltamag_(t) = (mag_(t)-lastmag_(t))/(mag_(t)+lastmag_(t));

			// remove potential peak if frequency too irrelevant
			if(abs(frequency_(t) -t*fundamental_)>.5*fundamental_)
				frequency_(t)=0;

			lastfrequency_(t) = frequency_(t);
			lastmag_(t) = mag_(t);
		}
		// select local maxima
		realvec peaks_=mag_;
		Peaker peaker("Peaker");
	//	peaker.updctrl("mrs_real/peakStrength", 0.2);
		// to be set as a control
		peaker.updctrl("mrs_natural/peakStart", (mrs_natural) floor(250/osrate_*size_*2));   // 0
		peaker.updctrl("mrs_natural/peakEnd", (mrs_natural) floor(cuttingFrequency_/osrate_*size_*2));  // size_
		peaker.updctrl("mrs_natural/inSamples", mag_.getCols());
		peaker.updctrl("mrs_natural/inObservations", mag_.getRows());
		peaker.updctrl("mrs_natural/onSamples", peaks_.getCols());
		peaker.updctrl("mrs_natural/onObservations", peaks_.getRows());

		peaker.process(mag_, peaks_);

		
			

		realvec tmp_(kmax_*2);
		for(t=0 ; t<N2 ; t++)
		{
			if(!frequency_(t))// || frequency_(t)>500)  // 250 2500
				peaks_(t) = 0;
		}

		// keep only the kmax_ highest amplitude local maxima
		MaxArgMax max("MaxArgMax");
		max.updctrl("mrs_natural/nMaximums", kmax_);
		max.updctrl("mrs_natural/inSamples", size_);
		max.updctrl("mrs_natural/inObservations", 1);
		max.update();
		max.process(peaks_, tmp_);

		nbPeaks_=tmp_.getSize()/2;
		realvec index_(nbPeaks_);
		mrs_natural i;
		for (i=0 ; i<nbPeaks_ ; i++)
			index_(i) = tmp_(2*i+1);
		realvec index2_ = index_;
		index2_.sort();

		// search for bins interval
		realvec interval_(nbPeaks_*2);
		interval_.setval(0);
		getShortBinInterval(interval_, index2_, mag_);

		// fill output with peaks data

	/*	MATLAB_PUT(mag_, "peaks");
		MATLAB_PUT(peaks_, "k");
		MATLAB_PUT(tmp_, "tmp");
	  MATLAB_PUT(interval_, "int");	
		MATLAB_EVAL("figure(1);clf;hold on ;plot(peaks);stem(k);stem(tmp(2:2:end)+1, peaks(tmp(2:2:end)+1), 'r')");
		MATLAB_EVAL("stem(int+1, peaks(int+1), 'k')");*/
	
    interval_ /= N2*2;
		out.setval(0);
		for (i=0;i<nbPeaks_;i++)
		{
			out(i+pkFrequency*kmax_) = frequency_((mrs_natural) index_(i));

			out(i+pkAmplitude*kmax_) = magCorr_((mrs_natural) index_(i));

			out(i+pkPhase*kmax_) = -phase_((mrs_natural) index_(i));

			out(i+pkDeltaFrequency*kmax_) = deltafrequency_((mrs_natural) index_(i));

			out(i+pkDeltaAmplitude*kmax_) = deltamag_((mrs_natural) index_(i));

			out(i+pkTime*kmax_) = time_;

			out(i+pkGroup*kmax_) = 0;
			out(i+pkVolume*kmax_) = 1;
			out(i+pkPan*kmax_) = 0;

			out(i+pkBinLow*kmax_) = interval_(2*i);
			out(i+pkBinHigh*kmax_) = interval_(2*i+1);
		}
	}
	else
		out.setval(0);

	time_++;
	// MATLAB_PUT(out, "peaks");
	// MATLAB_PUT(kmax_, "k");
	// MATLAB_EVAL("figure(1);clf;plot(peaks(6*k+1:7*k));");
	// cout << out;
}













