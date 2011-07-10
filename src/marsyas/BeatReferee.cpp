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
** \
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "BeatReferee.h"

using namespace std;
using namespace Marsyas;

#define NONE 0.0
#define BEAT 1.0
#define EVAL 2.0

#define INNER 3.0
#define OUTTER 4.0

#define MINIMUMREAL 0.000001 //(0.000001 minimum float recognized)
#define MINNEGATIVE -10000000000.0

BeatReferee::BeatReferee(mrs_string name):MarSystem("BeatReferee", name)
{
	addControls();

  	bestScore_ = MINNEGATIVE; //To allow initial negative scores
	bestAgentIndex_ = 0; //0 by default
	t_ = 0;
	outputCount_ = 0;
	lastBeatTime_ = -70;
	lastBeatPeriod_ = -1;
	inductionFinished_ = false;
	bestFinalAgent_ = -1;
}

BeatReferee::BeatReferee(const BeatReferee& a) : MarSystem(a)
{
	// For any MarControlPtr in a MarSystem 
	// it is necessary to perform this getctrl 
	// in the copy constructor in order for cloning to work 
	ctrl_mutedAgents_ = getctrl("mrs_realvec/mutedAgents");
	ctrl_inductionEnabler_ = getctrl("mrs_realvec/inductionEnabler");
	ctrl_firstHypotheses_ = getctrl("mrs_realvec/beatHypotheses");
	ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
	ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
	ctrl_srcFs_ = getctrl("mrs_real/srcFs");
	ctrl_maxPeriod_ = getctrl("mrs_natural/maxPeriod");
	ctrl_minPeriod_ = getctrl("mrs_natural/minPeriod");
	ctrl_agentControl_ = getctrl("mrs_realvec/agentControl");
	ctrl_beatDetected_ = getctrl("mrs_real/beatDetected");
	ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
	ctrl_obsoleteFactor_ = getctrl("mrs_real/obsoleteFactor");
	ctrl_lostFactor_ = getctrl("mrs_natural/lostFactor");
	ctrl_childrenScoreFactor_ = getctrl("mrs_real/childrenScoreFactor");
	ctrl_bestFactor_ = getctrl("mrs_real/bestFactor");
	ctrl_eqPhase_ = getctrl("mrs_natural/eqPhase");
	ctrl_eqPeriod_ = getctrl("mrs_natural/eqPeriod");
	ctrl_corFactor_ = getctrl("mrs_real/corFactor");
	ctrl_child1Factor_ = getctrl("mrs_real/child1Factor");
	ctrl_child2Factor_ = getctrl("mrs_real/child2Factor");
	ctrl_child3Factor_ = getctrl("mrs_real/child3Factor");
	ctrl_metricalChangeTime_ = getctrl("mrs_natural/metricalChangeTime");
	ctrl_backtrace_ = getctrl("mrs_bool/backtrace");
	ctrl_logFile_ = getctrl("mrs_bool/logFile");
	ctrl_logFileName_= getctrl("mrs_string/logFileName");
	ctrl_soundFileSize_= getctrl("mrs_natural/soundFileSize");
	ctrl_bestFinalAgentHistory_= getctrl("mrs_realvec/bestFinalAgentHistory");
	ctrl_nonCausal_ = getctrl("mrs_bool/nonCausal");
	
	t_ = a.t_;
	lastBeatPeriod_ = a.lastBeatPeriod_;
	historyCount_ = a.historyCount_;
	historyBeatTimes_ = a.historyBeatTimes_;
	lastBeatTime_ = a.lastBeatTime_;
	bestScore_ = a.bestScore_;
	bestAgentIndex_ = a.bestAgentIndex_;
	outputCount_ = a.outputCount_;
	statsPeriods_ = a.statsPeriods_;
	statsPhases_ = a.statsPhases_;
	statsAgentsLifeCycle_ = a.statsAgentsLifeCycle_;
	statsMuted_ = a.statsMuted_;
	inductionFinished_ = a.inductionFinished_;
	initPeriod_ = a.initPeriod_;
	corFactor_ = a.corFactor_;
	backtrace_ = a.backtrace_;
	logFile_ = a.logFile_;
}

BeatReferee::~BeatReferee()
{
}

MarSystem* 
BeatReferee::clone() const 
{
	return new BeatReferee(*this);
}

void 
BeatReferee::addControls()
{
	//Add specific controls needed by this MarSystem.
	addctrl("mrs_realvec/mutedAgents", realvec(), ctrl_mutedAgents_);
	addctrl("mrs_realvec/inductionEnabler", realvec(), ctrl_inductionEnabler_);
	addctrl("mrs_realvec/beatHypotheses", realvec(), ctrl_firstHypotheses_);
	addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
	setctrlState("mrs_natural/inductionTime", true);
	addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
	setctrlState("mrs_natural/hopSize", true);
	addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
	setctrlState("mrs_real/srcFs", true);
	addctrl("mrs_natural/maxPeriod", -1, ctrl_maxPeriod_);
	setctrlState("mrs_natural/maxPeriod", true);
	addctrl("mrs_natural/minPeriod", -1, ctrl_minPeriod_);
	setctrlState("mrs_natural/minPeriod", true);
	addctrl("mrs_realvec/agentControl", realvec(), ctrl_agentControl_);
	addctrl("mrs_real/beatDetected", 0.0, ctrl_beatDetected_);
	addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
	addctrl("mrs_real/obsoleteFactor", 0.8, ctrl_obsoleteFactor_);
	setctrlState("mrs_real/obsoleteFactor", true);
	addctrl("mrs_natural/lostFactor", 4, ctrl_lostFactor_);
	setctrlState("mrs_natural/lostFactor", true);
	addctrl("mrs_real/childrenScoreFactor", 0.01, ctrl_childrenScoreFactor_);
	setctrlState("mrs_real/childrenScoreFactor", true);
	addctrl("mrs_real/bestFactor", 1.1, ctrl_bestFactor_);
	setctrlState("mrs_real/bestFactor", true);
	addctrl("mrs_natural/eqPhase", 2, ctrl_eqPhase_);
	setctrlState("mrs_natural/eqPhase", true);
	addctrl("mrs_natural/eqPeriod", 1, ctrl_eqPeriod_);
	setctrlState("mrs_natural/eqPeriod", true);
	addctrl("mrs_real/corFactor", 0.5, ctrl_corFactor_);
	setctrlState("mrs_real/corFactor", true);
	addctrl("mrs_real/child1Factor", 2.0, ctrl_child1Factor_);
	setctrlState("mrs_real/child1Factor", true);
	addctrl("mrs_real/child2Factor", 0.5, ctrl_child2Factor_);
	setctrlState("mrs_real/child2Factor", true);
	addctrl("mrs_real/child3Factor", 1.0, ctrl_child3Factor_);
	setctrlState("mrs_real/child3Factor", true);
	addctrl("mrs_natural/metricalChangeTime", 0, ctrl_metricalChangeTime_);
	setctrlState("mrs_natural/metricalChangeTime", true);
	addctrl("mrs_bool/backtrace", false, ctrl_backtrace_);
	setctrlState("mrs_bool/backtrace", true);
	addctrl("mrs_bool/logFile", false, ctrl_debug_);
	setctrlState("mrs_bool/logFile", true);
	addctrl("mrs_string/logFileName", "log.txt", ctrl_logFileName_);
	setctrlState("mrs_string/logFileName", true);
	addctrl("mrs_natural/soundFileSize", 0, ctrl_soundFileSize_);
	setctrlState("mrs_natural/soundFileSize", true);
	addctrl("mrs_realvec/bestFinalAgentHistory", realvec(), ctrl_bestFinalAgentHistory_);
	addctrl("mrs_bool/nonCausal", false, ctrl_nonCausal_);
	setctrlState("mrs_bool/nonCausal", true);
}

void
BeatReferee::myUpdate(MarControlPtr sender)
{
	MRSDIAG("BeatReferee.cpp - BeatReferee:myUpdate");
	
	ctrl_onSamples_->setValue(1, NOUPDATE);
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	lostFactor_ = ctrl_lostFactor_->to<mrs_natural>();
	obsoleteFactor_ = ctrl_obsoleteFactor_->to<mrs_real>();
	childrenScoreFactor_ = ctrl_childrenScoreFactor_->to<mrs_real>();
	bestFactor_ = ctrl_bestFactor_->to<mrs_real>();
	eqPhase_ = ctrl_eqPhase_->to<mrs_natural>();
	eqPeriod_ = ctrl_eqPeriod_->to<mrs_natural>();
	corFactor_ = ctrl_corFactor_->to<mrs_real>();
	child1Factor_ = ctrl_child1Factor_->to<mrs_real>();
	child2Factor_ = ctrl_child2Factor_->to<mrs_real>();
	child3Factor_ = ctrl_child3Factor_->to<mrs_real>();
	metricalChangeTime_ = ctrl_metricalChangeTime_->to<mrs_natural>();
	backtrace_ = ctrl_backtrace_->to<mrs_bool>();
	logFile_ = ctrl_logFile_->to<mrs_bool>();
	logFileName_ = ctrl_logFileName_->to<mrs_string>();
	hopSize_ = ctrl_hopSize_->to<mrs_natural>();
	srcFs_ = ctrl_srcFs_->to<mrs_real>();
	maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
	minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();
	nonCausal_ = ctrl_nonCausal_->to<mrs_bool>();

	//inObservations_ = number of BeatAgents in the pool
	nrAgents_ = inObservations_;	
	historyCount_.create(nrAgents_); //1index for each agent
	agentsJustCreated_.create(nrAgents_); //to know which agents were created on current frame

	//historyBeatTimes_.create(nrAgents_,1000); //1index for each agent
	//statsPeriods_.create(nrAgents_,10000); //1index for each agent
	//statsPhases_.create(nrAgents_,10000); //1index for each agent
	//statsAgentsLifeCycle_.create(nrAgents_,10000);
	//statsAgentsScore_.create(nrAgents_,10000);
	//statsMuted_.create(nrAgents_, 10000);

	soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();
	//max possible nr. of beats in the analysed sound file (*1.2 - tolerance due to possible limit surpassing)
	maxNrBeats_ = (mrs_natural) (ceil(((mrs_real) soundFileSize_) / ((mrs_real) minPeriod_)) * 1.2);
	agentsHistory_.create(nrAgents_, maxNrBeats_);

	score_.create(nrAgents_); //1index for each agent
	lastPeriods_.create(nrAgents_); //1index for each agent
	lastPhases_.create(nrAgents_); //1index for each agent
	mutedAgents_.create(nrAgents_);//1index for each agent
	mutedAgentsTmp_.create(nrAgents_);//1index for each agent
	beatCounter_.create(nrAgents_);//1index for each agent
	initPeriod_.create(nrAgents_);//1index for each agent
	missedBeatsCount_.create(nrAgents_);//1index for each agent

	//Agent control indexed matrix
	//Each line(observation) accounts for an agent
	//[New/Update_Flag|Period|Phase|Timming]
	agentControl_.create(nrAgents_, 4);
	updControl(ctrl_agentControl_, agentControl_);

	
	for(int i = 0; i < nrAgents_; i++)
	{
		mutedAgents_(0, i) = 1.0; //initially all agents are muted
		mutedAgentsTmp_(0, i) = 1.0; //initially all agents are muted
	}
	
	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	inductionEnabler_ = ctrl_inductionEnabler_->to<mrs_realvec>();

	//wait timeBeforeKill secs before considering killing obsolete agents
	mrs_real timeBeforeKill = 0.0;
	timeBeforeKilling_ = (mrs_natural)(timeBeforeKill*srcFs_/hopSize_);
}

//Routine for backtracing the first beat of the first created agents (for backtrace mode)
mrs_natural
BeatReferee::calcFirstBacktracedBeat(mrs_natural initPeriod, mrs_natural initPhase)
{
	mrs_natural count = - (mrs_natural) ((initPhase / (mrs_real)initPeriod)-MINIMUMREAL);
	mrs_natural firstBeat = (initPhase + initPeriod * count);

	//cout << "Period: " << initPeriod << "; Phase: " << initPhase << "; firstBeat: " << firstBeat << endl;
	return firstBeat;
}

//Routine for calculating the first beat time of the first created agents (after induction):
//(for continuous signal - no backtracing (induction window offset -> first beat must occur after the induction stage)):
mrs_natural
BeatReferee::calcFirstBeat(mrs_natural initPeriod, mrs_natural initPhase)
{
	mrs_natural count = (mrs_natural) ceil((inductionTime_ - initPhase) /  (mrs_real)initPeriod);
	mrs_natural firstBeat = (initPhase + initPeriod * count);
	
	//Due to the compensation bellow we must grant that nextBeat is bigger than inductionTime_
	//If it is equal than we must postpone the nextBeat to a period after
	if(firstBeat == inductionTime_)
	{
		firstBeat += initPeriod;
	}

	//cout << "initPeriod:" << initPeriod << "; initPhase:" << initPhase << "; firstBeat:" << firstBeat << endl;

	return firstBeat;
}

void
BeatReferee::checkAndKillEqualAgents(mrs_natural agentIndex)
{		
	for(mrs_natural a = 0; a < nrAgents_; a++)
	{
		//if considered agent is not equal to the agent we want to compare
		//&& if the angent we want to compare is alive (i.e. exists) => then procede with comparison
		if(agentIndex != a && !mutedAgentsTmp_(a))
		{
			//2 agents are considered equal if their periods don't differ 
			//more than 10ms (eq. 1frame) and their phases no more than 20ms (eq. 2frames)			
			if((abs(lastPeriods_(agentIndex) - lastPeriods_(a)) <= eqPeriod_) && 
			   (abs((lastPhases_(agentIndex) - lastPhases_(a)) / lastPeriods_(agentIndex)) <= (eqPhase_/lastPeriods_(agentIndex))))
			{
				//cout << "EQ!: " << o << "=" << oo << endl;
				//From the two equal agents kill the one with lower score (if it isn't the current best agent)
				ostringstream motif;
				if(score_(agentIndex) >= score_(a))
				{
					if(a != bestAgentIndex_) 
					{
						//cout << "1-KILL Agent " << oo << " (" << score_(oo) << ") EQUAL TO Agent " << o << " (" << score_(o) << ")" << endl;
						//motif << "Equal to " << o;
						motif << "EQ";
						killAgent(a, motif.str(), agentIndex);
					}
					else 
					{
						//cout << "2-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
						//motif << "Equal to " << oo;
						motif << "EQ";
						killAgent(agentIndex, motif.str(), a);
						break; //in this case breaks because considered agent no longer exists.
					}
				}
				else
				{
					if(agentIndex != bestAgentIndex_) 
					{
						//cout << "3-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
						//motif << "Equal to " << oo;
						motif << "EQ";
						killAgent(agentIndex, motif.str(), a);
						break; //in this case breaks because considered agent no longer exists.
					}
					else 
					{
						//motif << "Equal to " << o;
						motif << "EQ";
						killAgent(a, motif.str(), agentIndex);
						//cout << "1-KILL Agent " << oo << " (" << score_(oo) << ") EQUAL TO Agent " << o << " (" << score_(o) << ")" << endl;
					}
				}
			}
		}
	}
}

mrs_natural
BeatReferee::getFirstAliveAgent()
{
	mrs_natural firstAlive = 0;
	for(mrs_natural a = 0; a < nrAgents_; a++)
	{
		if(!mutedAgentsTmp_(a))
		{
			firstAlive = a;
			break;
		}
	}
	return firstAlive;
}

//Get the current worst agent of the pool:
mrs_natural
BeatReferee::getWorstAgent()
{
	//By default lowest score = score from first agent
	mrs_natural firstAlive= getFirstAliveAgent();
	mrs_real lowestScore = score_(firstAlive);
	mrs_natural lowestIndex = firstAlive;
	
	for(mrs_natural a = firstAlive; a < nrAgents_; a++)
	{
		if(score_(a) < lowestScore)
		{
			lowestScore = score_(a);
			lowestIndex = a;
		}
	}

	//return worst agent
	return lowestIndex;
}

mrs_realvec 
BeatReferee::calculateNewHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error)
{
	mrs_natural newPeriod;
	mrs_natural nextBeat;
	/*
	//To avoid too small or big periods, or too distanced from agent's initial period:
	if(newPeriod > minPeriod_ && newPeriod < maxPeriod_ && 
	fabs(initPeriod_(agentIndex) - newPeriod) < 0.1*initPeriod_(agentIndex))
	*/

	newPeriod =  oldPeriod + ((mrs_natural) ((error*corFactor_) + ((error/abs(error)) * 0.5)));
	
	//To avoid too small or big periods
	if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		newPeriod = oldPeriod;

	nextBeat = prevBeat + newPeriod + ((mrs_natural) ((error*corFactor_) + ((error/abs(error))) * 0.5));

	
	/*
	  newPeriod = oldPeriod + error;
	  if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
	  newPeriod = oldPeriod;
	  nextBeat = prevBeat + newPeriod;
	*/
	//if(agentIndex == bestAgentIndex_)
	//	cout << "Agent" << agentIndex << ": Error:" << error << "; AgentPeriod:" << newPeriod << endl;

	//cout << "Agent " << agentIndex << "; oldPeriod: " << oldPeriod << "; NewPeriod: " << newPeriod <<
	//	"; NextBeat: "  << nextBeat << "; Error: " << error << "; Correction: " << correction << endl;
	//cout << "Agent " << agentIndex << " History: " << historyCount_(agentIndex) << endl;

	mrs_realvec newHypothesis(2);
	newHypothesis(0) = newPeriod;
	newHypothesis(1) = nextBeat;

	return newHypothesis;
}

mrs_natural
BeatReferee::calcNewPeriod(mrs_natural oldPeriod, mrs_natural error, mrs_real beta)
{
	//cout << "error: " << error << "; beta: " << beta << endl;
	mrs_natural newPeriod = oldPeriod + ((mrs_natural) ((error * beta) + (error/abs(error)) * 0.5));
	
	if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		newPeriod = oldPeriod;

	//cout << "oldPeriod: " << oldPeriod << "; newPeriod: " << newPeriod << "; Error: " << error << 
	//	"; Beta: " << beta << endl;

	return newPeriod;
}

mrs_realvec 
BeatReferee::calcChildrenHypothesis(mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error)
{
	mrs_natural nextBeat1 = -100, nextBeat2 = -100, nextBeat3 = -100;
	mrs_natural newPeriod1 = -100, newPeriod2 = -100, newPeriod3 = -100;
	mrs_realvec newHypotheses(3,3); //for 3 children

	//cout << "ChildFactor1:" << child1Factor_ << "; ChildFactor2:" << child2Factor_ << "; ChildFactor3:" << child3Factor_ << endl;

	//(if childiFactor = -1.0 then only phase adjusted)
	if(child1Factor_ == 2.0)
	{
		newPeriod1 = oldPeriod;
		nextBeat1 = prevBeat + newPeriod1 + error;
	}	
	else
	{
		newPeriod1 = calcNewPeriod(oldPeriod, error, child1Factor_);
		//if newPeriod surprasses bpm interval then do only full phase adjustment
		//if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		//	nextBeat = prevBeat + oldPeriod + error;
		//else
		nextBeat1 = prevBeat + newPeriod1 + ((mrs_natural) ((error*child1Factor_) + ((error/abs(error))) * 0.5));
		//nextBeat = prevBeat + newPeriod;
	}
	newHypotheses(0,0) = newPeriod1;
	newHypotheses(0,1) = nextBeat1;
	newHypotheses(0,2) = 1;
	
	if(child2Factor_ == 2.0)
	{
		newPeriod2 = oldPeriod;
		nextBeat2 = prevBeat + newPeriod2 + error;
	}
	else
	{
		/*
		  newPeriod = oldPeriod + error;
		  if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		  newPeriod = oldPeriod;
		  nextBeat = prevBeat + newPeriod;
		*/

		newPeriod2 = calcNewPeriod(oldPeriod, error, child2Factor_);
		//if newPeriod surprasses bpm interval then do only full phase adjustment
		//if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		//	nextBeat = prevBeat + oldPeriod + error;
		//else
		nextBeat2 = prevBeat + newPeriod2 + ((mrs_natural) ((error*child2Factor_) + ((error/abs(error))) * 0.5));
	}
	newHypotheses(1,0) = newPeriod2;
	newHypotheses(1,1) = nextBeat2;
	
	//To avoid creating equal agents:
	if(abs(newPeriod2 - newPeriod3) <= eqPeriod_ && abs(nextBeat2 - nextBeat1) <= eqPhase_)
		newHypotheses(1,2) = 0;
	else newHypotheses(1,2) = 1;

	if(child3Factor_ == 2.0)
	{
		newPeriod3 = oldPeriod;
		nextBeat3 = prevBeat + newPeriod3 + error;
	}
	else
	{
		newPeriod3 = calcNewPeriod(oldPeriod, error, child3Factor_);
		//if newPeriod surprasses bpm interval then do only full phase adjustment
		//if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		//	nextBeat = prevBeat + oldPeriod + error;
		//else
		nextBeat3 = prevBeat + newPeriod3 + ((mrs_natural) ((error*child3Factor_) + ((error/abs(error))) * 0.5));
	}
	newHypotheses(2,0) = newPeriod3;
	newHypotheses(2,1) = nextBeat3;

	//To avoid creating equal agents:
	if((abs(newPeriod3 - newPeriod1) <= eqPeriod_ && abs(nextBeat3 - nextBeat1) <= eqPhase_)
	   || (abs(newPeriod3 - newPeriod2) <= eqPeriod_ && abs(nextBeat3 - nextBeat2) <= eqPhase_))
		newHypotheses(2,2) = 0;
	else newHypotheses(2,2) = 1;

	//cout << "oldPeriod: " << oldPeriod << "; newPeriod: " << newPeriod << "; Error: " << error << 
	//	"; prevBeat: " << prevBeat << "; nextBeat: " << nextBeat << endl;

	return newHypotheses;
}

//Routine for creating new agents from existent one
void
BeatReferee::createChildren(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error, 
							mrs_real agentScore, mrs_real beatCount)
{
	//mrs_real deltaS = fabs(childrenScoreFactor_ * agentScore);
	//mrs_real newScore = agentScore - deltaS;
	
	mrs_real newScore;
	if(agentScore >= 0)
		newScore = agentScore * childrenScoreFactor_;
	else
		newScore = agentScore / childrenScoreFactor_;

	mrs_realvec newHypotheses = calcChildrenHypothesis(oldPeriod, prevBeat, error);

	//mrs_realvec newHypothesis = calculateNewHypothesis(agentIndex, oldPeriod, prevBeat, error);
	//setNewHypothesis(agentIndex, (mrs_natural) newHypothesis(0), (mrs_natural) newHypothesis(1));

	if(child1Factor_ != -1.0)
		createNewAgent((mrs_natural) newHypotheses(0,0), (mrs_natural) newHypotheses(0,1), newScore, beatCount, agentIndex);
	if(child2Factor_ != -1.0 && newHypotheses(1,2) == 1)
		createNewAgent((mrs_natural) newHypotheses(1,0), (mrs_natural) newHypotheses(1,1), newScore, beatCount, agentIndex);
	if(child3Factor_ != -1.0 && newHypotheses(2,2) == 1)
		createNewAgent((mrs_natural) newHypotheses(2,0), (mrs_natural) newHypotheses(2,1),  newScore, beatCount, agentIndex);

	//Display Created BeatAgent:
	//cout << "NEW AGENT(" << t_ << "-" << ((t_ * hopSize_) - (hopSize_/2)) / srcFs_ << ") (reqBy:" << agentIndex << 
	//") -> PrevBeat:" << prevBeat << " Period:" << oldPeriod << " NextBeat1:" << newHypotheses(1) << " NewPeriod1:" << 
	//	newHypotheses(0) << " NextBeat2:" << newHypotheses(3) << " NewPeriod2:" << newHypotheses(2) << 
	//	" NextBeat3:" << newHypotheses(5) << " NewPeriod3:" << newHypotheses(4) << 
	//	" Error:" << error << " Score:" << newScore << endl;					
}

//Routine for updating existent agent hypothesis
//(Used when beat of agent is found inside its inner tolerance with some error):
void
BeatReferee::updateAgentHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
								   mrs_natural prevBeat, mrs_natural error)
{
	//if agent effectively exists (for safechecking)
	if(!mutedAgentsTmp_(agentIndex))
	{
		mrs_realvec newHypothesis = calculateNewHypothesis(agentIndex, oldPeriod, prevBeat, error);
		setNewHypothesis(agentIndex, (mrs_natural) newHypothesis(0), (mrs_natural) newHypothesis(1));

		if(logFile_)
			debugAddEvent("UPDATE", agentIndex, (mrs_natural) lastPeriods_(agentIndex), 
						  (mrs_natural) newHypothesis(1), score_(agentIndex), bestScore_); 
	}

	//Display Updated BeatAgent:
	//cout << "UPDATING AGENT" << agentIndex <<" (" << t_ << ")" << " -> oldPeriod: " << oldPeriod << 
	//	" newPeriod: " << newHypothesis(0) << " prevBeat: " << prevBeat << " nextBeat: " << newHypothesis(1) <<  
	//	" Error: " << error << endl;
}

//Define new Hypothesis in indexed AgentControl Matrix:
void
BeatReferee::setNewHypothesis(mrs_natural agentIndex, mrs_natural newPeriod, mrs_natural nextBeat)
{
	agentControl_(agentIndex, 0) = 1.0; //is New or Updated
	agentControl_(agentIndex, 1) = newPeriod;
	agentControl_(agentIndex, 2) = nextBeat;
	agentControl_(agentIndex, 3) = t_;

	updControl(ctrl_agentControl_, agentControl_);

	lastPeriods_(agentIndex) = newPeriod;
}

void
BeatReferee::grantPoolSpace(mrs_natural callAgent, mrs_real newAgentScore)
{
	mrs_bool isAvailable = false;
	
	for(int a = 0; a < mutedAgentsTmp_.getSize(); a++)
	{
		if(mutedAgentsTmp_(a))
		{
			isAvailable = true;
			break;
		}
	}

	//if there are no free agents -> remove worst!
	if(!isAvailable)
	{
		mrs_natural agentInd2Kill = getWorstAgent();
		
		//only kills current worst agent in the pool if its score is smaller than the new agent score
		if(score_(agentInd2Kill) <= newAgentScore)
			killAgent(agentInd2Kill, "POOL", callAgent);
	}
}

//Generic routine for creating new agents given their hypotheses:
mrs_natural
BeatReferee::createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat, 
							mrs_real newScore, mrs_real beatCount, mrs_natural fatherAgent)
{
	//if father agent died in the same timestep as it requests a son creation then this request in unconsidered
	if(fatherAgent >= 0 && mutedAgentsTmp_(fatherAgent) == 1.0)
	{
		if(logFile_)
			debugAddEvent("CREATE_REF_KF", -1, newPeriod, firstBeat, newScore, bestScore_, fatherAgent); 
		return -1;
	}
	//or if the score of the new agent is obsolete the newAgent request is also unconsidered
	else if (t_ > timeBeforeKilling_ && newScore < bestScore_ && fabs(bestScore_-newScore) > 0.1 
			 && fabs(bestScore_ - newScore) > fabs(bestScore_ * obsoleteFactor_))
	{
		if(logFile_)
			debugAddEvent("CREATE_REF_SCORE", -1, newPeriod, firstBeat, newScore, bestScore_, fatherAgent); 
		return -1;
	}
	
	//Grant available space in the pool, by removing the worst agent, if needed
	grantPoolSpace(fatherAgent, newScore);

	mrs_natural returnCreatedAgent = -1;
	for(int a = 0; a < mutedAgentsTmp_.getSize(); a++)
	{
		//Look for first disabled agent (in temporary vector):		
		if(mutedAgentsTmp_(a))
		{
			//Activate new agent
			//mutedAgents_(a) = 0.0;
			//updControl(ctrl_mutedAgents_, mutedAgents_);

			mutedAgentsTmp_(a) = 0.0;
			updControl(ctrl_mutedAgents_, mutedAgentsTmp_);

			//Diplay Created BeatAgent:
			//cout << " CREATED: Agent " << a << endl;

			//Defines new hypothesis for this agent:
			setNewHypothesis(a, newPeriod, firstBeat);
			
			//Update score:
			score_(a) =  newScore;

			//beatCounter of this agent equals beatCounter of its father
			beatCounter_(a) = beatCount;

			//Update Agents' Periods and Phases (for equality checking)
			lastPeriods_(a) = newPeriod; //(Periods in frames)
			lastPhases_(a) = firstBeat; //(Phases in frames)
			
			initPeriod_(a) = newPeriod; //save agent's initial IBI
			missedBeatsCount_(a) = 0.0; //reset missed beats counter

			//statsPeriods_(a, t_) = newPeriod; 
			//statsPhases_(a, t_) = firstBeat;

			//statsAgentsLifeCycle_(a, t_) = 1.0;
			
			//force timing update when agent is created
			//(don't know why when a new agent is created its time, in agentControl, keeps
			//the time of the previous tick)
			agentControl_(a, 3) = t_+1;
			updControl(ctrl_agentControl_, agentControl_);

			returnCreatedAgent = a;

			agentsJustCreated_(a) = 1.0;

			//in nonCausalAnalysis keep agents history till the end of the analysis
			if(nonCausal_)
			{
				for(int i = 0; i < beatCount; i++)
					agentsHistory_(a, i) = agentsHistory_(fatherAgent, i);
			}


			if(logFile_)
				debugAddEvent("CREATE", a, (mrs_natural) lastPeriods_(a), 
							  (mrs_natural) lastPhases_(a), score_(a), bestScore_, fatherAgent);

			break;
		}
	}

	return returnCreatedAgent;
}

//Generic routine for killing agents:
void
BeatReferee::killAgent(mrs_natural agentIndex, mrs_string motif, mrs_natural callAgent)
{
	//Never kill a best agent (for enforcing security) -> a best agent must live until being replaced by a better one
	//&& agent effectively exists (for safechecking)
	if(agentIndex != bestAgentIndex_ && !mutedAgentsTmp_(agentIndex))
	{		
		//Diplay killed BeatAgent:
		//cout << "KILLED AGENT " << agentIndex << " (" << motif << ") With Score: " << score_(agentIndex) << " / " << bestScore_ << endl;

		if(logFile_)
		{
			ostringstream killMotif;
			killMotif << "KILL_" << motif;
			debugAddEvent(killMotif.str(), agentIndex, (mrs_natural) lastPeriods_(agentIndex),
						  (mrs_natural) lastPhases_(agentIndex), score_(agentIndex), bestScore_, callAgent);
		}

		//mutedAgents_(agentIndex) = 1.0;
		//updControl(ctrl_mutedAgents_, mutedAgents_);
		mutedAgentsTmp_(agentIndex) = 1.0;
		updControl(ctrl_mutedAgents_, mutedAgentsTmp_);


		score_(agentIndex) = 0.0;
		//beatCounter_(agentIndex) = 0.0;

		//statsAgentsLifeCycle_(agentIndex, t_) = -1;
		lastPeriods_(agentIndex) = 0.0; //(Periods in frames)
		lastPhases_(agentIndex) = 0.0; //(Phases in frames)

		missedBeatsCount_(agentIndex) = 0.0; //reset missed beats counter
	}
	else if(logFile_)
	{
		ostringstream killMotif;
		killMotif << "BESTKILL_" << motif;
		debugAddEvent(killMotif.str(), agentIndex, (mrs_natural) lastPeriods_(agentIndex),
					  (mrs_natural) lastPhases_(agentIndex), score_(agentIndex), bestScore_, callAgent);
	}
}

void
BeatReferee::calcAbsoluteBestScore()
{
	mrs_bool bestChange = false;
	mrs_natural firstAliveAgent = getFirstAliveAgent();
	mrs_real bestLocalScore = score_(firstAliveAgent);
	mrs_natural bestLocalAgent = firstAliveAgent;
	for (mrs_natural o = firstAliveAgent+1; o < nrAgents_; o++)
	{
		//Only consider alive agents:
		if(!mutedAgentsTmp_(o))
		{
			if(score_(o) > bestLocalScore)
			{
				bestLocalScore = score_(o);
				bestLocalAgent = o;
			}
		}
	}

	if((bestScore_ >= 0 && bestLocalScore > bestFactor_ * bestScore_) || 
	   (bestScore_ < 0 && bestLocalScore > bestScore_ / bestFactor_))
	{
		//Avoid metrical changes within requested time of the analysis
		if((t_ <= metricalChangeTime_) || (t_ > metricalChangeTime_) && 
		   (lastPeriods_(bestLocalAgent) > 0.7 * lastPeriods_(bestAgentIndex_)) && 
		   (lastPeriods_(bestLocalAgent) < 1.3 * lastPeriods_(bestAgentIndex_)))
		{
			
			if(logFile_)
				debugAddEvent("BEST", bestLocalAgent, (mrs_natural) lastPeriods_(bestLocalAgent), 
							  (mrs_natural) lastPhases_(bestLocalAgent), bestLocalScore, bestScore_);

			bestScore_ = bestLocalScore;
			bestAgentIndex_ = bestLocalAgent;
			bestFinalAgent_ = bestAgentIndex_;
		}
	}
	//cout << "Absolute Best Agent: " << bestAgentIndex_ << " BestScore: " << bestScore_ << endl;
}

void
BeatReferee::debugCreateFile()
{
	fstream outStream;
	outStream.open(logFileName_.c_str(), ios::out|ios::trunc);
	cout << "\nLog File: " << logFileName_ << endl;
	outStream.close();
}

void
BeatReferee::debugAddEvent(mrs_string ibtEvent, mrs_natural agentIndex, 
						   mrs_natural period, mrs_natural lastBeat, mrs_real score, mrs_real bestScore, mrs_natural callAgent)
{
	//event is appended in the end of the file
	fstream outStream;
	outStream.open(logFileName_.c_str(), ios::out|ios::app);
	outStream << ibtEvent << "|" << t_ << "|" << agentIndex << "|" << period << "|" << lastBeat
			  << "|" << score << "|" << bestScore << "|" << callAgent << endl;
	//cout << ibtEvent << "|" << t_ << "|" << agentIndex << "|" << period << "|" << lastBeat
	//	<< "|" << score << "|" << bestScore << "|" << callAgent << endl;
	outStream.close();
}

void 
BeatReferee::myProcess(realvec& in, realvec& out)
{
	//inititally desactivate all agents
	//(and deactivate induction's period estimation (ACF) -> computer expensive)
	if(t_ == 0 && !inductionFinished_)
	{
		updControl(ctrl_mutedAgents_, mutedAgents_);
		
		inductionEnabler_(0, 0) = 1.0; //diable = muted	
		updControl(ctrl_inductionEnabler_, inductionEnabler_);
	}

	//cout << "BR_t: " << t_ << endl;

	//while no best beat detected => outputs 0 (no beat)
	out.setval(0.0);
	ctrl_beatDetected_->setValue(0.0);
	agentControl_ = ctrl_agentControl_->to<mrs_realvec>();
	//always updates every agents' timming to equalize referee's (+1 for considering next time frame)
	for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
	{
		agentControl_(i, 3) = t_+1;
		updControl(ctrl_agentControl_, agentControl_); //AKIII!!!!
		agentsJustCreated_(i) = 0.0; //reset at all frames
	}
	//also pass timer value to the other MarSystems (+1 for considering next time frame)
	ctrl_tickCount_->setValue(t_+1);

	//cout << "BRef: " << t_ << "; Ind: " << inductionTime_ << endl;
	
	//realvec with the enable flag of all the BeatAgents in the pool
	//(0 -> agent active; 1 -> agent desactivated)
	mutedAgents_ = ctrl_mutedAgents_->to<mrs_realvec>();
	//created tmp agents vector for just updating mutedAgents_ vector in the next tick
	mutedAgentsTmp_ = ctrl_mutedAgents_->to<mrs_realvec>();

	//Display Input from BeatAgents:
	//cout << "INPUT (" << t_ << "): ";
/*	for(mrs_natural a = 0; a < nrAgents_; a++)
	cout << "\n" << a << "-> " << in(a, 0) << " | " << in(a, 1) << " | " << in(a, 2) << " | " << in(a, 3); 
	cout << endl;
*/

	//cout << "Beat1: " << firstHypotheses_(0,0) << " BPM1: " << firstHypotheses_(0,1) 
	//	<< " Beat2: " << firstHypotheses_(1,0) << " BPM2: " << firstHypotheses_(1,1) 
	//		<< " Beat3: " << firstHypotheses_(2,0) << " BPM3: " << firstHypotheses_(2,1) << endl;
	
	//After induction:
	if(inductionFinished_)
	{
		//INPUT: [Beat/Eval/None|Period|PrevBeat|Inner/Outter|Error|Score] -> inSamples_ = 6
		mrs_real agentFlag;
		mrs_natural agentPeriod;
		mrs_natural agentPrevBeat;
		mrs_real agentTolerance;
		mrs_natural agentError;
		mrs_real agentDScore;

		//Process all the agent pool for score, tempo and phase updates:
		for (mrs_natural o = 0; o < nrAgents_; o++)
		{
			agentFlag = in(o, 0);

			//statsPeriods_(o, t_) = lastPeriods_(o); 
			//statsPhases_(o, t_) = lastPhases_(o); 
			//statsAgentsScore_(o, t_) = score_(o);
			//statsMuted_(o, t_) = mutedAgents_(o);

			//Only consider alive agents that send new evaluation:
			//(Remind that each ouputed beat by the agents is only
			//evaluated at the end of its beat position + outterWindow tolerance)
			if(!mutedAgents_(o) && !mutedAgentsTmp_(o) && agentFlag == EVAL)
			{	
				agentDScore = in(o, 5);
				agentPeriod = (mrs_natural) in(o, 1);
				agentPrevBeat = (mrs_natural) in(o, 2);

				//Update Agents' Periods and Phases (for equality checking)
				lastPeriods_(o) = agentPeriod; //(Periods in frames)
				lastPhases_(o) = agentPrevBeat; //(Phases in frames)

				//statsPeriods_(o, t_) = agentPeriod; 
				//statsPhases_(o, t_) = agentPrevBeat; 

				//Update Agents' Score
				score_(o) += agentDScore;

				//if(o == bestAgentIndex_)
				//	cout << "Agent" << o << "-dScore:" << agentDScore << "; agentScore:" << score_(o) 
				//	<< "(" << bestScore_ << ")" << endl;

				//If the bestAgent drops or increases its score the BestScore has to drop correspondingly
				//if((score_(bestAgentIndex_) < bestScore_) || (score_(bestAgentIndex_) > bestScore_))
				if(score_(bestAgentIndex_) != bestScore_)
				{
					//cout << "t:" << t_ << "; Updating bestScore: " << "OLD: " << bestScore_ 
					//	<< " from Agent" << bestAgentIndex_ << "-NEW: " << score_(bestAgentIndex_) << endl;
					bestScore_ = score_(bestAgentIndex_);
					//calcAbsoluteBestScore();
				}
				//Kill agent if it is overly erroneous (many consecutive beats found outside the predicted tolerance inner window)
				if(missedBeatsCount_(o) >= lostFactor_)
				{
					killAgent(o, "LOST");
				}
				//Kill Agent if its score is bellow minimum (wait 5seconds before taking it into consideration)
				else if(t_ > timeBeforeKilling_ && score_(o) < bestScore_ && fabs(bestScore_-score_(o)) > 0.1 
						&& fabs(bestScore_ - score_(o)) > fabs(bestScore_ * obsoleteFactor_))
				{
					//cout << "Agent " << o << " Killed: Score below minimum (" << score_(o) << "\\" << bestScore_ << ")" << endl;
					killAgent(o, "OBS");
				}
				else //Checks if there are equal agents to current EVALuated one
					checkAndKillEqualAgents(o);

				//statsAgentsScore_(o, t_) = score_(o);

				//Display Scores from BeatAgents:
				/*	cout << "SCORES(" << t_ << ") (reqBy:" << o << "): " << endl;
					for(mrs_natural a = 0; a < nrAgents_; a++)
					cout << a << "-> " << score_(a) << " ";
					cout << endl;
				*/
			}
		}

		//Process all the agent pool for testing best beat and consider
		//the generation of new beats or the killing of existent ones
		for (mrs_natural o = 0; o < nrAgents_; o++)
		{
			//Only consider alive agents that send new evaluation:
			//(unconsider agents created in the current frame)
			if(!mutedAgents_(o) && !mutedAgentsTmp_(o) && !agentsJustCreated_(o))
			{
				agentFlag = in(o, 0);

				if(agentFlag == EVAL)
				{			
					agentPeriod = (mrs_natural) in(o, 1);
					agentPrevBeat = (mrs_natural) in(o, 2);
					agentTolerance = in(o, 3);
					agentError = (mrs_natural) in(o, 4);
					agentDScore = in(o, 5);

					//If a beat of an agent is inside its Inner tolerance but it has an error:
					//Update agent phase and period hypotheses:
					if(agentTolerance == INNER)
					{
						missedBeatsCount_(o) = 0.0; //reset (consecutive) missed beats counter for this agent 

						if(abs(agentError) > 0)
						{
							updateAgentHypothesis(o, agentPeriod, agentPrevBeat, agentError);
						}
						else if(logFile_)
							debugAddEvent("UPDATE", o, agentPeriod, agentPrevBeat, score_(o), bestScore_);
					}

					//If a beat of an agent is detected outside its Inner tolerance window:
					//Is created another agent that keeps this agent hypothesis, and updates its phase
					//and period to account for the given error:
					if(agentTolerance == OUTTER)
					{	
						//CREATE NEW AGENT WITH THIS NEW HYPOTHESIS KEEPING THE SCORE OF o-delta:
						//(the agent that generates a new one keeps its original hypothesis);
						//New agent must look for a new beat on the next (updated) period
						createChildren(o, agentPeriod, agentPrevBeat, agentError, score_(o), beatCounter_(o));
						//statsAgentsLifeCycle_(o, t_) = 2;

						missedBeatsCount_(o)++; //increment missed beats counter for this agent
					}
				}
			
				if(agentFlag == BEAT)
				{
					//in nonCausal analysis keep agents' beat history till the end of the analysis
					if(nonCausal_)
					{
						agentsHistory_(o, (mrs_natural) beatCounter_(o)) = t_;
					}

					//Increment beat counter of each agent
					beatCounter_(o)++;

					//Display Beats from BeatAgents:
					//cout << "Agent " << o << "(" << t_ << ") -> AgentScore: " << score_(o) << " BestAgent: " 
					//	<< bestAgentIndex_ << " BestScore: " << bestScore_ << " BeatCount: " << beatCounter_(o) << endl;
					
					//if best agent sends beat => outputs best beat
					if(o == bestAgentIndex_)
					{	
						//to avoid beats given by different agents distanced less then minPeriod frames (+ a tolerance = -1)
						if((t_ - lastBeatTime_) >= 0.7*lastBeatPeriod_)
						{
							//Display Outputted Beat:
							//cout << "OUTPUT(" << t_ << "-" << ((t_ * hopSize_) - (hopSize_/2)) / srcFs_ << "):Beat from Agent " << bestAgentIndex_ << 
							//	" BestScore:" << bestScore_ << " (" << score_(bestAgentIndex_) << ")";
							//cout << ":" << (60.0 / (t_ - lastBeatTime_)) * (ctrl_srcFs_->to<mrs_real>() / ctrl_hopSize_->to<mrs_natural>()) << "BPM" << endl;
							//cout << "BEST_AgentPeriod: " << lastPeriods_(bestAgentIndex_) << "(" << (t_ - lastBeatTime_) << ")" << endl;
							
							ctrl_beatDetected_->setValue(1.0);
							out.setval(1.0);
							//Updates agent history, which accounts for the total number
							//of the detected best (considered) beats of each agent:
							historyCount_(o)++;
							//historyBeatTimes_(o, outputCount_) = t_;
							outputCount_ ++;
							lastBeatTime_ = t_;
							lastBeatPeriod_ = (mrs_natural) lastPeriods_(o);

							if(logFile_)
								debugAddEvent("==========> BEAT", o, (mrs_natural) lastPeriods_(o), 
									(mrs_natural) lastPhases_(o), score_(o), bestScore_);					
						}
						else if(logFile_)
							debugAddEvent("BEAT CANCEL", o, (mrs_natural) lastPeriods_(o), 
										  (mrs_natural) lastPhases_(o), score_(o), bestScore_);

						calcAbsoluteBestScore();
					}
				}
			}
		}
	}

	//just before the induction take effect estimate periodicity (via ACF peaks)
	if(t_ == inductionTime_-1 && !inductionFinished_)
	{
		inductionEnabler_(0, 0) = 0.0; //enable = unmuted	
		updControl(ctrl_inductionEnabler_, inductionEnabler_);
	}

	//Create the first BeatAgents with new hypotheses just after Tseconds of induction:
	//(new agents' score shall be the average of all the already existent ones)
	else if(t_ == inductionTime_ && !inductionFinished_)
	{
		firstHypotheses_ = ctrl_firstHypotheses_->to<mrs_realvec>();

		//update values for eventual forced periods in induction stage
		maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
		minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();
		//max possible nr. of beats in the analysed sound file (*1.7 - tolerance due to possible limit surpassing)
		maxNrBeats_ = (mrs_natural) (ceil(((mrs_real) soundFileSize_) / ((mrs_real) minPeriod_)) * 1.7);
		agentsHistory_.create(nrAgents_, maxNrBeats_);

		mrs_natural newAgentPeriod;
		mrs_natural newAgentPhase;
		mrs_real newAgentScore;

		if(logFile_)
			debugCreateFile();

		//Updating initial bestScore and creating first agents:
		//(nr of initial agents equals nr of bpm hypotheses)
		for(mrs_natural i = 0; i < firstHypotheses_.getRows(); i++)
		{
			if((mrs_natural) firstHypotheses_(i,0) > 0) //only consider valid hypothesis:
			{
				//firstHypotheses_ -> matrix with i generated beat hypotheses + score, in the induction stage
				//[ BPMi | Beati | Score i ]
				newAgentPeriod = (mrs_natural) firstHypotheses_(i,0);
				newAgentPhase = (mrs_natural) firstHypotheses_(i,1);
				newAgentScore = firstHypotheses_(i,2);

				mrs_natural createdAgent = createNewAgent(newAgentPeriod, newAgentPhase, newAgentScore, 0);
				if(newAgentScore > bestScore_)
				{
					bestScore_ = newAgentScore;
					bestAgentIndex_ = createdAgent;
					bestFinalAgent_ = bestAgentIndex_;
					//cout << "Best Score From: " << i << "(" << bestScore_ << ")-Period: " << newAgentPeriod << endl;
				}
				
				//cout << "nAP: " << newAgentPeriod << "; nAPh: " << newAgentPhase << "; Score" << i << ": " << newAgentScore << endl;
				
				if(i == nrAgents_-1)
				{
					MRSWARN("Last hypotheses discarted because the nr. of hypotheses surpasses the nr. of BeatAgents");
					break;
				}

				if(newAgentPeriod == 0)
				{
					MRSWARN("Last hypotheses discarted because no more periods considered");
					break;
				}
			}

			//multiplied by sqrt(period) for disinflating the faster agents (with smaller periods)
			//bestScore_ *= sqrt((mrs_real)maxPeriod_);
		}
		
		//if backtrace restart tick counter
		//(timing updated here, for bactracing, for being contablized in the log file)
		if(backtrace_)
		{
			t_ = -1; //-1 because timer is updated by the end of the cycle (so next cycle is the actual 0)
			//put all MarSystem's timer to 0:
			for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
			{
				agentControl_(i, 3) = 0;
				updControl(ctrl_agentControl_, agentControl_);
			}
			ctrl_tickCount_->setValue(0);
		}
		//incremented for counting 5sec after induction before enabling obsolete killing
		else
		{
			timeBeforeKilling_+=inductionTime_;
			metricalChangeTime_+=inductionTime_;
		}

		//After finnishing induction disable induction functioning (tempoinduction Fanout):
		for(mrs_natural i = 0; i < inductionEnabler_.getSize(); i++)
			inductionEnabler_(0, i) = 1.0; //diable = muted
			
		updControl(ctrl_inductionEnabler_, inductionEnabler_);
		
		inductionFinished_ = true;
	}

	//MATLAB_PUT(agentsHistory_, "agentsHistory");
	//MATLAB_PUT(bestFinalAgent_, "bestFinalAgent");
	
	//if in nonCausalMode save last best agent history as the final output
	if(nonCausal_)
	{
		soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();
		//by the end of the analysis:
		if(t_ == soundFileSize_-1) //[! -1 for acouting on time of timing reset on backtrace mode]
		{
			mrs_natural bestAgentHistorySize = (mrs_natural) beatCounter_(bestFinalAgent_);
			bestFinalAgentHistory_.create(bestAgentHistorySize);
			for(int i = 0; i < bestAgentHistorySize; i++)
			{
				//cout << "I: " << i << "; History: " << agentsHistory_(bestFinalAgent_, i) << endl;
				bestFinalAgentHistory_(i) = agentsHistory_(bestFinalAgent_, i);
			}

			if(bestAgentHistorySize == 0) //if no beats detected [to avoid crash]
			{
				bestFinalAgentHistory_.create(1);
				bestFinalAgentHistory_(0) = -1.0;
			}

			updControl(ctrl_bestFinalAgentHistory_, bestFinalAgentHistory_);
			//MATLAB_PUT(bestFinalAgentHistory_, "bestAgentHistory");
		}
	}

	//MATLAB_PUT(statsAgentsScore_, "agentsScore");
	//MATLAB_PUT(in, "BeatAgents");
	
	//MATLAB_PUT(out, "BeatReferee");
	/*
	  MATLAB_PUT(historyBeatTimes_, "HistoryBeatTimes");
	  MATLAB_PUT(historyCount_, "HistoryCount");
	  MATLAB_PUT(statsPeriods_, "statsPeriods");
	  MATLAB_PUT(statsAgentsLifeCycle_, "statsAgentsLifeCycle");
	
	  MATLAB_PUT(bestScore_, "bestScore");
	  MATLAB_EVAL("bestAgentScore = [bestAgentScore bestScore];");
	*/
	//MATLAB_EVAL("FinalBeats = [FinalBeats, BeatReferee];");
	
	//MATLAB_EVAL("hold on;");
	//MATLAB_EVAL("plot(BeatAgentsTS)");
	//MATLAB_EVAL("stem(t, 1, 'r');");
	//MATLAB_EVAL("hold off;");

	t_++; //increment timer by the end of each cycle

	//cout << "size: " << soundFileSize_ << endl;
}
