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

#include "Sum.h"

using namespace std;
using namespace Marsyas;

Sum::Sum(string name):MarSystem("Sum",name)
{
	addControls();
}

Sum::Sum(const Sum& a): MarSystem(a) 
{
	ctrl_weight_ = getctrl("mrs_real/weight");
}



Sum::~Sum()
{
}

void 
Sum::addControls()
{
	addctrl("mrs_real/weight", 1.0, ctrl_weight_);

}

MarSystem* 
Sum::clone() const 
{
	return new Sum(*this);
}

void
Sum::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("Sum.cpp - Sum:myUpdate");

	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}


void 
Sum::myProcess(realvec& in, realvec& out)
{
	mrs_real weightValue = ctrl_weight_->to<mrs_real>();

	out.setval(0.0);
	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_; t++)
		{
			out(0,t) += (weightValue * in(o,t));
		}
}










