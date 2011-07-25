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
	\ingroup Processing
    \brief Central agency responsible for causally evaluating a pool of active BeatAgents around each beat prediction, 
	and selecting the best one at each time ("frame" - tick), based on a given heuristics (score function) which affers the 
	goodness-of-fit between each agent prediction and local maxima in the observed data 
	(given by the onset detection function calculated by the Spectral Flux).
	
	Given such, this entity is responsible for:
	- determine the best agent at each time, whose beats are validated;
	- adjust each agent's current beat rate (period) and phase hypotheses, in order to compensate eventual rhythmic deviations;
	- create new agents to follow alternative metrical paths (at most three children are created at each request);
	- terminate an agent operation if:
		* it has become obsolete (if its score insignificant in comparison to the best agent’s);
		* it is found to be duplicating the work of another agent.

	Input: Matrix with the beat\evaluation information from each BeatAgent of the pool (restricted to a defined maximum of M agents): 
	
		[Beati/Evali/Nonei|Periodi|PrevBeatTimei|Inneri/Outteri|Errori|dScorei]
		[       ...       |  ...  |     ...     |      ...     |  ... |  ...  ]
		[BeatM/EvalM/NoneM|PeriodM|PrevBeatTimeM|InnerM/OutterM|ErrorM|dScoreM]
	
	Output: [BEAT/~BEAT]

	Controls:
	- \b mrs_realvec/mutedAgents [rw] : BeatAgents' pool (Fanout) enable/disable flags vector.
	- \b mrs_realvec/inductionEnabler [rw] : Induction stage (Fanout) enable/disable flag vector.
	- \b mrs_realvec/firstHypotheses [r] : vector with the first N {period, phase} hypotheses retrieved from the beat induction stage
	- \b mrs_natural/inductionTime [r] : time (in tick counts) dispended in the initial induction stage.
	- \b mrs_natural/hopSize [r] : hop size of the analysis.
	- \b mrs_real/srcFs [r] : input sampling rate.
	- \b mrs_natural/maxTempo [r] : maximum tempo considered (in BPMs)
	- \b mrs_natural/minTempo [r] : minimum tempo considered (in BPMs)
	- \b mrs_realvec/agentControl [w] : feedback matrix for controlling each BeatAgent's {period, phase} hypotheses, their lifecycle and timming situation.
	- \b mrs_real/beatDetected [w] : flag for triggering beats to the noise (clicks) synthesizer.
	- \b mrs_natural/tickCount [w] : control for sharing the current considered time (tick count) with the annotation output block (BeatTimesSink).
	- \b mrs_real/obsoleteFactor [r] : an agent is killed if, at any time (after the initial 5secs), the difference between its score and the current bestScore is below obsoleteFactor * bestScore.
	- \b mrs_real/childrenScoreFactor [r] : (inertia1) each created agent imports its father score decremented by the current father's score multiplied by this factor.
	- \b mrs_real/bestFactor [r] : (inertia2) mutiple of the current bestScore an agent's score must have for replacing the current best agent.
	- \b mrs_natural/eqPhase [r] : phase (in ticks) threshold which identifies two agents as predicting the same phase (the worst duplicated agent is killed).
	- \b mrs_natural/eqPeriod [r] : period (IBI, in ticks) threshold which identifies two agents as predicting the same period (the worst duplicated agent is killed).
	- \b mrs_real/corFactor [r] : correction factor for compensating each agents' own {phase, period} hypothesis errors.
	- \b mrs_real/child1Factor [r] : correction factor (error proportion) of child1 for compensating its father's {phase, period} hypothesis - when error outside innerWindow tolerance.
	- \b mrs_real/child2Factor [r] : correction factor (error proportion) of child2 for compensating its father's {phase, period} hypothesis - when error outside innerWindow tolerance.
	- \b mrs_real/child3Factor [r] : correction factor (error proportion) of child3 for compensating its father's {phase, period} hypothesis - when error outside innerWindow tolerance.
	- \b mrs_natural/metricalChangeTime [r] : initial time (in ticks) allowed for eventual metrical changes within tracking.
	- \b mrs_bool/backtrace [r] : flag for backtracing the analysis to the beginning, after the induction stage.
*/


class BeatReferee: public MarSystem
{
private: 
  //Add specific controls needed by this MarSystem.
	void addControls();
	MarControlPtr ctrl_mutedAgents_; //control with the enable flags of the overall BeatAgent pool.
	MarControlPtr ctrl_firstHypotheses_;
	MarControlPtr ctrl_inductionTime_;
	MarControlPtr ctrl_inductionEnabler_;
	MarControlPtr ctrl_hopSize_;
	MarControlPtr ctrl_srcFs_;
	MarControlPtr ctrl_maxPeriod_;
	MarControlPtr ctrl_minPeriod_;
	MarControlPtr ctrl_agentControl_;
	MarControlPtr ctrl_beatDetected_;
	MarControlPtr ctrl_tickCount_;
	MarControlPtr ctrl_obsoleteFactor_;
	MarControlPtr ctrl_childrenScoreFactor_;
	MarControlPtr ctrl_bestFactor_;
	MarControlPtr ctrl_eqPhase_;
	MarControlPtr ctrl_eqPeriod_;
	MarControlPtr ctrl_corFactor_;
	MarControlPtr ctrl_child1Factor_;
	MarControlPtr ctrl_child2Factor_;
	MarControlPtr ctrl_child3Factor_;
	MarControlPtr ctrl_metricalChangeTime_;
	MarControlPtr ctrl_backtrace_;
	MarControlPtr ctrl_logFile_;
	MarControlPtr ctrl_logFileName_;
	MarControlPtr ctrl_lostFactor_;
	MarControlPtr ctrl_soundFileSize_;
	MarControlPtr ctrl_bestFinalAgentHistory_;
	MarControlPtr ctrl_nonCausal_;

	mrs_realvec agentsJustCreated_;
	mrs_realvec bestFinalAgentHistory_;
	mrs_natural bestFinalAgent_;
	mrs_bool nonCausal_;
	mrs_natural maxNrBeats_;
	mrs_realvec agentsHistory_;
	mrs_natural soundFileSize_;
	mrs_natural lostFactor_;
	mrs_string logFileName_;
	mrs_bool inductionFinished_;
	mrs_bool backtrace_;
	mrs_bool logFile_;
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
	mrs_realvec mutedAgentsTmp_;
	mrs_realvec inductionEnabler_;
	mrs_real bestScore_;
	mrs_natural inductionTime_;
	mrs_natural bestAgentIndex_;
	mrs_natural t_;
	mrs_natural outputCount_;
	mrs_real obsoleteFactor_;
	mrs_real childrenScoreFactor_;
	mrs_real bestFactor_;
	mrs_natural eqPhase_;
	mrs_natural eqPeriod_;
	mrs_realvec initPeriod_;
	mrs_natural hopSize_;
	mrs_real srcFs_;
	mrs_real corFactor_;
	mrs_real child1Factor_;
	mrs_real child2Factor_;
	mrs_real child3Factor_;
	mrs_real metricalChangeTime_;
	mrs_natural timeBeforeKilling_;
	mrs_natural lastBeatPeriod_;
	mrs_realvec missedBeatsCount_;

	void myUpdate(MarControlPtr sender);

public:
  BeatReferee(std::string name);
	BeatReferee(const BeatReferee& a);
  ~BeatReferee();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
  void updateAgentHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
								   mrs_natural prevBeat, mrs_natural error);
  mrs_natural createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat, 
	  mrs_real newScore, mrs_real beatCount, mrs_natural fatherAgent = -1);
  void grantPoolSpace(mrs_natural callAgent, mrs_real newAgentScore);
  mrs_natural getWorstAgent();
  void setNewHypothesis(mrs_natural agentIndex, mrs_natural newPeriod, mrs_natural nextBeat);
  mrs_natural calcFirstBeat(mrs_natural initPeriod, mrs_natural initPhase);
  mrs_natural calcFirstBacktracedBeat(mrs_natural initPeriod, mrs_natural initPhase);
  void killAgent(mrs_natural agentIndex, mrs_string motif, mrs_natural callAgent = -1);
  void createChildren(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error, 
						mrs_real agentScore, mrs_real beatCount);
  mrs_realvec calculateNewHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
	  mrs_natural prevBeat, mrs_natural error);
  mrs_realvec calcChildrenHypothesis(mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error);
  mrs_natural getFirstAliveAgent();
  void calcAbsoluteBestScore();
  mrs_natural calcNewPeriod(mrs_natural oldPeriod, mrs_natural error, mrs_real beta);
  void debugCreateFile();
  void debugAddEvent(mrs_string ibtEvent, mrs_natural agentIndex, mrs_natural period, mrs_natural lastBeat,
	  mrs_real score, mrs_real bestScore, mrs_natural callAgent = -1);
  void checkAndKillEqualAgents(mrs_natural agentIndex);
};

}//namespace Marsyas

#endif
