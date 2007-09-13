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

#include "PowerSpectrum.h"

using namespace std;
using namespace Marsyas;

#define PSD_POWER 1
#define PSD_MAG 2
#define PSD_DB  3
#define PSD_PD  4 

PowerSpectrum::PowerSpectrum(string name):MarSystem("PowerSpectrum",name)
{
	ntype_ = PSD_POWER;
	N2_ = 0;
	re_ = 0.0;
	im_ = 0.0;
	dB_ = 0.0;
	pwr_ = 0.0;

	addControls();
}

PowerSpectrum::PowerSpectrum(const PowerSpectrum& a):MarSystem(a)
{
	ctrl_spectrumType_ = getctrl("mrs_string/spectrumType");
}


PowerSpectrum::~PowerSpectrum()
{
}

void
PowerSpectrum::addControls()
{
	addctrl("mrs_string/spectrumType", "power", ctrl_spectrumType_);
	setctrlState("mrs_string/spectrumType", true);
}

MarSystem* 
PowerSpectrum::clone() const 
{
	return new PowerSpectrum(*this);
}

void 
PowerSpectrum::myUpdate(MarControlPtr sender)
{
	(void) sender;
	ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
	ctrl_onObservations_->setValue((ctrl_inObservations_->to<mrs_natural>() /2), NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());// / ctrl_inSamples_->to<mrs_natural>());
	ctrl_onObsNames_->setValue(ctrl_inObsNames_);

	stype_ = ctrl_spectrumType_->to<mrs_string>();
	if (stype_ == "power")
		ntype_ = PSD_POWER;
	else if (stype_ == "magnitude") 
		ntype_ = PSD_MAG;
	else if (stype_ == "decibels")
		ntype_ = PSD_DB;
	else if (stype_ == "powerdensity")
		ntype_ = PSD_PD;

	inObservations_ = ctrl_inObservations_->to<mrs_natural>();

	N2_ = inObservations_ / 2;
	ostringstream oss;

	for (mrs_natural n=0; n < N2_; n++)
		oss << "mbin_" << n << ",";
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void 
PowerSpectrum::myProcess(realvec& in, realvec& out)
{
	for(t=0; t < inSamples_; ++t)
	{
		for (o=0; o < N2_; o++)
		{
			if (o==0)
			{
				re_ = in(0,t);
				im_ = 0.0;
			}
			else if (o == N2_) //never happens because we are not outputting the due N/2+1 points, but only N/2 points...
			{
				re_ = in(1,t);
				im_ = 0.0;
			}
			else
			{
				re_ = in(2*o, t);
				im_ = in(2*o+1, t);
			}

			switch(ntype_)
			{
			case PSD_POWER:
				out(o, t) = re_*re_ + im_*im_;	  
				break;
			case PSD_MAG:
				out(o,t) = sqrt(re_ * re_ + im_ * im_);
				break;
			case PSD_DB:
				dB_ = (mrs_real)(20 * log10(re_ * re_ + im_ * im_ + 0.000000001));
				if (dB_ < -100) dB_ = -100;
				out(o,t) = dB_;	  
				break;
			case PSD_PD:
				pwr_ = re_ * re_ + im_ * im_;	  
				out(o,t) = (mrs_real)(2.0 * pwr_) / N2_;	  
				break;
			}
		} 
	}
}













