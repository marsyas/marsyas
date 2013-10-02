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

#ifndef MARSYAS_TEMPOHYPOTHESES_H
#define MARSYAS_TEMPOHYPOTHESES_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class TempoHypotheses
	\ingroup Processing
    \brief Organizes a NN x 3 matrix given NN = N x M raw {period, phase, periodSalience} hypotheses.
	(if no periods were retrieved some manually defined periods will be assumed)

	Input: N period and M phase hypotheses retrieved from the analysis of the initial induction window:

		[x|Phasei|x|Phasei+1|                   ...                              |x|PhaseM]
		[PeriodSaliencei|Periodi|PeriodSaliencei+1|Periodi+1| ... |PeriodSalienceN|PeriodN]

	Output: (NxM) x 3 matrix with M phases for each of the N predicted periods:

			[  periodi  |  phasej  |  periodSaliencei  ]
			|  periodi  | phasej+1 |  periodSaliencei  |
			|    ...    |   ...    |        ...        |
			|  periodi  |  phaseM  |  periodSaliencei  |
			| periodi+1 |  phasej  | periodSaliencei+1 |
			| periodi+1 | phasej+1 | periodSaliencei+1 |
			|    ...    |   ...    |        ...        |
			|  periodN  |  phasej  |  periodSalienceN  |
			|    ...    |   ...    |        ...        |
			[  periodN  |  phaseM  ]  periodSalienceN  ]

	Controls:
	- \b mrs_natural/nPhases [r] : nr. of considered beat phase (in "frames"-ticks) hypotheses (M).
	- \b mrs_natural/nPeriods [r] : nr. of considered beat period (in "frames"-ticks) hypotheses (N).
	- \b mrs_natural/inductionTime [r] : time (in tick counts) dispended in the initial induction stage.
	- \b mrs_natural/hopSize [r] : hop size of the analysis.
	- \b mrs_real/srcFs [r] : input sampling rate.
	- \b mrs_natural/tickCount [r] : current considered time (tick count) updated from BeatReferee.
*/


class TempoHypotheses: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_nPhases_;
  MarControlPtr ctrl_nPeriods_;
  MarControlPtr ctrl_inductionTime_;
  MarControlPtr ctrl_srcFs_;
  MarControlPtr ctrl_hopSize_;
  MarControlPtr ctrl_tickCount_;
  MarControlPtr ctrl_dumbInduction_;
  MarControlPtr ctrl_dumbInductionRequest_;
  MarControlPtr ctrl_triggerInduction_;
  MarControlPtr ctrl_accSize_;
  MarControlPtr ctrl_maxPeriod_;
  MarControlPtr ctrl_minPeriod_;

  mrs_natural maxPeriod_;
  mrs_natural minPeriod_;
  mrs_natural accSize_;
  mrs_bool triggerInduction_;
  mrs_bool dumbInductionRequest_;
  mrs_natural hopSize_;
  mrs_real srcFs_;
  mrs_natural inductionSize_;
  mrs_natural timeElapsed_;
  mrs_natural nPhases_;
  mrs_natural nPeriods_;
  mrs_bool foundPeriods_;
  mrs_bool foundPhases_;
  mrs_natural manualBPMs_[10];
  void myUpdate(MarControlPtr sender);

public:
  TempoHypotheses(std::string name);
  TempoHypotheses(const TempoHypotheses& a);
  ~TempoHypotheses();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
