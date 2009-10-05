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
	ctrl_stereo_ = getctrl("mrs_bool/stereo");
}



Sum::~Sum()
{
}

void
Sum::addControls()
{
	addctrl("mrs_real/weight", 1.0, ctrl_weight_);
	addctrl("mrs_bool/stereo", false, ctrl_stereo_);
}

MarSystem*
Sum::clone() const
{
	return new Sum(*this);
}

void
Sum::myUpdate(MarControlPtr sender)
{
	// Start with the default MarSystem setup with equal input/output
	//stream format ...
	MarSystem::myUpdate(sender);

	/// ... but set the number of output observations to one.
	mrs_bool stereo = ctrl_stereo_->to<mrs_bool>();
	if (!stereo)
	  ctrl_onObservations_->setValue(1, NOUPDATE);
	else
 	  ctrl_onObservations_->setValue(2, NOUPDATE);
}


void
Sum::myProcess(realvec& in, realvec& out)
{
	mrs_real weightValue = ctrl_weight_->to<mrs_real>();
	mrs_bool stereo = ctrl_stereo_->to<mrs_bool>();
	
	// Sum the observation channels per sample.
	if (!stereo) 
	  {
	    for (t = 0; t < inSamples_; t++)
	      {
		out(0, t) = 0;
		for (o = 0; o < inObservations_; o++)
		  {
		    out(0, t) += (weightValue * in(o, t));
		  }
	      }
	  }
	else 			// stereo 
	  {
	    for (t = 0; t < inSamples_; t++)
	      for (c=0; c < 2; c++) 
		{
		  out(c, t) = 0;
		  for (o = c; o < inObservations_; o+=2)
		    {
		      out(c, t) += (weightValue * in(o, t));
		    }
		}
	  }
	  
}

