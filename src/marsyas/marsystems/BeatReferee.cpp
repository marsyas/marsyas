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
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

#define NONE 0.0
#define BEAT 1.0
#define EVAL 2.0

#define INNER 3.0
#define OUTTER 4.0

#define MINIMUMREAL 0.000001 //(0.000001 minimum float recognized)
#define NA -10000.0 //undefined value flag (just a big negative nr)

BeatReferee::BeatReferee(mrs_string name):MarSystem("BeatReferee", name)
{
  addControls();

  bestScore_ = NA; //To allow initial negative scores
  bestAgentIndex_ = -1; //-1 by default
  timeElapsed_ = 0;
  outputCount_ = 0;
  triggerCount_ = 0;
  lastBeatTime_ = (mrs_natural) NA;
  lastBeatPeriod_ = -1;
  bestFinalAgent_ = -1;
  processInduction_ = false;
  triggerInduction_ = false;
  startSystem_ = true;
  startTracking_ = false;
  lostGTBeatsCount_ = 0;
  lastGTBeatPos_ = -1;
  backtraceEndTime_ = -1;
  bestAgentBeforeTrigger_ = -1;
  lastGTFalsePos_ = false;
  logFile_ = false;
  // these will be overwritten later
  nrAgents_ = 100;
  maxNrBeats_ = 100;
  //
  initialization();
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
  ctrl_backtrace_ = getctrl("mrs_bool/backtrace");
  ctrl_logFile_ = getctrl("mrs_string/logFile");
  ctrl_logFileName_= getctrl("mrs_string/logFileName");
  ctrl_soundFileSize_= getctrl("mrs_natural/soundFileSize");
  ctrl_bestFinalAgentHistory_= getctrl("mrs_realvec/bestFinalAgentHistory");
  ctrl_nonCausal_ = getctrl("mrs_bool/nonCausal");
  ctrl_triggerInduction_ = getctrl("mrs_bool/triggerInduction");
  ctrl_triggerInductionExternalRequest_ = getctrl("mrs_bool/triggerInductionExternalRequest");
  ctrl_gtInductionMode_ = getctrl("mrs_string/gtInductionMode");
  ctrl_triggerGtTolerance_ = getctrl("mrs_natural/triggerGtTolerance");
  ctrl_gtBeatsFile_ = getctrl("mrs_string/gtBeatsFile");
  ctrl_curBestScore_ = getctrl("mrs_real/curBestScore");
  ctrl_adjustment_ = getctrl("mrs_natural/adjustment");
  ctrl_inductionMode_ = getctrl("mrs_string/inductionMode");
  ctrl_beatTransitionTol_ = getctrl("mrs_real/beatTransitionTol");
  ctrl_destFileName_ = getctrl("mrs_string/destFileName");
  ctrl_triggerTimesFile_ = getctrl("mrs_string/triggerTimesFile");
  ctrl_resetAfterNewInduction_ = getctrl("mrs_bool/resetAfterNewInduction");
  ctrl_resetFeatWindow_ = getctrl("mrs_bool/resetFeatWindow");
  ctrl_supervisedTriggerThres_ = getctrl("mrs_real/supervisedTriggerThres");

  beatTransitionTol_ = a.beatTransitionTol_;
  considerAgentTransitionBeat_ = a.considerAgentTransitionBeat_;
  considerFatherTransitionBeat_ = a.considerFatherTransitionBeat_;
  timeElapsed_ = a.timeElapsed_;
  lastBeatPeriod_ = a.lastBeatPeriod_;
  historyCount_ = a.historyCount_;
  historyBeatTimes_ = a.historyBeatTimes_;
  lastBeatTime_ = a.lastBeatTime_;
  bestScore_ = a.bestScore_;
  bestAgentIndex_ = a.bestAgentIndex_;
  outputCount_ = a.outputCount_;
  initPeriod_ = a.initPeriod_;
  corFactor_ = a.corFactor_;
  backtrace_ = a.backtrace_;
  logFile_ = a.logFile_;
  logFileName_ = a.logFileName_;
  logFileUnits_ = a.logFileUnits_;
  triggerInduction_ = a.triggerInduction_;
  triggerInductionExternalRequest_ = a.triggerInductionExternalRequest_;
  processInduction_ = a.processInduction_;
  triggerInductionTime_ = a.triggerInductionTime_;
  inductionMode_ = a.inductionMode_;
  startSystem_ = a.startSystem_;
  startTracking_ = a.startTracking_;
  agentsHistory_ = a.agentsHistory_;
  agentsFamilyHist_ = a.agentsFamilyHist_;
  lostGTBeatsCount_ = a.lostGTBeatsCount_;
  lastGTBeatPos_ = a.lastGTBeatPos_;
  lastGTFileBeat_ = a.lastGTFileBeat_;
  triggerGtTolerance_ = a.triggerGtTolerance_;
  backtraceEndTime_ = a.backtraceEndTime_;
  bestAgentBeforeTrigger_ = a.bestAgentBeforeTrigger_;
  frames2SecsAdjustment_ = a.frames2SecsAdjustment_;
  lastGTFalsePos_ = a.lastGTFalsePos_;
  triggerTimes_ = a.triggerTimes_;
  transitionTimes_ = a.transitionTimes_;
  transitionsConsidered_ = a.transitionsConsidered_;
  resetAfterNewInduction_ = a.resetAfterNewInduction_;
  supervisedTriggerThres_ = a.supervisedTriggerThres_;
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
  addctrl("mrs_realvec/inductionEnabler", realvec(2,2), ctrl_inductionEnabler_);
  addctrl("mrs_realvec/beatHypotheses", realvec(), ctrl_firstHypotheses_);
  addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
  addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_natural/maxPeriod", -1, ctrl_maxPeriod_);
  setctrlState("mrs_natural/maxPeriod", true);
  addctrl("mrs_natural/minPeriod", -1, ctrl_minPeriod_);
  setctrlState("mrs_natural/minPeriod", true);
  addctrl("mrs_realvec/agentControl", realvec(50,4), ctrl_agentControl_);
  addctrl("mrs_real/beatDetected", 0.0, ctrl_beatDetected_);
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_real/obsoleteFactor", 0.8, ctrl_obsoleteFactor_);
  setctrlState("mrs_real/obsoleteFactor", true);
  addctrl("mrs_natural/lostFactor", 4, ctrl_lostFactor_);
  setctrlState("mrs_natural/lostFactor", true);
  addctrl("mrs_real/childrenScoreFactor", 0.8, ctrl_childrenScoreFactor_);
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
  addctrl("mrs_bool/backtrace", false, ctrl_backtrace_);
  setctrlState("mrs_bool/backtrace", true);
  addctrl("mrs_string/logFile", "-1", ctrl_logFile_);
  addctrl("mrs_string/logFileName", "log.txt", ctrl_logFileName_);
  addctrl("mrs_natural/soundFileSize", 0, ctrl_soundFileSize_);
  setctrlState("mrs_natural/soundFileSize", true);
  addctrl("mrs_realvec/bestFinalAgentHistory", realvec(), ctrl_bestFinalAgentHistory_);
  addctrl("mrs_bool/nonCausal", false, ctrl_nonCausal_);
  setctrlState("mrs_bool/nonCausal", true);
  addctrl("mrs_bool/triggerInduction", false, ctrl_triggerInduction_);
  setctrlState("mrs_bool/triggerInduction", true);
  addctrl("mrs_bool/triggerInductionExternalRequest", false, ctrl_triggerInductionExternalRequest_);
  setctrlState("mrs_bool/triggerInductionExternalRequest", true);
  addctrl("mrs_string/gtInductionMode", "-1", ctrl_gtInductionMode_);
  setctrlState("mrs_string/gtInductionMode", true);
  addctrl("mrs_natural/triggerGtTolerance", 5, ctrl_triggerGtTolerance_);
  setctrlState("mrs_natural/triggerGtTolerance", true);
  addctrl("mrs_string/gtBeatsFile", "input.txt", ctrl_gtBeatsFile_);
  addctrl("mrs_real/curBestScore", NA, ctrl_curBestScore_);
  setctrlState("mrs_real/curBestScore", true);
  addctrl("mrs_natural/adjustment", 0, ctrl_adjustment_);
  setctrlState("mrs_natural/adjustment", true);
  addctrl("mrs_string/inductionMode", "single", ctrl_inductionMode_);
  addctrl("mrs_real/beatTransitionTol", 0.6, ctrl_beatTransitionTol_);
  addctrl("mrs_string/destFileName", "output", ctrl_destFileName_);
  addctrl("mrs_string/triggerTimesFile", "input_trigger.txt", ctrl_triggerTimesFile_);
  addctrl("mrs_bool/resetAfterNewInduction", true, ctrl_resetAfterNewInduction_);
  setctrlState("mrs_bool/resetAfterNewInduction", true);
  addctrl("mrs_bool/resetFeatWindow", true, ctrl_resetFeatWindow_);
  setctrlState("mrs_bool/resetFeatWindow", true);
  addctrl("mrs_real/supervisedTriggerThres", 0.0, ctrl_supervisedTriggerThres_);
}

void
BeatReferee::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
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
  backtrace_ = ctrl_backtrace_->to<mrs_bool>();
  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();
  maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
  minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();
  nonCausal_ = ctrl_nonCausal_->to<mrs_bool>();
  inductionMode_ = ctrl_inductionMode_->to<mrs_string>();
  gtBeatsFile_ = ctrl_gtBeatsFile_->to<mrs_string>();
  triggerGtTolerance_ = ctrl_triggerGtTolerance_->to<mrs_natural>();
  frames2SecsAdjustment_ = ctrl_adjustment_->to<mrs_natural>();
  beatTransitionTol_ = ctrl_beatTransitionTol_->to<mrs_real>();
  triggerTimesFile_ = ctrl_triggerTimesFile_->to<mrs_string>();
  resetAfterNewInduction_ = ctrl_resetAfterNewInduction_->to<mrs_bool>();
  supervisedTriggerThres_ = ctrl_supervisedTriggerThres_->to<mrs_real>();

  //cout << "TRIGGERTIME @ " << timeElapsed_ << ": " <<  triggerInductionTime_ << endl;
  //triggerInductionTime_ = ctrl_triggerInductionTime_->to<mrs_natural>();
  //cout << "TRIGGERTIME @ " << timeElapsed_ << ": " <<  triggerInductionTime_ << endl;

  //inObservations_ = number of BeatAgents in the pool
  nrAgents_ = inObservations_;
  historyCount_.create(nrAgents_); //1index for each agent
  agentsJustCreated_.create(nrAgents_); //to know which agents were created on current frame

  soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();
  //max possible nr. of beats in the analysed sound file (*1.2 - tolerance due to possible limit surpassing)
  maxNrBeats_ = (mrs_natural) (ceil(((mrs_real) soundFileSize_) / ((mrs_real) minPeriod_)) * 1.2);

  inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
  inductionEnabler_ = ctrl_inductionEnabler_->to<mrs_realvec>();

  //wait timeBeforeKill secs before considering killing obsolete agents
  mrs_real timeBeforeKill = 0.0;
  timeBeforeKilling_ = (mrs_natural)(timeBeforeKill*srcFs_/hopSize_);
}

mrs_bool
BeatReferee::loadTriggerTimes(mrs_string triggerTimesFile)
{
  if (fopen(triggerTimesFile.c_str(), "r"))
  {
    cerr << "TriggerTimes File: " << triggerTimesFile.c_str() << endl;

    ifstream inStream;
    mrs_string line;
    inStream.open(triggerTimesFile.c_str());
    getline (inStream, line);

    mrs_natural countTriggers = 0;
    while(strtod(line.c_str(), NULL) > 0.0)
    {
      getline (inStream, line);
      countTriggers++;
    }
    triggerTimes_.create(countTriggers);
    transitionTimes_.create(countTriggers);
    transitionsConsidered_.create(countTriggers);
    inStream.close();
    inStream.open(triggerTimesFile.c_str());
    mrs_natural validTriggersCount = 0;
    mrs_real indTimeSecs = ((inductionTime_ * hopSize_) - (frames2SecsAdjustment_)) / srcFs_;
    for(mrs_natural i = 0; i < countTriggers; i++)
    {
      getline (inStream, line);
      mrs_real triggerTime = strtod(line.c_str(), NULL);
      //cout << "triggerTime: " << triggerTime << "; inductionTime: " << indTimeSecs << endl;
      if(triggerTime >= indTimeSecs)
      {
        //sum the size of the induction window to every trigger time
        //so the actual trigger is only activated one window after
        transitionTimes_(validTriggersCount) = ((mrs_natural) (triggerTime * (srcFs_/hopSize_) + 0.5));
        triggerTimes_(validTriggersCount) =  transitionTimes_(validTriggersCount) + inductionTime_;
        transitionsConsidered_(validTriggersCount) = 0.0;
        validTriggersCount++;
      }
    }

    //cout << "trigger times (" << countTriggers << "): ";
    //for(mrs_natural i = 0; i < triggerTimes_.getSize(); i++)
    //{
    //	cout << triggerTimes_(i) << " ";
    //}
    //cout << endl;

    inStream.close();

    return true;
  }
  else
  {
    cerr << "Bad or nonexistent transition times file: " << triggerTimesFile.c_str() << "\nPlease specify a supported one." << endl;
    return false;
  }
}

mrs_bool
BeatReferee::isGTFileInLine(mrs_string line)
{

  //for beat groundtruth files (in line, separated by spaces):
  mrs_natural pos0;
  pos0 = (mrs_natural) line.find_first_of(" ", 0); //initial delimiter

  //if pos0 < 0 => one beat time per line
  return (pos0 > 0 ? true : false);
}

//Routine for comparing the current detected beat to the correspondent ground-truth beat-time
//(to be used in trigger induction ground-truth mode):
//0-> beat matches!
//1-> false positive!
//n-> nr. of false negatives
mrs_natural
BeatReferee::checkBeatInGTFile()
{
  mrs_natural localBeatErrorCount = 0;
  mrs_real fMeasureTol = 0.07; //70ms used in Fmeasure tolerance
  mrs_real beatTime = ((timeElapsed_ * hopSize_) - (frames2SecsAdjustment_)) / srcFs_;
  //cout << "\nchecking: " << gtBeatsFile_ << "; beatTime: " << beatTime << endl;

  ifstream inStream;
  mrs_string line;
  inStream.open(gtBeatsFile_.c_str());
  getline (inStream, line); //get first line

  //if beat times in gt file are all given in the first line (separated by spaces)
  if(isGTFileInLine(line))
  {
    mrs_bool beatPosFound = false;
    mrs_natural curGTBeatPos, firstGTBeatPos;
    mrs_real lastGTBeatTime = 0, curGTBeatTime;
    mrs_real lastDiffBeats; //just a big number
    mrs_real diffBeats = 0;
    mrs_real tolWinLft, tolWinRgt;

    //check gtfile for eof at first run
    if(lastGTBeatPos_ < 0)
    {
      std::istringstream iss(line);
      char c[10]; //big enough array
      while (iss >> c) //space ("") delimiter
      {
        //last c contains last value in file
        lastGTFileBeat_ = atof(c); //save end value
      }
      iss.clear();
    }

    //check if end of file
    mrs_real beatTimeCheck = (((timeElapsed_+2) * hopSize_) - (frames2SecsAdjustment_)) / srcFs_; //(timeElapsed_+1 (+1 tolerance) because induction is always called in the next tick)
    if(beatTimeCheck >= lastGTFileBeat_)
      return 0;

    //cout << "pos1: " << pos1 << "; pos2: " << pos2 << "; pos3: " << pos3 << "bt: " << strtod(line.substr(pos1, pos2).c_str(), NULL) << endl;

    //discard initial induction time if in real-time mode (no backtrace)
    if((backtraceEndTime_ == -1 && !backtrace_) || timeElapsed_ > backtraceEndTime_)
    {
      mrs_real indTimeSecs = ((inductionTime_ * hopSize_) - (frames2SecsAdjustment_)) / srcFs_;
      do
      {
        //cout << "1-lastGTBeatTime: " << lastGTBeatTime << "; indTime: " << indTimeSecs << endl;
        curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_); //current delimiter
        lastGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);

        if(lastGTBeatTime >= indTimeSecs) break;

        lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current last delimiter

      } while(lastGTBeatTime < indTimeSecs);
    }
    else //retrieve first gt beat time (starting from previous call) [First iteration must be outside cycle!!]
    {
      //cout << "1-BeatTime: " << beatTime << "; GTBeatTime: " << lastGTBeatTime << "; lasGTPos: " << lastGTBeatPos_ << endl;

      lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_); //current last delimiter
      curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current delimiter
      lastGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);
    }

    firstGTBeatPos = lastGTBeatPos_; //save current first gt beat position

    //cout << "2-BeatTime: " << beatTime << "; GTBeatTime: " << lastGTBeatTime << "; lasGTPos: " << lastGTBeatPos_ << endl;

    //first match check:
    tolWinLft = lastGTBeatTime - fMeasureTol;
    tolWinRgt = lastGTBeatTime + fMeasureTol;
    if(beatTime >= tolWinLft && beatTime <= tolWinRgt)
    {
      localBeatErrorCount = 0;
      lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //go to next position (for next computed beat)
      //cout << "MATCHES1-> B: " << beatTime << "; gtB: " << lastGTBeatTime << "; tolL: " << tolWinLft << "; tolR: " << tolWinRgt << endl;
    }
    else //if first gt not matches:
    {
      //FALSE NEGATIVE:
      //if last error was a false positive
      if(beatTime > tolWinRgt && lastGTFalsePos_)
      {
        //advance one ground-truth beat (due to transition beat between FP and FN)
        lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current last delimiter
        curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current delimiter
        lastGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);
        tolWinLft = lastGTBeatTime - fMeasureTol;

        tolWinRgt = lastGTBeatTime + fMeasureTol;

        //cout << "2.2-BeatTime: " << beatTime << "; GTBeatTime: " << lastGTBeatTime << "; lasGTPos: " << lastGTBeatPos_ << endl;

        firstGTBeatPos = lastGTBeatPos_; //update current first gt beat position

        lastGTFalsePos_ = false;
      }

      lastDiffBeats = abs(beatTime - lastGTBeatTime);

      //cout << "(1)gtCurBeat: " << lastGTBeatTime << "(" << tolWinRgt << ") beatTime: " << beatTime << endl;
      //STILL FALSE NEGATIVE
      if(beatTime > tolWinRgt)
      {
        //start false negative search with current global errors
        mrs_natural beatErrorCount = 0;

        //cout << "FALSE_NEG(S)-> ";
        //count nr. of false negatives
        do {
          beatErrorCount++;

          //cout << "bT: " << beatTime << "; lastGTBT: " << lastGTBeatTime << " bE: " << beatErrorCount << " bT: " << triggerGtTolerance_ << endl;

          lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current last delimiter
          curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current delimiter
          lastGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);
          tolWinLft = lastGTBeatTime - fMeasureTol;
          tolWinRgt = lastGTBeatTime + fMeasureTol;

          //cout << lastGTBeatTime << "; ";

          //when beat-time matches current ground-truth beat-time:
          //- clean the global beat error count if < triggerGtTolerance_
          //- assign value to global error if surpasses triggerGtTolerance_ (for triggering induction)
          if(beatTime >= tolWinLft && beatTime <= tolWinRgt)
          {

            if((beatErrorCount + lostGTBeatsCount_) < triggerGtTolerance_)
              beatErrorCount = 0;

            //go to next delimiter
            lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1);

            break; //finishes false negative search when beat matches!
          }
          else if(beatTime <= tolWinRgt) break; //finishes false negative search when beat surpasses gt beat-time!

        } while(beatTime > tolWinRgt);

        localBeatErrorCount = beatErrorCount;
        //go to next delimiter
        //lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1);

        //cout << "(" << beatTime << ")" << endl;
      }
      //FALSE POSITIVE
      else
      {
        //mrs_real keepInitGTBeatTime = lastGTBeatTime; //keep it for printing in FP error
        do //check if false postive (reinforcement -> one iteration should be enough)
        {
          lastGTBeatPos_ = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current last delimiter
          curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current delimiter
          curGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);

          diffBeats = abs(beatTime - curGTBeatTime);
          //cout << "gtCurBeat: " << curGTBeatTime << "; beatTime: " << beatTime << "; diff: " << diffBeats << endl;

          //find min distance between computed and annotated beat time
          if(diffBeats > lastDiffBeats) //if current distance bigger than previous => check if computed is correct
          {
            //computed beat is correct if it matches the annotated beat within a tolerance window
            tolWinLft = lastGTBeatTime - fMeasureTol;
            tolWinRgt = lastGTBeatTime + fMeasureTol;

            //cout << "tolLft: " << tolWinLft << "; tolRgt: " << tolWinRgt << endl;
            if(beatTime >= tolWinLft && beatTime <= tolWinRgt) //for safecheck
            {
              //beat matches!
              localBeatErrorCount = 0;
              //cout << "MATCHES2-> B: " << beatTime << "; gtB: " << lastGTBeatTime << "; tolL: " << tolWinLft << "; tolR: " << tolWinRgt << endl;
            }
            else //false positive!
            {
              localBeatErrorCount = 1;
              //cout << "FALSE_POS-> B: " << beatTime << " (" << keepInitGTBeatTime << ") -> GT: " << lastGTBeatTime << endl;
              lastGTBeatPos_ = firstGTBeatPos; //go back to previous beat
              lastGTFalsePos_ = true;
            }

            beatPosFound = true;
            break;
          }
          lastDiffBeats = diffBeats;
          lastGTBeatTime = curGTBeatTime;

        } while(beatPosFound == false);
      }
    }
  }
  /*
  else //if beat times given in column (one beat times per line)
  {

  }
  */
  /*
  //if more than two beatTimes given in the annotation file
  //discart initial beatTime (due to inconsistencies in the beggining of some annotation files)
  if(pos1 >= 0)
  {
  	gtBeatTime1_ = strtod(line.substr(pos0+1, pos1).c_str(), NULL);
  	gtBeatTime2_ = strtod(line.substr(pos1+1, pos2).c_str(), NULL);
  	gtAfter2ndBeat_ = true;
  }
  else //if only two beatTimes given keep them as they are
  {
  	gtBeatTime1_ = strtod(line_.substr(0, pos1).c_str(), NULL);
  	gtBeatTime2_ = strtod(line_.substr(pos0+1, pos1).c_str(), NULL);
  }

  //==========================================================

  //to assure that it could read from file => beatFile in column (ifnot => beatFile in row)
  if(gtBeatTime1_ == gtBeatTime2_ || gtBeatTime2_ == 0.0 || gtBeatTime2_ > 40)
  {
  	gtBeatTime1_ = atof(line_.c_str());
  	getline (inStream_, line_);
  	gtBeatTime2_ = atof(line_.c_str());

  	//if more than two beatTimes given in the annotation file
  	//discart initial beatTime (due to inconsistencies in the beggining of some annotation files)
  	if(getline (inStream_, line_) > 0)
  	{
  		gtBeatTime1_ = gtBeatTime2_;
  		gtBeatTime2_ = atof(line_.c_str());
  		gtAfter2ndBeat_ = true;
  	}
  }
  */
  //for printing:
  //mrs_natural curGTBeatPos = (mrs_natural) line.find_first_of(" ", lastGTBeatPos_+1); //current delimiter
  //mrs_real lastGTBeatTime = strtod(line.substr(lastGTBeatPos_+1, curGTBeatPos).c_str(), NULL);
  //cout << "nextGTBeatTime: " << lastGTBeatTime << endl;
  //cout << "beatError: " << localBeatErrorCount << endl;
  return localBeatErrorCount;
}

void
BeatReferee::grantPoolSpaceForTriggerAgents(mrs_realvec triggerAgentsHypotheses)
{
  mrs_real agentInitScore;
  for(int a = 0; a < triggerAgentsHypotheses.getRows(); a++)
  {
    agentInitScore = triggerAgentsHypotheses(a, 2);
    grantPoolSpace(-1, agentInitScore);

    //cout << timeElapsed_ << ": CHECKING POOL SPACE for score: " << agentInitScore << endl;



  }
}

//define clusters of similar periods
//(like IBI histograms -> Dixon2001)
mrs_realvec
BeatReferee::clusterIBIs()
{
  mrs_natural similarityTol = 2; //(2frames tol => ~23ms with hopSize = 512 && srcFr = 44100)
  mrs_real minPerDiff = MAXREAL;
  mrs_natural cluster = -1;
  mrs_realvec periodClusters(nrAgents_, nrAgents_); //(agents belonging to that cluster, per cluster index)
  mrs_realvec periodClustersInfo(nrAgents_, 2); //(nr. of cluster members, per cluster index; mean Period of each cluster, per cluster index)
  mrs_real agentPeriod;
  mrs_natural nrClusterMembers;
  mrs_real clusterPerMean;

  for(int a = 0; a < nrAgents_; a++) //for all agents
  {
    //if(!mutedAgentsTmp_(a)) //if agent exists
    //only consider alive agents, which existed before (not created in this frame - to avoid adoption between brothers of the same generation)
    if(!mutedAgentsTmp_(a) && !agentsJustCreated_(a) && agentsFamilyHist_(a, ((mrs_natural) beatCounter_(a)-1)) == a)
    {
      agentPeriod = lastPeriods_(a);

      //cout << "AGENT " << a << ": PERIOD: " << agentPeriod << endl;

      for(int c = 0; c < nrAgents_; c++) //for all clusters
      {
        nrClusterMembers = (mrs_natural)periodClustersInfo(c, 0);
        clusterPerMean = periodClustersInfo(c, 1);

        //cout << "\nCluster: " << c << "; nrMembers: " << nrClusterMembers << "; perMean: " << clusterPerMean;

        if(nrClusterMembers > 0) //if cluster already have members
        {
          //calc diff between period and cluster current mean
          mrs_real perDiff = fabs(agentPeriod - clusterPerMean);

          //cout << "; perDiff: " << perDiff << "; minDiff: " << minPerDiff << endl;

          if(perDiff <= similarityTol) //check if diff respects tolerance
          {
            //check minimum difference to assign period to that cluster
            if(perDiff < minPerDiff)
            {
              minPerDiff = perDiff;
              cluster = c; //keep cluster index which presents minimum difference

              //cout << "Agent " << a << " tmp assigned to " << cluster << endl;
            }
          } //if not below tolerance keepon searching for a similar cluster
        }
      }

      if(cluster >= 0) //if cluster exists => assign agent and its period to chosen cluster
      {
        mrs_natural selectedClusterNrMembers = (mrs_natural)periodClustersInfo(cluster, 0);
        mrs_real selectedClusterMean = 0.0;

        periodClusters(cluster, selectedClusterNrMembers) = a; //save agent in free cluster index

        selectedClusterNrMembers++;
        //calculate cluster mean period
        for(int m = 0; m < selectedClusterNrMembers; m++)
        {
          selectedClusterMean += lastPeriods_((mrs_natural)periodClusters(cluster, m));
          //if(timeElapsed_ == 3898)
          //	cout << "curClMean: " << selectedClusterMean << "; curPer: " << lastPeriods_((mrs_natural)periodClusters(cluster, m))
          //		<< " from " << periodClusters(cluster, m) << endl;
        }
        selectedClusterMean = selectedClusterMean / selectedClusterNrMembers;

        //if(timeElapsed_ == 3898)
        //	cout << "\nAgent " << a << "(" << periodClusters(cluster, selectedClusterNrMembers-1) << ") ASSIGNED TO "
        //		<< cluster << "; newPer: " << agentPeriod << "; lastMean: " << periodClustersInfo(cluster, 1)
        //		<< "; updMembers: " << selectedClusterNrMembers << "; updClPerMean: " << selectedClusterMean << endl;

        periodClustersInfo(cluster, 0) = selectedClusterNrMembers;
        periodClustersInfo(cluster, 1) = selectedClusterMean;

        minPerDiff = MAXREAL;
        cluster = -1; //reset selected cluster
      }
      else //if not -> create new cluster with this agent as first member
      {
        //cout << "NEW CLUSTER FOR AGENT " << a << endl;

        for(int c = 0; c < nrAgents_; c++) //for all clusters -> select first empty
        {
          nrClusterMembers = (mrs_natural)periodClustersInfo(c, 0);
          if(nrClusterMembers == 0) //if cluster is empty
          {
            periodClusters(c, nrClusterMembers) = a;
            periodClustersInfo(c, 0) = 1;
            periodClustersInfo(c, 1) = agentPeriod; //initial cluster mean period

            //cout << "Agent " << a << "(" << periodClusters(c, nrClusterMembers) << ") ASSIGNED TO NEW: " << c
            //	<< "; updMembers: " << periodClustersInfo(c, 0) << "; updClPerMean: " << periodClustersInfo(c, 1) << endl;

            break;
          }
        }
      }
    }
  }

  //MATLAB_PUT(periodClusters, "clustersPer1");
  //MATLAB_PUT(periodClustersInfo, "clustersPerInfo1");

  //in the end cluster similar clusters due to cluster period mean being constantly updated during search above
  //(use same tolerance to cluster them)
  mrs_natural nrClusterMembers1, nrClusterMembers2, nrClusterMembersTot;
  mrs_real clusterPerMean1, clusterPerMean2;
  mrs_real clusterPerMeanTot = 0.0;
  for(int c = 0; c < nrAgents_; c++) //for all clusters
  {
    nrClusterMembers1 = (mrs_natural)periodClustersInfo(c, 0);

    //cout << "\nVERIFYING cluster1: " << c << "; memb1: " << nrClusterMembers1;

    if(nrClusterMembers1 > 0) //if cluster exists
    {
      clusterPerMean1 = periodClustersInfo(c, 1);

      //cout << "; perMean1: " << clusterPerMean1;

      for(int cc = 0; cc < nrAgents_; cc++) //for all other clusters
      {
        nrClusterMembers2 = (mrs_natural)periodClustersInfo(cc, 0);

        //cout << "\nCOMP with2: " << cc << "; meb2: " << nrClusterMembers2;

        if(cc != c && nrClusterMembers2 > 0) //if 2nd cluster is different than first, and it also exists
        {
          clusterPerMean2 = periodClustersInfo(cc, 1);

          //cout << "; perMean2: " << clusterPerMean2 << endl;

          //if both clusters are considered similar
          if(fabs(clusterPerMean1 - clusterPerMean2) <= similarityTol)
          {
            //transfer all info from second cluster to first and erase second
            nrClusterMembersTot = nrClusterMembers1 + nrClusterMembers2;
            for(int i = 0; i < nrClusterMembers2; i++)
            {
              periodClusters(c, nrClusterMembers1+i) = periodClusters(cc, i);
              periodClusters(cc, i) = 0.0;
            }

            //calculate cluster mean period
            for(int m = 0; m < nrClusterMembersTot; m++)
              clusterPerMeanTot += lastPeriods_((mrs_natural)periodClusters(c, m));

            clusterPerMeanTot = clusterPerMeanTot / nrClusterMembersTot;

            //update first cluster info
            periodClustersInfo(c, 0) = nrClusterMembersTot;
            periodClustersInfo(c, 1) = clusterPerMeanTot;
            //erase second cluster info
            periodClustersInfo(cc, 0) = 0.0;
            periodClustersInfo(cc, 1) = 0.0;

            //cout << "CLUSTER2: " << cc << " CLUSTERED WITH1: " << c << endl;
          }
        }
      }
    }
  }

  //MATLAB_PUT(periodClusters, "clustersPer2");
  //MATLAB_PUT(periodClustersInfo, "clustersPerInfo2");

  //cout << "\n===================!!OK!!====================" << endl;

  //return all clusters members, per cluster index (per line); and last two columns with nrElems; meanPer of each cluster
  mrs_realvec completedClustersPer(nrAgents_, nrAgents_+2);
  //copy periodClusters realvec
  for(int i = 0; i < nrAgents_; i++)
  {
    for(int j = 0; j < nrAgents_; j++)
    {
      completedClustersPer(i, j) = periodClusters(i, j);
      //cout << "copied " << i << " + " << j << "; completedClustersPer: " << completedClustersPer(i, j) << endl;
    }

    //add column with mean periods of each cluster
    completedClustersPer(i, nrAgents_) = periodClustersInfo(i, 0);
    completedClustersPer(i, nrAgents_+1) = periodClustersInfo(i, 1);

    //MATLAB_PUT(completedClustersPer, "completedClustersPerTMP");

    //cout << "copied " << i << " + " << nrAgents_ << ": " << completedClustersPer(i, nrAgents_) << "; info: " << periodClustersInfo(i, 1) << endl;
  }

  //cout << "\n===================!!OKTOTAL!!====================" << endl;

  //MATLAB_PUT(completedClustersPer, "completedClustersPer");

  return completedClustersPer;
}

//Get best similar agent to selected -> pick the best agent from the most similar period cluster
mrs_natural
BeatReferee::getBestSimilarAgent3(mrs_natural newAgentPeriod, mrs_realvec completedClustersPer)
{
  //if(timeElapsed_ == 3583)
  //	MATLAB_PUT(completedClustersPer, "ClusterPerBestSimilar");

  //if(timeElapsed_ == 3583)
  //	cout << "GET BEST SIMILAR FOR PERIOD " << newAgentPeriod << endl;

  //check to each period cluster belongs the new agent
  mrs_real minPerDiff = MAXREAL;
  mrs_natural selectedCluster = -1;
  for(int c = 0; c < nrAgents_; c++) //for all clusters
  {
    if(completedClustersPer(c, nrAgents_) > 0.0) //if cluster exists
    {
      mrs_real clusterPeriod = completedClustersPer(c, nrAgents_+1);
      mrs_real perDiff = fabs(newAgentPeriod - clusterPeriod);

      //cout << "\nCLUSTER " << c << "; clusterPer: " << clusterPeriod << "; perDiff: " << perDiff << "; minDiff: " << minPerDiff << endl;

      //check min diff between with all clusters
      if(perDiff < minPerDiff)
      {
        minPerDiff = perDiff;
        selectedCluster = c;

        //cout << "Tmp assigned to cluster " << selectedCluster << " with diff: " << perDiff << endl;
      }
    }
  }

  //pick best agent from selected period cluster
  mrs_real bestClusterScore = NA;
  mrs_natural bestClusterAgent = -1;
  mrs_natural clusterAgent;
  mrs_natural nrSelectedClusterMembers = (mrs_natural) completedClustersPer(selectedCluster, nrAgents_);

  //if(timeElapsed_ == 3898)
  //	cout << "\nASSIGNED TO CLUSTER " << selectedCluster << " WITH " << nrSelectedClusterMembers
  //		<< " ELEMENTS and " << completedClustersPer(selectedCluster, nrAgents_+1)
  //		<< " PERIOD for Agentperiod: " << newAgentPeriod << endl;

  for(int a = 0; a < nrSelectedClusterMembers; a++)
  {
    clusterAgent = (mrs_natural) completedClustersPer(selectedCluster, a);

    //cout << "From Cluster " << selectedCluster << "; agent: " << clusterAgent << "; Score: " << score_(clusterAgent)
    //	<< "; bestClSc: " << bestClusterScore << endl;

    if(!mutedAgents_(clusterAgent) && (clusterAgent) > bestClusterScore) //verify if agent really exists
    {
      bestClusterScore = score_(clusterAgent);
      bestClusterAgent = clusterAgent;

      //cout << "Tmp Best Similar: " << bestClusterAgent << "; bestClSc: " << bestClusterScore << endl;
    }
  }

  //if(timeElapsed_ == 3100)
  //	cout << "BEST CLUSTER AGENT: " << bestClusterAgent << " WITH SCORE: " << bestClusterScore << "(" << score_(clusterAgent) << ")" << endl;

  return bestClusterAgent;
}

//Get best similar agent to selected -> based on heuristics
//[CHANGE -> IGNORE PHASE IN HEURISTICS -> transition phase is handled by handleAgentsTansition()!!!]
mrs_natural
BeatReferee::getBestSimilarAgent(mrs_natural newAgentPeriod, mrs_natural newAgentInitPhase, mrs_real newAgentScore)
{
  mrs_real bestSimilarScore = NA; //just a big negative nr
  mrs_natural bestSimilarAgent = -1;
  mrs_natural period, phaseRaw, phase, k;
  mrs_realvec periodDiffs(nrAgents_);
  mrs_realvec phaseDiffs(nrAgents_);
  mrs_realvec bestSimilarity(nrAgents_);
  mrs_real fraction;

  //if(timeElapsed_ == 2461 && newAgentPeriod == 57)
  //	cout << "NEW AGENT-> period: " << newAgentPeriod << "; phase: " << newAgentInitPhase << endl;

  //Grant available space in the pool, by removing the worst agent, if needed
  grantPoolSpace(-1, newAgentScore);

  //calculate maximum possible difference (for normalizing the best similarity heuristics)
  //maximum difference occur when one agent has the maxPeriod and other the minPeriod
  //=> maximum phase difference for this period difference = minPeriod
  mrs_real maxHypDiff = (3 * (maxPeriod_ - minPeriod_) + minPeriod_);
  //save all period and phase differences with each agent
  for(int a = 0; a < nrAgents_; a++)
  {
    //only consider alive agents, which existed before (not created in this frame - to avoid adoption between brothers of the same generation)
    //if(!mutedAgentsTmp_(a) && !agentsJustCreated_(a))
    if(!mutedAgentsTmp_(a) && !agentsJustCreated_(a) && agentsFamilyHist_(a, ((mrs_natural) beatCounter_(a)-1)) == a)
    {
      //cout << "Agent " << a << " ok: " << agentsFamilyHist_(a, ((mrs_natural) beatCounter_(a)-1)) << endl;
      period = (mrs_natural) lastPeriods_(a);
      phaseRaw = (mrs_natural) lastPhases_(a);

      //transpose compared agent phase to new agent phase location
      k = (mrs_natural) (((mrs_real)(newAgentInitPhase - phaseRaw) / period) + 0.5);
      phase = phaseRaw + k * period;

      periodDiffs(a) = abs(newAgentPeriod - period);
      phaseDiffs(a) = abs(newAgentInitPhase - phase);

      //heuristics for calculating degree of best similarity with each agent
      //compared agent's score proportion of (3*periodDiff + 1*phaseDiff) [give 3x more relevance to period than phase]
      //bestSimilarity(a) = ((1/(3*periodDiffs(a))) + (1/phaseDiffs(a))) * score_(a);
      fraction = (1 - ((3*periodDiffs(a) + phaseDiffs(a)) / maxHypDiff));

      if(score_(a) > 0) //to avoid bestScore inversions
        bestSimilarity(a) = fraction * score_(a);
      else
        bestSimilarity(a) = score_(a) / fraction;

      //check who's best similar based on heuristics above
      if(bestSimilarity(a) > bestSimilarScore)
      {
        //if(timeElapsed_ == 2461 && newAgentPeriod == 57)
        //	cout << "============ curBestSimilarScore: " << bestSimilarScore << "; new from " << a << ": " << bestSimilarity(a) << endl;
        bestSimilarScore = bestSimilarity(a);
        bestSimilarAgent = a;
      }

      //if(timeElapsed_ == 2461 && newAgentPeriod == 57)
      //	cout << "Agent " << a << "; period: " << period << "; phaseRaw: " << phaseRaw << "; phase: " << phase << "(" << k
      //		<< "); perDiff: " << periodDiffs(a) << "; phDiff: " << phaseDiffs(a) << "; fraction: " << fraction
      //		<< "; score: " << score_(a) << "; simSc: " << bestSimilarity(a) << " (" << bestSimilarScore << "->"
      //		<< bestSimilarAgent << ")" << endl;
    }
  }

  //if(timeElapsed_ == 2461 && newAgentPeriod == 57)
  //	cout << "BEST SIMILAR AGENT: " << bestSimilarAgent << endl;

  return bestSimilarAgent;
}

/*
//get similar existing agent with best score
//[MAYBE CHANGE THIS TO (1/PERIODDIFF)*SCORE]
mrs_natural
BeatReferee::getBestSimilarAgent2(mrs_natural newAgentPeriod, mrs_natural newAgentInitPhase)
{
	mrs_natural nrSimilar = 3; //nr of considered most simlar agents
	mrs_real bestSimilarScore = NA;
	mrs_natural bestSimilarAgent = -1;
	mrs_natural period;
	mrs_natural minPerDiff = 1000; //just a big number
	mrs_natural mostSimilarAgent = -1;
	mrs_realvec periodDiffs(nrAgents_);

	//save all period differences with each agent
	for(int a = 0; a < nrAgents_; a++)
	{
		period = (mrs_natural) lastPeriods_(a);
		periodDiffs(a) = abs(newAgentPeriod - period);
	}

	//cout << "NewAgent!" << endl;

	//sort periodDiffs realvec
	mrs_realvec periodDiffsSorted = periodDiffs;
	periodDiffsSorted.sort();
	mrs_natural maxPeriodDiff = (mrs_natural) periodDiffsSorted(nrSimilar);

	//check nrSimilar most similar agents
	for(int a = 0; a < nrAgents_; a++)
	{
		//cout << "Diff " << a << ": " << periodDiffsSorted(a) << "; UnSort: " << periodDiffs(a) << "; Score: " << score_(a) << endl;
		if((mrs_natural) periodDiffs(a) <= maxPeriodDiff) //only consider nrSimilar most similar agents
		{
			//check who's best
			if(score_(a) > bestSimilarScore)
			{
				bestSimilarScore = score_(a);
				bestSimilarAgent = a;

				//cout << "BestSimilar: " << bestSimilarAgent << "; Score: " << bestSimilarScore << endl;
			}
		}
	}

	return bestSimilarAgent;
}
*/

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
/*
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
*/

mrs_natural
BeatReferee::existEqualBetterAgents(mrs_natural agentIndex, mrs_natural agentPeriod, mrs_natural agentPhase, mrs_real newAgentScore)
{
  //cout << timeElapsed_ << ": isEqualAgents? period: " << agentPeriod  << "; phase: " << agentPhase << endl;

  mrs_natural firstEqualBetterAgent = -1;
  mrs_realvec equalAgents = getEqualAgents(agentIndex, agentPeriod, agentPhase);

  for(mrs_natural a = 0; a < nrAgents_; a++)
  {
    if((mrs_natural) equalAgents(a) == 1)
    {
      if(score_(a) >= newAgentScore)
      {
        firstEqualBetterAgent = a;
        break;
      }
    }
  }

  return firstEqualBetterAgent;
}

//return -1 if no existing equal agents or index of first equal agent
mrs_natural
BeatReferee::existEqualAgents(mrs_natural agentIndex, mrs_natural agentPeriod, mrs_natural agentPhase)
{
  //cout << timeElapsed_ << ": isEqualAgents? period: " << agentPeriod  << "; phase: " << agentPhase << endl;

  mrs_natural firstEqualAgent = -1;
  mrs_realvec equalAgents = getEqualAgents(agentIndex, agentPeriod, agentPhase);

  for(mrs_natural a = 0; a < nrAgents_; a++)
  {
    if((mrs_natural) equalAgents(a) == 1)
    {
      firstEqualAgent = a;
      break;
    }
  }

  return firstEqualAgent;
}

//get a vector with all agents equal to agentIndex
mrs_realvec
BeatReferee::getEqualAgents(mrs_natural agentIndex, mrs_natural agentPeriod, mrs_natural agentPhase)
{
  mrs_realvec equalAgents(nrAgents_);

  for(mrs_natural a = 0; a < nrAgents_; a++)
  {
    //if considered agent is not equal to the agent we want to compare
    //&& if the angent we want to compare is alive (i.e. exists) => then procede with comparison
    //(agenIndex == -1 => call after trigger induction)
    if(agentIndex == -1 || (agentIndex != a && !mutedAgentsTmp_(a)))
    {
      //2 agents are considered equal if their periods don't differ
      //more than 10ms (eq. 1frame) and their phases no more than 20ms (eq. 2frames)
      mrs_real phaseDiff = abs((agentPhase - lastPhases_(a)) / agentPeriod);
      mrs_natural periodDiff = abs(agentPeriod - (mrs_natural)lastPeriods_(a));

      //if(timeElapsed_ == 3949 || timeElapsed_ == 3968)
      //	cout << "Comparing " << agentIndex << " w/ per: " << agentPeriod << " & ph: " << agentPhase << " with Agent "
      //		<< a << "; period: " << lastPeriods_(a) << "; phase: " << lastPhases_(a)
      //		<< "; periodDiff: " << periodDiff << "(" << eqPeriod_ << "); phaseDiff:" << (phaseDiff - floor(phaseDiff))
      //		<< "(" << ((mrs_real)eqPhase_/(mrs_real)agentPeriod) << ")" << endl;

      if((periodDiff <= eqPeriod_) && ((phaseDiff - floor(phaseDiff)) <= ((mrs_real)eqPhase_/(mrs_real)agentPeriod)
                                       || (phaseDiff - floor(phaseDiff)) >= (1-((mrs_real)eqPhase_/(mrs_real)agentPeriod))))
      {
        equalAgents(a) = 1; //set agent index as true if equal

        //if(timeElapsed_ == 3949)
        //	cout << "====================" << timeElapsed_ << ": Agent " << agentIndex << " has EQUAL: " << a << endl;
      }
    }
  }

  return equalAgents;
}

//checks and kill the worse from all existing equal agents to agentIndex
void
BeatReferee::checkAndKillEqualAgents(mrs_natural agentIndex)
{
  //if(timeElapsed_ == 3949)
  //	cout << timeElapsed_ << ": REGULARLY CHECKING EQUAL AGENTS" << endl;

  mrs_realvec equalAgents = getEqualAgents(agentIndex, (mrs_natural) lastPeriods_(agentIndex), (mrs_natural) lastPhases_(agentIndex));

  for(mrs_natural a = 0; a < nrAgents_; a++)
  {
    if((mrs_natural)equalAgents(a) == 1) //if equal agent
    {
      //cout << "EQ!: " << agentIndex << "=" << a << endl;
      //From the two equal agents kill the one with lower score (if it isn't the current best agent)
      //(always kill the equal agents created at triggered inductions)
      ostringstream motif;
      if(score_(agentIndex) >= score_(a))
      {
        if(a != bestAgentIndex_)
        {
          motif << "EQ";
          killAgent(a, motif.str(), agentIndex);

          //cout << "1-KILL Agent " << a << " (" << score_(a) << ") EQUAL TO Agent " << agentIndex
          //	<< " (" << score_(agentIndex) << ")" << endl;
        }
        else //if in trigger induction always kill just created agent (agentIndex), if equal to existing
        {
          motif << "EQ";
          killAgent(agentIndex, motif.str(), a);
          break; //in this case breaks because considered agent no longer exists.

          //cout << timeElapsed_ << ": 2-KILL Agent " << agentIndex << " (" << score_(agentIndex)
          //	<< ") EQUAL TO Agent " << a << " (" << score_(a) << ")" << endl;
        }
      }
      else
      {
        if(agentIndex != bestAgentIndex_)
        {
          motif << "EQ";
          killAgent(agentIndex, motif.str(), a);
          break; //in this case breaks because considered agent no longer exists.

          //cout << "3-KILL Agent " << agentIndex << " (" << score_(agentIndex) << ") EQUAL TO Agent "
          //	<< a << " (" << score_(a) << ")" << endl;
        }
        else
        {
          motif << "EQ";
          killAgent(a, motif.str(), agentIndex);

          //cout << "1-KILL Agent " << a << " (" << score_(a) << ") EQUAL TO Agent "
          //	<< agentIndex << " (" << score_(agentIndex) << ")" << endl;
        }
      }
    }
  }
}

//returns true if agent created at trigered induction is killed due to be equal to existing agent
/*
mrs_bool
BeatReferee::checkAndKillEqualAgents(mrs_natural agentIndex, mrs_bool triggerInductionCheck)
{
	mrs_bool killedAgentIndex = false;
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
				//cout << "EQ!: " << agentIndex << "=" << a << endl;
				//From the two equal agents kill the one with lower score (if it isn't the current best agent)
				//(always kill the equal agents created at triggered inductions)
				ostringstream motif;
				if(score_(agentIndex) >= score_(a))
				{
					//if(a != bestAgentIndex_)
					if(a != bestAgentIndex_ && !triggerInductionCheck)
					{
						motif << "EQ";
						killAgent(a, motif.str(), agentIndex);
						killedAgentIndex = false;

						//cout << "1-KILL Agent " << a << " (" << score_(a) << ") EQUAL TO Agent " << agentIndex
						//	<< " (" << score_(agentIndex) << ")" << endl;
					}
					else //if in trigger induction always kill just created agent (agentIndex), if equal to existing
					{
						motif << "EQ";
						killAgent(agentIndex, motif.str(), a);
						killedAgentIndex = true;
						break; //in this case breaks because considered agent no longer exists.

						//if(triggerInductionCheck)
						//	cout << timeElapsed_ << ": 2-KILL Agent " << agentIndex << " (" << score_(agentIndex)
						//		<< ") EQUAL TO Agent " << a << " (" << score_(a) << ")" << endl;
					}
				}
				else
				{
					if(agentIndex != bestAgentIndex_)
					{
						motif << "EQ";
						killAgent(agentIndex, motif.str(), a);
						killedAgentIndex = true;
						break; //in this case breaks because considered agent no longer exists.

						//cout << "3-KILL Agent " << agentIndex << " (" << score_(agentIndex) << ") EQUAL TO Agent "
							//	<< a << " (" << score_(a) << ")" << endl;
					}
					else
					{
						motif << "EQ";
						killAgent(a, motif.str(), agentIndex);
						killedAgentIndex = false;

						//cout << "1-KILL Agent " << a << " (" << score_(a) << ") EQUAL TO Agent "
							//	<< agentIndex << " (" << score_(agentIndex) << ")" << endl;
					}
				}
			}
		}
	}
	return killedAgentIndex;
}
*/

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
BeatReferee::getWorstAgent(mrs_natural callAgent)
{
  //By default lowest score = score from first agent
  //mrs_natural firstAlive= getFirstAliveAgent();
  //mrs_real lowestScore = score_(firstAlive);

  mrs_real lowestScore = MAXREAL;
  mrs_natural lowestIndex = 0;

  //if(timeElapsed_ == 3100)
  //	cout << "Getting Worst Agent - FIRST: " << lowestIndex << "; sc: " << lowestScore << "; mutedAgentsTmp: " << mutedAgentsTmp_(lowestIndex);

  for(mrs_natural a = lowestIndex; a < nrAgents_; a++)
  {
    //never kill bestAgentBeforeTrigger && verify that agent really exists and it's not the call agent
    if(a != bestAgentBeforeTrigger_ && !mutedAgentsTmp_(a) && a != callAgent && score_(a) < lowestScore)
    {
      lowestScore = score_(a);
      lowestIndex = a;

      //if(timeElapsed_ == 876)
      //	cout << "; Actual: " << lowestIndex << "; sc: " << lowestScore << "; Agent: " << a
      //		<< "; bestAgentBeforeTrigger: " << bestAgentBeforeTrigger_ << endl;
    }
  }

  //if(timeElapsed_ == 3100)
  //	cout << endl;

  //return worst agent
  return lowestIndex;
}

mrs_realvec
BeatReferee::calculateNewHypothesis(mrs_natural agentIndex, mrs_natural oldPeriod, mrs_natural prevBeat, mrs_natural error)
{
  (void) agentIndex; // [!] what was this supposed to do?
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
  //	"; Beta: " << beta << "; maxPer: " << maxPeriod_ << "; minPer: " << minPeriod_ << endl;

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
  if(abs(newPeriod2 - newPeriod1) <= eqPeriod_ && abs(nextBeat2 - nextBeat1) <= eqPhase_)
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

  //if(timeElapsed_ == 687)
  //	cout << "CREATE AGENTS FROM: " << agentIndex << "; error: " << error << "; per: " << oldPeriod << "; ph: " << prevBeat << endl;

  mrs_real newScore;
  if(agentScore >= 0.0)
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
  //if(timeElapsed_ == 2046)
  //	cout << "NEW AGENT(" << timeElapsed_ << "-" << ((timeElapsed_ * hopSize_) - (hopSize_/2)) / srcFs_ << ") (reqBy:" << agentIndex <<
  //		") -> PrevBeat:" << prevBeat << " Period:" << oldPeriod << " NextBeat1:" << newHypotheses(0,1) << " NewPeriod1:" <<
  //		newHypotheses(0,0) << " NextBeat2:" << newHypotheses(1,1) << " NewPeriod2:" << newHypotheses(1,0) <<
  //		" NextBeat3:" << newHypotheses(2,1) << " NewPeriod3:" << newHypotheses(2,0) <<
  //		" Error:" << error << " Score:" << newScore << endl;
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
  }

  //Display Updated BeatAgent:
  //cout << "UPDATING AGENT" << agentIndex <<" (" << timeElapsed_ << ")" << " -> oldPeriod: " << oldPeriod <<
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
  agentControl_(agentIndex, 3) = timeElapsed_;

  updControl(ctrl_agentControl_, agentControl_);

  lastPeriods_(agentIndex) = newPeriod;
}

//grantPoolSpace given the nr of elements in the newAgentsScore realvec
void
BeatReferee::grantPoolSpace2(mrs_natural callAgent, mrs_natural nrRequired, mrs_realvec newAgentsScore)

{

  //if(timeElapsed_ == 3100)
  //{
  //	for(int i = 0; i < nrRequired; i++)
  //		cout << "NEW " << i << "-> Agents Score: " << newAgentsScore(i) << "; elems: " << nrRequired << endl;
  //}

  mrs_natural nrAvailable = 0;
  for(int a = 0; a < mutedAgentsTmp_.getSize(); a++)
  {
    if(mutedAgentsTmp_(a))
      nrAvailable++;
  }

  //if(timeElapsed_ == 3100)
  //	cout << "t: " << timeElapsed_ << ": AVAILABLE POOLSPACE: " << nrAvailable << "; required: " << nrRequired << endl;

  nrRequired = nrRequired - nrAvailable; //updated nr of required spaces given the ammount of free ones

  //if there are no free agents -> remove worsts!
  while(nrRequired > 0)
  {
    mrs_natural agentInd2Kill = getWorstAgent(callAgent);

    //if(timeElapsed_ == 3100)
    //	cout << "t: " << timeElapsed_ << ": KillWorstAgent: " << agentInd2Kill << "; newAgentSc: " << newAgentsScore(nrRequired-1) << "; worstScore: " << score_(agentInd2Kill) << endl;

    //only kills current worst agent in the pool if its score is smaller than the new agent score
    if(score_(agentInd2Kill) <= newAgentsScore(nrRequired-1))
      killAgent(agentInd2Kill, "POOL", callAgent);

    nrRequired--;
  }
}

void
BeatReferee::grantPoolSpace(mrs_natural callAgent, mrs_real newAgentScore)
{
  mrs_bool isAvailable = false;

  for(int a = 0; a < mutedAgentsTmp_.getSize(); a++)
  {
    //if(timeElapsed_ == 876)
    //		cout << "t: " << timeElapsed_ << ": CHECKING POOLSPACE @: " << a << "; newAgentSc: " << newAgentScore << endl;

    if(mutedAgentsTmp_(a))
    {
      //if(timeElapsed_ == 876)
      //	cout << "t: " << timeElapsed_ << ": FREE POOLSPACE @: " << a << "; newAgentSc: " << newAgentScore << endl;

      isAvailable = true;
      break;
    }
  }

  //if there are no free agents -> remove worst!
  if(!isAvailable)
  {
    mrs_natural agentInd2Kill = getWorstAgent(callAgent);

    //if(timeElapsed_ == 876)
    //	cout << "t: " << timeElapsed_ << ": KillWorstAgent: " << agentInd2Kill << "; newAgentSc: " << newAgentScore << "; worstScore: " << score_(agentInd2Kill) << endl;

    //only kills current worst agent in the pool if its score is smaller than the new agent score
    if(score_(agentInd2Kill) <= newAgentScore)
      killAgent(agentInd2Kill, "POOL", callAgent);
    //else if(logFile_)
    //	debugAddEvent("NO_POOL", callAgent, -1, -1, newAgentScore, score_(agentInd2Kill), agentInd2Kill);
  }
}

//routine for checking and recovering beat at transition bewteen father and son (for non-causal mode)
void
BeatReferee::handleAgentsTansition(mrs_natural agent)
{
  /*if(agent == 8 && timeElapsed_ >= 900)
  {
  	cout << timeElapsed_ << "-> FLAG- ";
  	for(int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
  		cout << j << ": " << considerFatherTransitionBeat_(agent, j) << "; ";

  	cout << endl;
  }*/

  mrs_natural fatherAgent, sonAgent;
  if(considerAgentTransitionBeat_(agent) == -1) //unconsider first beat from agent
  {
    sonAgent = agent;

    //if(sonAgent == 19 && timeElapsed_ >= 2461)
    //if(timeElapsed_ == 3946)
    //{
    //	cout << timeElapsed_ << ": TRANS_REM from " << sonAgent << "-> BEAT " << (beatCounter_(sonAgent)) << ": "
    //		<< agentsHistory_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) << "("
    //		<< (((agentsHistory_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) * hopSize_) - frames2SecsAdjustment_) / srcFs_)
    //		<< ") by: " << agentsFamilyHist_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) << endl;
    //	cout << timeElapsed_ << ": BEAT COUNT " << sonAgent << " = " << beatCounter_(sonAgent) << endl;
    //}

    agentsHistory_(sonAgent, (mrs_natural) beatCounter_(sonAgent)) = -1;
    agentsFamilyHist_(agent, (mrs_natural) beatCounter_(sonAgent)) = -1;

    beatCounter_(sonAgent)--; //decrement one count because it is incremented just after

    considerAgentTransitionBeat_(agent) = NA; //clean flag
  }

  fatherAgent = agent;
  //process all array to account for every sons' request
  for(int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
  {
    if(considerFatherTransitionBeat_(agent, j) >= 0) //consider all valid positions in father's flag array
    {
      sonAgent = (mrs_natural) considerFatherTransitionBeat_(agent, j);

      agentsHistory_(sonAgent, (mrs_natural) beatCounter_(sonAgent))
      = agentsHistory_(fatherAgent, (mrs_natural) beatCounter_(fatherAgent));
      agentsFamilyHist_(sonAgent, (mrs_natural) beatCounter_(sonAgent))
      = agentsFamilyHist_(fatherAgent, (mrs_natural) beatCounter_(fatherAgent));

      //if(sonAgent == 27 && fatherAgent == 8)
      //if(timeElapsed_ == 3946)
      //{
      //	cout << timeElapsed_ << ": TRANS_ADD to "<< sonAgent << "-> BEAT " << (beatCounter_(sonAgent)) << ": "
      //		<< agentsHistory_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) << "("
      //		<< (((agentsHistory_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) * hopSize_) - frames2SecsAdjustment_) / srcFs_)
      //		<< ") by: " << agentsFamilyHist_(sonAgent, (mrs_natural) (beatCounter_(sonAgent))) << endl;
      //	cout << timeElapsed_ << ": BEAT COUNT " << sonAgent << " = " << beatCounter_(sonAgent)+1 << endl;
      //}

      beatCounter_(sonAgent)++;

      considerFatherTransitionBeat_(agent, j) = NA; //clean flag
    }
  }
}
//clean agent's transition flag and its call in its father's flag
void
BeatReferee::cleanAgentAndFatherTransitionFlags(mrs_natural agentIndex)
{
  //if(agentIndex == 17 && timeElapsed_ == 1416)
  //	cout << "Agent: " << agentIndex << "; agentFlag: " << considerAgentTransitionBeat_(agentIndex) << endl;

  if(considerAgentTransitionBeat_(agentIndex) >= 0) //if it already has an assigned father whom requested its flag
  {
    mrs_natural fatherRequested = (mrs_natural) considerAgentTransitionBeat_(agentIndex);

    //if(agentIndex == 17 && timeElapsed_ == 1416)
    //	cout << timeElapsed_ << ": Agent: " << agentIndex << " CHECKING fatherAgentFLAG: " << fatherRequested << endl;

    //look in its father array for cleaning
    for(int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
    {
      if(considerFatherTransitionBeat_(fatherRequested, j) == agentIndex)
      {
        //if(timeElapsed_ == 3946)
        //if(agentIndex == 17 && timeElapsed_ == 1416)
        //	cout << timeElapsed_ << ": CLEANED agent " << agentIndex << "; from FATHER: " << fatherRequested << endl;

        considerFatherTransitionBeat_(fatherRequested, j) = NA;
        break;
      }
    }
  }
  considerAgentTransitionBeat_(agentIndex) = NA;
}


//Generic routine for creating new agents given their hypotheses:
mrs_natural
BeatReferee::createNewAgent(mrs_natural newPeriod, mrs_natural firstBeat,
                            mrs_real newScore, mrs_real beatCount, mrs_natural fatherAgent)
{
  //if father agent died in the same timestep as it requests a son creation then this request is unconsidered
  if(fatherAgent >= 0 && mutedAgentsTmp_(fatherAgent) == 1.0)
  {
    if(logFile_)
      debugAddEvent("CREATE_REF_KF", -1, newPeriod, firstBeat, newScore, bestScore_, fatherAgent);
    return -1;
  }
  //or if the score of the new agent is obsolete the newAgent request is also unconsidered
  else if (timeElapsed_ > timeBeforeKilling_ && newScore < bestScore_ && fabs(bestScore_-newScore) > 0.1
           && fabs(bestScore_ - newScore) > fabs(bestScore_ * obsoleteFactor_))
  {
    if(logFile_)
      debugAddEvent("CREATE_REF_SCORE", -1, newPeriod, firstBeat, newScore, bestScore_, fatherAgent);
    return -1;
  }

  //if(timeElapsed_ == 876)
  //	cout << " CREATED: Agent at: " << timeElapsed_ << "; Pe: " << newPeriod << "; Ph: " << firstBeat << endl;

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
      //if(timeElapsed_ == 876)
      //	cout << " CREATED: Agent " << a << " at: " << timeElapsed_ << "; Pe: " << newPeriod << "; Ph: " << firstBeat << endl;

      //Defines new hypothesis for this agent:
      setNewHypothesis(a, newPeriod, firstBeat);

      //Update score:
      score_(a) =  newScore;

      //if(timeElapsed_ == 3100)
      //	cout << "t: " << timeElapsed_ << "; CREATED AGENT: " << a << "; beatCountBef: " << beatCounter_(a) << "; bc4Bef: " << beatCounter_(4);

      //beatCounter of this agent equals beatCounter of its father
      beatCounter_(a) = beatCount;

      //if(timeElapsed_ > 3707)
      //	cout <<  "; beatCountAfter: " << beatCounter_(a) << "; bc4After: " << beatCounter_(4) << endl;

      //Update Agents' Periods and Phases (for equality checking)
      lastPeriods_(a) = newPeriod; //(Periods in frames)
      //lastPhases_(a) = firstBeat; //(Phases in frames
      lastPhases_(a) = (firstBeat - newPeriod); //(assuem last phase equal to first beat - agentPeriod)

      initPeriod_(a) = newPeriod; //save agent's initial IBI
      missedBeatsCount_(a) = 0.0; //reset missed beats counter

      //statsPeriods_(a, timeElapsed_) = newPeriod;
      //statsPhases_(a, timeElapsed_) = firstBeat;

      //statsAgentsLifeCycle_(a, timeElapsed_) = 1.0;

      //force timing update when agent is created
      //(don't know why when a new agent is created its time, in agentControl, keeps
      //the time of the previous tick)
      agentControl_(a, 3) = timeElapsed_+1;
      updControl(ctrl_agentControl_, agentControl_);

      returnCreatedAgent = a;

      agentsJustCreated_(a) = 1.0;

      if(logFile_)
        debugAddEvent("CREATE", a, newPeriod, firstBeat, score_(a), bestScore_, fatherAgent);

      //in nonCausalAnalysis keep agents history till the end of the analysis
      if(nonCausal_)
      {
        //if(timeElapsed_ == 1382 || timeElapsed_ == 1441)
        //	cout << "============t: " << timeElapsed_ << " - created new agent: " << a << " from: " << fatherAgent << endl;

        if(fatherAgent >= 0)
        {
          mrs_natural lastFlagInd = 0;

          //if(timeElapsed_ == 3946)
          //if(a == 9 && fatherAgent == 8 && timeElapsed_ == 3946)
          //	cout << "firstBeat: " << firstBeat << "; fatherAgent: " << fatherAgent << "; agent: "
          //		<< a << "; lastFPh: " << lastPhases_(fatherAgent) << "; FPer: " << lastPeriods_(fatherAgent)
          //		<< "; transFlag: " << considerFatherTransitionBeat_(a, 0) << "; agentTrans: "
          //		<< considerAgentTransitionBeat_(a) << endl;

          //clean agent's transition flag and its call in its father's flag
          //(just to be sure since it could be updated within the same frame)
          cleanAgentAndFatherTransitionFlags(a);

          //check beat at transition between father and son:
          //if(((firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) < -1.6)
          //if((abs(firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) < 0.6)
          if((abs(firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) < beatTransitionTol_)
          {
            //last beat from father and first beat from son are too close (in comparison to last father's ibi)
            //unconsider less than 60% difference => unconsider first son's beat
            //(set considerTransitionBeatFlag to -1)
            considerAgentTransitionBeat_(a) = -1;

            if(logFile_)
              debugAddEvent("TRANSITION_REM", a, (mrs_natural) lastPeriods_(a),
                            (mrs_natural) (firstBeat), score_(a), bestScore_, fatherAgent);


            //cout << timeElapsed_ << ": REQUEST UNCONSIDER: " << a << "; firstBeat: " << firstBeat << "; lastPhFather: " << lastPhases_(fatherAgent)
            //	<< " (" << (abs(firstBeat - lastPhases_(fatherAgent))) << "); lastPerFather: " << lastPeriods_(fatherAgent)
            //	<< "(" << (0.7 * lastPeriods_(fatherAgent)) << "); Flag: " << considerAgentTransitionBeat_(a) << endl;
          }
          //last beat from father and first beat from son are too far (in comparison to last father's ibi)
          //consider more than 60% difference => consider next father's beat as first son's beat
          //(set considerTransitionBeatFlag to agent's index onto his father's index)
          //if(((firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) > 1.6)
          else if((abs(firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) > (1+beatTransitionTol_))
          {
            //considerFatherTransitionBeat_(fatherAgent) = a;
            //retrieve first available position in the flag
            lastFlagInd = 0;
            for(int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
            {
              if(considerFatherTransitionBeat_(fatherAgent, j) < 0)
              {
                //if(a == 27 && fatherAgent == 8 && timeElapsed_ == 933)
                //	cout << timeElapsed_ << ": FREE FATHER " << fatherAgent << " w/ Flag @ ind: " << j << endl;

                lastFlagInd = j;
                break;
              }
            }
            considerFatherTransitionBeat_(fatherAgent, lastFlagInd) = a;

            considerAgentTransitionBeat_(a) = fatherAgent; //register son's call

            if(logFile_)
              debugAddEvent("TRANSITION_ADD", a, (mrs_natural) lastPeriods_(a),
                            (mrs_natural) (firstBeat+lastPeriods_(a)), score_(a), bestScore_, fatherAgent);

            //if(a == 27 && fatherAgent == 8 && timeElapsed_ == 933)
            //	cout << timeElapsed_ << ": REQUEST RECONSIDER: " << a << " from Father: " << fatherAgent << "; firstBeat: " << firstBeat
            //		<< "; lastPhFather: " << lastPhases_(fatherAgent) << " (" << (abs(firstBeat - lastPhases_(fatherAgent)))
            //		<< "); lastPerFather: " << lastPeriods_(fatherAgent) << "(" << (1.9 * lastPeriods_(fatherAgent))
            //		<< "); FatherFlag: " << considerFatherTransitionBeat_(fatherAgent, lastFlagInd) << "; sonFlag: "
            //		<< considerAgentTransitionBeat_(a) << endl;
          }

          //pass history (beats and family) of father agent to created one
          for(int i = 0; i < beatCount; i++)
          {
            agentsHistory_(a, i) = agentsHistory_(fatherAgent, i); //beats history
            agentsFamilyHist_(a, i) = agentsFamilyHist_(fatherAgent, i);

            //if((a == 1 && fatherAgent == 2 && timeElapsed_ == 1382) || (a == 0 && fatherAgent == 1 && timeElapsed_ == 1441))
            //if(a == 27 && fatherAgent == 8 && timeElapsed_ == 933)
            //if(timeElapsed_ == 3946)
            //{
            //	cout << timeElapsed_ << ": Agent " << a << " created by " << fatherAgent << "-> BEAT " << i << ": "
            //		<< (agentsHistory_(a, i)) << " by: " << agentsFamilyHist_(a, i) << endl;
            //	cout << "t: " << timeElapsed_ << "-BEAT " << i << ": " << (((agentsHistory_(a, i) * hopSize_) - hopSize_/2) / srcFs_)
            //		<< " by: " << agentsFamilyHist_(a, i) << endl;
            //	cout << "Diff btw " << a << " and Father " << fatherAgent << ": " << (abs(firstBeat - lastPhases_(fatherAgent)) / lastPeriods_(fatherAgent)) << endl;
            //}
          }

          //if((a == 1 && fatherAgent == 2 && timeElapsed_ == 1382) || (a == 0 && fatherAgent == 1 && timeElapsed_ == 1441))
          //if(timeElapsed_ == 3946)
          //if(a == 9 && fatherAgent == 8 && timeElapsed_ == 3946)
          //	cout << "2-BEAT " << beatCount << ": " << (((agentsHistory_(a, beatCount) * hopSize_) - hopSize_/2) / srcFs_)
          //		<< " by: " << agentsFamilyHist_(a, beatCount) << "; TRANSFLAG: " << considerFatherTransitionBeat_(a, 0)
          //		<< "; fatherTRANSFLAG: " << considerFatherTransitionBeat_(fatherAgent, lastFlagInd) << "; AgentFlag: "
          //		<< considerAgentTransitionBeat_(a) << endl;
        }
      }

      break;
    }
  }

  return returnCreatedAgent;
}

//Routine for killing all agents -> reseting the system
void
BeatReferee::resetSystem(mrs_natural saveAgent)
{
  //kill all agents
  for(int a = 0; a < mutedAgentsTmp_.getSize(); a++)
  {
    if(a != saveAgent) killAgent(a, "RESET");
  }

  //reset best score
  bestScore_ = NA; //To allow initial negative scores

  if(logFile_)
    debugAddEvent("RESET_SYSTEM", -1, -1, -1, -1, bestScore_, saveAgent);
}

//Generic routine for killing agents:
void
BeatReferee::killAgent(mrs_natural agentIndex, mrs_string motif, mrs_natural callAgent)
{
  //Never kill a best agent or a best before trigger agent (for enforcing security) -> a best agent must live until being replaced by a better one
  //&& verify that agent effectively exists (for safechecking)
  if(strcmp(motif.c_str(), "RESET") == 0 || (agentIndex != bestAgentBeforeTrigger_ && agentIndex != bestAgentIndex_ && !mutedAgentsTmp_(agentIndex)))
  {
    //Diplay killed BeatAgent:
    //if(timeElapsed_ == 1416)
    //	cout << "KILLED AGENT " << agentIndex << " (" << motif << ") With Score: " << score_(agentIndex) << " / " << bestScore_ << endl;

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

    score_(agentIndex) = NA;
    beatCounter_(agentIndex) = 0.0;

    //statsAgentsLifeCycle_(agentIndex, timeElapsed_) = -1;
    lastPeriods_(agentIndex) = 0.0; //(Periods in frames)
    lastPhases_(agentIndex) = 0.0; //(Phases in frames)

    missedBeatsCount_(agentIndex) = 0.0; //reset missed beats counter

    //erase transition flags from this agent -> to be sure it is no longer considered
    cleanAgentAndFatherTransitionFlags(agentIndex);
    for (int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
      considerFatherTransitionBeat_(agentIndex, j) = NA;

    //if(timeElapsed_ == 1416)
    //	cout << "Killed Agent " << agentIndex << "; muted: " << mutedAgentsTmp_(agentIndex) << "; sc: " << score_(agentIndex)
    //		<< "; bc: " << beatCounter_(agentIndex) << "; lper: " << lastPeriods_(agentIndex) << "; lph: " << lastPhases_(agentIndex)
    //		<< "; mbc: " << missedBeatsCount_(agentIndex) << endl;
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
    if(logFile_)
      debugAddEvent("BEST", bestLocalAgent, (mrs_natural) lastPeriods_(bestLocalAgent),
                    (mrs_natural) lastPhases_(bestLocalAgent), bestLocalScore, bestScore_);

    bestScore_ = bestLocalScore;
    bestAgentIndex_ = bestLocalAgent;
    bestFinalAgent_ = bestAgentIndex_;
  }
  //cout << "Absolute Best Agent: " << bestAgentIndex_ << " BestScore: " << bestScore_ << endl;
}

void
BeatReferee::debugCreateFile()
{
  mrs_string beatTrackingMode, triggerGTTol;
  if(nonCausal_)
    beatTrackingMode = "non-causal";
  else beatTrackingMode = "causal";
  if(strcmp(inductionMode_.c_str(), "groundtruth") == 0)
  {
    stringstream tmp;
    tmp << triggerGtTolerance_;
    triggerGTTol = tmp.str();
  }
  else triggerGTTol = "NA";

  fstream outStream;
  outStream.open(logFileName_.c_str(), ios::out|ios::trunc);
  outStream << "Beat-Tracking: " << beatTrackingMode << "; Induction-Mode: " << inductionMode_ << "; TriggerGtTol: "
            << triggerGTTol << "; Induction: " << ctrl_gtInductionMode_->to<mrs_string>() << "; Induction-Time: " << inductionTime_ << "<<"
            << (((inductionTime_ * hopSize_) - frames2SecsAdjustment_) / srcFs_) << ">>" << endl;
  cerr << "Log File in " << logFileUnits_ << ": " << logFileName_ << endl;
  outStream.close();
}

void
BeatReferee::debugAddEvent(mrs_string ibtEvent, mrs_natural agentIndex,
                           mrs_natural period, mrs_natural lastBeat, mrs_real score, mrs_real bestScore, mrs_natural callAgent)
{
  //event is appended in the end of the file
  fstream outStream;
  outStream.open(logFileName_.c_str(), ios::out|ios::app);

  if(strcmp(logFileUnits_.c_str(), "frames") == 0) //timesteps in frames
  {
    outStream << ibtEvent << "|" << timeElapsed_ << "|" << agentIndex << "|" << period << "|" << lastBeat
              << "|" << score << "|" << bestScore << "|" << callAgent << endl;
  }
  else if(strcmp(logFileUnits_.c_str(), "seconds") == 0) //timesteps in seconds
    outStream << ibtEvent << "|" << (((timeElapsed_ * hopSize_) - frames2SecsAdjustment_) / srcFs_) << "|" << agentIndex << "|"
              << period << "|" << (((lastBeat * hopSize_) - frames2SecsAdjustment_) / srcFs_) << "|" << score << "|"
              << bestScore << "|" << callAgent << endl;
  else if(strcmp(logFileUnits_.c_str(), "frames+seconds") == 0) //timesteps in frames + seconds
    outStream << ibtEvent << "|" << timeElapsed_ << "<<" << (((timeElapsed_ * hopSize_) - frames2SecsAdjustment_) / srcFs_) << ">>|"
              << agentIndex << "|" << period << "|" << lastBeat << "<<" << (((lastBeat * hopSize_) - frames2SecsAdjustment_) / srcFs_)
              << ">>|" << score << "|" << bestScore << "|" << callAgent << endl;

  //cout << ibtEvent << "|" << timeElapsed_ << "|" << agentIndex << "|" << period << "|" << lastBeat
  //	<< "|" << score << "|" << bestScore << "|" << callAgent << endl;

  outStream.close();
}

void
BeatReferee::debugAddMsg(mrs_string ibtMsg)
{
  fstream outStream;
  outStream.open(logFileName_.c_str(), ios::out|ios::app);

  if(strcmp(logFileUnits_.c_str(), "frames") == 0) //timesteps in frames
    outStream << ibtMsg << "|" << timeElapsed_ << endl;
  else if(strcmp(logFileUnits_.c_str(), "seconds") == 0) //timesteps in seconds
    outStream << ibtMsg << "|" << (((timeElapsed_ * hopSize_) - frames2SecsAdjustment_) / srcFs_) << endl;
  else if(strcmp(logFileUnits_.c_str(), "frames+seconds") == 0) //timesteps in frames + seconds
    outStream << ibtMsg << "|" << timeElapsed_ << "<<" << (((timeElapsed_ * hopSize_) - frames2SecsAdjustment_) / srcFs_) << ">>" << endl;

  outStream.close();

}

void //system intialization (at absolute t=0)
BeatReferee::initialization()
{
  agentsHistory_.create(nrAgents_, maxNrBeats_);
  agentsFamilyHist_.create(nrAgents_, maxNrBeats_);
  generationCount_.create(nrAgents_); //1index for each agent
  score_.create(nrAgents_); //1index for each agent
  lastPeriods_.create(nrAgents_); //1index for each agent
  lastPhases_.create(nrAgents_); //1index for each agent
  mutedAgents_.create(nrAgents_);//1index for each agent
  mutedAgentsTmp_.create(nrAgents_);//1index for each agent
  beatCounter_.create(nrAgents_);//1index for each agent
  initPeriod_.create(nrAgents_);//1index for each agent
  missedBeatsCount_.create(nrAgents_);//1index for each agent
  considerAgentTransitionBeat_.create(nrAgents_);//1index for each agent
  considerFatherTransitionBeat_.create(nrAgents_,10);//10 indexes (would be enough) for each agent

  //Agent control indexed matrix
  //Each line(observation) accounts for an agent
  //[New/Update_Flag|Period|Phase|Timming]
  agentControl_.create(nrAgents_, 4);
  updControl(ctrl_agentControl_, agentControl_);

  //intialize mutedAgents vectors
  for(int i = 0; i < nrAgents_; i++)
  {
    mutedAgents_(0, i) = 1.0; //initially all agents are muted
    mutedAgentsTmp_(0, i) = 1.0; //initially all agents are muted

    //by default consider every beat at transition bewteen father and son (for non-causal)
    for (int j = 0; j < considerFatherTransitionBeat_.getCols(); j++)
      considerFatherTransitionBeat_(i, j) = NA;

    considerAgentTransitionBeat_(i) = NA;
  }
  updControl(ctrl_mutedAgents_, mutedAgents_); //initially deactivate all agents

  inductionEnabler_ = ctrl_inductionEnabler_->to<mrs_realvec>();
  //in the new vesion of IBT onsets are no more considered in the induction process (so deactivate this!!)
  //(deactivated instead of removed for not changing the overall network structure)
  inductionEnabler_(1, 0) = 1.0; //diable = muted

  //initially deactivate induction's period estimation (ACF) -> computer expensive
  inductionEnabler_(0, 0) = 1.0; //diable = muted
  updControl(ctrl_inductionEnabler_, inductionEnabler_);

  if(!strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "-1") == 0 &&
      !strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
  {
    logFileName_ = ctrl_logFileName_->to<mrs_string>();
    logFileUnits_ = ctrl_logFile_->to<mrs_string>();
    logFile_ = true;
    debugCreateFile();
  }

  //if induction in "givetransitions" mode -> load groundtruth transition times
  if(strcmp(inductionMode_.c_str(), "givetransitions") == 0)
    loadTriggerTimes(triggerTimesFile_);
  else if(strcmp(inductionMode_.c_str(), "supervised") == 0)
  {
    supervisedBestScores_.resize(1); //minimum size
    supervisedBestScoresMeans_.resize(1); //minimum size
    lastTriggerInductionTime_ = 0;
    lastBestScoreMeanDiff_ = supervisedTriggerThres_;
  }

  triggerInductionTime_ = inductionTime_; //initially assume first induction time
}

void
BeatReferee::myProcess(realvec& in, realvec& out)
{
  //run initialization at the very begining of the analysis
  if(startSystem_)
  {
    initialization();
    startSystem_ = false;
  }

  //if(timeElapsed_ == 3100)
  //	cout << "BR: " << timeElapsed_ << "; beatTransTol: " << beatTransitionTol_ << endl;

  //cout << "BR: " << timeElapsed_ << "; triggerInd: " << triggerInduction_ << "; backtrace: " << backtrace_
  //	<< "; maxPer: " << maxPeriod_ << "; minPer: " << minPeriod_ << endl;

  //while no best beat detected => outputs 0 (no beat)
  out.setval(0.0);
  ctrl_beatDetected_->setValue(0.0);

  agentControl_ = ctrl_agentControl_->to<mrs_realvec>();
  //always updates every agents' timming to equalize referee's (+1 for considering next time frame)
  for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
  {
    agentControl_(i, 3) = timeElapsed_+1;
    updControl(ctrl_agentControl_, agentControl_);
    agentsJustCreated_(i) = 0.0; //reset at all frames
  }
  //also pass timer value to the other MarSystems (+1 for considering next time frame)
  ctrl_tickCount_->setValue(timeElapsed_+1);

  //cout << "BRef: " << timeElapsed_ << "; Ind: " << inductionTime_ << "; Sc: " << bestScore_ << "; BA: " << bestAgentIndex_ << endl;

  //realvec with the enable flag of all the BeatAgents in the pool
  //(0 -> agent active; 1 -> agent desactivated)
  mutedAgents_ = ctrl_mutedAgents_->to<mrs_realvec>();
  //created tmp agents vector for just updating mutedAgents_ vector in the next tick
  mutedAgentsTmp_ = ctrl_mutedAgents_->to<mrs_realvec>();

  /*
  if(timeElapsed_ >= inductionTime_-1)
  {
  	//Display Input from BeatAgents:
  	cout << "INPUT (" << timeElapsed_ << "): ";
  	for(mrs_natural a = 0; a < nrAgents_; a++)
  	//cout << "\n" << a << "-> " << in(a, 0) << " | " << in(a, 1) << " | " << in(a, 2) << " | " << in(a, 3);
  		cout << "\n" << a << "-> " << lastPeriods_(a) << " | " << lastPhases_(a) << " | " << score_(a) << " | " << bestScore_;
  	cout << endl;
  }
  */

  //Start beat-tracking after 1st induction:
  if(startTracking_)
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

      //statsPeriods_(o, timeElapsed_) = lastPeriods_(o);
      //statsPhases_(o, timeElapsed_) = lastPhases_(o);
      //statsAgentsScore_(o, timeElapsed_) = score_(o);
      //statsMuted_(o, timeElapsed_) = mutedAgents_(o);

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

        //statsPeriods_(o, timeElapsed_) = agentPeriod;
        //statsPhases_(o, timeElapsed_) = agentPrevBeat;

        //Update Agents' Score
        score_(o) += agentDScore;

        //if(o == bestAgentIndex_)
        //	cout << "Agent" << o << "-dScore:" << agentDScore << "; agentScore:" << score_(o)
        //	<< "(" << bestScore_ << ")" << endl;

        //If the bestAgent drops or increases its score the BestScore has to drop correspondingly
        //if((score_(bestAgentIndex_) < bestScore_) || (score_(bestAgentIndex_) > bestScore_))
        if(score_(bestAgentIndex_) != bestScore_)
        {
          //cout << "t:" << timeElapsed_ << "; Updating bestScore: " << "OLD: " << bestScore_
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
        else if(timeElapsed_ > timeBeforeKilling_ && score_(o) < bestScore_ && fabs(bestScore_-score_(o)) > 0.1
                && fabs(bestScore_ - score_(o)) > fabs(bestScore_ * obsoleteFactor_))
        {
          //cout << "Agent " << o << " Killed: Score below minimum (" << score_(o) << "\\" << bestScore_ << ")" << endl;
          killAgent(o, "OBS");
        }
        else //Checks if there are equal agents to current EVALuated one
          checkAndKillEqualAgents(o);

        //statsAgentsScore_(o, timeElapsed_) = score_(o);

        //Display Scores from BeatAgents:
        /*	cout << "SCORES(" << timeElapsed_ << ") (reqBy:" << o << "): " << endl;
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

          //if(o == 21)
          //	cout << "T: " << timeElapsed_ << " EVAL - Agent: " << o << "; ERROR: " << agentError << endl;

          //If a beat of an agent is inside its Inner tolerance but it has an error:
          //Update agent phase and period hypotheses:
          if(agentTolerance == INNER)
          {
            if(logFile_)
              debugAddEvent("UPDATE", o, agentPeriod, agentPrevBeat, score_(o), bestScore_, o);

            missedBeatsCount_(o) = 0.0; //reset (consecutive) missed beats counter for this agent

            if(abs(agentError) > 0)
            {
              updateAgentHypothesis(o, agentPeriod, agentPrevBeat, agentError);
            }
          }

          //If a beat of an agent is detected outside its Inner tolerance window:
          //Is created another agent that keeps this agent hypothesis, and updates its phase
          //and period to account for the given error:
          if(agentTolerance == OUTTER)
          {
            if(logFile_)
              debugAddEvent("UPD_OUTTER", o, agentPeriod, agentPrevBeat, score_(o), bestScore_, o);

            //CREATE NEW AGENT WITH THIS NEW HYPOTHESIS KEEPING THE SCORE OF o-delta:
            //(the agent that generates a new one keeps its original hypothesis);
            //New agent must look for a new beat on the next (updated) period
            createChildren(o, agentPeriod, agentPrevBeat, agentError, score_(o), beatCounter_(o));
            //statsAgentsLifeCycle_(o, timeElapsed_) = 2;

            missedBeatsCount_(o)++; //increment missed beats counter for this agent
          }
        }

        if(agentFlag == BEAT)
        {
          //in nonCausal analysis keep agents' beat history
          if(nonCausal_)
          {
            agentsHistory_(o, (mrs_natural) beatCounter_(o)) = timeElapsed_; //save beat-time
            agentsFamilyHist_(o, (mrs_natural) beatCounter_(o)) = o; //save agent whose beat-time is

            //if(o == 26 && timeElapsed_ >= 4546)
            //	cout << "=========t: " << timeElapsed_ << "; BC: " << beatCounter_(o) << "; BEAT: " << agentsHistory_(o, (mrs_natural) beatCounter_(o)) << endl;

            handleAgentsTansition(o);
          }

          //if(timeElapsed_ >= 4196 && (o == 15 || o == 24))
          //	cout << "t:" << timeElapsed_ << "-BEAT CHECK " << beatCounter_(o) << ": " << (((agentsHistory_(o, beatCounter_(o)) * hopSize_) - hopSize_/2) / srcFs_)
          //		<< " by: " << agentsFamilyHist_(o, beatCounter_(o)) << endl;

          //Increment beat counter of each agent
          beatCounter_(o)++;

          //update agent's last phase
          lastPhases_(o) = timeElapsed_; //(Phases in frames)

          //Display Beats from BeatAgents:
          //if(o == 8)
          //	cout << "t: " << timeElapsed_ << "; BEAT - Agent " << o << "(" << ((timeElapsed_ * hopSize_) - (hopSize_/2)) / srcFs_
          //		<< ") -> AgentPeriod: " << lastPeriods_(o) << "; AgentScore: " << score_(o) << " BestAgent: "
          //		<< bestAgentIndex_ << " BestScore: " << bestScore_ << " BeatCount: " << beatCounter_(o) << endl;

          //if best agent sends beat => outputs best beat
          if(o == bestAgentIndex_)
          {
            //beat transition safecheck-> to avoid beats given by different agents distanced less than a tolerance (60% of current ibi)
            //(ignore this requirement in the beat just after reseting the system, with trigger induction)
            //if((timeElapsed_ - lastBeatTime_) >= 0.6*lastBeatPeriod_)
            if((timeElapsed_ - lastBeatTime_) >= beatTransitionTol_*lastBeatPeriod_)
            {
              //Display Outputted Beat:
              //cout << "OUTPUT(" << timeElapsed_ << "-" << ((timeElapsed_ * hopSize_) - (hopSize_/2)) / srcFs_ << "):Beat from Agent " << bestAgentIndex_ <<
              //	" BestScore:" << bestScore_ << " (" << score_(bestAgentIndex_) << ")";
              //cout << ":" << (60.0 / (timeElapsed_ - lastBeatTime_)) * (ctrl_srcFs_->to<mrs_real>() / ctrl_hopSize_->to<mrs_natural>()) << "BPM" << endl;
              //cout << "BEST_AgentPeriod: " << lastPeriods_(bestAgentIndex_) << "(" << (timeElapsed_ - lastBeatTime_) << ")" << endl;

              //SEND BEAT COMMAND!
              ctrl_beatDetected_->setValue(1.0);
              out.setval(1.0);

              //Updates agent history, which accounts for the total number
              //of the detected best (considered) beats of each agent:
              historyCount_(o)++;
              //historyBeatTimes_(o, outputCount_) = timeElapsed_;
              outputCount_ ++;
              lastBeatTime_ = timeElapsed_;
              lastBeatPeriod_ = (mrs_natural) lastPeriods_(o);

              if(logFile_)
                debugAddEvent("==========> BEAT", o, (mrs_natural) lastPeriods_(o),
                              (mrs_natural) lastPhases_(o), score_(o), bestScore_);

              //if trigger mode defined as ground_truth
              if(strcmp(inductionMode_.c_str(), "groundtruth") == 0)
              {
                //check beat-time distance to ground-truth (check if any false positive or negative)
                mrs_natural localBeatError = checkBeatInGTFile();
                localBeatError == 0 ? lostGTBeatsCount_ = 0 : lostGTBeatsCount_+=localBeatError;
                //cout << "lostBeatsCount: " << lostGTBeatsCount_ << endl;

                //while beat error count = 0 -> assign bestAgentBeforeTrigger as current best agent
                if(lostGTBeatsCount_ == 0)
                {
                  bestAgentBeforeTrigger_ = bestAgentIndex_;

                  if(logFile_)
                    debugAddEvent("BEST_TRIGGER", bestAgentBeforeTrigger_, (mrs_natural) lastPeriods_(bestAgentBeforeTrigger_),
                                  (mrs_natural) lastPhases_(bestAgentBeforeTrigger_), score_(bestAgentBeforeTrigger_), bestScore_);

                  //cout << timeElapsed_ << "=======: bestAgentBeforeTrigger: " << bestAgentBeforeTrigger_
                  //	<< "; bestAgent: " << bestAgentBeforeTrigger_ << endl;
                }

                //request new induction if detected more consecutive beat misses than the tolerated
                if(lostGTBeatsCount_ >= triggerGtTolerance_)
                {
                  //cout << "LOST req TRIGGER at: " << timeElapsed_ << " (" << ((timeElapsed_ * hopSize_) - (hopSize_/2)) / srcFs_ << ")" << endl;
                  lostGTBeatsCount_ = 0; //reset beat error count
                  triggerInductionExternalRequest_ = true;

                  //cout << "2: " << ctrl_logFile_->to<mrs_string>().c_str() << endl;

                  //trigger logfile:
                  if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
                  {
                    triggerCount_++; //counts the number of triggers for the trigger logfile
                    ostringstream oss;
                    oss << ctrl_destFileName_->to<mrs_string>() << "_logTrigger.txt";
                    ofstream trigCountStream;
                    trigCountStream.open(oss.str().c_str(), ios::out|ios::trunc);
                    trigCountStream << triggerCount_ << endl;
                    trigCountStream.close();
                  }
                }
              }
              else if(strcmp(inductionMode_.c_str(), "repeated") == 0 ||
                      (strcmp(inductionMode_.c_str(), "random") == 0) ||
                      (strcmp(inductionMode_.c_str(), "supervised") == 0))
              {
                //in trigger modes other than "groundtruth" assign the bestAgentBeforeTrigger as the bestAgent at every estimated beat
                bestAgentBeforeTrigger_ = bestAgentIndex_;

                if(logFile_)
                  debugAddEvent("BEST_TRIGGER", bestAgentBeforeTrigger_, (mrs_natural) lastPeriods_(bestAgentBeforeTrigger_),
                                (mrs_natural) lastPhases_(bestAgentBeforeTrigger_), score_(bestAgentBeforeTrigger_), bestScore_);
              }
              //in triggertimes trigger mode consider the bestAgentBeforeTrigger as the best agent at the last beat-time before the effective transitions
              else if(strcmp(inductionMode_.c_str(), "givetransitions") == 0 &&
                      (timeElapsed_ + (mrs_natural)lastPeriods_(bestAgentIndex_)) >= transitionTimes_(triggerCount_) && transitionsConsidered_(triggerCount_) == 0.0)
              {
                bestAgentBeforeTrigger_ = bestAgentIndex_;
                resetSystem(bestAgentBeforeTrigger_); //kill everyone except the bestAgent before trigger
                transitionsConsidered_(triggerCount_) = 1;

                if(logFile_)
                  debugAddEvent("BEST_TRIGGER", bestAgentBeforeTrigger_, (mrs_natural) lastPeriods_(bestAgentBeforeTrigger_),
                                (mrs_natural) lastPhases_(bestAgentBeforeTrigger_), score_(bestAgentBeforeTrigger_), bestScore_);
              }
            }
            else if(logFile_)
              debugAddEvent("BEAT CANCEL", o, (mrs_natural) lastPeriods_(o),
                            (mrs_natural) lastPhases_(o), score_(o), bestScore_);

            //redo agents' scores calculation
            calcAbsoluteBestScore();
          }
        }
      }
    }
  }

  //Create the first BeatAgents with new hypotheses just after Tseconds of induction:
  if(processInduction_)
  {
    if(logFile_)
      debugAddMsg("=========== INDUCTION ===========");

    firstHypotheses_ = ctrl_firstHypotheses_->to<mrs_realvec>();

    //update values for eventual forced periods in induction stage
    maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
    minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();
    //max possible nr. of beats in the analysed sound file (*1.7 - tolerance due to possible limit surpassing)
    maxNrBeats_ = (mrs_natural) (ceil(((mrs_real) soundFileSize_) / ((mrs_real) minPeriod_)) * 1.7);
    agentsHistory_.stretch(nrAgents_, maxNrBeats_);
    agentsFamilyHist_.stretch(nrAgents_, maxNrBeats_);

    mrs_natural newAgentPeriod;
    mrs_natural newAgentPhase;
    mrs_real newAgentScore;

    if(strcmp(inductionMode_.c_str(), "givetransitions") == 0)
      bestAgentBeforeTrigger_ = bestAgentIndex_;
    if(startTracking_ && resetAfterNewInduction_)
    {
      resetSystem(bestAgentBeforeTrigger_); //kill everyone except the bestAgent at transition
      lastBeatPeriod_ = 0; //reset lastBeatPeriod to avoid BEAT_CANCEL int the first beat after a system reset
    }

    //cout << "TRIGGER AGENTS @ " << timeElapsed_ << endl;

    //before getting IBI clusters we need to make sure which agents will be killed
    //for don't consider them as possible fathers to adopt trigger agents
    mrs_realvec newAgentsScore(firstHypotheses_.getRows());
    mrs_natural count = 0;
    for(int i = 0; i < firstHypotheses_.getRows(); i++)
    {
      if((mrs_natural) firstHypotheses_(i,0) > 0) //only consider valid hypothesis:
      {
        //firstHypotheses_ -> matrix with i generated beat hypotheses + score, in the induction stage
        //[ BPMi | Beati | Score i ]
        newAgentPeriod = (mrs_natural) firstHypotheses_(i,0);
        newAgentPhase = (mrs_natural) firstHypotheses_(i,1);
        newAgentScore = firstHypotheses_(i,2);

        if(!resetAfterNewInduction_) //only consider below in non RESET trigger mode
        {
          //only considered agents which aren't replicated
          if(existEqualBetterAgents(-1, newAgentPeriod, newAgentPhase, newAgentScore) == -1)
          {
            newAgentsScore(count) = newAgentScore;

            //if(timeElapsed_ == 3100)
            //	cout << "NEW1 " << i << "-> Agents Score: " << newAgentsScore(count) << "; period: "
            //	<< newAgentPeriod << "; elems: " << count << endl;

            count++;
          }
          else //if existed equal agents erase hypotheses (so this agent is not considered for creation)
          {
            firstHypotheses_(i,0) = 0.0;
            firstHypotheses_(i,1) = 0.0;
            firstHypotheses_(i,2) = 0.0;

            if(logFile_)
              debugAddEvent("EQ_TRIGGER", -1, newAgentPeriod, newAgentPhase, newAgentScore,
                            bestScore_, existEqualAgents(-1, newAgentPeriod, newAgentPhase));
          }
        }
      }


      grantPoolSpace2(-1, count, newAgentsScore); //grant pool space for all needed agents to be created
    }

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

        //cout << "t: " << timeElapsed_ << "; nAP: " << newAgentPeriod << "; nAPh: " << newAgentPhase << "; Score" << i << ": " << newAgentScore << endl;

        //create new agent
        mrs_natural createdAgent = -1;
        if(startTracking_)
        {
          //after first induction, at each trigger all agents derive from last best good agent
          createdAgent = createNewAgent(newAgentPeriod, newAgentPhase, newAgentScore, beatCounter_(bestAgentBeforeTrigger_), bestAgentBeforeTrigger_);
        }
        else //at first induction all agents are created as first generation
          createdAgent = createNewAgent(newAgentPeriod, newAgentPhase, newAgentScore, 0, -1);

        //cout << "t: " << timeElapsed_ << "; BESTAGENT: " << bestAgentIndex_ << endl;

        //only if agent is actually created it is considered for best check
        if(createdAgent >= 0)
        {
          if(newAgentScore > bestScore_)
          {
            bestScore_ = newAgentScore;
            bestAgentIndex_ = createdAgent;
            bestFinalAgent_ = bestAgentIndex_;

            //at the initial induction the best agent before trigger is assigned as the best initial agent
            if(!startTracking_)
            {
              bestAgentBeforeTrigger_ = bestAgentIndex_;

              if(logFile_)
                debugAddEvent("BEST_TRIGGER", bestAgentBeforeTrigger_, (mrs_natural) lastPeriods_(bestAgentBeforeTrigger_),
                              (mrs_natural) lastPhases_(bestAgentBeforeTrigger_), score_(bestAgentBeforeTrigger_), bestScore_);
            }

            if(logFile_)
              debugAddEvent("BEST", bestAgentIndex_, newAgentPeriod, newAgentPhase, newAgentScore, bestScore_);
          }
        }
        //else cout << timeElapsed_ << ": AGENT " << createdAgent << " KILLED BY EQUAL CHECK!" << endl;

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

      //reset supervisedBestScores buffers at every induction
      if(strcmp(inductionMode_.c_str(), "supervised") == 0)
      {
        supervisedBestScores_.resize(1); //minimum size
        supervisedBestScoresMeans_.resize(1); //minimum size
        lastTriggerInductionTime_ = timeElapsed_; //keep last induction time
        lastBestScoreMeanDiff_ = supervisedTriggerThres_;
      }

    }

    //if backtrace restart tick counter
    //(timing updated here, for backtracing, for being contablized in the log file)
    if(backtrace_)
    {
      backtraceEndTime_ = timeElapsed_; //save end time as time when backtrace requested

      //retrocede all MarSystem's timer by inductionTime:
      for(mrs_natural i = 0; i < agentControl_.getRows(); i++)
      {
        agentControl_(i, 3) = timeElapsed_-inductionTime_;
        updControl(ctrl_agentControl_, agentControl_);
      }
      ctrl_tickCount_->setValue(timeElapsed_-inductionTime_);

      timeElapsed_ = timeElapsed_-inductionTime_-1; //-1 because timer is updated by the end of the cycle (so next cycle is the actual 0)
      //cout << "Back: " << timeElapsed_ << "; newT: " << timeElapsed_ << "with ind: " << inductionTime_ << endl;

      //disable backtrace after first induction (for non-causal mode)
      updControl(ctrl_backtrace_, false);
    }
    //incremented for counting inductionTime_ after induction before enabling obsolete killing
    else
      timeBeforeKilling_+=inductionTime_;

    //After finnishing induction deactivate induction's period estimation (ACF) -> computer expensive
    inductionEnabler_(0, 0) = 1.0; //diable = muted
    updControl(ctrl_inductionEnabler_, inductionEnabler_);

    if(startTracking_ && resetAfterNewInduction_)
    {
      killAgent(bestAgentBeforeTrigger_, "RESET", -1); //kill bestAgent at transition
      calcAbsoluteBestScore();
    }

    processInduction_ = false;
    startTracking_ = true; //start tracking (after first induction, tracking keeps on running)
    updControl(ctrl_triggerInduction_, false); //deactivate trigger induction!!

  }

  //MATLAB_PUT(agentsHistory_, "agentsHistory");
  //MATLAB_PUT(bestFinalAgent_, "bestFinalAgent");

  //clean first half of the feature window buffer before every new induction
  if(timeElapsed_ > backtraceEndTime_ && ((ctrl_triggerInductionExternalRequest_->to<mrs_bool>() || timeElapsed_ == triggerInductionTime_-2)
                                          || (strcmp(inductionMode_.c_str(), "givetransitions") == 0 && timeElapsed_ == triggerTimes_(triggerCount_)-2)))
  {
    updControl(ctrl_resetFeatWindow_, true);

    if(ctrl_triggerInductionExternalRequest_->to<mrs_bool>())
    {
      triggerInductionExternalRequest_ = false;
      triggerInduction_ = true;
    }

  }
  //just before the induction triggering activate periodicity estimation (via ACF peaks)
  //(don't call this wihtin backtrace window)
  else if(timeElapsed_ > backtraceEndTime_ && ((triggerInduction_ || timeElapsed_ == triggerInductionTime_-1)
          || (strcmp(inductionMode_.c_str(), "givetransitions") == 0 && timeElapsed_ == triggerTimes_(triggerCount_)-1)))
  {
    mrs_real outTmp = out(0,0); //[!!] don't know why when calling updControl() out is reset

    //cout << "Trigger Mode: " << inductionMode_ << endl;

    //activate periodicity estimation (via ACF peaks)
    inductionEnabler_(0, 0) = 0.0; //enable = unmuted
    updControl(ctrl_inductionEnabler_, inductionEnabler_);
    //cout << "Induction triggered at: " << timeElapsed_ << endl;

    //cout << "triggerInd: " << triggerInduction_ << "@" << timeElapsed_
    //	<< "; bestAgent: " << bestAgentBeforeTrigger_ << endl;

    //send current and inductionTime before best agents score to new induction stage
    if(startTracking_)
    {
      if(resetAfterNewInduction_) bestScore_ = NA;
      else //update current best score before trigger
      {
        calcAbsoluteBestScore();
      }
    }

    updControl(ctrl_curBestScore_, bestScore_);

    updControl(ctrl_triggerInduction_, true); //request induction!!

    processInduction_ = true; //update system with new induction values
    triggerInduction_ = false;

    //recover value above
    out.setval(outTmp);

    if(startTracking_)
      triggerCount_++; //counts the number of triggers

    if(strcmp(inductionMode_.c_str(), "repeated") == 0 ||
        strcmp(inductionMode_.c_str(), "random") == 0 ||
        strcmp(inductionMode_.c_str(), "supervised") == 0)
    {
      if(strcmp(inductionMode_.c_str(), "repeated") == 0) //repeat induction mode (repeated every induction time)
        triggerInductionTime_ += 100; //for testing repeated induction x in x secs
      else if(strcmp(inductionMode_.c_str(), "random") == 0)
      {
        mrs_natural minTriggerRep = maxPeriod_;
        mrs_natural maxTriggerRep = inductionTime_ * 3;
        srand((mrs_natural)(timeElapsed_+time(NULL))); //just a random seed

        mrs_natural randTriggerRep = (mrs_natural) ((rand() % (maxTriggerRep-minTriggerRep)) + minTriggerRep);

        //cout << "lastIndTime:" << triggerInductionTime_;

        triggerInductionTime_ += randTriggerRep;
      }

      //trigger logfile:
      if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
      {
        ostringstream oss;
        oss << ctrl_destFileName_->to<mrs_string>() << "_logTrigger.txt";
        ofstream trigCountStream;
        trigCountStream.open(oss.str().c_str(), ios::out|ios::trunc);
        trigCountStream << triggerCount_ << endl;
        trigCountStream.close();
      }

    }
    else triggerInductionTime_ = -1; //avoid another induction by time when backtracing
  }

  //if in nonCausalMode save last best agent history as the final output
  if(nonCausal_)
  {
    soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();

    //cout << "T: " << timeElapsed_ << "; soundFileSize: " << soundFileSize_ << endl;

    //by the end of the audio analysis:
    if(timeElapsed_ == soundFileSize_-1) //[! -1 for acouting on time of timing reset on backtrace mode]
    {
      mrs_natural bestAgentHistorySize = (mrs_natural) beatCounter_(bestFinalAgent_);
      bestFinalAgentHistory_.create(bestAgentHistorySize);

      if(logFile_)
        debugAddMsg("=========== NON-CAUSAL BEAT-TIMES ===========");

      for(int i = 0; i < bestAgentHistorySize; i++)
      {
        //cout << "I: " << i << "; History: " << agentsHistory_(bestFinalAgent_, i) << endl;
        bestFinalAgentHistory_(i) = agentsHistory_(bestFinalAgent_, i); //Output Vector

        if(logFile_)
        {
          //prints non-causal beat-times
          mrs_natural agent = (mrs_natural) agentsFamilyHist_(bestFinalAgent_, i);
          debugAddEvent("BEAT", agent, -1, (mrs_natural) bestFinalAgentHistory_(i), -1, -1);
        }
      }

      if(logFile_)
        debugAddMsg("==========================");

      /*
      cout << "BEST FAMILY: " << endl;
      for(int i = 0; i < bestAgentHistorySize; i++)
      {
      	cout << agentsFamilyHist_(bestFinalAgent_, i) << "; ";
      }
      */

      if(bestAgentHistorySize == 0) //if no beats detected [to avoid crash]
      {
        bestFinalAgentHistory_.create(1);
        bestFinalAgentHistory_(0) = -1.0;
      }

      updControl(ctrl_bestFinalAgentHistory_, bestFinalAgentHistory_);
    }
  }

  //in supervised induction mode:
  //1- keep a buffer with all bestScore values since previous induction
  //2- do a mean of all bestScores in a window with the size of the induction at 1sec increments
  //3- subtract every mean of bestScores with the previous:
  //   if difference < supervisedTriggerThres -> request new induction
  if(strcmp(inductionMode_.c_str(), "supervised") == 0)
  {
    mrs_natural smoothResolution = 3; //3secs resolution
    smoothResolution = (mrs_natural) (smoothResolution * (srcFs_/hopSize_));
    //supervision logfile:
    ofstream diffBS, meanBS, rawBS;
    if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
    {
      ostringstream ossDiff, ossMean, ossBS;
      ossDiff << ctrl_destFileName_->to<mrs_string>() << "_diffBestScore.txt";
      ossMean << ctrl_destFileName_->to<mrs_string>() << "_meanBestScore.txt";
      ossBS << ctrl_destFileName_->to<mrs_string>() << "_bestScore.txt";

      if(timeElapsed_ == 0)
      {
        diffBS.open(ossDiff.str().c_str(), ios::out|ios::trunc);
        meanBS.open(ossMean.str().c_str(), ios::out|ios::trunc);
        rawBS.open(ossBS.str().c_str(), ios::out|ios::trunc);
      }
      else
      {
        diffBS.open(ossDiff.str().c_str(), ios::out|ios::app);
        meanBS.open(ossMean.str().c_str(), ios::out|ios::app);
        rawBS.open(ossBS.str().c_str(), ios::out|ios::app);
      }

      rawBS << bestScore_ << endl;
      rawBS.close();
    }

    if(bestScore_ == NA)
      supervisedBestScores_.push_back(0.0);
    else supervisedBestScores_.push_back(bestScore_);

    //cout << "time: " << timeElapsed_ << "; BS: " << supervisedBestScores_.at(supervisedBestScores_.size()-1)
    //	 << "; Size: " << supervisedBestScores_.size() << endl;

    //wait the first induction time to fill the supervisedBestScores buffer
    if(timeElapsed_ >= lastTriggerInductionTime_ + inductionTime_)
    {
      mrs_natural stepTime = timeElapsed_ - (lastTriggerInductionTime_ + inductionTime_);
      mrs_natural stepSize = 1; // 1 second
      stepSize = (mrs_natural) (stepSize * (srcFs_/hopSize_));
      if(!(stepTime % stepSize))
      {
        mrs_real sum = 0.0;
        for(mrs_natural s = (mrs_natural) (supervisedBestScores_.size()-smoothResolution); s < (mrs_natural) supervisedBestScores_.size(); s++)
          sum += supervisedBestScores_.at(s);
        mrs_real mean = sum /= smoothResolution;
        supervisedBestScoresMeans_.push_back(mean);

        mrs_real bestScoreMeanDiff = 0.1;
        if(supervisedBestScoresMeans_.size() > 1)


          bestScoreMeanDiff = supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-1)-supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-2);

        //cout << "AKI @ " << timeElapsed_ << "; stepTime: " << stepTime << "; min: "
        //<< (supervisedBestScores_.size()-inductionTime_) << "; max: " << supervisedBestScores_.size() //<< "; BSMeanDiff: " << bestScoreMeanDiff
        //<< "; thisMean: " << supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-1)
        //<< "; lastMean: " << supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-2) << endl;
        //<< "; thisDiff: " << bestScoreMeanDiff << "; lastDiff: " << lastBestScoreMeanDiff_ << endl;

        if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
        {
          diffBS << bestScoreMeanDiff << endl;
          diffBS.close();
          meanBS << supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-1) << endl;
          meanBS.close();
        }

        //trigger induction time after signalization
        if((bestScoreMeanDiff < (supervisedTriggerThres_*supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-1)))
            && (lastBestScoreMeanDiff_ > (supervisedTriggerThres_*supervisedBestScoresMeans_.at(supervisedBestScoresMeans_.size()-2))))
        {
          //save lastTriggerInductionTime for plotting
          //mrs_natural lastTriggerInductionTime = triggerInductionTime_;

          triggerInductionTime_ = timeElapsed_ + 3; //trigger new induction now (sum needed 3 frames)

          //supervision logfile:
          if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0)
          {
            ostringstream oss;
            oss << ctrl_destFileName_->to<mrs_string>() << "_triggerTimes.txt";
            ofstream triggerBS;
            if(lastTriggerInductionTime_ == inductionTime_)
              triggerBS.open(oss.str().c_str(), ios::out|ios::trunc);
            else triggerBS.open(oss.str().c_str(), ios::out|ios::app);

            triggerBS << triggerInductionTime_ << endl;
            triggerBS.close();
          }

          //cout << "TRIGGER @ " << timeElapsed_ << "; TT: " << triggerInductionTime_ << endl;
          //cout << "THRES: " << supervisedTriggerThres_ << endl;
        }

        lastBestScoreMeanDiff_ = bestScoreMeanDiff;
      }
      else if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0) //DEBUGGING
      {
        diffBS << NA << endl;
        diffBS.close();
        meanBS << NA << endl;
        meanBS.close();
      }
    }
    else if(strcmp(ctrl_logFile_->to<mrs_string>().c_str(), "trigger") == 0) //DEBUGGING
    {
      diffBS << NA << endl;
      diffBS.close();
      meanBS << NA << endl;
      meanBS.close();
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

  timeElapsed_++; //increment timer by the end of each cycle

}
