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
#include "MaxArgMax.h";

#include <algorithm>

using namespace std;
using namespace Marsyas;

#ifdef _MATLAB_ENGINE_
#include "MATLABengine.h"
#endif 

PeConvert::PeConvert(string name):MarSystem("PeConvert",name)
{
  //type_ = "PeConvert";
  //name_ = name;
  
	psize_ = 0;
  size_ = 0;
nbParameters_ = 3;

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
PeConvert::localUpdate()
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/Sinusoids").toNatural()*(nbParameters_+1));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte").toReal() * getctrl("mrs_natural/inObservations").toNatural());  

  //defaultUpdate(); [!]
	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
  
  size_ = inObservations_ /2 +1;
  
  if (size_ != psize_)
    {
      lastphase_.stretch(size_);
      phase_.stretch(size_);
      mag_.stretch(size_);
			frequency_.stretch(size_);
      sortedmags_.stretch(size_);
      sortedpos_.stretch(size_);
    }
  
  psize_ = size_;
  
  factor_ = ((getctrl("mrs_real/osrate").toReal()) / 
	     (mrs_real)( getctrl("mrs_natural/Decimation").toNatural()* TWOPI));
  fundamental_ = (mrs_real) (getctrl("mrs_real/osrate").toReal() / (mrs_real)getctrl("mrs_natural/inObservations").toNatural());
  kmax_ = getctrl("mrs_natural/Sinusoids").toNatural();

}






void 
PeConvert::process(realvec& in, realvec& out)
{

	checkFlow(in,out); 


	mrs_natural N2 = inObservations_/2;
	mrs_real a;
	mrs_real b;
	mrs_real phasediff;

	// handle amplitudes
	for (t=0; t <= N2; t++)
	{
		if (t==0)
		{
			a = in(2*t,0);
			b = 0.0;
		}
		else if (t == N2)
		{
			a = in(1, 0);
			b = 0.0;
		}
		else
		{
			a = in(2*t, 0);
			b = in(2*t+1, 0);
		}

		// computer magnitude value 
		mag_(t) = sqrt(a*a + b*b);
		sortedmags_(t) = mag_(t);
		// compute phase
		phase_(t) = -atan2(b,a);

		// compute frequency
		phasediff = phase_(t) - lastphase_(t);
		lastphase_(t) = phase_(t);	

	
		// phase unwrapping 
		while (phasediff > PI) 
			phasediff -= TWOPI;
		while (phasediff < -PI) 
			phasediff += TWOPI;

		frequency_(t) = phasediff * factor_ + t * fundamental_;

	}
	// select local maxima
	realvec peaks_=mag_;
	Peaker peaker("Peaker");
	peaker.updctrl("mrs_real/peakStrength", 0.1);
	peaker.updctrl("mrs_natural/peakStart", 0);
	peaker.updctrl("mrs_natural/peakEnd", size_);
	peaker.process(mag_, peaks_);

	realvec index_(kmax_*2);

	// keep only the kmax_ highest amplitude local maxima
	MaxArgMax max("MaxArgMax");
	max.updctrl("mrs_natural/nMaximums", kmax_);
	max.updctrl("mrs_natural/inSamples", size_);
	max.updctrl("mrs_natural/inObservations", 1);
	max.update();
  max.process(peaks_, index_);

#ifdef _MATLAB_ENGINE_
  MATLAB->putVariable(peaks_, "mag");
	MATLAB->putVariable(index_, "peaks");
	MATLAB->evalString("figure(1);plot(mag)");
	MATLAB->evalString("figure(2);plot(peaks(1:2:end))");
#endif

	// fill output with peaks data
	int i ;
	for (i=0;i<nbPeaks_;i++)
	{
		out(i) = frequency_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+kmax_) = mag_(index_(2*i+1));
	}
	for (i=0;i<nbPeaks_;i++)
	{
		out(i+2*kmax_) = phase_(index_(2*i+1));
	}
}






	

	

	
	
      
