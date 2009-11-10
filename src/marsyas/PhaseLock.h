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

#ifndef MARSYAS_PHASELOCK_H
#define MARSYAS_PHASELOCK_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class PhaseLock
	\ingroup Processing Basic
    \brief Retrieves the N best {period, phase} hypotheses by affering the phase more suited to each period
	(by correlating each of the N*M hypotheses with the onset detection function within the initial induction window).
	Besides, it tries to infer the correct metre (duple or triple) by atributing the correspondent hierarchical set of initial scores,
	normalized in accordance to the weigth imposed by the time of the initial induction window.

	Input: Onset detection fucntion (uses Spectral Flux).
	Output: Matrix with the N ( = nrPeriodHyps) best {period, phase} hypotheses:
				
		[Periodi|bestPhaseForPeriodi|initialScorei]
		[  ...  |        ...        |     ...     ]
		[PeriodN|bestPhaseForPeriodN|initialScoreN]

	Controls:
	- \b mrs_realvec/beatHypotheses [w] : matrix with the raw hypothesis, giving M phases for each of the N predicted periods (+ the correspondent period salience).
	- \b mrs_natural/inductionTime [r] : time (in tick counts) dispended in the initial induction stage.
	- \b mrs_natural/nrPhasesPerPeriod [r] : Nr. of considered beat timming hypotheses (M).
	- \b mrs_natural/nrPeriodHyps[r] : Nr. of considered beat tempo hypotheses (N).
	- \b mrs_string/scoreFunc [r] : heuristics which conducts the beat tracking
*/


class PhaseLock: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_beatHypotheses_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_nrPeriodHyps_;
	MarControlPtr ctrl_nrPhasesPerPeriod_;
	MarControlPtr ctrl_scoreFunc_;

	mrs_string scoreFunc_;
	mrs_natural nrPhasesPerPeriod_;
	mrs_natural nrPeriodHyps_;
	mrs_natural hopSize_;
	mrs_natural inductionTime_;
	mrs_natural nInitHyp_;
	mrs_realvec beatHypotheses_;
	mrs_realvec hypSignals_;
	mrs_realvec sum_;
	mrs_natural t_;
	mrs_natural maxPeriod_;
	mrs_realvec maxSum_;
	mrs_realvec maxSumInd_;
	mrs_realvec period_;
	mrs_realvec bestPhasePerPeriod_;
	mrs_realvec metricalSalience_;
	mrs_realvec rawScore_;
	mrs_realvec rawScoreNorm_;
	mrs_realvec score_;
	mrs_realvec scoreNorm_;

	void myUpdate(MarControlPtr sender);
	mrs_realvec calcRelationalScore(mrs_bool duple, mrs_realvec rawScoreVec);
	mrs_natural metricalRelation(mrs_bool duple, mrs_bool triple, mrs_real period1, mrs_real period2);

public:
  PhaseLock(std::string name);
	PhaseLock(const PhaseLock& a);
  ~PhaseLock();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
