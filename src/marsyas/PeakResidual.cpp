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

#include "PeakResidual.h"

using namespace std;
using namespace Marsyas;

PeakResidual::PeakResidual(string name):MarSystem("PeakResidual", name)
{

	addControls();
}

PeakResidual::PeakResidual(const PeakResidual& a) : MarSystem(a)
{
	ctrl_SNR_ = getctrl("mrs_real/SNR");
}

PeakResidual::~PeakResidual()
{
}

MarSystem* 
PeakResidual::clone() const 
{
	return new PeakResidual(*this);
}

void 
PeakResidual::addControls()
{
	addctrl("mrs_real/SNR", 0.0, ctrl_SNR_);
}

void
PeakResidual::myUpdate(MarControlPtr sender)
{
	(void) sender;
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>()/2, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	ostringstream oss;
	string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	string inObsName;
	string temp;
	for(o=0; o < ctrl_onObservations_->to<mrs_natural>(); o++)
	{
		inObsName = inObsNames.substr(0, inObsNames.find(","));
		temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
		inObsNames = temp;
		oss << inObsName << "_residual,";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void 
PeakResidual::myProcess(realvec& in, realvec& out)
{
	mrs_real snr = -80.0;
	mrs_real originalPower;
	mrs_real synthPower;
	mrs_real diffPower;

	for (o=0; o < inObservations_/2; o++)
	{
		originalPower=0;
		synthPower=0;
		diffPower=0;
		for (t = 0; t < inSamples_; t++)
		{
			out(o,t) =  in(o,t)-in(o+1, t);
			synthPower += in(o, t)*in(o, t);
			diffPower += out(o, t)*out(o, t);
			originalPower += in(o+1, t)*in(o+1, t);
		}

		//originalPower/=inSamples_;
		//synthPower/=inSamples_;
		//diffPower/=inSamples_;

		if(synthPower > .001 && originalPower > .01) //[?]
		{
			snr = log10((originalPower)/(diffPower+MINREAL)); // +synthPower
		}
	}

	ctrl_SNR_->setValue(snr);
}








