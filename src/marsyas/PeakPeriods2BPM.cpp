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

#include "PeakPeriods2BPM.h"

using namespace std;
using namespace Marsyas;

PeakPeriods2BPM::PeakPeriods2BPM(string name):MarSystem("PeakPeriods2BPM",name)
{
	addControls();
}


PeakPeriods2BPM::~PeakPeriods2BPM()
{
}


PeakPeriods2BPM::PeakPeriods2BPM(const PeakPeriods2BPM& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_hopsize_ = getctrl("mrs_natural/hopSize");
}

void 
PeakPeriods2BPM::addControls()
{
  //Add specific controls needed by this MarSystem.
  //By default it uses typical values for the source sampling rate and for the hopsize of the analysis.
  addctrl("mrs_real/srcFs", 1.0, ctrl_srcFs_);
  addctrl("mrs_natural/hopSize", 1, ctrl_hopsize_);
}

MarSystem* 
PeakPeriods2BPM::clone() const 
{
  return new PeakPeriods2BPM(*this);
}


void
PeakPeriods2BPM::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PeakPeriods2BPM.cpp - PeakPeriods2BPM:myUpdate");
  MarSystem::myUpdate(sender);
}

void 
PeakPeriods2BPM::myProcess(realvec& in, realvec& out)
{
	srcFs_ = ctrl_srcFs_->to<mrs_real>();
	hopsize_ = ctrl_hopsize_->to<mrs_natural>();

	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_/2; t++)
		{
			out(o,2*t) = in(o,2*t);  //input pair indexes = peaks amplitude (retrieved from MaxArgMax)
			//input odd indexes = peaks argument (retrieved from MaxArgMax)
			out(o,2*t+1) = (mrs_natural) ((60.0 / (in(o, 2*t+1) * hopsize_)) * srcFs_);
		}

	//MATLAB_PUT(in, "Peaker_out");
	//MATLAB_PUT(out, "MaxPeaksBPM");
}







	

	
