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

#include "ADRessSpectrum.h"

using namespace std;
using namespace Marsyas;

ADRessSpectrum::ADRessSpectrum(string name):MarSystem("ADRessSpectrum", name)
{
	addControls();
	N2_ = 0;
}

ADRessSpectrum::ADRessSpectrum(const ADRessSpectrum& a) : MarSystem(a)
{
	N2_ = a.N2_;
	ctrl_d_ = getctrl("mrs_real/d");
	ctrl_H_ = getctrl("mrs_natural/H");
}

ADRessSpectrum::~ADRessSpectrum()
{
}

MarSystem*
ADRessSpectrum::clone() const
{
	return new ADRessSpectrum(*this);
}

void
ADRessSpectrum::addControls()
{
	addctrl("mrs_real/d", 0.5, ctrl_d_);
	addctrl("mrs_natural/H", 2, ctrl_H_);
}

void
ADRessSpectrum::myUpdate(MarControlPtr sender)
{
	(void) sender;

	mrs_real pN2 = N2_;

	//left and right channels ADRess info is stacked vertically in the input
	N2_ = ctrl_inObservations_->to<mrs_natural>() / 2; // = N/2+1 spectrum points for each channel

	ctrl_onSamples_->setValue(1, NOUPDATE);
	//output a complex spectrum for each each channel, stacked vertically
	ctrl_onObservations_->setValue(((N2_-1)*2)*2); //2*N

	if(pN2 != N2_)
	{
		ostringstream oss;
		//Left channel
		oss << "LeftADRess_rbin_0" << ","; //DC bin (only has real part)
		oss << "LeftADRess_rbin_" << N2_-1 << ","; // = N/2, i.e. Nyquist bin (only has real part)
		for (mrs_natural n=2; n < N2_; n++)
		{
			oss << "LeftADRess_rbin_" << n-1 << ",";
			oss << "LeftADRess_ibin_" << n-1 << ",";
		}
		//right channel
		oss << "RightADRess_rbin_0" << ","; //DC bin (only has real part)
		oss << "RightADRess_rbin_" << N2_-1 << ","; // = N/2, i.e. Nyquist bin (only has real part)
		for (mrs_natural n=2; n < N2_; n++)
		{
			oss << "RightADRess_rbin_" << n-1 << ",";
			oss << "RightADRess_ibin_" << n-1 << ",";
		}
		ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
	}

	beta_ = ctrl_onSamples_->to<mrs_natural>()-2;
}

void
ADRessSpectrum::myProcess(realvec& in, realvec& out)
{
// 	//output spectrum of the "selected" source, given d and H
// 	mrs_natural H = ctrl_H_->to<mrs_natural>();
// 	if(H < 1)
// 	{
// 		H = 1;
// 		ctrl_H_->setValue(1);
// 	}
// 	if(H > beta_)
// 	{
// 		H = beta_;
// 		ctrl_H_->setValue(beta_);
// 	}
// 	mrs_natural H2 = H/2;
// 
// 	mrs_natural d = (mrs_natural)(beta_*ctrl_d_->to<mrs_real>());
// 	if(d < 0)
// 	{
// 		d = 0;
// 		ctrl_d_->setValue(0.0);
// 	}
// 	if(d > beta_)
// 	{
// 		d = beta_;
// 		ctrl_d_->setValue(1.0);
// 	}
// 
// 	mrs_real ampL, ampR, phaseL, phaseR;
// 	for(mrs_natural k=0; k < N2_; ++k)
// 	{
// 		//get left channel spectrum bin
// 		if (k==0)
// 		{
// 			rel_ = in(0,0);
// 			iml_ = 0.0;
// 		}
// 		else if (k == N4_) 
// 		{
// 			rel_ = in(1, 0);
// 			iml_ = 0.0;
// 		}
// 		else
// 		{
// 			rel_ = in(2*k, 0);
// 			iml_ = in(2*k+1, 0);
// 		}
// 		//get right channel spectrum bin
// 		if (k==0)
// 		{
// 			rer_ = in(N2_,0);
// 			imr_ = 0.0;
// 		}
// 		else if (k == N2_) 
// 		{
// 			rer_ = in(N2_+1, 0);
// 			imr_ = 0.0;
// 		}
// 		else
// 		{
// 			rer_ = in(N2_ + 2*k, 0);
// 			imr_ = in(N2_ + 2*k+1, 0);
// 		}
// 
// 		//calculate the separated "source" magnitudes
// 		ampL = ampR = 0.0;
// 		for(mrs_natural i=d-H2; i <= d+H2; ++i)
// 		{
// 			ampL += AZl_(k,i);
// 			ampR += AZr_(k,i);
// 		}
// 
// 		//calculate the phases from original spectrum
// 		phaseL = atan2(iml_, rel_);
// 		phaseR = atan2(imr_, rer_);
// 
// 		//convert back to rectangular form
// 		rel_ = ampL*cos(phaseL);
// 		iml_ = ampL*sin(phaseL);
// 		rer_ = ampR*cos(phaseR);
// 		imr_ = ampR*sin(phaseR);
// 
// 		//write left channel spectrum bin to output
// 		if (k==0)
// 		{
// 			out(0,0) = rel_;
// 		}
// 		else if (k == N4_) 
// 		{
// 			out(1, 0) = rel_;
// 		}
// 		else
// 		{
// 			out(2*k, 0) = rel_;
// 			out(2*k+1, 0) = iml_;
// 		}
// 		//write right channel spectrum bin to output
// 		if (k==0)
// 		{
// 			out(N2_,0) = rer_;
// 		}
// 		else if (k == N2_) 
// 		{
// 			out(N2_+1, 0) = rer_;
// 		}
// 		else
// 		{
// 			out(N2_ + 2*k, 0) = rer_;
// 			out(N2_ + 2*k+1, 0) = imr_;
// 		}
// 	}
}


