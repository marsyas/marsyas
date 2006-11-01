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

#include <algorithm>

using namespace std;
using namespace Marsyas;

PeConvert::PeConvert(string name):MarSystem("PeConvert",name)
{
	//type_ = "PeConvert";
	//name_ = name;

	psize_ = 0;
	size_ = 0;
	nbParameters_ = 7; // f, a, p, df, da, t, g  // should be set as a control [!]
	time_ = 0;

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
	addctrl("mrs_natural/Sinusoids", 1);
	setctrlState("mrs_natural/Sinusoids", true);
}

void
PeConvert::myUpdate()
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
PeConvert::myProcess(realvec& in, realvec& out)
{
	checkFlow(in,out); 

	mrs_natural N2 = inObservations_/4;
	mrs_real a, c;
	mrs_real b, d;
	mrs_real phasediff;

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
			deltafrequency_(t) = frequency_(t)-lastfrequency_(t);
		deltamag_(t) = mag_(t)-lastmag_(t);

		// remove potential peak if frequency too irrelevant
		if(abs(frequency_(t) -t*fundamental_)>.5*fundamental_)
			frequency_(t)=0;

		lastfrequency_(t) = frequency_(t);
		lastmag_(t) = mag_(t);
	}
	// select local maxima
	realvec peaks_=mag_;
	Peaker peaker("Peaker");
	peaker.updctrl("mrs_real/peakStrength", 0.02);
	peaker.updctrl("mrs_natural/peakStart", 0);
	peaker.updctrl("mrs_natural/peakEnd", size_);
	peaker.process(mag_, peaks_);

	/*	#ifdef _MATLAB_ENGINE_
	 MATLAB->putVariable(mag_, "peaks");
	 MATLAB->putVariable(peaks_, "k");
	 MATLAB->evalString("figure(1);clf;plot(peaks);");
	#endif*/

	realvec index_(kmax_*2);
	for(t=0 ; t<N2 ; t++)
	{
		if(!frequency_(t))
			peaks_(t) = 0;
	}

	// keep only the kmax_ highest amplitude local maxima
	MaxArgMax max("MaxArgMax");
	max.updctrl("mrs_natural/nMaximums", kmax_);
	max.updctrl("mrs_natural/inSamples", size_);
	max.updctrl("mrs_natural/inObservations", 1);
	max.update();
	max.process(peaks_, index_);

	nbPeaks_=index_.getSize()/2;

	// fill output with peaks data
	int i ;
	out.setval(0);
	for (i=0;i<nbPeaks_;i++)
	{
		out(i) = frequency_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+kmax_) = magCorr_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+2*kmax_) = -phase_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+3*kmax_) = deltafrequency_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+4*kmax_) = deltamag_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+5*kmax_) = time_;
	}

	time_++;

	// MATLAB_PUT(out, "peaks");
	// MATLAB_PUT(kmax_, "k");
	// MATLAB_EVAL("figure(1);clf;plot(peaks(6*k+1:7*k));");
}






	

	

	
	
      
