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
\class LPC2Cepstrum
\brief Convert LPC coefficients to Cepstrum coefficients.

This MarSystem is expecting to receive at its input LPC coefficients + Pitch + Power,
as output by the LPCWarped MarSystem (see LPCWarped.cpp/.h). It only converts the LPC
coefficients to cepstral coefficients and ignores the pitch and power values received 
from LPCwarped.

Code by:
Luís Gustavo Martins - lmartins@inescporto.pt
November 2006
*/

#include "LPC2Cepstrum.h"

using namespace std;
using namespace Marsyas;

LPC2Cepstrum::LPC2Cepstrum(string name):MarSystem("LPC2Cepstrum",name)
{
	addControls();
}

LPC2Cepstrum::LPC2Cepstrum(const LPC2Cepstrum& a) : MarSystem(a)
{
	ctrl_cepstralOrder_ = getctrl("mrs_natural/cepstralOrder");
}

LPC2Cepstrum::~LPC2Cepstrum()
{
}

MarSystem* 
LPC2Cepstrum::clone() const 
{
	return new LPC2Cepstrum(*this);
}

void 
LPC2Cepstrum::addControls()
{
	addctrl("mrs_natural/cepstralOrder", 10, ctrl_cepstralOrder_); 
	ctrl_cepstralOrder_->setState(true);

	LPCorder_ = 10; //just a default value equal to the default cepstralOrder
}

void
LPC2Cepstrum::myUpdate()
{ 
	MRSDIAG("LPC2Cepstrum.cpp - LPC2Cepstrum:myUpdate");

	ctrl_onSamples_->setValue(ctrl_inSamples_);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
	// output nr of observations (i.e. coefficients) is the 
	// number of cepstral coeffs
	ctrl_onObservations_->setValue(ctrl_cepstralOrder_, NOUPDATE);

	//LPC2Cepstrum features names
	ostringstream oss;
	for (mrs_natural i = 0; i < ctrl_cepstralOrder_->to<mrs_natural>(); i++)
		oss << "Cepstrum_" << i+1 << ",";

	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
	
	//input LPC order (LPC coefs - 1 pitch coeff - 1 power coeff)
	LPCorder_ = ctrl_inObservations_->to<mrs_natural>() - 2;
}

void 
LPC2Cepstrum::myProcess(realvec& in, realvec& out)
{
	mrs_real sum;
	mrs_natural cepstralOrder = ctrl_cepstralOrder_->to<mrs_natural>();

	/**************************************************************************/
	/*from HTK LPC2Cepstrum()                                                                    */
	/**************************************************************************/
	for (mrs_natural n = 0; n < cepstralOrder; n++)  
	{
		sum = 0.0;
		for (mrs_natural i=0; i < n; i++)
			sum = sum + (n-i-1) * in(i) * out(n-i-1);
		out(n) = -(in(n) + sum / (n+1));
	}
}


