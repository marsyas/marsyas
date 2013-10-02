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

#include <marsyas/system/MarSystem.h>
#include <string.h>
using namespace std;

namespace Marsyas
{
/**
	\ingroup Processing
    \brief Retrieves the N best {period, phase} hypotheses by affering the phase more suited to each period
	(by correlating each of the N*M hypotheses with the onset detection function within the initial induction window).
	Besides, it tries to infer the correct metre (duple or triple) by atributing the correspondent hierarchical set of initial scores,
	normalized in accordance to the weigth imposed by the time of the initial induction window.
	It supports ground-truth mode which inputs the first two beats retrieved from a ground-truth beat times annotation file,
	for initially assigning (only) one agent with the ground-truth period and phase.

	Input: Onset detection fucntion (uses Spectral Flux).
	Output: Matrix with the N ( = nrPeriodHyps) best {period, phase} hypotheses:

		[Periodi|bestPhaseForPeriodi|initialScorei]
		[  ...  |        ...        |     ...     ]
		[PeriodN|bestPhaseForPeriodN|initialScoreN]

	Controls:
	- \b mrs_realvec/beatHypotheses [w] : matrix with the raw hypothesis, giving M phases for each of the N predicted periods (+ the correspondent period salience).
	- \b mrs_natural/inductionTime [r] : time (in tick counts) dispended in the initial induction stage.
	- \b mrs_natural/nrPhasesPerPeriod [r] : Nr. of considered beat timming hypotheses (M).
	- \b mrs_natural/nrPeriodHyps[r] : nr. of considered beat tempo hypotheses (N).
	- \b mrs_string/scoreFunc [r] : heuristics which conducts the beat tracking.
	- \b mrs_natural/hopSize [r] : hop size of the analysis.
	- \b mrs_real/srcFs [r] : input sampling rate.
	- \b mrs_string/mode [r] : mode of operation - regular operation or annotated induction (retrieved from the first two beats of the input beat times file).
	- \b mrs_string/gtBeatsFile [r] : path of the input beat times file.
	- \b mrs_natural/tickCount [r] : current considered time (tick count) updated from BeatReferee.
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
  MarControlPtr ctrl_tickCount_;
  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_hopSize_;
  MarControlPtr ctrl_srcFs_;
  MarControlPtr ctrl_gtBeatsFile_;
  MarControlPtr ctrl_backtrace_;
  MarControlPtr ctrl_innerMargin_;
  MarControlPtr ctrl_lftOutterMargin_;
  MarControlPtr ctrl_rgtOutterMargin_;
  MarControlPtr ctrl_corFactor_;
  MarControlPtr ctrl_maxPeriod_;
  MarControlPtr ctrl_minPeriod_;
  MarControlPtr ctrl_adjustment_;
  MarControlPtr ctrl_dumbInduction_;
  MarControlPtr ctrl_inductionOut_;
  MarControlPtr ctrl_triggerInduction_;
  MarControlPtr ctrl_curBestScore_;
  MarControlPtr ctrl_triggerBestScoreFactor_;

  mrs_real lastGTIBI_;
  mrs_real triggerBestScoreFactor_;
  mrs_real lastGTBeatTime_;
  mrs_real curBestScore_;
  mrs_bool triggerInduction_;
  mrs_bool gtAfter2ndBeat_;
  mrs_string inductionOut_;
  mrs_bool dumbInduction_;
  mrs_real adjustment_;
  mrs_natural minPeriod_;
  mrs_natural maxPeriod_;
  mrs_natural innerMargin_;
  mrs_string gtBeatsFile_;
  mrs_bool inductionFinished_;
  mrs_natural hopSize_;
  mrs_real srcFs_;
  mrs_string mode_;
  mrs_string line_;
  ifstream inStream_;
  mrs_real gtScore_;
  mrs_natural gtInitPeriod_;
  mrs_natural gtLastPeriod_;
  mrs_string sourceFile_;
  mrs_natural gtInitPhase_;
  mrs_natural gtLastPhase_;
  mrs_string scoreFunc_;
  mrs_natural nrPhasesPerPeriod_;
  mrs_natural nrPeriodHyps_;
  mrs_natural inductionTime_;
  mrs_natural nInitHyp_;
  mrs_realvec beatHypotheses_;
  mrs_realvec hypSignals_;
  mrs_realvec firstBeatPoint_;
  mrs_realvec trackingScore_;
  mrs_natural timeElapsed_;
  mrs_realvec maxLocalTrackingScore_;
  mrs_realvec maxLocalTrackingScoreInd_;
  mrs_realvec metricalSalience_;
  mrs_realvec rawScore_;
  mrs_realvec rawScoreNorm_;
  mrs_realvec metricalRelScore_;
  mrs_realvec scoreNorm_;
  mrs_realvec initPhases_;
  mrs_realvec lastPhases_;
  mrs_realvec initPeriods_;
  mrs_realvec lastPeriods_;
  mrs_realvec beatCount_;
  mrs_bool backtrace_;
  mrs_natural outterWinLft_;
  mrs_natural outterWinRgt_;
  mrs_real lftOutterMargin_;
  mrs_real rgtOutterMargin_;
  mrs_real corFactor_;

  void myUpdate(MarControlPtr sender);
  mrs_real calcRelationalScore(mrs_natural i, mrs_realvec rawScoreVec);
  mrs_natural metricalRelation(mrs_real period1, mrs_real period2);
  void regularFunc(realvec& in, realvec& out);
  mrs_realvec readGTFile(mrs_string gtFilePath);
  void handleGTHypotheses(realvec& in, realvec& out, mrs_string gtFilePath, mrs_realvec gtHypotheses);
  mrs_realvec GTInitialization(realvec& in, realvec& out, mrs_natural gtIniPhase,
                               mrs_natural gtInitPeriod);
  void forceInitPeriods(mrs_string mode);

public:
  PhaseLock(std::string name);
  PhaseLock(const PhaseLock& a);
  ~PhaseLock();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
