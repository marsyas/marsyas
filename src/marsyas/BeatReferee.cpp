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

BeatReferee::BeatReferee(string name):MarSystem("BeatReferee", name)
{
	addControls();

  	bestScore_ = 0.0;
	bestAgentIndex_ = 0; //0 by default
	t_ = 0;
	outputCount_ = 0;
	lastBeatTime_ = 0;
	inductionFinnished_ = false;
}

BeatReferee::BeatReferee(const BeatReferee& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
	t_ = a.t_;
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
	inductionFinnished_ = a.inductionFinnished_;
	initPeriod_ = a.initPeriod_;

	ctrl_mutedAgents_ = getctrl("mrs_realvec/mutedAgents");
	ctrl_inductionEnabler_ = getctrl("mrs_realvec/inductionEnabler");
	ctrl_firstHypotheses_ = getctrl("mrs_realvec/beatHypotheses");
	ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
	ctrl_agentTimming_ = getctrl("mrs_natural/agentTimming");
	ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
	ctrl_srcFs_ = getctrl("mrs_real/srcFs");
	ctrl_maxTempo_ = getctrl("mrs_natural/maxTempo");
	ctrl_minTempo_ = getctrl("mrs_natural/minTempo");
	ctrl_agentControl_ = getctrl("mrs_realvec/agentControl");
	ctrl_beatDetected_ = getctrl("mrs_real/beatDetected");
	ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
	ctrl_obsoleteFactor_ = getctrl("mrs_real/obsoleteFactor");
	ctrl_childFactor_ = getctrl("mrs_real/childFactor");
	ctrl_bestFactor_ = getctrl("mrs_real/bestFactor");
	ctrl_eqPhase_ = getctrl("mrs_natural/eqPhase");
	ctrl_eqPeriod_ = getctrl("mrs_natural/eqPeriod");
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
	addctrl("mrs_natural/inductionTime", 1, ctrl_inductionTime_);
	addctrl("mrs_natural/agentTimming", 0, ctrl_agentTimming_);
	addctrl("mrs_natural/hopSize", 1, ctrl_hopSize_);
	setctrlState("mrs_natural/hopSize", true);
	addctrl("mrs_real/srcFs", 1.0, ctrl_srcFs_);
	setctrlState("mrs_real/srcFs", true);
	addctrl("mrs_natural/maxTempo", 1, ctrl_maxTempo_);
	setctrlState("mrs_natural/maxTempo", true);
	addctrl("mrs_natural/minTempo", 1, ctrl_minTempo_);
	setctrlState("mrs_natural/minTempo", true);
	addctrl("mrs_realvec/agentControl", realvec(), ctrl_agentControl_);
	addctrl("mrs_real/beatDetected", 0.0, ctrl_beatDetected_);
	addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
	addctrl("mrs_real/obsoleteFactor", 1.5, ctrl_obsoleteFactor_);
	setctrlState("mrs_real/obsoleteFactor", true);
	addctrl("mrs_real/childFactor", 0.05, ctrl_childFactor_);
	setctrlState("mrs_real/childFactor", true);
	addctrl("mrs_real/bestFactor", 1.15, ctrl_bestFactor_);
	setctrlState("mrs_real/bestFactor", true);
	addctrl("mrs_natural/eqPhase", 2, ctrl_eqPhase_);
	setctrlState("mrs_natural/eqPhase", true);
	addctrl("mrs_natural/eqPeriod", 1, ctrl_eqPeriod_);
	setctrlState("mrs_natural/eqPeriod", true);
}

void
BeatReferee::myUpdate(MarControlPtr sender)
{
	MRSDIAG("BeatReferee.cpp - BeatReferee:myUpdate");
	
	ctrl_onSamples_->setValue(1, NOUPDATE);
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	obsoleteFactor_ = ctrl_obsoleteFactor_->to<mrs_real>();
	childFactor_ = ctrl_childFactor_->to<mrs_real>();
	bestFactor_ = ctrl_bestFactor_->to<mrs_real>();
	eqPhase_ = ctrl_eqPhase_->to<mrs_natural>();
	eqPeriod_ = ctrl_eqPeriod_->to<mrs_natural>();

	//inObservations_ = number of BeatAgents in the pool
	nrAgents_ = inObservations_;
		
	historyCount_.create(nrAgents_); //1index for each agent
	//historyBeatTimes_.create(nrAgents_,1000); //1index for each agent
	//statsPeriods_.create(nrAgents_,10000); //1index for each agent
	//statsPhases_.create(nrAgents_,10000); //1index for each agent
	//statsAgentsLifeCycle_.create(nrAgents_,10000);
	//statsAgentsScore_.create(nrAgents_,10000);
	//statsMuted_.create(nrAgents_, 10000);
	score_.create(nrAgents_); //1index for each agent
	lastPeriods_.create(nrAgents_); //1index for each agent
	lastPhases_.create(nrAgents_); //1index for each agent
	mutedAgents_.create(nrAgents_);//1index for each agent
	beatCounter_.create(nrAgents_);//1index for each agent
	initPeriod_.create(nrAgents_);//1index for each agent

	//Agent control indexed matrix
	//Each line(observation) accounts for an agent
	//[New/Update_Flag|Period|Phase|Timming]
	agentControl_.create(nrAgents_, 4);
	updctrl(ctrl_agentControl_, agentControl_);

	for(int i = 0; i < nrAgents_; i++)
	{
		historyCount_(i) = 1.0;
		score_(i) = 0.0;
		lastPeriods_(i) = 0.0;
		lastPhases_(i) = 0.0;
		mutedAgents_(0, i) = 1.0;
		beatCounter_(i) = 0.0;
		initPeriod_(i) = 0.0;
	}

	inductionEnabler_ = ctrl_inductionEnabler_->to<mrs_realvec>();

	//Calculate minimumPeriod (eq. considered possible maximumTempo)
	mrs_natural hopSize = ctrl_hopSize_->to<mrs_natural>();
	mrs_real srcFs = ctrl_srcFs_->to<mrs_real>();

	mrs_natural maxTempo = ctrl_maxTempo_->to<mrs_natural>();
	mrs_natural minTempo = ctrl_minTempo_->to<mrs_natural>();
	minPeriod_ = (mrs_natural) floor((mrs_real) 60 / (maxTempo * hopSize) * srcFs);
	maxPeriod_ = (mrs_natural) ceil((mrs_real) 60 / (minTempo * hopSize) * srcFs);
}

//used like this because signal restarted:
mrs_natural
BeatReferee::calculateFirstBeat(mrs_natural initPeriod, mrs_natural initPhase)
{
	return initPhase;
}

mrs_natural
BeatReferee::getFirstAliveAgent()
{
	mrs_natural firstAlive = 0;
	for(mrs_natural a = 0; a < nrAgents_; a++)
	{
		if(!mutedAgents_(a))
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
	mrs_natural nextBeat;
	mrs_real correctionMax = 8.0; //2*innerWindow
	mrs_real correctionMin = 1.0;
	mrs_real correction = 4.0;
	
	if(historyCount_(agentIndex) < correctionMax && historyCount_(agentIndex) > correctionMin)
		correction = historyCount_(agentIndex);
	else 
	{
		if(historyCount_(agentIndex) > correctionMax)
			correction = correctionMax;
		
		if(historyCount_(agentIndex) < correctionMin)
			correction = correctionMin;
	}

	//mrs_natural newPeriod =  oldPeriod + ((mrs_natural) ((error/correction)+0.5));
	mrs_natural newPeriod =  oldPeriod + ((mrs_natural) ((error/2.0)+0.5));
	
	//To avoid too small or big periods, or too distanced from agent's initial period:
	if(newPeriod > minPeriod_ && newPeriod < maxPeriod_ && 
		abs(initPeriod_(agentIndex) - newPeriod) < 0.1*initPeriod_(agentIndex))
	{
		//nextBeat = prevBeat + newPeriod + ((mrs_natural) ((error/correction)+0.5));
		nextBeat = prevBeat + newPeriod + error;
		nextBeat = prevBeat + newPeriod;
	}
	else 
	{
		nextBeat = prevBeat + oldPeriod;
		newPeriod = oldPeriod;
	}

	//cout << "Agent " << agentIndex << "; oldPeriod: " << oldPeriod << "; NewPeriod: " << newPeriod <<
	//	"; Error: " << error << "; Correction: " << correction << endl;
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
	mrs_natural newPeriod = oldPeriod +  (mrs_natural) ((error * beta)+0.5);
	if(newPeriod < minPeriod_ || newPeriod > maxPeriod_)
		newPeriod = oldPeriod;

	//cout << "oldPeriod: " << oldPeriod << "; newPeriod: " << newPeriod << "; Error: " << error << 
	//	"; Beta: " << beta << endl;

	return newPeriod;
}

mrs_realvec 
BeatReferee::calcChildrenHypothesis(mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error)
{
	mrs_natural nextBeat;
	mrs_natural newPeriod;
	mrs_realvec newHypotheses(6); //for 3 children
	mrs_real beta;

	newPeriod = oldPeriod;
	nextBeat = prevBeat + newPeriod + error;	
	newHypotheses(0) = newPeriod;
	newHypotheses(1) = nextBeat;
	
	beta = 0.5;
	newPeriod = calcNewPeriod(oldPeriod, error, beta);
	nextBeat = prevBeat + newPeriod + error;
	newHypotheses(2) = newPeriod;
	newHypotheses(3) = nextBeat;

	//cout << "oldPeriod: " << oldPeriod << "; newPeriod: " << newPeriod << "; Error: " << error << 
	//	"; prevBeat: " << prevBeat << "; nextBeat: " << nextBeat << endl;

	beta = 0.125; //0.125
	newPeriod = calcNewPeriod(oldPeriod, error, beta);
	nextBeat = prevBeat + newPeriod + error;
	newHypotheses(4) = newPeriod;
	newHypotheses(5) = nextBeat;

	return newHypotheses;
}

//Routine for creating new agents from existent one
void
BeatReferee::createChildren(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error, 
						mrs_real agentScore, mrs_real beatCount)
{
	mrs_real deltaS = abs(childFactor_ * agentScore);
	mrs_real newScore = agentScore - deltaS;

	mrs_realvec newHypotheses = calcChildrenHypothesis(oldPeriod, prevBeat, error);
	
	createNewAgent((mrs_natural) newHypotheses(0), (mrs_natural) newHypotheses(1), newScore, beatCount);
	createNewAgent((mrs_natural) newHypotheses(2), (mrs_natural) newHypotheses(3), newScore, beatCount);
	createNewAgent((mrs_natural) newHypotheses(4), (mrs_natural) newHypotheses(5),  newScore, beatCount);

	//Display Created BeatAgent:
/*	cout << "NEW AGENT(" << t_ <<") (reqBy:" << agentIndex << ") -> PrevBeat: " << prevBeat << " Period: " 
		<< oldPeriod << " NextBeat: " << newHypothesis(1) << " NewPeriod: " << newHypothesis(0) << 
		" Error: " << error << " Score: " << newScore;				
*/
}

//Routine for updating existent agent hypothesis
//(Used when beat of agent is found inside its inner tolerance with some error):
void
BeatReferee::updateAgentHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
								   mrs_natural prevBeat, mrs_natural error)
{
	mrs_realvec newHypothesis = calculateNewHypothesis(agentIndex, oldPeriod, prevBeat, error);
	setNewHypothesis(agentIndex, (mrs_natural) newHypothesis(0), (mrs_natural) newHypothesis(1));

	//Display Updated BeatAgent:
/*	cout << "UPDATING AGENT" << agentIndex <<" (" << t_ << ")" << " -> oldPeriod: " << oldPeriod << 
		" newPeriod: " << newHypothesis(0) << " prevBeat: " << prevBeat << " nextBeat: " << newHypothesis(1) <<  
		" Error: " << error << endl;
*/
}

//Define new Hypothesis in indexed AgentControl Matrix:
void
BeatReferee::setNewHypothesis(mrs_natural agentIndex, mrs_natural newPeriod, mrs_natural nextBeat)
{
	agentControl_(agentIndex, 0) = 1.0; //is New or Updated
	agentControl_(agentIndex, 1) = newPeriod;
	agentControl_(agentIndex, 2) = nextBeat;
	agentControl_(agentIndex, 3) = t_;

	updctrl(ctrl_agentControl_, agentControl_);
}

void
BeatReferee::grantPoolSpace()
{
	mrs_bool isAvailable = false;
	
	for(int a = 0; a < mutedAgents_.getSize(); a++)
	{
		if(mutedAgents_(a))
		{
			isAvailable = true;
			break;
		}
	}

	//if there are no free agents -> remove worst!
	if(!isAvailable)
	{
		mrs_natural agentInd2Kill = getWorstAgent();
		killAgent(agentInd2Kill, "Pool");
	}
}

//Generic routine for creating new agents given their hypotheses:
void
BeatReferee::createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat, mrs_real newScore, mrs_real beatCount)
{
	//Grant available space in the pool, by removing the worst agent, if needed
	grantPoolSpace();

	for(int a = 0; a < mutedAgents_.getSize(); a++)
	{
		//Look for first disabled agent:		
		if(mutedAgents_(a))
		{
			//Activate new agent
			mutedAgents_(a) = 0.0;
			updctrl(ctrl_mutedAgents_, mutedAgents_);

			//Diplay Created BeatAgent:
			//cout << " CREATED: Agent " << a << endl;

			//Defines new hypothesis for this agent:
			setNewHypothesis(a, newPeriod, firstBeat);
			
			//Update score:
			score_(a) =  newScore;
			beatCounter_(a) = beatCount;

			//Update Agents' Periods and Phases (for equality checking)
			lastPeriods_(a) = newPeriod; //(Periods in frames)
			lastPhases_(a) = firstBeat; //(Phases in frames)
			
			initPeriod_(a) = newPeriod; //save agent's initial IBI

			//statsPeriods_(a, t_) = newPeriod; 
			//statsPhases_(a, t_) = firstBeat;

			//statsAgentsLifeCycle_(a, t_) = 1.0;
			
			break;
		}
	}
}

//Generic routine for killing agents:
void
BeatReferee::killAgent(mrs_natural agentIndex, mrs_string motif)
{
	//Never kill a best agent (for increasing inertia) -> a best agent must live until being replaced by a better one
	if(agentIndex != bestAgentIndex_)
	{		
		//Diplay killed BeatAgent:
		//cout << "KILLED AGENT " << agentIndex << " (" << motif << ") With Score: " << score_(agentIndex) << " / " << bestScore_ << endl;

		mutedAgents_(agentIndex) = 1.0;
		updctrl(ctrl_mutedAgents_, mutedAgents_);
		score_(agentIndex) = 0.0;
		beatCounter_(agentIndex) = 0.0;
		historyCount_(agentIndex) = 1.0;

		//statsAgentsLifeCycle_(agentIndex, t_) = -1;
		lastPeriods_(agentIndex) = 0; //(Periods in frames)
		lastPhases_(agentIndex) = 0; //(Phases in frames)
	}
}

void
BeatReferee::calcAbsoluteBestScore()
{
	for (mrs_natural o = 0; o < nrAgents_; o++)
		{
			//Only consider alive agents:
			if(!mutedAgents_(o))
			{
				if((bestScore_ >= 0 && score_(o) >= bestFactor_ * bestScore_) || 
					(bestScore_ < 0 && score_(o) >= bestScore_ / bestFactor_))
				{
					bestScore_ = score_(o);
					bestAgentIndex_ = o;
				}
			}
		}

	//cout << "Absolute Best Agent: " << bestAgentIndex_ << " BestScore: " << bestScore_ << endl;
}

void 
BeatReferee::myProcess(realvec& in, realvec& out)
{
	//Inititally desactivate all agents:
	if(t_ == 0)
		updctrl(ctrl_mutedAgents_, mutedAgents_);

	//While no best beat detected => outputs 0 (no beat)
	out.setval(0.0);
	ctrl_beatDetected_->setValue(0.0);

	t_++;
	//pass value to beatSink (if used)
	ctrl_tickCount_->setValue(t_);
	
	agentControl_ = ctrl_agentControl_->to<mrs_realvec>();
	//Always updates agent's timming to equalize referee's:
	for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
	{
		agentControl_(i, 3) = t_;
		updctrl(ctrl_agentControl_, agentControl_);
	}

	//Display Input from BeatAgents:
	//cout << "INPUT (" << t_ << "): ";
/*	for(mrs_natural a = 0; a < nrAgents_; a++)
		cout << "\n" << a << "-> " << in(a, 0) << " | " << in(a, 1) << " | " << in(a, 2) << " | " << in(a, 3); 
	cout << endl;
*/

	//realvec with the enable flag of all the BeatAgents in the pool
	//(0 -> agent active; 1 -> agent desactivated)
	mutedAgents_ = ctrl_mutedAgents_->to<mrs_realvec>();

	//cout << "Beat1: " << firstHypotheses_(0,0) << " BPM1: " << firstHypotheses_(0,1) 
	//	<< " Beat2: " << firstHypotheses_(1,0) << " BPM2: " << firstHypotheses_(1,1) 
	//		<< " Beat3: " << firstHypotheses_(2,0) << " BPM3: " << firstHypotheses_(2,1) << endl;

	//Create the first BeatAgents with new hypotheses just after Tseconds of induction:
	//(new agents' score shall be the average of all the already existent ones)
	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	if(t_ == inductionTime_ && !inductionFinnished_)
	{
		firstHypotheses_ = ctrl_firstHypotheses_->to<mrs_realvec>();

		mrs_natural newAgentPeriod;
		mrs_natural newAgentPhase;
		mrs_real newAgentScore;

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

				if((bestScore_ >= 0 && newAgentScore >= bestFactor_ * bestScore_) || 
					(bestScore_ < 0 && newAgentScore >= bestScore_ / bestFactor_))
				{
					bestScore_ = newAgentScore;
					bestAgentIndex_ = i;
				}

				createNewAgent(newAgentPeriod, calculateFirstBeat(newAgentPeriod, newAgentPhase), newAgentScore, 0);

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
		}

		//After finnishing induction disable induction functioning (tempoinduction Fanout):
		for(mrs_natural i = 0; i < inductionEnabler_.getSize(); i++)
			inductionEnabler_(0, i) = 1.0; //diable = muted
			
		updctrl(ctrl_inductionEnabler_, inductionEnabler_);
		
		//restart tick counter
		t_ = 1;
		inductionFinnished_ = true;
	}
	
	//After induction:
	if(inductionFinnished_)
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
			if(!mutedAgents_(o) && agentFlag == EVAL)
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

				//If the bestAgent drops or increases its score the BestScore has to drop correspondingly
				if((score_(bestAgentIndex_) < bestScore_) || (score_(bestAgentIndex_) > bestScore_))
				{
					//cout << "Updating bestScore: " << "OLD: " << bestScore_ << " NEW: " << score_(bestAgentIndex_) << endl;
					bestScore_ = score_(bestAgentIndex_);
					calcAbsoluteBestScore();
				}

				//Updating bestScore:
				if((bestScore_ >= 0 && score_(o) >= bestFactor_ * bestScore_) || 
					(bestScore_ < 0 && score_(o) >= bestScore_ / bestFactor_))
				{
					//cout << "bestScore_OLD: " << bestScore_ << " NEW_bestScore: " << score_(o) << endl;
					bestScore_ = score_(o);
					bestAgentIndex_ = o;
				}

				//Kill Agent if its score is bellow minimum
				
				if (score_(o) < bestScore_ && abs(bestScore_ - score_(o)) > max(abs(bestScore_ * obsoleteFactor_), 10000.0))
				{
					//cout << "Agent " << o << " Killed: Score below minimum (" << score_(o) << "\\" << bestScore_ << ")" << endl;
					killAgent(o, "Obsolete");
				}
				
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
			agentFlag = in(o, 0);

			//Only consider alive agents that send new evaluation:
			if(!mutedAgents_(o) && agentFlag == EVAL)
			{
				
				agentPeriod = (mrs_natural) in(o, 1);
				agentPrevBeat = (mrs_natural) in(o, 2);
				agentTolerance = in(o, 3);
				agentError = (mrs_natural) in(o, 4);
				agentDScore = in(o, 5);

				//If a beat of an agent is inside its Inner tolerance but it has an error:
				//Update agent phase and period hypotheses:
				if(agentTolerance == INNER && abs(agentError) > 0)
				{
					updateAgentHypothesis(o, agentPeriod, agentPrevBeat, agentError);
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
				}
				
				//Checks if there are 2 equal agents
				//2 agents are considered equal if their periods don't differ 
				//more than 10ms (eq. 1frame) and their phases no more than 20ms (eq. 2frames)				
				for(mrs_natural oo = 0; oo < nrAgents_; oo++)
				{
					if(oo != o)
					{						
						//if((abs(agentPeriod - lastPeriods_(oo)) < eqPeriod_) && (abs(agentPrevBeat - lastPhases_(oo) < eqPhase_)))
						if((abs(agentPeriod - in(oo, 1)) < eqPeriod_) && (abs(agentPrevBeat - in(oo, 2) < eqPhase_)))
						{
							//From the two equal agents kill the one with lower score (if it isn't the current best agent)
							ostringstream motif;
							if(score_(o) >= score_(oo))
							{
								if(oo != bestAgentIndex_) 
								{
									//cout << "1-KILL Agent " << oo << " (" << score_(oo) << ") EQUAL TO Agent " << o << " (" << score_(o) << ")" << endl;
									motif << "Equal to " << o;
									killAgent(oo, motif.str());
								}
								else 
								{
									//cout << "2-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
									motif << "Equal to " << oo;
									killAgent(o, motif.str());
									break; //in this case breaks because considered agent no longer exists.
								}
							}
							else
							{
								if(o != bestAgentIndex_) 
								{
									//cout << "3-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
									motif << "Equal to " << oo;
									killAgent(o, motif.str());
									break; //in this case breaks because considered agent no longer exists.
								}
								else 
								{
									motif << "Equal to " << o;
									killAgent(oo, motif.str());
									//cout << "1-KILL Agent " << oo << " (" << score_(oo) << ") EQUAL TO Agent " << o << " (" << score_(o) << ")" << endl;
								}
							}
						}
					}
				}
				
			}
			
			if(!mutedAgents_(o) && agentFlag == BEAT)
			{
				//Increment beat counter of each agent
				beatCounter_(o)++;

				//Display Beats from BeatAgents:
				//cout << "Agent " << o << "(" << t_ << ") -> AgentScore: " << score_(o) << " BestAgent: " 
				//	<< bestAgentIndex_ << " BestScore: " << bestScore_ << " BeatCount: " << beatCounter_(o) << endl;
				
				//if best agent sends beat => outputs best beat
				if(o == bestAgentIndex_)
				{	
					//to avoid beats given by different agents distanced less then minPeriod frames (+ a tolerance = -5)
					if(t_ - lastBeatTime_ >= (minPeriod_-3))
					{
						//Display Outputted Beat:
						//cout << "OUTPUT(" << t_ << ")->Beat from Agent " << bestAgentIndex_ << 
						//	" BestScore: " << bestScore_ << " (" << score_(bestAgentIndex_) << ")->" << 
						//	(60.0 / (t_ - lastBeatTime_)) * (ctrl_srcFs_->to<mrs_real>() / ctrl_hopSize_->to<mrs_natural>()) << endl;
						//cout << "BEST_AgentPeriod: " << lastPeriods_(bestAgentIndex_) << "(" << (t_ - lastBeatTime_) << ")" << endl;
						
						ctrl_beatDetected_->setValue(1.0);
						out.setval(1.0);
						//Updates agent history, which accounts for the total number
						//of the detected best (considered) beats of each agent:
						historyCount_(o)++;
						//historyBeatTimes_(o, outputCount_) = t_;
						outputCount_ ++;
						lastBeatTime_ = t_;
					}				
				}
			}
		}
	}


	//MATLAB_PUT(in, "BeatAgents");
	/*
	MATLAB_PUT(out, "BeatReferee");
	MATLAB_PUT(historyBeatTimes_, "HistoryBeatTimes");
	MATLAB_PUT(historyCount_, "HistoryCount");
	MATLAB_PUT(statsPeriods_, "statsPeriods");
	MATLAB_PUT(statsAgentsLifeCycle_, "statsAgentsLifeCycle");
	MATLAB_PUT(statsAgentsScore_, "agentsScore");
	MATLAB_PUT(statsMuted_, "mutedAgents");
	MATLAB_PUT(bestScore_, "bestScore");
	MATLAB_EVAL("bestAgentScore = [bestAgentScore bestScore];");
	MATLAB_EVAL("FinalBeats = [FinalBeats, BeatReferee];");
	*/
	//MATLAB_EVAL("hold on;");
	//MATLAB_EVAL("plot(BeatAgentsTS)");
	//MATLAB_EVAL("stem(t, 1, 'r');");
	//MATLAB_EVAL("hold off;");
}