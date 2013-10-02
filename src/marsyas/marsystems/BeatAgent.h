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

#ifndef MARSYAS_BeatAgent_H
#define MARSYAS_BeatAgent_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class BeatAgent
	\ingroup Processing
    \brief Entity representing a given {period, phase} hypothesis regarding a train of beat positions paced by the following tempo.
	A set of initial agents are feeded by the initial hypothesis given by an initial tempo induction stage.
	Each BeatAgent is recursively evaluated in real-time, by a given score function (heuristics), around each predicted beat.
	According to the goodness-of-fit between each agent's prediction and the correspondent local maxima in the observed data
	(given by the onset detection function calculated by the Spectral Flux) the agent's current score is incremented or decremented,
	calling for the creation of new agents (children) when large discrepancies are observed.

	Input: Onset detection fucntion (uses Spectral Flux).
	Output Format: [Beat/Eval/None|Tempo|PrevBeat|Inner/Outter|Error|Score]

	Controls:
	- \b mrs_string/identity [r] : agent's identity according to its BeatAgent pool (Fanout) index.
	- \b mrs_realvec/agentControl [r] : feedback matrix, retrieved from BeatReferee, containing each BeatAgent's (updated) {period, phase} hypotheses, their lifecycle and timming situation.
	- \b mrs_string/scoreFunc [r] : heuristics which conducts the beat tracking, by causally evaluating the goodness-of-fit between each agent's prediction and the correspondent local maxima in the onset detection function.
	- \b mrs_real/lftOutterMargin [r] : the size of the outer half-window (in % of the IBI) before the predicted beat time.
	- \b mrs_real/rgtOutterMargin [r] : the size of the outer half-window (in % of the IBI) after the predicted beat time.
	- \b mrs_real/innerMargin [r] : inner tolerance window margin size (= half inner window size -> in ticks).
*/


class BeatAgent: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_identity_;
  MarControlPtr ctrl_timming_;
  MarControlPtr ctrl_agentControl_;
  MarControlPtr ctrl_scoreFunc_;
  MarControlPtr ctrl_lftOutterMargin_;
  MarControlPtr ctrl_rgtOutterMargin_;
  MarControlPtr ctrl_innerMargin_;
  MarControlPtr ctrl_maxPeriod_;
  MarControlPtr ctrl_minPeriod_;

  mrs_real periodFraction_;
  mrs_natural minPeriod_;
  mrs_natural maxPeriod_;
  mrs_real lftOutterMargin_;
  mrs_real rgtOutterMargin_;
  mrs_real innerMargin_;
  mrs_string scoreFunc_;
  mrs_natural innerWin_;
  mrs_natural outterWinLft_;
  mrs_natural outterWinRgt_;
  mrs_bool isNewOrUpdated_;
  mrs_string identity_;
  mrs_real fraction_;
  mrs_real score_;
  mrs_natural error_;
  mrs_natural curBeat_;
  mrs_natural prevBeat_;
  mrs_natural beatCount_;
  mrs_natural period_;
  mrs_natural phase_;
  mrs_natural timeElapsed_;
  mrs_natural lastBeatPoint_;
  mrs_real curBeatPointValue_;
  mrs_realvec history_;
  mrs_natural myIndex_;
  mrs_realvec agentControl_;

  void fillOutput(realvec& out, mrs_real flag, mrs_real tempo, mrs_real phase,
                  mrs_real tolerance, mrs_real error, mrs_real score);
  mrs_natural getChildIndex();
  void myUpdate(MarControlPtr sender);
  mrs_real calcDScoreCorr(realvec& in, mrs_natural maxInd);
  mrs_real calcDScoreCorrSquare(realvec& in);

public:
  BeatAgent(std::string name);
  BeatAgent(const BeatAgent& a);
  ~BeatAgent();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
