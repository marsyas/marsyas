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

#include "OnsetTimes.h"

using namespace std;
using namespace Marsyas;

OnsetTimes::OnsetTimes(mrs_string name):MarSystem("OnsetTimes", name)
{
  addControls();
  t_ = 0;
  count_ = 0;
}

OnsetTimes::OnsetTimes(const OnsetTimes& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
	ctrl_n1stOnsets_ = getctrl("mrs_natural/n1stOnsets");
	ctrl_lookAheadSamples_ = getctrl("mrs_natural/lookAheadSamples");
	ctrl_nPeriods_ = getctrl("mrs_natural/nPeriods");
	ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
	ctrl_accSize_ = getctrl("mrs_natural/accSize");
	ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
	
	count_ = a.count_;
}

OnsetTimes::~OnsetTimes()
{
}

MarSystem* 
OnsetTimes::clone() const 
{
  return new OnsetTimes(*this);
}

void 
OnsetTimes::addControls()
{
  //Add specific controls needed by this MarSystem.
	addctrl("mrs_natural/n1stOnsets", 1, ctrl_n1stOnsets_);
	setctrlState("mrs_natural/n1stOnsets", true);
	addctrl("mrs_natural/lookAheadSamples", 1, ctrl_lookAheadSamples_);
	addctrl("mrs_natural/nPeriods", 1, ctrl_nPeriods_);
	setctrlState("mrs_natural/nPeriods", true);
	addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
	setctrlState("mrs_natural/inductionTime", true);
	addctrl("mrs_natural/accSize", -1, ctrl_accSize_);
	setctrlState("mrs_natural/accSize", true);
	addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
}

void
OnsetTimes::myUpdate(MarControlPtr sender)
{
	MRSDIAG("OnsetTimes.cpp - OnsetTimes:myUpdate");
	n_ = ctrl_n1stOnsets_->to<mrs_natural>();
	nPeriods_ = ctrl_nPeriods_->to<mrs_natural>();	
	
	//if the nr of requested bpm hypotheses is bigger
	//than the requested phases we need to fill the rest of the vector with
	//0s -> to avoid crash in tempohypotheses Fanout
	mrs_natural size = 2 * max(n_, nPeriods_);
	setctrl("mrs_natural/onSamples",  size);
	
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	
	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	accSize_ = ctrl_accSize_->to<mrs_natural>();

}


void 
OnsetTimes::myProcess(realvec& in, realvec& out)
{
	//t_ is constantly updated with the referee's next time frame
	t_ = ctrl_tickCount_->to<mrs_natural>();

	//cout << "OTime: " << t_ << endl;

	lookAhead_ = ctrl_lookAheadSamples_->to<mrs_natural>();
	
	mrs_natural inc = 0; //nr. of first ignored onsets
	if((t_ - lookAhead_) > 0 && in(0,0) == 1.0) //avoid onset at 0 frame
	{
		//if task isn't still done && (first peak || peak distance at least 5 frames from last peak)
		if(count_ == inc || (count_ > inc && count_ < n_ + inc && ((t_-lookAhead_)+(accSize_-1 - inductionTime_)) > out(0, 2*(count_-inc)-1) + 5))
		{
			//cout << "Out Last Arg: " << out(0, 2*(count_-inc)+1) << " Current Arg: " << t_ - lookAhead_ << endl;
			out(0, 2*(count_-inc)) = in(0,0);
			//out(0, 2*(count_-inc)+1) = t_ - lookAhead_; 
			//onsetTime equals to peakerTime (t_-lookAhead_) + difference between last point in the  
			//accumulator/ShiftInput (accSize_-1) and the considered inductionTime
			out(0, 2*(count_-inc)+1) = (t_-lookAhead_)+(accSize_-1 - inductionTime_);

			count_++;
		}
		//cout << "t-" << t_ << ": Onset at: " << t_ - lookAhead_ << endl;
	}

	//MATLAB_PUT(out, "OnsetTimes");
}
