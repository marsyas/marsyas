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

#include "PhaseLock.h"

using namespace std;
using namespace Marsyas;

PhaseLock::PhaseLock(string name):MarSystem("PhaseLock", name)
{
  addControls();
  t_ = 0;
}

PhaseLock::PhaseLock(const PhaseLock& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work
	ctrl_beatHypotheses_ = getctrl("mrs_realvec/beatHypotheses");
	ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
	ctrl_nrPeriodHyps_ = getctrl("mrs_natural/nrPeriodHyps");
	ctrl_nrPhasesPerPeriod_ = getctrl("mrs_natural/nrPhasesPerPeriod");
	t_ = a.t_;
}

PhaseLock::~PhaseLock()
{
}

MarSystem* 
PhaseLock::clone() const 
{
  return new PhaseLock(*this);
}

void 
PhaseLock::addControls()
{
  //Add specific controls needed by this MarSystem.
	addctrl("mrs_realvec/beatHypotheses", realvec(), ctrl_beatHypotheses_);
	addctrl("mrs_natural/inductionTime", 1, ctrl_inductionTime_);
	addctrl("mrs_natural/nrPeriodHyps", 1, ctrl_nrPeriodHyps_);
	setctrlState("mrs_natural/nrPeriodHyps", true);
	addctrl("mrs_natural/nrPhasesPerPeriod", 1, ctrl_nrPhasesPerPeriod_);
	setctrlState("mrs_natural/nrPhasesPerPeriod", true);
}

void
PhaseLock::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PhaseLock.cpp - PhaseLock:myUpdate");

	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	nrPeriodHyps_ = ctrl_nrPeriodHyps_->to<mrs_natural>();
	nrPhasesPerPeriod_ = ctrl_nrPhasesPerPeriod_->to<mrs_natural>();

	ctrl_onSamples_->setValue(3, NOUPDATE);
	ctrl_onObservations_->setValue(nrPeriodHyps_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	nInitHyp_ = nrPeriodHyps_ * nrPhasesPerPeriod_; //Nr. of initial hypothesis

	hypSignals_.create(nInitHyp_, inductionTime_+1); //N hypothesis signals of induction timming size.
	sum_.create(nInitHyp_);
	
	//Zero hypSignals:
	for(int i = 0; i < nInitHyp_; i++)
	{
		for(int j = 0; j < inductionTime_+1; j++)
		{
			hypSignals_(i, j) = 0.0;
		}
		//Zero sum:
		sum_(i) = 0.0;
	}
}


void 
PhaseLock::myProcess(realvec& in, realvec& out)
{
	t_++;

	//Output only defined just after induction time
	//until then output is undefined...
	for (o=0; o < onObservations_; o++)
    {
		for (t = 0; t < onSamples_; t++)
		{
			out(o,t) = -1.0;
		}
    }

	//After induction, given the initial beat hypotheses
	//Calculate the M (nrPeriodHyps_) best (period, phase) pairs + respective scores:
	if (t_ == inductionTime_)
	{
		beatHypotheses_ = ctrl_beatHypotheses_->to<mrs_realvec>();

		//matrix with all generated beat hypotheses in the induction stage
		//					[ Periodi | Beatj  ]
		// 					| Periodi | Beatj+1|
		//					|  ... |  ...   |
		//					| Periodi | Beatn  |
		//					|Periodi+1| Beatj  |
		//					|Periodi+1| Beatj+1|
		//					|  ... |  ...   |
		//					| Periodm | Beatj  |
		//					|  ... |  ...   | 
		//					[ Periodm | Beatn  ]

		//Build N hypotheses signals (phase + k*periods):
		mrs_realvec beatCount;
		beatCount.create(nInitHyp_);
		for(int h = 0; h < nInitHyp_; h++)
		{	
			mrs_natural frameCount = 0;
			mrs_natural k = 0;
			mrs_natural phase = (mrs_natural) beatHypotheses_(h, 1); 
			beatCount(h) = 0;
			do
			{
				//frameCount = phase + k*period
				//cout << "Hypothesis (" << h << "): Phase: " << phase << " Period: " << beatHypotheses_(h, 1) << endl;
				frameCount = (mrs_natural) (phase + k * beatHypotheses_(h, 0));
				if(frameCount > inductionTime_ || beatHypotheses_(h, 0) == 0 || beatHypotheses_(h, 1) == 0) break;
				
				hypSignals_(h, frameCount) = 1.0;
				beatCount(h)++;
				k++;

			}while (frameCount < inductionTime_);

			//Calculate Sum of every (period, phase) pair multiplied by the
			//accumulated flux signal -> The highest sum will correspond
			//to the best hypotheses
			for(int i = 0; i < inductionTime_; i++)
			{
				sum_(h) += hypSignals_(h, i) * in(i);
			}
		}

		//Retrieve best M (nrPeriodHyps_) {period, phase} pairs, by Period:
		for(int i = 0; i < nrPeriodHyps_; i++)
		{
			mrs_real maxSum = 0.0;
			mrs_natural maxSumInd = 0;
			for(int j = i*nrPhasesPerPeriod_; j < (i*nrPhasesPerPeriod_)+nrPhasesPerPeriod_; j++)
			//for(int j = i; j < sum_.getSize(); j+=nrPhasesPerPeriod_)
			{
				if(sum_(j) > maxSum)
				{
					maxSum = sum_(j);
					maxSumInd = j;
				}
			}

			//Period:
			out(i, 0) = beatHypotheses_(maxSumInd, 0);
			//Phase:
			out(i, 1) = beatHypotheses_(maxSumInd, 1);
			
			//Initial Score = Average Score of each hypotheses by summing every beat salience:
			mrs_real sc = maxSum / beatCount(maxSumInd);
			out(i, 2) = maxSum / beatCount(maxSumInd);
		}

		//MATLAB_PUT(out, "FinalHypotheses");
	}

	//MATLAB_PUT(in, "Flux_FlowThrued");
	//MATLAB_PUT(out, "PhaseLockOut");
	//MATLAB_EVAL("hold on;");
	//MATLAB_EVAL("plot(Flux_FlowThrued), g");
	//MATLAB_EVAL("FluxFlowTS = [FluxFlowTS, Flux_FlowThrued];");
}

