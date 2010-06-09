/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BeatPhase.h"

using namespace std;
using namespace Marsyas;

BeatPhase::BeatPhase(string name):MarSystem("BeatPhase", name)
{
	addControls();
	sampleCount_ = 0.0;
}

BeatPhase::BeatPhase(const BeatPhase& a) : MarSystem(a)
{
	ctrl_tempos_ = getctrl("mrs_realvec/tempos");
	ctrl_temposcores_ = getctrl("mrs_realvec/tempo_scores");
	ctrl_phase_tempo_ = getctrl("mrs_real/phase_tempo");
	
	ctrl_beats_ = getctrl("mrs_realvec/beats");
	ctrl_bhopSize_ = getctrl("mrs_natural/bhopSize");
	ctrl_bwinSize_ = getctrl("mrs_natural/bwinSize");
	ctrl_timeDomain_ = getctrl("mrs_realvec/timeDomain");
	
	sampleCount_ = 0.0;
}

BeatPhase::~BeatPhase()
{
}

MarSystem* 
BeatPhase::clone() const 
{
	return new BeatPhase(*this);
}

void 
BeatPhase::addControls()
{
	//Add specific controls needed by this MarSystem.
	addctrl("mrs_realvec/tempos", realvec(2), ctrl_tempos_);
	addctrl("mrs_realvec/tempo_scores", realvec(2), ctrl_temposcores_);
	
	addctrl("mrs_real/phase_tempo", 100.0, ctrl_phase_tempo_);
	addctrl("mrs_realvec/beats", realvec(), ctrl_beats_);
	addctrl("mrs_natural/bhopSize", 64, ctrl_bhopSize_);
	addctrl("mrs_natural/bwinSize", 1024, ctrl_bwinSize_);
	addctrl("mrs_realvec/timeDomain", realvec(), ctrl_timeDomain_);
}

void
BeatPhase::myUpdate(MarControlPtr sender)
{
	// no need to do anything BeatPhase-specific in myUpdate 
	MarSystem::myUpdate(sender);

	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

	if (pinSamples_ != inSamples_)
	{
		{
			MarControlAccessor acc(ctrl_beats_);
			mrs_realvec& beats = acc.to<mrs_realvec>();
			beats.create(inSamples_);
		}
	}
   
	pinSamples_ = inSamples_;


   
}


void 
BeatPhase::myProcess(realvec& in, realvec& out)
{
	mrs_natural o,t, tm;
	
	MarControlAccessor acct(ctrl_tempos_);
	mrs_realvec& tempos = acct.to<mrs_realvec>();

	MarControlAccessor accts(ctrl_temposcores_);
	mrs_realvec& tempo_scores = accts.to<mrs_realvec>();
	
	
	mrs_natural bwinSize = ctrl_bwinSize_->to<mrs_natural>();
	mrs_natural bhopSize = ctrl_bhopSize_->to<mrs_natural>();

	MarControlAccessor acc(ctrl_beats_);
	mrs_realvec& beats = acc.to<mrs_realvec>();

	mrs_real period;
	mrs_real sum_phase = 0.0;
	mrs_real max_sum_phase = 0.0;
	mrs_natural max_phase = 0;
	mrs_real max_phase_tempo;
	mrs_real avg_period, next_peak;
	
	
	beats.setval(0.0);

	for (o=0; o < inObservations_; o++)
	{
		for (t = 0; t < inSamples_; t++)
		{
			out (o,t) = in(o,t);
			beats (o,t) = in(o,t);
		}
	}
	
	
	mrs_real tempo;
	mrs_real max_tm;
	
	max_sum_phase = 0;
	for (tm = 0; tm < 1; tm++)
	{
		tempo = tempos(tm);
		
		
		

		
		sum_phase = 0.0;
		for (t=0; t < inSamples_; t++) 
		{
			sum_phase += in(0,t);
			if (t+period < inSamples_)
				sum_phase += in(0,t+period);
		}
		if (sum_phase >= max_sum_phase)
		{
			max_sum_phase = sum_phase;
			max_tm = tm;
		}
	}
	
	ctrl_phase_tempo_->setValue(tempos(0));	
		
	tempo = tempos(0);

	if (tempo !=0)
		period = 2.0 * osrate_ * 60.0 / tempo; // flux hopSize is half the winSize 
	else 
		period = 0;
	period = (mrs_natural)(period+0.5);	

	cout << "TEMPO = " << tempo << endl;
	cout << "period = " << period << endl;
	
	max_sum_phase = 0.0;
	
	for (t= inSamples_-1; t >= inSamples_-1-period; t--) 
	{
		sum_phase = 0.0;
		sum_phase += in(0,t);
		sum_phase += in(0, t-period);
		sum_phase += in(0, t-2*period);
		sum_phase += in(0, t-3*period);
		if (sum_phase >= max_sum_phase)
		{
			max_sum_phase = sum_phase;
			max_phase = t;
		}
	}

	
	
	
	
	mrs_natural delay = (bwinSize - bhopSize);
	
	cout << "max_phase = " << max_phase << endl;

	
	mrs_natural start;
	mrs_natural end;
	mrs_natural prev_phase = max_phase;
	avg_period = 0;
	
	for (int k=0; k < 4; k++)
	{
		start = max_phase - period - 2;
		end  =  max_phase - period + 2;
		max_sum_phase = 0.0;
		for (t=start; t <= end; t++)
		{
			sum_phase = in(0,t);
			if (sum_phase >= max_sum_phase) 
			{
				max_sum_phase = sum_phase;
				max_phase = t;
			}
		}
		// cout << "max_phase1 = " << max_phase << endl;
		avg_period += (prev_phase - max_phase);
		prev_phase = max_phase;
	}
	
	avg_period /= 4;
	
	cout << "avg_period = " << avg_period  << endl;
	cout << "avg_tempo = " << 2.0 * osrate_ * 60.0 / avg_period << endl;
	

	
	
	
	
	/* for (int k = 0; k < bhopSize; k++) 
	{


		
		if ((beats(0, k) == -0.5)&&(beats(0,0) != 0.0))
		{
			// cout << (sampleCount_  - delay) / (2.0 * osrate_) * 1000.0 << endl;
			// Audacity label output
			// cout << (sampleCount_  - delay) / (2.0 * osrate_) << "\t";
			// cout << (sampleCount_-1 - delay) / (2.0 * osrate_) << " b" << endl;
			
			beats(0,k) = -1.0;
		}
		sampleCount_ ++;
	}
	*/ 
	
	
}







	
