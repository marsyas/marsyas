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

#include "ADRess.h"

using namespace std;
using namespace Marsyas;

ADRess::ADRess(string name):MarSystem("ADRess", name)
{
	addControls();
}

ADRess::ADRess(const ADRess& a) : MarSystem(a)
{
	ctrl_beta_ = getctrl("mrs_natural/beta");
	ctrl_d_ = getctrl("mrs_real/d");
	ctrl_H_ = getctrl("mrs_natural/H");
}


ADRess::~ADRess()
{
}

MarSystem*
ADRess::clone() const
{
	return new ADRess(*this);
}

void
ADRess::addControls()
{
	addctrl("mrs_natural/beta", 100, ctrl_beta_);
	addctrl("mrs_real/d", 0.5, ctrl_d_);
	addctrl("mrs_natural/H", 2, ctrl_H_);
}

void
ADRess::myUpdate(MarControlPtr sender)
{
	MRSDIAG("ADRess.cpp - ADRess:myUpdate");
	(void) sender;
	
	ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
	ctrl_onObservations_->setValue((ctrl_inObservations_), NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	N4_ = inObservations_ / 4;
	N2_ = inObservations_ / 2;

	ostringstream oss;
	//Left channel
	oss << "LeftADRess_rbin_0" << ","; //DC bin (only has real part)
	oss << "LeftADRess_rbin_" << N4_ << ","; //Nyquist bin (only has real part)
	for (mrs_natural n=2; n < N4_; n++)
	{
		oss << "LeftADRess_rbin_" << n-1 << ",";
		oss << "LeftADRess_ibin_" << n-1 << ",";
	}
	//right channel
	oss << "RightADRess_rbin_0" << ","; //DC bin (only has real part)
	oss << "RightADRess_rbin_" << N4_ << ","; //Nyquist bin (only has real part)
	for (mrs_natural n=2; n < N4_; n++)
	{
		oss << "RightADRess_rbin_" << n-1 << ",";
		oss << "RightADRess_ibin_" << n-1 << ",";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
ADRess::myProcess(realvec& in, realvec& out)
{
	MATLAB_PUT(in, "in");

	mrs_real a, b, g;
	mrs_natural beta = ctrl_beta_->to<mrs_natural>();

	AZr_.stretch(N4_, beta+1);
	AZl_.stretch(N4_, beta+1);

	for(mrs_natural i=0; i <= beta; ++i)
	{
		g = i*1.0/beta;

		minAZr_ = MAXREAL;
		minAZl_ = MAXREAL;
		maxAZr_ = MINREAL;
		maxAZl_ = MINREAL;

		for (mrs_natural k=0; k < N4_; k++)
		{
			//get left channel spectrum bin
			if (k==0)
			{
				rel_ = in(0,0);
				iml_ = 0.0;
			}
			else if (k == N4_) 
			{
				rel_ = in(1, 0);
				iml_ = 0.0;
			}
			else
			{
				rel_ = in(2*k, 0);
				iml_ = in(2*k+1, 0);
			}
			//get right channel spectrum bin
			if (k==0)
			{
				rer_ = in(N2_,0);
				imr_ = 0.0;
			}
			else if (k == N2_) 
			{
				rer_ = in(N2_+1, 0);
				imr_ = 0.0;
			}
			else
			{
				rer_ = in(N2_ + 2*k, 0);
				imr_ = in(N2_ + 2*k+1, 0);
			}

			//right freq-azimuth spectrogram
			a = rel_ - g*rer_;
			b = iml_ - g*imr_;
			AZr_(k,i) = sqrt(a*a + b*b);
			//get maximums and minimums
			if(AZr_(k,i) > maxAZr_)
				maxAZr_ = AZr_(k,i);
			if(AZr_(k,i) < minAZr_)
				minAZr_ = AZr_(k,i);

			//left freq-azimuth spectrogram
			a = rer_ - g*rel_;
			b = imr_ - g*iml_;
			AZl_(k,i) = sqrt(a*a + b*b);
			//get maximums and minimums
			if(AZl_(k,i) > maxAZl_)
				maxAZl_ = AZl_(k,i);
			if(AZl_(k,i) < minAZl_)
				minAZl_ = AZl_(k,i);
		}

		//compute the magnitudes of the frequency dependent nulls 
		for (mrs_natural k=0; k < N4_; k++)
		{
			//right channel
			if(AZr_(k,i)== minAZr_)
				AZr_(k,i) = maxAZr_-minAZr_;
			else
				AZr_(k,i) = 0.0;

			//left channel
			if(AZl_(k,i)== minAZl_)
				AZl_(k,i) = maxAZl_-minAZl_;
			else
				AZl_(k,i) = 0.0;
		}
	}

	MATLAB_PUT(AZr_, "AZr");
	MATLAB_PUT(AZl_, "AZl");
	MATLAB_EVAL("figure(1);imagesc(AZl);figure(2);imagesc(AZr)");

	//get the "selected" source, given d and H
	mrs_natural H = ctrl_H_->to<mrs_natural>();
	if(H < 1)
	{
		H = 1;
		ctrl_H_->setValue(1);
	}
	if(H > beta)
	{
		H = beta;
		ctrl_H_->setValue(beta);
	}
	mrs_natural H2 = H/2;

	mrs_natural d = (mrs_natural)(beta*ctrl_d_->to<mrs_real>());
	if(d < 0)
	{
		d = 0;
		ctrl_d_->setValue(0.0);
	}
	if(d > beta)
	{
		d = beta;
		ctrl_d_->setValue(1.0);
	}
	
	mrs_real ampL, ampR, phaseL, phaseR;
	for(mrs_natural k=0; k < N4_; ++k)
	{
		//get left channel spectrum bin
		if (k==0)
		{
			rel_ = in(0,0);
			iml_ = 0.0;
		}
		else if (k == N4_) 
		{
			rel_ = in(1, 0);
			iml_ = 0.0;
		}
		else
		{
			rel_ = in(2*k, 0);
			iml_ = in(2*k+1, 0);
		}
		//get right channel spectrum bin
		if (k==0)
		{
			rer_ = in(N2_,0);
			imr_ = 0.0;
		}
		else if (k == N2_) 
		{
			rer_ = in(N2_+1, 0);
			imr_ = 0.0;
		}
		else
		{
			rer_ = in(N2_ + 2*k, 0);
			imr_ = in(N2_ + 2*k+1, 0);
		}
				
		//calculate the separated "source" magnitudes
		ampL = ampR = 0.0;
		for(mrs_natural i=d-H2; i <= d+H2; ++i)
		{
			ampL += AZl_(k,i);
			ampR += AZr_(k,i);
		}
		
		//calculate the phases from original spectrum
		phaseL = atan2(iml_, rel_);
		phaseR = atan2(imr_, rer_);

		//convert back to rectangular form
		rel_ = ampL*cos(phaseL);
		iml_ = ampL*sin(phaseL);
		rer_ = ampR*cos(phaseR);
		imr_ = ampR*sin(phaseR);

		//write left channel spectrum bin to output
		if (k==0)
		{
			out(0,0) = rel_;
		}
		else if (k == N4_) 
		{
			out(1, 0) = rel_;
		}
		else
		{
			out(2*k, 0) = rel_;
			out(2*k+1, 0) = iml_;
		}
		//write right channel spectrum bin to output
		if (k==0)
		{
			out(N2_,0) = rer_;
		}
		else if (k == N2_) 
		{
			out(N2_+1, 0) = rer_;
		}
		else
		{
			out(N2_ + 2*k, 0) = rer_;
			out(N2_ + 2*k+1, 0) = imr_;
		}
	}
}


