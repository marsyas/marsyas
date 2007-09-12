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

	srate_ = 0.0;
	pnumbins_ = 0;
}

Spectrum2Chroma::Spectrum2Chroma(const Spectrum2Chroma& a) : MarSystem(a)
{
	ctrl_numBins_ = getctrl("mrs_natural/numBins");
	ctrl_weightCenterFreq_ = getctrl("mrs_real/weightCenterFreq");
	ctrl_weightStdDev_ = getctrl("mrs_real/weightStdDev");

	srate_ = a.srate_;
	pnumbins_ = a.pnumbins_;
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
	addctrl("mrs_natural/numBins", 12, ctrl_numBins_);
	addctrl("mrs_real/weightCenterFreq", 1000.0, ctrl_weightCenterFreq_);
	addctrl("mrs_real/weightStdDev", 1.0, ctrl_weightStdDev_);

	ctrl_numBins_->setState(true);
	ctrl_weightCenterFreq_->setState(true);
	ctrl_weightStdDev_->setState(true);
}

void
Spectrum2Chroma::myUpdate(MarControlPtr sender)
{
	(void) sender;

	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_numBins_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_);
	
	if (pnumbins_ != ctrl_numBins_->to<mrs_natural>())
	{
		pnumbins_ = ctrl_numBins_->to<mrs_natural>();
		ostringstream oss;
		for (mrs_natural n=0; n < pnumbins_; n++)
		{
			oss << "ChromaBin_" << n << ",";
		}
		ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
	}

	//get the original audio sampling rate
	srate_ = ctrl_israte_->to<mrs_real>()*ctrl_inObservations_->to<mrs_natural>()*2;

	//calculate the Chroma map
	chromaMap_.create(ctrl_numBins_->to<mrs_natural>(), ctrl_inObservations_->to<mrs_natural>());
	//
	//
	//
	//
	// TODO!!!!!! [!]
	//
	//
	//
	//
	//
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


