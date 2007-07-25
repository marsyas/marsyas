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
	\class RBF
	\ingroup Special
	\brief Radial Basis Functions

	Controls:
	- \b mrs_string/RBFtype [rw] : chose RBF function to use.
	- \b mrs_real/Beta [rw] : sets the Beta constant used in some of the RBF functions.
	- \b mrs_bool/symmetricIn [w]: if the input is a symmetric Matrix, setting this to true improves performance
*/
#include "RBF.h"
#include <cmath>

using namespace std;
using namespace Marsyas;

RBF::RBF(string name):MarSystem("RBF", name)
{
	addControls();
}

RBF::RBF(const RBF& a) : MarSystem(a)
{
	ctrl_RBFtype_ = getctrl("mrs_string/RBFtype");
	ctrl_Beta_ = getctrl("mrs_real/Beta");
	ctrl_symmetricIn_ = getctrl("mrs_bool/symmetricIn");
}

RBF::~RBF()
{
}

MarSystem* 
RBF::clone() const 
{
	return new RBF(*this);
}

void 
RBF::addControls()
{
	addctrl("mrs_string/RBFtype", "Gaussian", ctrl_RBFtype_);
	ctrl_RBFtype_->setState(true);

	addctrl("mrs_real/Beta", 1.0, ctrl_Beta_);

	addctrl("mrs_bool/symmetricIn", false, ctrl_symmetricIn_);
}

mrs_real 
RBF::GaussianRBF(const mrs_real val) const
{
	//as defined in:
	//http://en.wikipedia.org/wiki/Radial_basis_function
	return exp(-1.0*val*val*ctrl_Beta_->to<mrs_real>());
}

mrs_real
RBF::MultiquadraticRBF(const mrs_real val) const
{
	//as defined in:
	//http://en.wikipedia.org/wiki/Radial_basis_function
	mrs_real beta = ctrl_Beta_->to<mrs_real>();
	return sqrt(val*val+beta*beta);
}

mrs_real
RBF::ThinPlateSplineRBF(const mrs_real val) const
{
	//as defined in:
	//http://en.wikipedia.org/wiki/Radial_basis_function
	return val*val*log(val);
}

void
RBF::myUpdate(MarControlPtr sender)
{
	RBFtype_ = ctrl_RBFtype_->to<mrs_string>();
	if(RBFtype_ == "Gaussian")
		RBFfunc_ = &RBF::GaussianRBF;
	else if(RBFtype_ == "Multiquadratic")
		RBFfunc_ = &RBF::MultiquadraticRBF;
	else if(RBFtype_ == "ThinPlateSpline")
		RBFfunc_ = &RBF::ThinPlateSplineRBF;
	else
	{
		RBFfunc_ = NULL;
		MRSWARN("RBF::myUpdate - unsupported RBF function: " + RBFtype_);
	}

	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE); //[?]
	ostringstream oss;
	string inObsNames = ctrl_inObsNames_->toString();
	for (mrs_natural i = 0; i < inObservations_; i++)
	{
		string inObsName;
		string temp;
		inObsName = inObsNames.substr(0, inObsNames.find(","));
		temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
		inObsNames = temp;
		oss << "RBF_" << RBFtype_ << "_" << inObsName << ",";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void 
RBF::myProcess(realvec& in, realvec& out)
{
	mrs_real res;

	if(ctrl_symmetricIn_->isTrue())
	{
		for(t=0; t<inSamples_; ++t)
		{
			for(o=0; o<=t; ++o)
			{
				if(this->RBFfunc_)
				{
					res = (this->*RBFfunc_)(in(o,t));
					//check for NaN and Inf results
					if(res != res)
					{
						MRSERR("RBF::myProcess(): calculation of RBF(x) @" << prefix_ << " is returning NaN/Inf for x(" << o << "," << t << ") = " << in(o,t));
					}
					else if(res == 0)
					{
						MRSERR("RBF::myProcess(): calculation of RBF(x) @" << prefix_ << " is returning 0 for x(" << o <<"," << t << ") = " << in(o,t));
					}
					out(o,t) = res;
				}
				else
					out(o,t) = in(o,t); //just bypass data, unmodified

				//symmetry
				out(t,o) = out(o,t);
			}
		}
	}
	else
	{
		for(t=0; t<inSamples_; ++t)
		{
			for(o=0; o<inObservations_; ++o)
			{
				if(this->RBFfunc_)
				{
					res = (this->*RBFfunc_)(in(o,t));
					//check for NaN and Inf results
					if(res != res)
					{
						MRSERR("RBF::myProcess(): calculation of RBF(x) @" << prefix_ << " is returning NaN/Inf for x(" << o << "," << t << ") = " << in(o,t));
					}
					else if(res == 0)
					{
						MRSERR("RBF::myProcess(): calculation of RBF(x) @" << prefix_ << " is returning 0 for x(" << o <<"," << t << ") = " << in(o,t));
					}
					out(o,t) = res;
				}
				else
					out(o,t) = in(o,t); //just bypass data, unmodified
			}
		}
	}
}








