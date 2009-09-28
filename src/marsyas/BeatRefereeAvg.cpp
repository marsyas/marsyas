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

#include "BeatRefereeAvg.h"

using namespace std;
using namespace Marsyas;

#define NONE 0.0
#define BEAT 1.0
#define EVAL 2.0

#define INNER 3.0
#define OUTTER 4.0

#define AVG_FACTOR 1 //(1)

BeatRefereeAvg::BeatRefereeAvg(string name):MarSystem("BeatRefereeAvg", name)
{
	addControls();

  	bestScore_ = 0.0;
	bestAgentIndex_ = 0; //0 by default
	t_ = 0;
	outputCount_ = 0;
	lastBeatTime_ = 0;
	inductionFinnished_ = false;
}

BeatRefereeAvg::BeatRefereeAvg(const BeatRefereeAvg& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
	t_ = a.t_;
	beatCounter_ = a.beatCounter_;
	historyCount_ = a.historyCount_;
	historyBeatTimes_ = a.historyBeatTimes_;
	lastBeatTime_ = a.lastBeatTime_;
	bestScore_ = a.bestScore_;
	bestAgentIndex_ = a.bestAgentIndex_;
	outputCount_ = a.outputCount_;
	statsPeriods_ = a.statsPeriods_;
	statsPhases_ = a.statsPhases_;
	statsAgentsLifeCycle_ = a.statsAgentsLifeCycle_;
	inductionFinnished_ = a.inductionFinnished_;

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
	ctrl_obsoleteFactor_ = getctrl("mrs_natural/obsoleteFactor");
	ctrl_childFactor_ = getctrl("mrs_natural/childFactor");
	ctrl_bestFactor_ = getctrl("mrs_real/bestFactor");
	ctrl_eqPhase_ = getctrl("mrs_natural/eqPhase");
	ctrl_eqPeriod_ = getctrl("mrs_natural/eqPeriod");
}

BeatRefereeAvg::~BeatRefereeAvg()
{
}

MarSystem* 
BeatRefereeAvg::clone() const 
{
  return new BeatRefereeAvg(*this);
}

void 
BeatRefereeAvg::addControls()
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
	addctrl("mrs_natural/obsoleteFactor", 10, ctrl_obsoleteFactor_);
	setctrlState("mrs_natural/obsoleteFactor", true);
	addctrl("mrs_natural/childFactor", 2, ctrl_childFactor_);
	setctrlState("mrs_natural/childFactor", true);
	addctrl("mrs_real/bestFactor", 1.0, ctrl_bestFactor_);
	setctrlState("mrs_real/bestFactor", true);
	addctrl("mrs_natural/eqPhase", 2, ctrl_eqPhase_);
	setctrlState("mrs_natural/eqPhase", true);
	addctrl("mrs_natural/eqPeriod", 1, ctrl_eqPeriod_);
	setctrlState("mrs_natural/eqPeriod", true);
}

void
BeatRefereeAvg::myUpdate(MarControlPtr sender)
{
	MRSDIAG("BeatRefereeAvg.cpp - BeatRefereeAvg:myUpdate");
	
	ctrl_onSamples_->setValue(1, NOUPDATE);
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	obsoleteFactor_ = ctrl_obsoleteFactor_->to<mrs_natural>();
	childFactor_ = ctrl_childFactor_->to<mrs_natural>();
	bestFactor_ = ctrl_bestFactor_->to<mrs_real>();
	eqPhase_ = ctrl_eqPhase_->to<mrs_natural>();
	eqPeriod_ = ctrl_eqPeriod_->to<mrs_natural>();

	//inObservations_ = number of BeatAgents in the pool
	nrAgents_ = inObservations_;
		
	historyCount_.create(nrAgents_); //1index for each agent
	historyBeatTimes_.create(nrAgents_,10000); //1index for each agent
	statsPeriods_.create(nrAgents_,100000); //1index for each agent
	statsPhases_.create(nrAgents_,100000); //1index for each agent
	statsAgentsLifeCycle_.create(nrAgents_,100000);
	statsAgentsScore_.create(nrAgents_,100000);
	score_.create(nrAgents_); //1index for each agent
	lastPeriods_.create(nrAgents_); //1index for each agent
	lastPhases_.create(nrAgents_); //1index for each agent
	mutedAgents_.create(1,nrAgents_);//1index for each agent
	beatCounter_.create(nrAgents_);

	//Agent control indexed matrix
	//Each line(observation) accounts for an agent
	//[New/Update_Flag|Period|Phase|Timming]
	agentControl_.create(nrAgents_, 4);
	updctrl(ctrl_agentControl_, agentControl_);

	for(int i = 0; i < nrAgents_; i++)
	{
		historyCount_(i) = 0.0;
		score_(i) = 0.0;
		lastPeriods_(i) = 0.0;
		lastPhases_(i) = 0.0;
		mutedAgents_(0, i) = 1.0;
		beatCounter_(i) = 1.0;
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
/*
//Routine for calculating the first beat time of the first created agents (after induction):
mrs_natural
BeatRefereeAvg::calculateFirstBeat(mrs_natural initPeriod, mrs_natural initPhase)
{
	mrs_natural count = (mrs_natural) ceil((inductionTime_ - initPhase) /  (mrs_real)initPeriod);
	mrs_natural firstBeat = (initPhase + initPeriod * count);
	
	//Due to the compensation bellow we must grant that nextBeat is bigger than inductionTime_
	//If it is equal than we must postpone the nextBeat to a period after
	if(firstBeat == inductionTime_)
	{
		firstBeat += initPeriod;
	}

	return firstBeat;
}
*/

mrs_natural
BeatRefereeAvg::calculateFirstBeat(mrs_natural initPeriod, mrs_natural initPhase)
{
	return initPhase;
}

mrs_natural
BeatRefereeAvg::getFirstAliveAgent()
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
BeatRefereeAvg::getWorstAgent()
{
	//By default lowest score = score from first agent
	mrs_natural firstAlive= getFirstAliveAgent();
	mrs_real lowestScore = score_(firstAlive) / beatCounter_(firstAlive);
	mrs_natural lowestIndex = firstAlive;
	
	for(mrs_natural a = firstAlive; a < nrAgents_; a++)
	{
		/*
		if(score_(a) / beatCounter_(a) < lowestScore)
		{
			lowestScore = score_(a) / beatCounter_(a);
			lowestIndex = a;
		}
		*/

		if(score_(a) >= 0 && (score_(a)*AVG_FACTOR) / beatCounter_(a) < lowestScore)
		{
			lowestScore = (score_(a)*AVG_FACTOR) / beatCounter_(a);
			lowestIndex = a;
		}	
		else if (score_(a) < 0 && (score_(a)/AVG_FACTOR) * beatCounter_(a) < lowestScore)			
		//if((bestScore_ >= 0 && score_(o) / beatCounter_(o) >= bestFactor_ * bestScore_) || 
		//	(bestScore_ < 0 && score_(o) / beatCounter_(o) >= bestScore_ / bestFactor_))
		{
			lowestScore = (score_(a)/AVG_FACTOR) * beatCounter_(a);
			lowestIndex = a;
		}
	}

	//return worst agent
	return lowestIndex;
}

mrs_realvec 
BeatRefereeAvg::calculateNewHypothesis(mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error)
{
	mrs_natural nextBeat;
	mrs_natural newPeriod = (mrs_natural) (oldPeriod + (oldPeriod + error)) / 2;
		
	//To avoid too small or big periods:
	if(newPeriod > minPeriod_ && newPeriod < maxPeriod_)
		nextBeat = prevBeat + newPeriod + error;

	else 
	{
		nextBeat = prevBeat + oldPeriod;
		newPeriod = oldPeriod;
	}

	mrs_realvec newHypothesis(2);
	newHypothesis(0) = newPeriod;
	newHypothesis(1) = nextBeat;

	return newHypothesis;
}

//Routine for creating new agent from existent one
void
BeatRefereeAvg::createChild(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error, 
						mrs_real newScore, mrs_real beatCount)
{
	mrs_realvec newHypothesis = calculateNewHypothesis(oldPeriod, prevBeat, error);
	createNewAgent((mrs_natural) newHypothesis(0), (mrs_natural) newHypothesis(1), newScore, beatCount);

	//Display Created BeatAgent:
/*	cout << "NEW AGENT(" << t_ <<") (reqBy:" << agentIndex << ") -> PrevBeat: " << prevBeat << " Period: " 
		<< oldPeriod << " NextBeat: " << newHypothesis(1) << " NewPeriod: " << newHypothesis(0) << 
		" Error: " << error << " Score: " << newScore;				
*/
}

//Routine for updating existent agent hypothesis
//(Used when beat of agent is found inside its inner tolerance with some error):
void
BeatRefereeAvg::updateAgentHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, 
								   mrs_natural prevBeat, mrs_natural error)
{
	mrs_realvec newHypothesis = calculateNewHypothesis(oldPeriod, prevBeat, error);
	setNewHypothesis(agentIndex, (mrs_natural) newHypothesis(0), (mrs_natural) newHypothesis(1));

	//Display Updated BeatAgent:
/*	cout << "UPDATING AGENT" << agentIndex <<" (" << t_ << ")" << " -> oldPeriod: " << oldPeriod << 
		" newPeriod: " << newHypothesis(0) << " prevBeat: " << prevBeat << " nextBeat: " << newHypothesis(1) <<  
		" Error: " << error << endl;
*/
}

//Define new Hypothesis in indexed AgentControl Matrix:
void
BeatRefereeAvg::setNewHypothesis(mrs_natural agentIndex, mrs_natural newPeriod, mrs_natural nextBeat)
{
	agentControl_(agentIndex, 0) = 1.0; //is New or Updated
	agentControl_(agentIndex, 1) = newPeriod;
	agentControl_(agentIndex, 2) = nextBeat;
	agentControl_(agentIndex, 3) = t_;

	updctrl(ctrl_agentControl_, agentControl_);
}

void
BeatRefereeAvg::grantPoolSpace()
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
		killAgent(agentInd2Kill);
	}
}

//Generic routine for creating new agents given their hypotheses:
void
BeatRefereeAvg::createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat, mrs_real newScore, mrs_real beatCount)
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
			
			statsPeriods_(a, t_) = newPeriod; 
			statsPhases_(a, t_) = firstBeat;

			statsAgentsLifeCycle_(a, t_) = 1.0;
			statsAgentsScore_(a, t_) = score_(a);
			
			break;
		}
	}
}

//Generic routine for killing agents:
void
BeatRefereeAvg::killAgent(mrs_natural agentIndex)
{	
	//Never kill a best agent (for increasing inertia) -> a best agent must live until being replaced by a better one
	if(agentIndex != bestAgentIndex_)
	{
		mutedAgents_(agentIndex) = 1.0;
		updctrl(ctrl_mutedAgents_, mutedAgents_);
		score_(agentIndex) = 0.0;
		beatCounter_(agentIndex) = 0.0;

		//Diplay killed BeatAgent:
		//cout << "KILLED AGENT " << agentIndex << endl;

		statsAgentsLifeCycle_(agentIndex, t_) = -1;
		lastPeriods_(agentIndex) = 0; //(Periods in frames)
		lastPhases_(agentIndex) = 0; //(Phases in frames)
	}
}

void
BeatRefereeAvg::calcAbsoluteBestScore()
{
	for (mrs_natural o = 0; o < nrAgents_; o++)
		{
			//Only consider alive agents:
			if(!mutedAgents_(o))
			{
				//if (score_(o) / beatCounter_(o) >= bestScore_)
				if(score_(o) >= 0 && (score_(o)*AVG_FACTOR) / beatCounter_(o) > bestScore_)
				{
					bestScore_ = (score_(o)*AVG_FACTOR) / beatCounter_(o);
					bestAgentIndex_ = o;
				}	
				else if (score_(o) < 0 && (score_(o)/AVG_FACTOR) * beatCounter_(o) > bestScore_)			
				//if((bestScore_ >= 0 && score_(o) / beatCounter_(o) >= bestFactor_ * bestScore_) || 
				//	(bestScore_ < 0 && score_(o) / beatCounter_(o) >= bestScore_ / bestFactor_))
				{
					bestScore_ = (score_(o)/AVG_FACTOR) * beatCounter_(o);
					bestAgentIndex_ = o;
				}
			}
		}
}

void 
BeatRefereeAvg::myProcess(realvec& in, realvec& out)
{
	//Inititally desactivate all agents:
	if(t_ == 0)
		updctrl(ctrl_mutedAgents_, mutedAgents_);

	//While no best beat detected => outputs 0 (no beat)
	out.setval(0.0);
	ctrl_beatDetected_->setValue(0.0);

	t_++;
	
	agentControl_ = ctrl_agentControl_->to<mrs_realvec>();

	//Always updates agent's timming to equalize referee's:
	//(If used also update BeatSink internal tick counter)
	for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
	{
		agentControl_(i, 3) = t_;
		updctrl(ctrl_agentControl_, agentControl_);
		ctrl_tickCount_->setValue(t_);
	}

	/*
	//Display Input from BeatAgents:
	cout << "INPUT (" << t_ << "): ";
	for(mrs_natural a = 0; a < nrAgents_; a++)
		cout << "\n" << a << "-> " << in(a, 0) << " | " << in(a, 1) << " | " << in(a, 2) << " | " << in(a, 3); 
	cout << endl;
	*/

	//realvec with the enable flag of all the BeatAgents in the pool
	//(0 -> agent active; 1 -> agent desactivated)
	mutedAgents_ = ctrl_mutedAgents_->to<mrs_realvec>();

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
			//firstHypotheses_ -> matrix with i generated beat hypotheses + score, in the induction stage
			//[ BPMi | Beati | Score i ]
			newAgentPeriod = (mrs_natural) firstHypotheses_(i,0);
			newAgentPhase = (mrs_natural) firstHypotheses_(i,1);
			newAgentScore = firstHypotheses_(i,2);

			if(newAgentScore > bestScore_)
			{
				bestScore_ = newAgentScore;
				bestAgentIndex_ = i;
			}

			createNewAgent(newAgentPeriod, calculateFirstBeat(newAgentPeriod, newAgentPhase), newAgentScore, 1.0);

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

		//After finnishing induction disable induction functioning (tempoinduction Fanout):
		for(mrs_natural i = 0; i < inductionEnabler_.getSize(); i++)
			inductionEnabler_(0, i) = 1.0; //diable = muted
			
		updctrl(ctrl_inductionEnabler_, inductionEnabler_);
		
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

			statsPeriods_(o, t_) = lastPeriods_(o); 
			statsPhases_(o, t_) = lastPhases_(o); 
			statsAgentsScore_(o, t_) = score_(o) / beatCounter_(o);

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

				statsPeriods_(o, t_) = agentPeriod; 
				statsPhases_(o, t_) = agentPrevBeat; 

				//Update Agents' Score
				score_(o) += agentDScore;

				//If the bestAgent drops or increases its score the BestScore has to drop correspondingly
				if(score_(bestAgentIndex_) >= 0 && (score_(bestAgentIndex_)*AVG_FACTOR) / beatCounter_(bestAgentIndex_) != bestScore_)					
				//if((score_(bestAgentIndex_) / beatCounter_(bestAgentIndex_) < bestScore_) || 
				//	(score_(bestAgentIndex_) / beatCounter_(bestAgentIndex_) > bestScore_))
				{
					bestScore_ = (score_(bestAgentIndex_)*AVG_FACTOR) / beatCounter_(bestAgentIndex_);
					calcAbsoluteBestScore();
				}
				else if(score_(bestAgentIndex_) < 0 && (score_(bestAgentIndex_)/AVG_FACTOR) * beatCounter_(bestAgentIndex_) != bestScore_)
				{
					bestScore_ = (score_(bestAgentIndex_)/AVG_FACTOR) * beatCounter_(bestAgentIndex_);
					calcAbsoluteBestScore();
				}

				//Updating bestScore:
				if(score_(o) >= 0 && (score_(o)*AVG_FACTOR) / beatCounter_(o) > bestScore_)
				{
					bestScore_ = (score_(o)*AVG_FACTOR) / beatCounter_(o);
					bestAgentIndex_ = o;
				}	
				else if (score_(o) < 0 && (score_(o)/AVG_FACTOR) * beatCounter_(o) > bestScore_)			
				//if((bestScore_ >= 0 && score_(o) / beatCounter_(o) >= bestFactor_ * bestScore_) || 
				//	(bestScore_ < 0 && score_(o) / beatCounter_(o) >= bestScore_ / bestFactor_))
				{
					bestScore_ = (score_(o)/AVG_FACTOR) * beatCounter_(o);
					bestAgentIndex_ = o;
				}

				//Kill Agent if its score is bellow minimum
				//(minimumScore = bestScore_/10) [! -> might change-not so relevant to performance!]
				if ((score_(o) >= 0 && (score_(o)*AVG_FACTOR) / beatCounter_(o) < bestScore_ && 
					abs(bestScore_ - (score_(o)*AVG_FACTOR) / beatCounter_(o)) > abs(bestScore_ * obsoleteFactor_)) ||
					(score_(o) < 0 && (score_(o)/AVG_FACTOR) * beatCounter_(o) < bestScore_ && 
					abs(bestScore_ - (score_(o)/AVG_FACTOR) * beatCounter_(o)) > abs(bestScore_ * obsoleteFactor_)))
				{
					killAgent(o);
				}
				
				//statsAgentsScore_(o, t_) = score_(o) / beatCounter_(o);

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
				//newPeriod = (oldPeriod + (oldPeriod +/- erro))/2
				//newPhase = prevBeat + newPeriod +/- error		
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
					//newPeriod = (oldPeriod + (oldPeriod +/- erro))/2
					//newPhase = prevBeat + newPeriod +/- error
					mrs_real deltaS = abs(agentDScore) / childFactor_;
					createChild(o, agentPeriod, agentPrevBeat, agentError, (score_(o)-deltaS), beatCounter_(o));

					statsAgentsLifeCycle_(o, t_) = 2;
				}
				
				//Checks if there are 2 equal agents
				//2 agents are considered equal if their periods don't differ 
				//more than 10ms (eq. 1frame) and their phases no more than 20ms (eq. 2frames)
				for(mrs_natural oo = 0; oo < nrAgents_; oo++)
				{
					if(oo != o)
					{						
						if((abs(agentPeriod - lastPeriods_(oo)) < eqPeriod_) && (abs(agentPrevBeat - lastPhases_(oo) < eqPhase_)))
						{
							//From the two equal agents kill the one with lower score (if it isn't the current best agent)
							//if(score_(o) / beatCounter_(o) >= score_(oo) / beatCounter_(oo))
							if((score_(o) >= 0 && (score_(o)*AVG_FACTOR) / beatCounter_(o) >= (score_(oo)*AVG_FACTOR) / beatCounter_(oo)) ||
								(score_(o) < 0 && (score_(o)/AVG_FACTOR) * beatCounter_(o) >= (score_(oo)/AVG_FACTOR) * beatCounter_(oo)))
							{
								if(oo != bestAgentIndex_) 
								{
									//cout << "1-KILL Agent " << oo << " (" << score_(oo) << ") EQUAL TO Agent " << o << " (" << score_(o) << ")" << endl;
									killAgent(oo);
								}
								else 
								{
									//cout << "2-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
									killAgent(o);
									break; //in this case breaks because considered agent no longer exists.
								}
							}
							else
							{
								if(o != bestAgentIndex_) 
								{
									//cout << "3-KILL Agent " << o << " (" << score_(o) << ") EQUAL TO Agent " << oo << " (" << score_(oo) << ")" << endl;
									killAgent(o);
									break; //in this case breaks because considered agent no longer exists.
								}
								else 
								{
									killAgent(oo);
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
					//Display Outputted Beat:
					//cout << "OUTPUT(" << t_ << ") -> Beat from Agent " << bestAgentIndex_ << endl;
					
					//to avoid beats given by different agents distanced less then minPeriod frames
					if(t_ - lastBeatTime_ >= minPeriod_)
					{
						out.setval(1.0);
						ctrl_beatDetected_->setValue(1.0);

						//Updates agent history, which accounts for the total number
						//of the detected best (considered) beats of each agent:
						historyCount_(o)++;
						historyBeatTimes_(o, outputCount_) = t_;
						outputCount_ ++;
					}
					lastBeatTime_ = t_;
				}
			}
		}
	}


	//MATLAB_PUT(in, "BeatAgents");
	/*
	MATLAB_PUT(out, "BeatRefereeAvg");
	MATLAB_PUT(historyBeatTimes_, "HistoryBeatTimes");
	MATLAB_PUT(historyCount_, "HistoryCount");
	MATLAB_PUT(statsPeriods_, "statsPeriods");
	MATLAB_PUT(statsAgentsLifeCycle_, "statsAgentsLifeCycle");
	MATLAB_PUT(statsAgentsScore_, "agentsScore");
	MATLAB_PUT(bestScore_, "bestScore");
	MATLAB_EVAL("bestAgentScore = [bestAgentScore bestScore];");
	MATLAB_EVAL("FinalBeats = [FinalBeats, BeatRefereeAvg];");
	*/
	//MATLAB_EVAL("hold on;");
	//MATLAB_EVAL("plot(BeatAgentsTS)");
	//MATLAB_EVAL("stem(t, 1, 'r');");
	//MATLAB_EVAL("hold off;");
}