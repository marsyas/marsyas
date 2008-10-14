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

#include "Windowing.h"

using namespace std;
using namespace Marsyas;

Windowing::Windowing(string name):MarSystem("Windowing",name)
{
	zeroPadding_ = 0;
	size_ = 0;
	addcontrols();
}

Windowing::Windowing(const Windowing& a):MarSystem(a)
{
	ctrl_type_ = getctrl("mrs_string/type");
	ctrl_zeroPhasing_ = getctrl("mrs_bool/zeroPhasing");
	ctrl_zeroPadding_ = getctrl("mrs_natural/zeroPadding");
	ctrl_size_ = getctrl("mrs_natural/size");
	ctrl_variance_ = getctrl("mrs_real/variance");
	ctrl_normalize_ = getctrl("mrs_bool/normalize");
	
}

Windowing::~Windowing()
{
}

MarSystem* 
Windowing::clone() const 
{
	return new Windowing(*this);
}

void
Windowing::addcontrols()
{
	addctrl("mrs_string/type", "Hamming", ctrl_type_);
	addctrl("mrs_bool/zeroPhasing", false, ctrl_zeroPhasing_);
	addctrl("mrs_natural/zeroPadding", 0, ctrl_zeroPadding_);
	addctrl("mrs_natural/size", 0, ctrl_size_);
	addctrl("mrs_real/variance", 0.4, ctrl_variance_);// used for the gaussian window
	addctrl("mrs_bool/normalize", false, ctrl_normalize_);
	
	setctrlState("mrs_string/type", true);
	setctrlState("mrs_bool/zeroPhasing", true);
	setctrlState("mrs_natural/zeroPadding", true);
	setctrlState("mrs_natural/size", true);
	setctrlState("mrs_real/variance", true);
	setctrlState("mrs_bool/normalize", true);
}

void
Windowing::myUpdate(MarControlPtr sender)
{
	(void) sender;
	string type = ctrl_type_->to<mrs_string>();
	
	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ostringstream oss;
	string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	string inObsName;
	string temps;
	for (int i = 0; i < inObservations_; i++)
	{
		inObsName = inObsNames.substr(0, inObsNames.find(","));
		temps = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
		inObsNames = temps;
		oss << "Win" << type <<"_" << inObsName << ",";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

	//if zeroPadding control changed...	
	if(ctrl_zeroPadding_->to<mrs_natural>() != onSamples_- inSamples_)
	{
		//zero padding should always be a positive or zero value
		if(ctrl_zeroPadding_->to<mrs_natural>() < 0)
			ctrl_zeroPadding_->setValue(0, NOUPDATE);
		ctrl_size_->setValue(ctrl_inSamples_->to<mrs_natural>() + 
													ctrl_zeroPadding_->to<mrs_natural>(), NOUPDATE);
		onSamples_ = ctrl_size_->to<mrs_natural>();
	}
	//if size control changed...
	if(ctrl_size_->to<mrs_natural>() != onSamples_)
	{
		//size should never be smaller than inSamples
		if(ctrl_size_->to<mrs_natural>() < inSamples_)
			ctrl_size_->setValue(inSamples_, NOUPDATE);
		ctrl_zeroPadding_->setValue(ctrl_size_->to<mrs_natural>() - 
																ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
	}
	
	ctrl_onSamples_->setValue(ctrl_size_, NOUPDATE);
	
	//check if zero phasing should be performed
	if(ctrl_zeroPhasing_->isTrue())
		delta_ = inSamples_/2+1;
	else
		delta_=0;

	
	tmp_.create(inSamples_);
	envelope_.create(inSamples_);
	mrs_real temp = 0.0;

	if (type == "Hamming")
	{   
		mrs_real A = (mrs_real)0.54;
		mrs_real B = (mrs_real)0.46;

		for (t=0; t < inSamples_; t++)
		{
			temp = 2*PI*t / (inSamples_-1);
			envelope_(t) = A - B * cos(temp);
		}
	}
	else
	{
		if (type == "Hanning")
		{
			mrs_real A = (mrs_real)0.5;
			mrs_real B = (mrs_real)0.5;

			for (t=0; t < inSamples_; t++)
			{
				temp = 2*PI*t / (inSamples_-1);
				envelope_(t) = A - B * cos(temp);
			}
		}
		if (type == "Triangle")
		{
			for (t=0; t < inSamples_;t++)
			{
				temp = abs(t - (inSamples_-1.0)/2.0);
				temp = inSamples_ /2.0 - temp;
				envelope_(t) = 2.0/inSamples_ * temp;
			}
		}
		// zero padded triangle function
		if (type == "Bartlett") //this seems too similar to the triangle above -> check! [?]
		{
			for (t=0;t<inSamples_;t++)
			{
				temp = abs(t -(inSamples_-1.0)/2.0);
				temp = (inSamples_-1.0 )/2.0 - temp;
				envelope_(t) = 2.0 /(inSamples_-1.0) * temp;
			}
		}
		if (type == "Gaussian")
		{
			for (t=0;t< inSamples_; t++)
			{
				temp = (t-(inSamples_-1.0)/2.0 )/(ctrl_variance_->to<mrs_real>()*(inSamples_-1.0)/2.0);
				temp = temp * temp;
				envelope_(t) = exp(-0.5*temp);
			}
		}
		if (type == "Blackman") //NOT WORKING! [?]
		{
			for (t=0;t<inSamples_;t++)
			{
				temp = (PI * t)/(mrs_real)inSamples_-1.0 ;
				envelope_(t) = 0.42-0.5*cos(2.0*temp) + 0.08*cos(4.0*temp);
			}
		}
		if (type == "Blackman-Harris") //NOT WORKING! [?]
		{
			for (t=0;t<inSamples_;t++)
			{
				temp = (PI * t)/(mrs_real)inSamples_-1.0;
				envelope_(t) = 0.35875 - 0.48829*cos(2.0*temp) + 0.14128*cos(4.0*temp) - 0.01168*cos(6.0*temp);
			}
		}

	}

	if(ctrl_normalize_->to<mrs_bool>() == true)
	{
		mrs_real sum = 0.0;
		
		for (t =0; t < inSamples_; t++) 
		{
			sum += envelope_(t);
		}
		mrs_real afac = (mrs_real)(2.0 /sum);
		envelope_ *= afac;
	}
	
}

void 
Windowing::myProcess(realvec& in, realvec& out)
{
  out.setval(0);
	
	for (o=0; o < inObservations_; o++)
	{
		//shift windowed data in case zeroPhasing is selected
		if(ctrl_zeroPhasing_->isTrue())
		{
		  //apply the window to the input data
		  for (t = 0; t < inSamples_; t++)
		    {
		      tmp_(t) =  in(o,t)*envelope_(t); // /(norm_);     
		    }
		  for (t = 0; t < inSamples_/2; t++)
		    out(o,t)=tmp_((t+delta_)%inSamples_);
		  for (t = inSamples_/2; t < inSamples_; t++)
		    out(o,t+(onSamples_-inSamples_))=tmp_((t+delta_)%inSamples_);
		}
		else
		{
		  for(t=0; t< inSamples_; ++t)
		    out(o,t) = in(o,t) * envelope_(t);
		}
	}
}
