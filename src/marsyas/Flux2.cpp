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

#include "Flux2.h"

using namespace std;
using namespace Marsyas;

Flux2::Flux2(string name):MarSystem("Flux2",name)
{
	diff_ = 0.0;
	flux_ = 0.0;
	max_ = 0.0;
}

Flux2::~Flux2()
{
}

MarSystem* 
Flux2::clone() const 
{
	return new Flux2(*this);
}

void
Flux2::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("Flux2.cpp - Flux2:myUpdate");
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue("Flux2_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

	prevWindow_.create(ctrl_inObservations_->to<mrs_natural>(),
		ctrl_inSamples_->to<mrs_natural>());
}

void 
Flux2::myProcess(realvec& in, realvec& out)
{
	for (t = 0; t < inSamples_; t++)
	{
		flux_ = 0.0;
		diff_ = 0.0;
		max_ = 0.0;
		for(o = 1; o < inObservations_; ++o)
		{
			//Simon's version
			tmp_ = in(o,t)  - prevWindow_(o,t);
			diff_ = (tmp_+abs(tmp_))/2;
			if(diff_ > max_)
				max_ = diff_;
			flux_ += diff_;
			prevWindow_(o,t) = in(o,t);

			//from Juan's Matlab
//			tmp_ = pow(in(o,t), 2.0)  - pow(prevWindow_(o,t), 2.0);
//			tmp2_ = (tmp_+abs(tmp_))/2;
//			diff_ = sqrt(tmp2_);
//			if(diff_ > max_)
//				max_ = diff_;
//			flux_ += diff_;
//			prevWindow_(o,t) = in(o,t);
		}

		//Normalizing with max_ was a bad idea
//		if(max_ != 0.0)
//			flux_ /= (max_ * inObservations_);
//		else
//			flux_ = 0.0;

		out(0,t) = flux_;

		//MATLAB_PUT(prevWindow_, "prevWindow_");
	}

	MATLAB_PUT(out, "Flux2_out");
	MATLAB_EVAL("FluxTS = [FluxTS, Flux2_out];");
	//MATLAB_EVAL("plot(FluxTS);");
}














