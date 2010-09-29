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

#include "TempoHypotheses.h"

using namespace std;
using namespace Marsyas;

TempoHypotheses::TempoHypotheses(mrs_string name):MarSystem("TempoHypotheses", name)
{
  addControls();
  t_ = 0;
  noBPMs_ = true;
  inductionFinished_ = false;
}

TempoHypotheses::TempoHypotheses(const TempoHypotheses& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
  ctrl_nPhases_ = getctrl("mrs_natural/nPhases");
  ctrl_nPeriods_ = getctrl("mrs_natural/nPeriods");
  ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_dumbInduction_ = getctrl("mrs_bool/dumbInduction");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");

  noBPMs_ = a.noBPMs_;
  dumbInduction_ = a.dumbInduction_;
  inductionFinished_ = a.inductionFinished_;
}

TempoHypotheses::~TempoHypotheses()
{
}

MarSystem* 
TempoHypotheses::clone() const 
{
  return new TempoHypotheses(*this);
}

void 
TempoHypotheses::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/nPhases", 1, ctrl_nPhases_);
  setctrlState("mrs_natural/nPhases", true);
  addctrl("mrs_natural/nPeriods", 1, ctrl_nPeriods_);
  setctrlState("mrs_natural/nPeriods", true);
  addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
  setctrlState("mrs_natural/inductionTime", true);
  addctrl("mrs_natural/hopSize", 1, ctrl_hopSize_);
  addctrl("mrs_real/srcFs", 1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_bool/dumbInduction", false, ctrl_dumbInduction_);
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
}

void
TempoHypotheses::myUpdate(MarControlPtr sender)
{
  MRSDIAG("TempoHypotheses.cpp - TempoHypotheses:myUpdate");
  
	nPhases_ = ctrl_nPhases_->to<mrs_natural>();
	nPeriods_ = ctrl_nPeriods_->to<mrs_natural>();
	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	srcFs_ = ctrl_srcFs_->to<mrs_real>();
	hopSize_ = ctrl_hopSize_->to<mrs_natural>();

	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	setctrl("mrs_natural/onSamples", 3);
	setctrl("mrs_natural/onObservations", nPhases_ * nPeriods_);
}


void 
TempoHypotheses::myProcess(realvec& in, realvec& out)
{
	//t_ is constantly updated with the referee's next time frame
	t_ = ctrl_tickCount_->to<mrs_natural>();

	//cout << "THyp: " << t_ << "; Ind: " << inductionTime_ << endl;

	if(t_ == inductionTime_ && !inductionFinished_)
	{
		dumbInduction_ = ctrl_dumbInduction_->to<mrs_bool>();

		if(!dumbInduction_) //if not in dumb induction mode
		{
			//retrieve Max Period Peak
			mrs_real maxPeriodPeak = 0.0;
			for (int i=0; i < nPeriods_; i++)
			{
				if(in(0, 2*i) > maxPeriodPeak)
					maxPeriodPeak = in(0, 2*i);
			}

			for (int i=0; i < nPeriods_; i++)
			{	
				int z = 0;
				for (int j = (i * nPhases_); j < ((i+1) * nPhases_); j++)
				{
					if(in(0, 2*i+1) > 1 && in(1, 2*z+1) > 0) //period > 1 because it may appear as decimals meaning 0
					{
						out(j, 0) = in(0, 2*i+1); //Periods
						out(j, 1) = in(1, 2*z+1); //Phases
						out(j, 2) = in(0, 2*i);// / maxPeriodPeak; //Normalized period peak magnitudes
						noBPMs_ = false;
					}
					z++;
				}
			}
		}
		
		//if no BPMs considered or in dumb induction mode
		if(noBPMs_ || dumbInduction_)
		{
			//Manual assorted values for filling BPM hypotheses vector when none are generated
			int forceBPM_[] = {120, 60, 240, 100, 160, 200, 80, 140, 180, 220};

			if(noBPMs_ && !dumbInduction_)
				cout << "\nUnable to find salient periodicities within the given induction window..." << endl;
			if(dumbInduction_)
				cout << "\nDumb Induction Mode..." << endl;
			
			cout << "...Replacing induction with the following BPMs: ";

			for (int i=0; i < nPeriods_; i++)
			{
				if(i == 10) break; //(to a maximum of 10 unconsidered)

				cout << forceBPM_[i] << "; ";
				
				int z = 0;
				for (int j = (i * nPhases_); j < ((i+1) * nPhases_); j++)
				{
					out(j, 0) = (mrs_natural) (((mrs_real) 60 / (forceBPM_[i] * hopSize_)) * (srcFs_)); //Periods
					out(j, 1) = in(1, 2*z+1); //Phases
					out(j, 2) = 1.0; //equal (max) peak sizes to all manual periods
					z++;
				}
			}

			noBPMs_ = false;
			dumbInduction_ = false;
			ctrl_dumbInduction_->setValue(true); //for passing this info to PhaseLock
			inductionFinished_ = true;
		}

		//MATLAB_PUT(out, "TempoHypotheses");	
		//MATLAB_PUT(in, "BeatData");
		//MATLAB_EVAL("plot(BeatData)");
	}
}







	
