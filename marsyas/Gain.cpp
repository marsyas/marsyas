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
    \class Gain
    \brief Multiply input realvec with gain

   Simple MarSystem example. Just multiply the values of the input realvec
with gain and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

#include "Gain.h"

using namespace std;
using namespace Marsyas;

Gain::Gain(string name):MarSystem("Gain", name)
{
  //Add any specific controls needed by Gain
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(), 
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

Gain::Gain(const Gain& a) : MarSystem(a)
{
  ctrl_gain_ = getctrl("mrs_real/gain");
	ctrl_RMScalc_ = getctrl("mrs_bool/RMScalc");
	ctrl_inRMS_ = getctrl("mrs_realvec/inRMS");
	ctrl_outRMS_ = getctrl("mrs_realvec/outRMS");
}

Gain::~Gain()
{
}

MarSystem* 
Gain::clone() const 
{
  return new Gain(*this);
}

void 
Gain::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/gain", 1.0, ctrl_gain_);
	addctrl("mrs_bool/RMScalc", false, ctrl_RMScalc_);
	addctrl("mrs_realvec/inRMS", inRMS_, ctrl_inRMS_);
	addctrl("mrs_realvec/outRMS", outRMS_, ctrl_outRMS_);
}

void
Gain::myUpdate(MarControlPtr sender)
{
	MarSystem::myUpdate(sender);

	inRMS_.create(ctrl_inObservations_->to<mrs_natural>());
	outRMS_.create(ctrl_onObservations_->to<mrs_natural>()); 
}


void 
Gain::myProcess(realvec& in, realvec& out)
{
	//get a local copy of the current gain control value
	//(it will be used for this entire processing, even if it's
	//changed by someone else, e.g. by a different thread)
	mrs_real gainValue = ctrl_gain_->to<mrs_real>();

	inRMS_.setval(0.0);
	outRMS_.setval(0.0);

	bool calcRMS = ctrl_RMScalc_->isTrue();

	for (o=0; o < inObservations_; o++)
	{
		for (t = 0; t < inSamples_; t++)
		{
			//apply gain to all channels
			out(o,t) = gainValue * in(o,t);
			
			//calculate first part of RMS values for each in/out channel
			if(calcRMS)
			{
				inRMS_(o) += in(o,t)*in(o,t);
				outRMS_(o) += out(o,t)*out(o,t);
			}
		}
		//calculate second part of RMS values for each in/out channel
		if(calcRMS)
		{
			inRMS_(o)/=(mrs_real)inSamples_;
			inRMS_(o)=sqrt(inRMS_(o));

			outRMS_(o)/=(mrs_real)onSamples_;
			outRMS_(o)=sqrt(outRMS_(o));
		}
	}

	if(calcRMS)
	{
		//ctrl_inRMS_->setValue(inRMS_, NOUPDATE);
		//updctrl(ctrl_inRMS_, inRMS_, NOUPDATE);
		setctrl(ctrl_inRMS_, inRMS_);
	
		//ctrl_outRMS_->setValue(outRMS_, NOUPDATE);
		//updctrl(ctrl_outRMS_, outRMS_, NOUPDATE);
	  setctrl(ctrl_outRMS_, outRMS_);
	}
}







	
