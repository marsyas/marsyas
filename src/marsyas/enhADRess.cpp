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

#include "enhADRess.h"

using namespace std;
using namespace Marsyas;

enhADRess::enhADRess(string name):MarSystem("enhADRess", name)
{
	addControls();
}

enhADRess::enhADRess(const enhADRess& a) : MarSystem(a)
{
	ctrl_beta_ = getctrl("mrs_natural/beta");
}

enhADRess::~enhADRess()
{
}

MarSystem*
enhADRess::clone() const
{
	return new enhADRess(*this);
}

void
enhADRess::addControls()
{
	addctrl("mrs_natural/beta", 100, ctrl_beta_);
}

void
enhADRess::myUpdate(MarControlPtr sender)
{
	MRSDIAG("enhADRess.cpp - enhADRess:myUpdate");
	(void) sender;

	N2_ = inObservations_ / 2; //i.e. we get two vertically stacked spectrums at the input
	N4_ = N2_/2 + 1; //i.e. for each spectrum, we have N/2+1 spectral bins

	ctrl_onSamples_->setValue(1 + ctrl_beta_->to<mrs_natural>()+1, NOUPDATE);//one column for the phases, the others for the panning map [0:beta]
	ctrl_onObservations_->setValue(N4_*2, NOUPDATE); //output data for N/2+1 spectral bins, stacked vertically for each channel
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	ostringstream oss;
	for(mrs_natural n=0; n< N4_; ++n)
		oss << "Left_bin_" << n <<",";
	for(mrs_natural n=0; n< N4_; ++n)
		oss << "Right_bin_" << n <<",";
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void
enhADRess::myProcess(realvec& in, realvec& out)
{
	out.setval(0.0);

	mrs_natural beta = ctrl_beta_->to<mrs_natural>();

	for (mrs_natural k=0; k < N4_; k++)
	{
		minAZr_ = MAXREAL;
		minAZl_ = MAXREAL;
		maxAZr_ = MINREAL;
		maxAZl_ = MINREAL;

		//get left channel spectrum bin
		if (k==0) //DC bin (i.e. 0)
		{
			rel_ = in(0,0);
			iml_ = 0.0;
		}
		else if (k == N4_-1) //Nyquist bin (i.e. N/2)
		{
			rel_ = in(1, 0);
			iml_ = 0.0;
		}
		else //all other bins
		{
			rel_ = in(2*k, 0);
			iml_ = in(2*k+1, 0);
		}

		//get right channel spectrum bin
		if (k==0) //DC bin (i.e. 0)
		{
			rer_ = in(N2_,0);
			imr_ = 0.0;
		}
		else if (k == N4_-1) //Nyquist bin (i.e. N/2)
		{
			rer_ = in(N2_+1, 0);
			imr_ = 0.0;
		}
		else //all other bins
		{
			rer_ = in(N2_ + 2*k, 0);
			imr_ = in(N2_ + 2*k+1, 0);
		}

		//store phases in first column of the output
		out(k,0) = atan2(iml_, rel_);		//left channel phases
		out(k+N4_, 0) = atan2(imr_, rer_); //right channel phases
		
		mrs_real deltaPhase = abs(out(k,0)-out(k+N4_, 0));

		//left amplitude value
		mrs_real Lk = sqrt(rel_*rel_ + iml_*iml_);

		//right amplitude value
		mrs_real Rk = sqrt(rer_*rer_ + imr_*imr_);

		//compute the minimums
		if(fmod(deltaPhase, 2*PI) < PI/2)
		{
			minAZl_ = Lk * sin(deltaPhase);
			minAZr_ = Rk * sin(deltaPhase);
		}
		else
		{
			minAZl_ = Lk;
			minAZr_ = Rk;
		}

		//estimate nearest panning indexes for each channel
		mrs_real gl = (Lk - minAZl_)/Rk;
		mrs_real gr = (Rk - minAZr_)/Lk;
		mrs_natural il = mrs_natural(gl*beta);
		mrs_natural ir = mrs_natural(gr*beta);
		mrs_natural i = min(il, ir);

		cout << "ir = " << ir << endl;
		cout << "il = " << il << endl;
		cout << "------------------" << endl;

		//compute the magnitudes of the frequency dependent nulls
		if(il > beta)
		{
			//right channel
			out(k+N4_,i+1) = Rk-minAZr_;
			//just filter out bins with amplitude inferior to -100dB
			if(20.0*log10(out(k+N4_,i+1)*out(k+N4_,i+1)+0.000000001)<-100.0)
				out(k+N4_,i+1) = 0.0;
		}
		if(ir > beta)
		{
			//left channel
			out(k,i+1) = Lk-minAZl_;
			//just filter out bins with amplitude inferior to -100dB
			if(20.0*log10(out(k,i+1)*out(k,i+1)+0.000000001)<-100.0)
				out(k,i+1) = 0.0;
		}
	}

	MATLAB_PUT(out, "out");
	MATLAB_EVAL("AZl = out(1:end/2,2:end);");
	MATLAB_EVAL("AZr = out(end/2+1:end,2:end);");
	MATLAB_EVAL("panMap = [AZl(:,1:end-1),AZr(:,end:-1:1)];figure(1);imagesc(panMap);");
}


