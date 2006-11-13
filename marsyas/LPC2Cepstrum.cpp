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
	ctrl_order_ = getctrl("mrs_natural/order");
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
	//read-only
	addctrl("mrs_natural/order", 1, ctrl_order_);
}

void
LPC2Cepstrum::myUpdate()
{ 
	MRSDIAG("LPC2Cepstrum.cpp - LPC2Cepstrum:myUpdate");

	ctrl_onSamples_->setValue(ctrl_inSamples_);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
	
	// output nr of observations (i.e. LPCC coefficients) is equal
	// to the number of LPC coefs (LPC coeffs - 1 pitch coeff - 1 power)
	mrs_natural order = ctrl_inObservations_->to<mrs_natural>() - 2;
	ctrl_order_->setValue(order, NOUPDATE);
	ctrl_onObservations_->setValue(order+1, NOUPDATE);

	//LPC2Cepstrum features names
	ostringstream oss;
	for (mrs_natural i = 0; i < ctrl_order_->to<mrs_natural>(); i++)
		oss << "LPCC_" << i+1 << ",";
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void 
LPC2Cepstrum::myProcess(realvec& in, realvec& out)
{
	mrs_real sum;
	mrs_natural order = ctrl_order_->to<mrs_natural>();

/************************************************************************/
/* Based on:
/* http://www.mathworks.com/access/helpdesk/
// help/toolbox/dspblks/index.html?/access/helpdesk/help/toolbox/dspblks/
/* lpctofromcepstralcoefficients.html
/************************************************************************/
	out.setval(0.0);
	out(0) = -log(in(order+1)); //[?]
	for (mrs_natural m = 1; m <= order; m++)  
	{
		sum = 0.0;
		for (mrs_natural k=1; k <= m-1; k++)
			sum = sum + (mrs_real)(m-k) * in(k-1) * out(m-k);
		out(m) = +in(m-1) + sum / (mrs_real)m;
	}

	MATLAB_PUT(in, "LPCC_in");
	MATLAB_PUT(out, "LPCC_out");
	MATLAB_EVAL("LPCC_test");
}


