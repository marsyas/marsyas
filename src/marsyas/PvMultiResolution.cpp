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

#include "PvMultiResolution.h"

using namespace std;
using namespace Marsyas;

PvMultiResolution::PvMultiResolution(string name):MarSystem("PvMultiResolution", name)
{
	addControls();
}

PvMultiResolution::PvMultiResolution(const PvMultiResolution& a) : MarSystem(a)
{
	ctrl_mode_ = getctrl("mrs_string/mode");
}


PvMultiResolution::~PvMultiResolution()
{
}

MarSystem*
PvMultiResolution::clone() const
{
	return new PvMultiResolution(*this);
}

void
PvMultiResolution::addControls()
{

	addctrl("mrs_string/mode", "long", ctrl_mode_);
}

void
PvMultiResolution::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PvMultiResolution.cpp - PvMultiResolution:myUpdate");

	(void) sender;
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() / 2, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	
	median_buffer_.create(20);
	mbindex_ = 0;
	
}

void
PvMultiResolution::myProcess(realvec& in, realvec& out)
{
	const mrs_string& mode = ctrl_mode_->to<mrs_string>();

	mrs_real max = DBL_MIN;
	mrs_real maxLong = DBL_MIN;
	mrs_real power = 0.0;
	
	
	// short window 
	for (o=0; o < inObservations_/2; o++)
		for (t = 0; t < inSamples_; t++)
		{
			out(o,t) = in(o, t);		
		}
	
	/* calculate power and use median for dynamic thresholding */ 
	for (o=0; o < onObservations_/2; o++) 
		for (t = 0; t < inSamples_; t++)
		{
			power += (out(2*o,t) * out(2*o,t));
		}
	power *= 1000000;
	
	median_buffer_(mbindex_) = power;
	mbindex_++;
	if (mbindex_ == 20)
	{
		mbindex_ = 0;
	}
	
	if (power - median_buffer_.median() <= 0.02) 
		{
			cout << 0 << endl;
			for (o=inObservations_/2; o < inObservations_; o++)
				for (t = 0; t < inSamples_; t++)
				{
					out(o-inObservations_/2,t) = in(o,t);
				}
			
			for (o=0; o < onObservations_/2; o++) 
				for (t = 0; t < inSamples_; t++)
				{
					out(2*o, t) = 2 * out(2*o,t);
				}
		}
	else 
		cout << power-median_buffer_.median() << endl;

	
	
}


