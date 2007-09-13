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

#include "Spectrum2Chroma.h"

using namespace std;
using namespace Marsyas;

Spectrum2Chroma::Spectrum2Chroma(string name):MarSystem("Spectrum2Chroma", name)
{
	addControls();

	pnbins_ = 0;
	pmiddleAfreq_ = 0.0;
	pweightCenterFreq_ = 0.0;
	pweightStdDev_ = 0.0;
}

Spectrum2Chroma::Spectrum2Chroma(const Spectrum2Chroma& a) : MarSystem(a)
{
	ctrl_nbins_ = getctrl("mrs_natural/nbins");
	ctrl_middleAfreq_ = getctrl("mrs_real/middleAfreq");
	ctrl_weightCenterFreq_ = getctrl("mrs_real/weightCenterFreq");
	ctrl_weightStdDev_ = getctrl("mrs_real/weightStdDev");

	chromaMap_ = a.chromaMap_;
	pnbins_ = a.pnbins_;
	pmiddleAfreq_ = a.pmiddleAfreq_;
	pweightCenterFreq_ = a.pweightCenterFreq_;
	pweightStdDev_ = a.pweightStdDev_;
}

Spectrum2Chroma::~Spectrum2Chroma()
{
}

MarSystem*
Spectrum2Chroma::clone() const
{
	return new Spectrum2Chroma(*this);
}

void
Spectrum2Chroma::addControls()
{
	addctrl("mrs_natural/nbins", 12, ctrl_nbins_); //diatonic chromatic scale by default
	addctrl("mrs_real/middleAfreq", 440.0, ctrl_middleAfreq_);
	mrs_real A0freq = 440.0/pow(2.0, 4.0);
	addctrl("mrs_real/weightCenterFreq", log(1000.0/A0freq)/log(2.0), ctrl_weightCenterFreq_);
	addctrl("mrs_real/weightStdDev", 1.0, ctrl_weightStdDev_);

	ctrl_nbins_->setState(true);
	ctrl_middleAfreq_->setState(true);
	ctrl_weightCenterFreq_->setState(true);
	ctrl_weightStdDev_->setState(true);
}

void
Spectrum2Chroma::myUpdate(MarControlPtr sender)
{
	(void) sender;

	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_nbins_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_);
	
	if (pnbins_ != ctrl_nbins_->to<mrs_natural>())
	{
		pnbins_ = ctrl_nbins_->to<mrs_natural>();
		ostringstream oss;
		for (mrs_natural n=0; n < pnbins_; n++)
		{
			oss << "ChromaBin_" << n << ",";
		}
		ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// calculate the Chroma map
	// based in the fft2chromamx.m MATLAB script by Dan Ellis
	// http://www.ee.columbia.edu/~dpwe/resources/matlab/chroma-ansyn/#1
	//////////////////////////////////////////////////////////////////////////////////////////////
	if(tinObservations_ != inObservations_ || 
		tonObservations_ != onObservations_ ||
		tisrate_ != israte_ || 
		pmiddleAfreq_ != ctrl_middleAfreq_->to<mrs_real>() ||
		pweightCenterFreq_ != ctrl_weightCenterFreq_->to<mrs_real>() ||
		pweightStdDev_ != ctrl_weightStdDev_->to<mrs_real>() )
	{
		pmiddleAfreq_ = ctrl_middleAfreq_->to<mrs_real>();
		pweightCenterFreq_ = ctrl_weightCenterFreq_->to<mrs_real>();
		pweightStdDev_ = ctrl_weightStdDev_->to<mrs_real>();
				
		mrs_natural nbins = ctrl_nbins_->to<mrs_natural>();
		mrs_natural nbins2 = (mrs_natural)floor(nbins/2.0+0.5); //equivalent to round()
		mrs_natural N2 = inObservations_;
		mrs_natural N = N2*2;

		//get the original audio sampling rate
		//NOTE: this assumes that the input only has the magnitudes for the first N/2 spectrum points
		//(and not the usual N/2+1 spectrum points) -> see PowerSpectrum.cpp
		mrs_real srate = israte_*N;

		//calculate the frequencies (in octaves) for each spectrum bin
		//Note: make up value for 0Hz bin -> 1.5 octaves below bin 1
		//(so chroma is 50% rotated from bin 1, and bin width is broad)
		realvec fftfreqbins(N2);
		for(o=1; o < N2; ++o)
		{
			fftfreqbins(o) = nbins * hertz2octs(o / N * srate, ctrl_middleAfreq_->to<mrs_real>());
		}
		fftfreqbins(0) = fftfreqbins(1)-1.5*nbins;

		//calculate the bin widths
		realvec binwidthbins(N2);
		for(o=0; o < N2-1; ++o)
		{
			binwidthbins(o) = fftfreqbins(o+1)-fftfreqbins(o);
			if(binwidthbins(o) < 1.0)
				binwidthbins(o) = 1.0;
		}
		binwidthbins(N2-1) = 1.0;

		//calculate chroma mapping
		chromaMap_.create(nbins, N2); //=wts in Dan Ellis MATLAB code
		realvec D(nbins, N2);
		for(o = 0; o < nbins; ++o)
		{
			for(t = 0; t < N2; ++t)
			{
				D(o,t) = fftfreqbins(t) - o;

				//project into range -nbins/2 .. nbins/2
				//(add on fixed offset of 10*nbins to ensure all values are positive
				//for the fmod remainder operation)
				D(o,t) = fmod(D(o,t) + nbins2 + 10*nbins, nbins) - nbins2;

				//Gaussian bumps (2*D(o,t) to make them narrower)
				chromaMap_(o,t) = exp(-0.5 * pow(2.0*D(o,t)/binwidthbins(t), 2.0) );
			}
		}
		//normalize each column by its RMS value
		mrs_real colRMS;
		for(t = 0; t < N2; ++t) //iterate over columns
		{
			colRMS = 0.0;
			//get RMS value for column t
			for(o = 0; o < nbins; ++o) //iterate over rows
			{
				colRMS += sqrt(chromaMap_(o,t)*chromaMap_(o,t));
			}
			//normalize column t
			if(colRMS != 0.0)
			{
				for(o = 0; o < nbins; ++o) //row
				{
					chromaMap_(o,t) = chromaMap_(o,t)/colRMS;
				}
			}
		}
		//Maybe apply scaling for fft bins
		mrs_real ctroct = ctrl_weightCenterFreq_->to<mrs_real>();
		mrs_real octwidth = ctrl_weightStdDev_->to<mrs_real>();
		if(octwidth > 0.0)
		{
			for(o = 0; o < nbins; ++o)
			{
				for(t = 0; t < N2; ++t)
				{
					chromaMap_(o,t) = exp(-0.5 * pow( (fftfreqbins(t)/nbins - ctroct)/octwidth , 2.0) );
				}
			}
		}
	}
}

void
Spectrum2Chroma::myProcess(realvec& in, realvec& out)
{
	//input must contain spectral magnitude/power/density/etc
	//(e.g. output of PowerSpectrum MarSystem)

	out.setval(0.0);
	for(t=0; t< inSamples_; ++t)
	{
		for(o=0; o< onObservations_; ++o)
		{
			for(mrs_natural i=0; i< inObservations_; ++i)
			{
				out(o,t)+= in(i,t)*chromaMap_(o,i);
			}
		}
	}
}


