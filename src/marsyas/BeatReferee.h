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

#ifndef MARSYAS_BEATREFEREE_H
#define MARSYAS_BEATREFEREE_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class BeatReferee
	\ingroup Processing Basic
    \brief Based on the agents' score and beat detection
	Outputs most accurate beat and kills or creates new beat agents.
	Must have as input a pool (FanOut) of BeatAgents.

	Input Format (from each agent): [Beat/Eval/None|Tempo|PrevBeat|Inner/Outter|Error|Score]
	Output: [B/~B]

	- \b mrs_natural/muted = enable flags of each BeatAgent of the pool.
*/


class BeatReferee: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_mutedAgents_; //control with the enable flags of the overall BeatAgent pool.
	MarControlPtr ctrl_firstHypotheses_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_agentTimming_;
	MarControlPtr ctrl_inductionEnabler_;
	MarControlPtr ctrl_hopSize_;
	MarControlPtr ctrl_srcFs_;
	MarControlPtr ctrl_maxTempo_;
	MarControlPtr ctrl_minTempo_;
	MarControlPtr ctrl_agentControl_;
	MarControlPtr ctrl_beatDetected_;
	MarControlPtr ctrl_tickCount_;
	MarControlPtr ctrl_obsoleteFactor_;
	MarControlPtr ctrl_childFactor_;
	MarControlPtr ctrl_bestFactor_;
	MarControlPtr ctrl_eqPhase_;
	MarControlPtr ctrl_eqPeriod_;

	mrs_bool inductionFinnished_;
	mrs_natural nrAgents_;
	mrs_natural lastBeatTime_;
	mrs_natural minPeriod_;
	mrs_natural maxPeriod_;
	mrs_realvec agentControl_;
	mrs_realvec historyCount_; //number of (system) outputed beats by each agent
	mrs_realvec historyBeatTimes_;
	mrs_realvec statsPeriods_;
	mrs_realvec statsPhases_;
	mrs_realvec statsAgentsLifeCycle_;
	mrs_realvec statsAgentsScore_;
	mrs_realvec statsMuted_;
	mrs_realvec score_; //score of each agent
	mrs_realvec lastPeriods_; //last tempo hypothesis of each agent
	mrs_realvec lastPhases_; //last phase hypothesis of each agent
	mrs_realvec firstHypotheses_;
	mrs_realvec beatCounter_;
	mrs_realvec mutedAgents_;
	mrs_realvec inductionEnabler_;
	mrs_real bestScore_;
	mrs_natural inductionTime_;
	mrs_natural bestAgentIndex_;
	mrs_natural t_;
	mrs_natural outputCount_;
	mrs_real obsoleteFactor_;
	mrs_real childFactor_;
	mrs_real bestFactor_;
	mrs_natural eqPhase_;
	mrs_natural eqPeriod_;
	mrs_realvec initPeriod_;

	void myUpdate(MarControlPtr sender);

public:
  BeatReferee(std::string name);
	BeatReferee(const BeatReferee& a);
  ~BeatReferee();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
  void updateAgentHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
								   mrs_natural prevBeat, mrs_natural error);
  void createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat, 
	  mrs_real newScore, mrs_real beatCount);
  void grantPoolSpace();
  mrs_natural getWorstAgent();
  void setNewHypothesis(mrs_natural agentIndex, mrs_natural newPeriod, mrs_natural nextBeat);
  mrs_natural calculateFirstBeat(mrs_natural initPeriod, mrs_natural initPhase);
  void killAgent(mrs_natural agentIndex, mrs_string motif);
  void createChildren(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error, 
						mrs_real agentScore, mrs_real beatCount);
  mrs_realvec calculateNewHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
	  mrs_natural prevBeat, mrs_natural error);
  mrs_realvec calcChildrenHypothesis(mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error);
  mrs_natural getFirstAliveAgent();
  void calcAbsoluteBestScore();
  mrs_natural
calcNewPeriod(mrs_natural oldPeriod, mrs_natural error, mrs_real beta);
};

}//namespace Marsyas

#endif
