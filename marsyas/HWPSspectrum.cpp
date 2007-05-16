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
    \class HWPSspectrum
    \brief Calculate HWPS enhanced spectrum
 
*/

#include "HWPSspectrum.h"
#include "PeUtilities.h"
#include "PeSimilarity.h"

using namespace std;
using namespace Marsyas;

HWPSspectrum::HWPSspectrum(string name):MarSystem("HWPSspectrum", name)
{
  nbParameters_ = nbPkParameters; // f, a, p, df, da, t, g  // should be set as a control [!]
	addControls();
}

HWPSspectrum::HWPSspectrum(const HWPSspectrum& a) : MarSystem(a)
{
}

HWPSspectrum::~HWPSspectrum()
{
}

MarSystem* 
HWPSspectrum::clone() const 
{
  return new HWPSspectrum(*this);
}

void 
HWPSspectrum::addControls()
{
	addctrl("mrs_natural/Sinusoids", 0);
	setctrlState("mrs_natural/Sinusoids", true);

	addctrl("mrs_natural/histSize", 20);
}

void
HWPSspectrum::myUpdate(MarControlPtr sender)
{
	MarSystem::myUpdate(sender);

	kmax_ = getctrl("mrs_natural/Sinusoids")->toNatural();
}

void 
HWPSspectrum::myProcess(realvec& in, realvec& out)
{
	out = in;

	peakFreqs_.stretch(kmax_);
	peakAmps_.stretch(kmax_);

	//get peaks frequencies and amplitudes
	for(mrs_natural i=0; i<kmax_; ++i)
	{
		freq_ = in(i+pkFrequency*kmax_);
		if(freq_ > 0)
		{
			peakFreqs_(i) = freq_;
			peakAmps_(i) = in(i+pkAmplitude*kmax_);
		}
		else
		{
			nbPeaks_ = i;
			break;
		}
	}

	//if there is at least two peaks(or bins), perform HWPS
	//otherwise, out will be a copy of in (see first line of this function)
	if(nbPeaks_ >= 2)
	{
		mrs_natural hsize = getctrl("mrs_natural/histSize")->toNatural();
		
		peakFreqs_.stretch(nbPeaks_);
		peakAmps_.stretch(nbPeaks_);

		x1_.stretch(hsize);
		x2_.stretch(hsize);
		x3_.stretch(hsize);
		x4_.stretch(hsize);

		mrs_real maxDist = MINREAL;
		for(mrs_natural i=0; i < nbPeaks_; ++i)
		{
			out(i+pkVolume*kmax_) = 0;
			for(mrs_natural j=0; j < nbPeaks_; ++j)
			{
				if(j==i) //do not perform HWPS for a peak itself
					continue;

				peakFreqsWrappedi_ = peakFreqs_;
				peakFreqsWrappedj_ = peakFreqs_;
				
				harmonicWrapping(peakFreqs_(i), peakFreqs_(j), peakFreqsWrappedi_, peakFreqsWrappedj_);
				
				cosineDist_ = cosinePeakSets(peakFreqsWrappedi_,peakAmps_, peakFreqsWrappedj_,peakAmps_,
					peakAmps_, peakAmps_, x1_, x2_, x3_, x4_, hsize);

				out(i+pkVolume*kmax_)+= cosineDist_; 
				if(out(i+pkVolume*kmax_) > maxDist)
					maxDist = out(i+pkVolume*kmax_);
			}
		}

		/*
		for(mrs_natural i=0; i < nbPeaks_; ++i)
		{
			out(i+pkAmplitude*kmax_) *= out(i+pkVolume*kmax_)/maxDist;
		}
		*/


		mrs_real dist;
		for(mrs_natural i=0; i < nbPeaks_; ++i)
		{
			dist = out(i+pkVolume*kmax_)/maxDist;
			if(dist < 0.75)
				out(i+pkAmplitude*kmax_) = 0;
		}
	}
	
	//MATLAB_PUT(in,"spect");
	//MATLAB_PUT(out,"HWPSspect");
	//MATLAB_EVAL("testHWPS");
}







	
