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

#include "PhaseLock.h"
#include "../common_source.h"

#include <marsyas/FileName.h>
//#include "../common_source.h"

#define MINIMUMREAL 0.000001 //(0.000001 minimum float recognized)
#define NA -10000.0 //undefined value flag

using namespace std;
using namespace Marsyas;

PhaseLock::PhaseLock(mrs_string name):MarSystem("PhaseLock", name)
{
  addControls();
  timeElapsed_ = 0;
  lastGTBeatTime_ = -1;
  inductionFinished_ = false;
  gtAfter2ndBeat_ = false;
  triggerInduction_ = false;
  lastGTIBI_ = 0.0;
}

PhaseLock::PhaseLock(const PhaseLock& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_beatHypotheses_ = getctrl("mrs_realvec/beatHypotheses");
  ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
  ctrl_nrPeriodHyps_ = getctrl("mrs_natural/nrPeriodHyps");
  ctrl_nrPhasesPerPeriod_ = getctrl("mrs_natural/nrPhasesPerPeriod");
  ctrl_scoreFunc_ = getctrl("mrs_string/scoreFunc");
  ctrl_gtBeatsFile_ = getctrl("mrs_string/gtBeatsFile");
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_backtrace_ = getctrl("mrs_bool/backtrace");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
  ctrl_innerMargin_ = getctrl("mrs_real/innerMargin");
  ctrl_lftOutterMargin_ = getctrl("mrs_real/lftOutterMargin");
  ctrl_rgtOutterMargin_ = getctrl("mrs_real/rgtOutterMargin");
  ctrl_corFactor_ = getctrl("mrs_real/corFactor");
  ctrl_maxPeriod_ = getctrl("mrs_natural/maxPeriod");
  ctrl_minPeriod_ = getctrl("mrs_natural/minPeriod");
  ctrl_adjustment_ = getctrl("mrs_natural/adjustment");
  ctrl_dumbInduction_ = getctrl("mrs_bool/dumbInduction");
  ctrl_inductionOut_ = getctrl("mrs_string/inductionOut");
  ctrl_triggerInduction_ = getctrl("mrs_bool/triggerInduction");
  ctrl_curBestScore_ = getctrl("mrs_real/curBestScore");
  ctrl_triggerBestScoreFactor_ = getctrl("mrs_real/triggerBestScoreFactor");

  inductionFinished_ = a.inductionFinished_;
  gtInitPhase_ = a.gtInitPhase_;
  gtScore_ = a.gtScore_;
  gtInitPeriod_ = a.gtInitPeriod_;
  gtLastPeriod_ = a.gtLastPeriod_;
  srcFs_ = a.srcFs_;
  hopSize_ = a.hopSize_;
  adjustment_ = a.adjustment_;
  gtAfter2ndBeat_ = a.gtAfter2ndBeat_;
  triggerInduction_ = a.triggerInduction_;
  mode_ = a.mode_;
  curBestScore_ = a.curBestScore_;
  lastGTBeatTime_ = a.lastGTBeatTime_;
  triggerBestScoreFactor_ = a.triggerBestScoreFactor_;
  lastGTIBI_ = a.lastGTIBI_;
}

PhaseLock::~PhaseLock()
{
}

MarSystem*
PhaseLock::clone() const
{
  return new PhaseLock(*this);
}

void
PhaseLock::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_realvec/beatHypotheses", realvec(), ctrl_beatHypotheses_);
  addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
  setctrlState("mrs_natural/inductionTime", true);
  addctrl("mrs_natural/nrPeriodHyps", 6, ctrl_nrPeriodHyps_);
  setctrlState("mrs_natural/nrPeriodHyps", true);
  addctrl("mrs_natural/nrPhasesPerPeriod", 20, ctrl_nrPhasesPerPeriod_);
  setctrlState("mrs_natural/nrPhasesPerPeriod", true);
  addctrl("mrs_string/scoreFunc", "regular", ctrl_scoreFunc_);
  setctrlState("mrs_string/scoreFunc", true);
  addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_string/gtBeatsFile", "input.txt", ctrl_gtBeatsFile_);
  addctrl("mrs_string/mode", "regular", ctrl_mode_);
  setctrlState("mrs_string/mode", true);
  addctrl("mrs_bool/backtrace", false, ctrl_backtrace_);
  setctrlState("mrs_bool/backtrace", true);
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_real/innerMargin", 3.0, ctrl_innerMargin_);
  setctrlState("mrs_real/innerMargin", true);
  addctrl("mrs_real/lftOutterMargin", 0.2, ctrl_lftOutterMargin_);
  setctrlState("mrs_real/lftOutterMargin", true);
  addctrl("mrs_real/rgtOutterMargin", 0.4, ctrl_rgtOutterMargin_);
  setctrlState("mrs_real/rgtOutterMargin", true);
  addctrl("mrs_real/corFactor", 0.5, ctrl_corFactor_);
  setctrlState("mrs_real/corFactor", true);
  addctrl("mrs_natural/maxPeriod", -1, ctrl_maxPeriod_);
  setctrlState("mrs_natural/maxPeriod", true);
  addctrl("mrs_natural/minPeriod", -1, ctrl_minPeriod_);
  setctrlState("mrs_natural/minPeriod", true);
  addctrl("mrs_natural/adjustment", 0, ctrl_adjustment_);
  setctrlState("mrs_natural/adjustment", true);
  addctrl("mrs_bool/dumbInduction", false, ctrl_dumbInduction_);
  addctrl("mrs_string/inductionOut", "-1", ctrl_inductionOut_);
  setctrlState("mrs_string/inductionOut", true);
  addctrl("mrs_bool/triggerInduction", false, ctrl_triggerInduction_);
  setctrlState("mrs_bool/triggerInduction", true);
  addctrl("mrs_real/curBestScore", NA, ctrl_curBestScore_);
  setctrlState("mrs_real/curBestScore", true);
  addctrl("mrs_real/triggerBestScoreFactor", 0.95, ctrl_triggerBestScoreFactor_);
}

void
PhaseLock::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("PhaseLock.cpp - PhaseLock:myUpdate");

  inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
  nrPeriodHyps_ = ctrl_nrPeriodHyps_->to<mrs_natural>();
  nrPhasesPerPeriod_ = ctrl_nrPhasesPerPeriod_->to<mrs_natural>();
  scoreFunc_ = ctrl_scoreFunc_->to<mrs_string>();
  mode_ = ctrl_mode_->to<mrs_string>();
  backtrace_ = ctrl_backtrace_->to<mrs_bool>();
  innerMargin_ = (mrs_natural) ctrl_innerMargin_->to<mrs_real>();
  lftOutterMargin_ = ctrl_lftOutterMargin_->to<mrs_real>();
  rgtOutterMargin_ = ctrl_rgtOutterMargin_->to<mrs_real>();
  corFactor_ = ctrl_corFactor_->to<mrs_real>();
  maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
  minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();
  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();
  adjustment_ = (mrs_real) ctrl_adjustment_->to<mrs_natural>();
  inductionOut_ = ctrl_inductionOut_->to<mrs_string>();
  triggerInduction_ = ctrl_triggerInduction_->to<mrs_bool>();
  curBestScore_ = ctrl_curBestScore_->to<mrs_real>();
  triggerBestScoreFactor_ = ctrl_triggerBestScoreFactor_->to<mrs_real>();

  ctrl_onSamples_->setValue(3, NOUPDATE);
  ctrl_onObservations_->setValue(nrPeriodHyps_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  nInitHyp_ = nrPeriodHyps_ * nrPhasesPerPeriod_; //Nr. of initial hypothesis

  hypSignals_.create(nInitHyp_, inSamples_); //N hypothesis signals of induction timming size.
  firstBeatPoint_.create(nInitHyp_); //for accumulating reversed phases (one for each hypothesis pair)
  trackingScore_.create(nInitHyp_);
  beatCount_.create(nInitHyp_);
  maxLocalTrackingScore_.create(nrPeriodHyps_);
  maxLocalTrackingScoreInd_.create(nrPeriodHyps_);
  initPhases_.create(nrPeriodHyps_);
  lastPhases_.create(nrPeriodHyps_);
  initPeriods_.create(nrPeriodHyps_);
  lastPeriods_.create(nrPeriodHyps_);
  metricalSalience_.create(nrPeriodHyps_);
  rawScore_.create(nrPeriodHyps_);
  rawScoreNorm_.create(nrPeriodHyps_);
  metricalRelScore_.create(nrPeriodHyps_);
  scoreNorm_.create(nrPeriodHyps_);

  // reset maxLoclTrackingScores and indexes to allow negative scores
  for (int i = 0; i < nrPeriodHyps_; i++)
  {
    maxLocalTrackingScore_(i) = NA;
    maxLocalTrackingScoreInd_(i) = -1;
  }
}

mrs_realvec
PhaseLock::GTInitialization(realvec& in, realvec& out, mrs_natural gtInitPhase, mrs_natural gtInitPeriod)
{
  (void) out;
  //MATLAB_PUT(in, "Flux_FlowThrued");

  //cout << "InitPhase: " << gtInitPhase << "(" << (((gtInitPhase * hopSize_)-hopSize_/2) / srcFs_) << "); initPeriod: " << gtInitPeriod << endl;

  mrs_realvec initHypothesis(5); //backtracedPhase, initPeriod, lastPhase, lastPeriod, initScore
  for(int i = 0; i < initHypothesis.getCols(); i++)
    initHypothesis(i) = 0; //clean array

  //Calculate score inside induction window (simulate tracking behaviour) given the ground-truth (two beats)
  //adjust initPhase around innerMargin
  mrs_real localPeakAmp = in(gtInitPhase);
  mrs_natural localPeak = gtInitPhase;
  //cout << "gtInitPhase: " << gtInitPhase << "; value: " << localPeakAmp << endl;
  for (int t = gtInitPhase-innerMargin_; t <= gtInitPhase+innerMargin_; t++)
  {
    //Limit analysis to induction window
    if(t >= (inSamples_-1-inductionTime_))
    {
      if(in(t) > localPeakAmp)
      {
        localPeakAmp = in(t);
        localPeak = t;
      }
    }
  }
  //cout << "2- adjInitPhase: " << localPeak << "; adjValue: " << localPeakAmp << " period: " << gtInitPeriod << endl;
  initHypothesis(0) = localPeak; //keep first beat point (adjusted to maximum wihtin innerMargin)
  initHypothesis(1) = gtInitPeriod; //keep initPeriod
  initHypothesis(4) += (localPeakAmp * (gtInitPeriod/(mrs_real)maxPeriod_)); //assume perfect initial beat prediction

  outterWinLft_ = (mrs_natural) ceil(gtInitPeriod * lftOutterMargin_); //(% of IBI)
  outterWinRgt_ = (mrs_natural) ceil(gtInitPeriod * rgtOutterMargin_); //(% of IBI)
  mrs_natural beatPoint = gtInitPhase;
  mrs_real scoreFraction = 0.0;
  mrs_natural error = 0;
  mrs_real errorIn = MINIMUMREAL; //to avoid divide by zero
  mrs_natural period = gtInitPeriod;
  mrs_natural periodTmp = 0;
  //mrs_natural it=1; //used for cout debugging
  do
  {
    //cout << "3- beatPoint: " << beatPoint << " period: " << period << endl;

    periodTmp = period + ((mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)));
    //if the updated period don't respect the limits then keeps the former value
    if(periodTmp >= minPeriod_ && periodTmp <= maxPeriod_)
      period = periodTmp;

    //(for safechecking)
    if((beatPoint + (period + (mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)))) >= inSamples_)
    {
      initHypothesis(2) = beatPoint; //Keep current lastPhase
      initHypothesis(3) = period; //Keep current lastPeriod
      break;
    }

    beatPoint += (period + (mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)));
    errorIn = MINIMUMREAL; //to avoid divide by zero

    initHypothesis(2) = beatPoint; //Keep current lastPhase
    initHypothesis(3) = period; //Keep current lastPeriod
    //Check maximum around outter window
    localPeakAmp = in(beatPoint);
    localPeak = beatPoint;
    //cout << "predicition: " << localPeak << "; predValue: " << localPeakAmp << "; period: " << period << endl;
    for (int t = beatPoint-outterWinLft_; t <= beatPoint+outterWinRgt_; t++)
    {
      if(t > (inSamples_-1-inductionTime_) && t < inSamples_)
      {
        //cout << "t: " << t << "; in(t): " << in(t) << endl;
        if(in(t) > localPeakAmp)
        {
          localPeakAmp = in(t);
          localPeak = t;

          //cout << "Max: " << localPeak << "; MaxV: " << localPeakAmp << endl;
        }
      }
    }

    //Check if maximum is inside inner window
    //calc positive regular score and adjust hypothesis
    error = localPeak-beatPoint;
    if(localPeak >= beatPoint-innerMargin_ && localPeak <= beatPoint+innerMargin_)
    {
      //only consider error in innerWindow for adjustment
      errorIn = error;
      scoreFraction = (mrs_real) ((mrs_real)abs(error) / outterWinRgt_);
      initHypothesis(4) += (((1 - scoreFraction) * localPeakAmp) * (period/(mrs_real)maxPeriod_)); //update score

      if(errorIn == 0)
        errorIn = MINIMUMREAL;
    }
    //if outside inner window calc negative regular score
    else
    {
      scoreFraction = (mrs_real) ((mrs_real)abs(error)/ outterWinRgt_);
      //cout << "SC: " << initHypothesis(4) << "; V: " << localPeakAmp << "; sf: " << scoreFraction << endl;
      initHypothesis(4) += ((-1 * scoreFraction * localPeakAmp) * (period/(mrs_real)maxPeriod_));  //update score
    }

    //cout << "-it:" << it << "; Error: " << error << "; Max: " << localPeak << "; MaxV: " << localPeakAmp
    //	<< "; Sf: " << scoreFraction << "(" << initHypothesis(4) << "); BeatPoint: " << beatPoint << "; period: " << period << endl;
    //it++;

    //don't evaluate beat if the outterWin around it surprasses the induction window
  } while(beatPoint < inSamples_);

  return initHypothesis;
}

//read beat-times from groundtruth file
mrs_realvec
PhaseLock::readGTFile(mrs_string gtFilePath)
{
  mrs_realvec gtHypotheses(4); //save gt hypotheses retrieved from gt file
  mrs_real gtBeatTimeInit1 = NA, gtBeatTimeInit2 = NA, gtBeatTimeLast1 = NA, gtBeatTimeLast2 = NA;

  //cout << "\nINDUCTION TIME: " << timeElapsed_ << "(" << (((timeElapsed_* hopSize_)-hopSize_/2) / srcFs_) << ") [" << inductionTime_ << "]" << endl;
  inStream_.open(gtFilePath.c_str());

  getline (inStream_, line_);

  //for beat groundtruth files (in line, separated by spaces):
  mrs_natural pos0, pos1, pos2;
  pos0 = (mrs_natural) line_.find_first_of(" ", 0); //initial delimiter
  pos1 = (mrs_natural) line_.find_first_of(" ", pos0+1); //second delimiter
  pos2 = (mrs_natural) line_.find_first_of(" ", pos1+1); //third delimiter

  if(pos1 >= 0) //if beat times column by column (on first line)
  {
    //check gtfile for eof at first run -> save last beat time
    if(lastGTBeatTime_ < 0)
    {
      std::istringstream iss(line_);
      char c[10]; //big enough array
      while (iss >> c) //space ("") delimiter
      {
        //last c contains last value in file
        lastGTBeatTime_ = atof(c); //save end value
      }
      iss.clear();
      //cout << "last GT beat time: " << lastGTBeatTime_ << endl;
    }

    //INIT PHASE & PERIOD
    //consider initial search point as inductionTime before the current time point
    mrs_real initBeatPoint = (((mrs_real)((timeElapsed_-inductionTime_) * hopSize_)) - adjustment_) / srcFs_;
    //cout << "InitSearchPoint: " << initBeatPoint << endl;
    //if more than two beatTimes given in the annotation file
    //discart initial beatTime (due to inconsistencies in the beggining of some annotation files)
    mrs_real lastGTBeatTimeInit1 = 0.0;
    //mrs_real lastGTBeatTimeLast1 = 0.0;
    do
    {
      //cout << "gtBeatTimeInit1: " << gtBeatTimeInit1 << "; gtBeatTimeInit2:"
      //		<< gtBeatTimeInit2 << "; initBeatPoint: " << initBeatPoint << endl;

      //gtBeatTimeInit1 == lastGTBeatTime => reached end of gt file =>
      //=> propagate values to current position based on last given hypotheses in the GT file
      if((gtBeatTimeInit1 == lastGTBeatTime_) && (gtBeatTimeInit1 < initBeatPoint))
      {
        mrs_real initGTIBI = abs(gtBeatTimeInit1 - lastGTBeatTimeInit1);
        mrs_natural k = (mrs_natural) ceil((initBeatPoint+MINIMUMREAL - gtBeatTimeInit1) / initGTIBI);
        gtBeatTimeInit1 = gtBeatTimeInit1 + (k * initGTIBI);
        gtBeatTimeInit2 = gtBeatTimeInit2 + initGTIBI;

        break;
      }
      else
      {
        //gtBeatTimeInit1 > initBeatPoint => reached gt beat time corresponding to current time point => ok
        if(gtBeatTimeInit1 > initBeatPoint)
          break;
        else
        {
          lastGTBeatTimeInit1 = gtBeatTimeInit1; //save last gtBeatTimeInit1

          gtBeatTimeInit1 = strtod(line_.substr(pos0+1, pos1).c_str(), NULL);
          gtBeatTimeInit2 = strtod(line_.substr(pos1+1, pos2).c_str(), NULL);

          //keep on looking
          pos0 = (mrs_natural) line_.find_first_of(" ", pos0+1); //initial delimiter
          pos1 = (mrs_natural) line_.find_first_of(" ", pos0+1); //second delimiter
        }
      }

    } while(gtBeatTimeInit1 <= initBeatPoint);

    //cout << "INIT-> gtBeatTime1: " << gtBeatTimeInit1 << "(" << pos0 << "); gtBeatTime2: " << gtBeatTimeInit2
    //	<< "(" << pos1 << "); initBP: " << initBeatPoint << endl;

    //LAST PHASE & PERIOD
    mrs_real lastBeatPoint = (((mrs_real)(timeElapsed_ * hopSize_)) - adjustment_) / srcFs_;
    if((gtBeatTimeInit2 < lastGTBeatTime_) && (gtBeatTimeInit2 > initBeatPoint)) //if still not reached end of file
    {
      do
      {
        //if still looking but beat2 already reached end of file =>
        //assign beat1 as last beat in file (beat2) and beat2 one period (based on last ibi) above
        if((gtBeatTimeLast2 == lastGTBeatTime_) && (gtBeatTimeLast1 < lastBeatPoint))
        {
          //cout << "it2: beat2 reached eof: gtBeatTimeLast2: " << gtBeatTimeLast2 << "; gtBeatTimeLast1: " << gtBeatTimeLast1
          //	<< "; lastGTBeatTime: " << lastGTBeatTime_ << "; lastBeatPoint: " << lastBeatPoint
          //	<< "; ibi: " << lastGTIBI_ << endl;

          lastGTIBI_ = abs(gtBeatTimeLast2 - gtBeatTimeLast1);
          //lastGTBeatTimeLast1 = gtBeatTimeLast1;
          gtBeatTimeLast1 = gtBeatTimeLast2;
          gtBeatTimeLast2 = gtBeatTimeLast1 + lastGTIBI_;
        }

        //gtBeatTimeLast1 == lastGTBeatTime => reached end of gt file =>
        //=> propagate values to current position based on last given hypotheses in the GT file
        if((gtBeatTimeLast1 == lastGTBeatTime_) && (gtBeatTimeLast1 < lastBeatPoint))
        {
          //cout << "it2: beat1 reached eof: gtBeatTimeLast1: " << gtBeatTimeLast1 << "(" << lastGTBeatTimeLast1 << ")"
          //	<< "; lastGTBeatTime: " << lastGTBeatTime_ << "; lastBeatPoint: " << lastBeatPoint << "; ibi: " << lastGTIBI_ << endl;

          mrs_natural k = (mrs_natural) ceil((lastBeatPoint+MINIMUMREAL - gtBeatTimeLast1) / lastGTIBI_);
          gtBeatTimeLast1 = gtBeatTimeLast1 + (k * lastGTIBI_);
          gtBeatTimeLast2 = gtBeatTimeLast1 + lastGTIBI_;

          break;
        }
        else
        {
          //gtBeatTimeLast1 > initBeatPoint => reached gt beat time corresponding to current time point => ok
          if(gtBeatTimeLast1 > lastBeatPoint)
            break;
          else
          {
            //lastGTBeatTimeLast1 = gtBeatTimeLast1; //save last gtBeatTimeLast1

            //keep on looking
            pos0 = (mrs_natural) line_.find_first_of(" ", pos0+1); //initial delimiter
            pos1 = (mrs_natural) line_.find_first_of(" ", pos0+1); //second delimiter

            gtBeatTimeLast1 = strtod(line_.substr(pos0+1, pos1).c_str(), NULL);
            gtBeatTimeLast2 = strtod(line_.substr(pos1+1, pos2).c_str(), NULL);

            if(gtBeatTimeLast2 > gtBeatTimeLast1)
              lastGTIBI_ = abs(gtBeatTimeLast2 - gtBeatTimeLast1);

            //cout << "gtBeatTimeLast1: " << gtBeatTimeLast1 << "; gtBeatTimeLast2:"
            //	<< gtBeatTimeLast2 << "; lastBeatPoint: " << lastBeatPoint << "; ibi: " << lastGTIBI_ << endl;
          }
        }

      } while(gtBeatTimeLast1 <= lastBeatPoint);
    }
    else //if already reached end of gt file on the first request (beat-time at the begining of this induction window)
    {
      //calculate gtBeatTimeLast1 and gtBeatTimeLast2 from init values
      cerr << "Reached end of ground-truth file...Last GT values propagated from the last hypotheses given by the GT file!" << endl;

      mrs_natural k = (mrs_natural) ceil((lastBeatPoint+MINIMUMREAL - gtBeatTimeInit1) / lastGTIBI_);
      gtBeatTimeLast1 = gtBeatTimeInit1 + (k * lastGTIBI_);
      gtBeatTimeLast2 = gtBeatTimeLast1 + lastGTIBI_;
    }

    //cout << "LAST-> gtBeatTime1: " << gtBeatTimeLast1 << "; gtBeatTime2: " << gtBeatTimeLast2 << endl;

    //cout <<  "gtInitBeatTime1: " << gtBeatTimeInit1 << "; gtInitBeatTime2: " << gtBeatTimeInit2
    //	<< "; gtLastBeatTime1: " << gtBeatTimeLast1 << "; gtLastBeatTime2: " << gtBeatTimeLast2
    //	<< "; initBeat: " << initBeatPoint << "; lastBeat: " << lastBeatPoint <<"; lastGtBeat: "
    //	<< lastGTBeatTime_ << endl;
  }

  gtHypotheses(0) = gtBeatTimeInit1;
  gtHypotheses(1) = gtBeatTimeInit2;
  gtHypotheses(2) = gtBeatTimeLast1;
  gtHypotheses(3) = gtBeatTimeLast2;

  return gtHypotheses;
}

//testing trigger ground-truth without calculating pseudo-tracking starting from gt values
void
PhaseLock::handleGTHypotheses(realvec& in, realvec& out,mrs_string gtFilePath, mrs_realvec gtHypotheses)
{
  mrs_real gtBeatTimeInit1 = gtHypotheses(0);
  mrs_real gtBeatTimeInit2 = gtHypotheses(1);
  mrs_real gtBeatTimeLast1 = gtHypotheses(2);
  mrs_real gtBeatTimeLast2 = gtHypotheses(3);

  //gt init period and phase in frames
  gtInitPeriod_ = (mrs_natural) (((abs(gtBeatTimeInit2 - gtBeatTimeInit1) * srcFs_) / hopSize_) + 0.5);
  gtInitPhase_ = (mrs_natural) ceil(((gtBeatTimeInit1 * srcFs_) + adjustment_) / hopSize_);
  //gtInitPhase_ = (mrs_natural) ceil(((gtBeatTimeIni1 * srcFs_)) / hopSize_);

  //gt last period and phase in frames
  gtLastPeriod_ = (mrs_natural) (((abs(gtBeatTimeLast2 - gtBeatTimeLast1) * srcFs_) / hopSize_) + 0.5);
  gtLastPhase_ = (mrs_natural) ceil(((gtBeatTimeLast1 * srcFs_) + adjustment_) / hopSize_);
  //gtLastPhase_ = (mrs_natural) ceil(((gtBeatTimeLast1 * srcFs_)) / hopSize_);

  //cout << "iniBeat1: " << gtBeatTimeInit1 << "; iniBeat2: " << gtBeatTimeInit2 << "; lasBeat1: " << gtBeatTimeLast1
  //	<< "; lasBeat2: "<< gtBeatTimeLast2 << "; gtIniPh: " << gtInitPhase_ << "; gtIniP: " << gtInitPeriod_
  //	<< "(" << (60/(gtBeatTimeInit2 - gtBeatTimeInit1)) << "BPM)(" << (gtBeatTimeInit2 - gtBeatTimeInit1) << "); gtLastPh: "
  //	<< gtLastPhase_ << "; gtLastP: " << gtLastPeriod_ << "(" << (60/(gtBeatTimeLast2 - gtBeatTimeLast1)) << "BPM)" << endl;

  //ADJUST PHASE TO INDUCTION (ACCUMULATOR) WINDOW
  mrs_natural phaseAccAdjust = (inSamples_ - 1 - timeElapsed_) + gtInitPhase_;
  //cout << "; gtPAdj: " << phaseAccAdjust << "t: " << timeElapsed_ << "; inS: " << inSamples_ << endl;

  //calculate k for reversing phase till minimum (by subtracting period multiples)
  mrs_natural k = (mrs_natural) (((mrs_real)((inSamples_-1-inductionTime_)-phaseAccAdjust)/(mrs_real)gtInitPeriod_)+MINIMUMREAL);
  k += 1; //start on 2nd beat prediction (due to unconsistencies on the very beggining of the accumulator)
  phaseAccAdjust = ((mrs_natural) phaseAccAdjust) + ((mrs_natural) (k * gtInitPeriod_));

  if(strcmp(mode_.c_str(), "2b2") == 0 || strcmp(mode_.c_str(), "2b") == 0) //adjust values to induction window
  {
    mrs_realvec gtInitHypothesis = GTInitialization(in, out, phaseAccAdjust, gtInitPeriod_);

    if(curBestScore_ == NA) //if initial induction window
    {
      gtScore_ = gtInitHypothesis(4);
      //cout << "InitGtScore: " << gtScore_ << endl;
    }
    else //if trigger induction
    {
      if(curBestScore_ > 0) //to avoid best score inversion
        gtScore_ = curBestScore_ * triggerBestScoreFactor_;
      else
        gtScore_ = curBestScore_ / triggerBestScoreFactor_;

      //cout << "t: " << timeElapsed_ << "; curBest: " << curBestScore_ << "; gtScore: " << gtScore_ << endl;
    }

    if(strcmp(mode_.c_str(), "2b") == 0) //adjust phases and periods
    {
      gtInitPeriod_ = (mrs_natural) gtInitHypothesis(1); //initPeriod
      gtLastPeriod_ = (mrs_natural) gtInitHypothesis(3); //lastPeriod
      gtInitPhase_ = (mrs_natural) gtInitHypothesis(0) - (inSamples_-1 - timeElapsed_);
      gtLastPhase_ = (mrs_natural) gtInitHypothesis(2) - (inSamples_-1 - timeElapsed_) + gtLastPeriod_;

      //cout << "real gtInitPhase: " << gtInitPhase_ << "; gtLastPhase: " << gtLastPhase_ << endl;

      //for safechecking (really unlikely to happen) if lastPhases still don't surprasses the induction window then sum other period
      if(gtLastPhase_ <= timeElapsed_)
        gtLastPhase_ += gtLastPeriod_;
    }

    //cout << "iniPh: " << gtInitPhase_ << "(" << (((gtInitPhase_ * hopSize_) - adjustment_) / srcFs_) << "); iniPer: " << gtInitPeriod_
    //	<< "; lasPh: " << gtLastPhase_ << "(" << (((gtLastPhase_ * hopSize_) - adjustment_) / srcFs_) << "); lasPer: " << gtLastPeriod_
    //	<< "; gtSc: " << gtScore_ << endl;

    cerr << "\nInduction replaced by IBI given by two beats from ground-truth file at: " << gtFilePath << endl;
  }

  //simulate tracking under induction windows, given first beat (or any beat)
  //retrieve initial score, backtraced phase (subtracting period multiples), and phase just after the induction
  //(ground-truth initial phase and period as calculated through ACF (normal induction))
  if(strcmp(mode_.c_str(), "1b") == 0 || strcmp(mode_.c_str(), "1b1") == 0)
  {
    //matrix with all the N generated beat hypotheses in the induction stage
    beatHypotheses_ = ctrl_beatHypotheses_->to<mrs_realvec>();

    mrs_realvec lastBeatPoint(nInitHyp_);
    mrs_realvec lastLocalPeriod(nInitHyp_);
    for(int i = 0; i < nrPeriodHyps_; i++)
    {
      mrs_natural phase = phaseAccAdjust;
      mrs_natural period = (mrs_natural) beatHypotheses_(i*nrPhasesPerPeriod_, 0);

      initPeriods_(i) = period; //keep each init period hypothesis
      if(period > 1) //(phase is necessarily valid)
      {
        mrs_realvec gtInitHypothesis = GTInitialization(in, out, phase, period);
        lastPeriods_(i) = (mrs_natural) gtInitHypothesis(3); //keep each lastPeriod

        if(strcmp(mode_.c_str(), "1b") == 0) //adjusted phases to induction window
        {
          initPhases_(i) = (mrs_natural) gtInitHypothesis(0) - (inSamples_-1 - timeElapsed_);
          lastPhases_(i) = (mrs_natural) gtInitHypothesis(2) - (inSamples_-1 - timeElapsed_) + lastPeriods_(i);

          //for safechecking (really unlikely to happen) if lastPhases still don't surprasses the induction window then sum other period
          if(lastPhases_(i) <= timeElapsed_)
            lastPhases_(i) += lastPeriods_(i);
        }

        trackingScore_(i) = gtInitHypothesis(4);

        rawScore_(i) = trackingScore_(i); //rawScore equals simulated trackingScore
        maxLocalTrackingScore_(i) = trackingScore_(i); //every hypothesis is maximum local because there's only one phase per period

        //cout << "i-" << i << "; iniPhase: " << initPhases_(i) << "; initPeriod: " << initPeriods_(i)
        //	<< "; lastPhase: " << lastPhases_(i) << "; lastPeriod: " << lastPeriods_(i)
        //	<< "; rawScore: " << rawScore_(i) << endl;
      }
    }

    mrs_real maxMetricalRelScore = NA; //to allow best negative score
    mrs_natural avgPeriod = 0;
    mrs_real avgLocalTrackingScore_ = 0.0;
    mrs_real maxGlobalTrackingScore_ = NA; //to allow best negative score
    //mrs_natural maxMetricalRelScoreInd = -1;
    for(int i = 0; i < nrPeriodHyps_; i++)
    {
      if(initPeriods_(i) > MINIMUMREAL) //if the given period_ > 0 (= valid period_) (initial phase, being ground-truth, is necessarily valid
      {
        metricalRelScore_(i) = calcRelationalScore(i, rawScore_);

        if(metricalRelScore_(i) > maxMetricalRelScore)
        {
          maxMetricalRelScore = metricalRelScore_(i);
          //maxMetricalRelScoreInd = i;
        }

        if(maxLocalTrackingScore_(i) > maxGlobalTrackingScore_)
          maxGlobalTrackingScore_ = maxLocalTrackingScore_(i);

        avgLocalTrackingScore_ += maxLocalTrackingScore_(i);
        avgPeriod += (mrs_natural) initPeriods_(i);

        //cout << "MetrRelScore" << i << ":" << metricalRelScore_(i) << "; iPeriod:" << initPeriods_(i)
        //	<< "; iPhase:" << initPhases_(i) << "(" << beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 1)
        //	<< "); MaxMetrRelScore:" << maxMetricalRelScore << "(" << maxMetricalRelScoreInd << ")" << endl;

      }
    }

    for(int i = 0; i < nrPeriodHyps_; i++)
    {
      if(strcmp(mode_.c_str(), "1b") == 0) //adjusted phase to induction window
      {
        if(backtrace_)
        {
          //Period:
          out(i, 0) = initPeriods_(i);
          //Phase:
          out(i, 1) = initPhases_(i);

          if(i == 0)
          {
            if(gtAfter2ndBeat_)
              cerr << "\nInitial phase backtraced from second beat in ground-truth file at: " << gtFilePath << endl;
            else
              cerr << "\nInitial phase backtraced from first beat in ground-truth file at: " << gtFilePath << endl;

            cerr << "Initial phase: " << (((initPhases_(i) * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
          }
        }
        else //if no backtrace
        {
          //Period:
          out(i, 0) = lastPeriods_(i);
          //Phase:
          out(i, 1) = lastPhases_(i);

          if(i == 0)
          {
            if(gtAfter2ndBeat_)
              cerr << "\nInitial phase adjusted from second beat in ground-truth file at: " << gtFilePath << endl;
            else
              cerr << "\nInitial phase adjusted from first beat in ground-truth file at: " << gtFilePath << endl;

            cerr << "Initial phase: " << (((lastPhases_(i) * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
          }
        }
      }
      else if(strcmp(mode_.c_str(), "1b1") == 0) //phase as given by gt file
      {
        if(backtrace_)
        {
          //Period:
          out(i, 0) = initPeriods_(i);
          //Phase:
          out(i, 1) = gtInitPhase_; //keep first phase as extracted from gt file

          if(i == 0)
          {
            if(gtAfter2ndBeat_)
              cerr << "\nInitial phase replaced by second beat in ground-truth file at: " << gtFilePath << endl;
            else
              cerr << "\nInitial phase replaced by first beat in ground-truth file at: " << gtFilePath << endl;

            cerr << "Initial phase: " << (((gtInitPhase_ * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
          }
        }
        else //if no backtrace (causal)
        {
          //Period:
          out(i, 0) = lastPeriods_(i);
          //Phase:
          out(i, 1) = gtLastPhase_; //keep first phase as extracted from gt file

          if(i == 0)
          {
            cerr << "\nInitial phase replaced by first beat in ground-truth file at: " << gtFilePath << endl;
            cerr << "Initial phase: " << (((gtLastPhase_ * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
          }
        }
      }

      //After-Induction Score Calulcation: ===========================================================
      mrs_real finalScore; //final score
      if(!dumbInduction_) //if not in dumbInduction consider metrical relations
      {
        mrs_real metricalRelFraction = 0.0;
        metricalRelFraction = (metricalRelScore_(i) / maxMetricalRelScore);

        //to avoid negative score inversions
        if(metricalRelScore_(i) < 0 && maxMetricalRelScore > 0 && maxGlobalTrackingScore_ > 0
            && abs(metricalRelScore_(i)) > maxMetricalRelScore)
          metricalRelFraction = -1;
        if(metricalRelScore_(i) < 0 && maxMetricalRelScore < 0 && maxGlobalTrackingScore_ > 0)
          metricalRelFraction = (maxMetricalRelScore / metricalRelScore_(i));


        if(curBestScore_ == NA) //initially consider a proportion of the maximum tracking score for this induction window
          finalScore = metricalRelFraction * maxGlobalTrackingScore_;
        else //at each trigger consider a proportion of the current best score
        {
          if(curBestScore_ > 0) //to avoid best score inversion
            finalScore = (metricalRelFraction * curBestScore_) * triggerBestScoreFactor_;
          else
            finalScore = (metricalRelFraction * curBestScore_) / triggerBestScoreFactor_;

          //cout << "relFrac: " << metricalRelFraction << "; curBestScore: " << curBestScore_ << "; finalScore: " << finalScore << endl;
        }

        //cout << i << ": " << finalScore << "; MetrScore: " << metricalRelScore_(i) << "; MaxMetrRelScore: "
        //	<< maxMetricalRelScore << "; MaxCorrScore: " << maxGlobalTrackingScore_ << "; initPeriod: " << initPeriods_(i)
        //	<< "; initPhase: " << initPhases_(i) << "; lastPeriod: " << lastPeriods_(i) << "; lastPhase: "
        //	<< lastPhases_(i) << endl;

        //cout << "scoreFunc:" << scoreFunc_ << "; avgMaxSum:" << avgMaxSum << "; avgPeriod:" << avgPeriod << endl;
      }
      else //if dumbInduction (either manual or needed-no salient periodicities) don't consider metrical relations
      {
        //cout << "DUMB INDUCTION" << endl;

        if(curBestScore_ == NA) //initially consider a proportion of the maximum tracking score for this induction window
          finalScore = rawScore_(i);
        else //at each trigger consider a proportion of the current best score
        {
          //final score = a fraction of the curBestScore given by the proportion of this agent's induction score
          //to the maximum induction score, and by a user-defined triggerBestScoreFactor

          //to avoid rawScoreFraction negative inversions
          mrs_real rawScoreFraction;
          //(to avoid divide by 0 if hypothesis exists)
          if(initPeriods_(i) > MINIMUMREAL && initPhases_(i) > MINIMUMREAL)
          {
            if(rawScore_(i) == 0.0) rawScore_(i) = MINIMUMREAL;
            if(maxGlobalTrackingScore_ == 0.0) maxGlobalTrackingScore_ = MINIMUMREAL;
          }

          //if rawScore negative and maxRawScore positive && rawScore or maxRawScore < 1
          if((rawScore_(i) < 0 && maxGlobalTrackingScore_ > 0) && (fabs(rawScore_(i)) < 1 || maxGlobalTrackingScore_ < 1))
            rawScoreFraction = rawScore_(i) * maxGlobalTrackingScore_;
          else
          {
            if(fabs(rawScore_(i)) < maxGlobalTrackingScore_) //if abs(rawScore) < maxRawScore
              rawScoreFraction = rawScore_(i) / maxGlobalTrackingScore_;
            else //if abs(rawScore) > maxRawScore
              rawScoreFraction = maxGlobalTrackingScore_ / rawScore_(i);
          }

          if(curBestScore_ > 0) //to avoid best score inversion
            finalScore = (rawScoreFraction * curBestScore_) * triggerBestScoreFactor_;
          else
            finalScore = (rawScoreFraction * curBestScore_) / triggerBestScoreFactor_;

          //cout << "rawSc: " << rawScore_(i) << "; maxRawSc: " << maxGlobalTrackingScore_ << "; rawFr: " << rawScoreFraction
          //	<< "; curBestScore: " << curBestScore_ << "; finalScore: " << finalScore << "; triggerScFact: "
          //	<< triggerBestScoreFactor_ << endl;
        }

        //reset dumb induction
        ctrl_dumbInduction_->setValue(false);
      }

      //aditional initial score normalization dependent of score function (due to their relative weights)
      if(strcmp(scoreFunc_.c_str(), "correlation") == 0 || strcmp(scoreFunc_.c_str(), "squareCorr") == 0 )
        finalScore *= 5; //("correlation" and "squareCorr" are, in average, 5times more reactive than "regular")

      out(i, 2) = finalScore;
    }
  }

  //cout << "Beat1: " << gtBeatTime1_ << "(" << gtPhase_ << "-" << gtInitPhase_ << "); Beat2: "
  //	<< gtBeatTime2_ << "; Period: " << gtInitPeriod_ << "("
  //	<< (((gtBeatTime2_ - gtBeatTime1_) * srcFs_) / hopSize_) << "); Score:" << gtScore_ << endl;

  inStream_.close();
}

mrs_natural
PhaseLock::metricalRelation(mrs_real period1, mrs_real period2)
{
  mrs_real periodA = (period1 > period2 ? period1 : period2);
  mrs_real periodB = (period1 > period2 ? period2 : period1);
  mrs_real multiple = periodA / periodB;
  mrs_real tolerance = 0.15; //4/periodB;

  mrs_natural floorMultiple = (mrs_natural) floor(multiple);
  mrs_natural ceilMultiple = (mrs_natural) ceil(multiple);
  mrs_natural rel = 0;

  //if(print)
  //cout << "Per1:" << period1 << "; Per2:" << period2 << "; Multiple:" << multiple
  //<< "(" << (multiple - tolerance) << ":" << (multiple + tolerance) << ")" << endl;

  //if periods are integerly related (with a given tolerance)
  if(multiple - tolerance <= floorMultiple)
  {
    if(floorMultiple == 2 || floorMultiple == 4)
      rel = 6 - floorMultiple; //then retrieve integer relation
    else if(floorMultiple == 3)
      rel = 3; //then retrieve integer relation
    else if(floorMultiple >= 5 && floorMultiple <= 8)
      rel = 1;
    else
      rel = 0;
  }

  else if(multiple + tolerance >= ceilMultiple)
  {
    if(ceilMultiple == 2 || ceilMultiple == 4)
      rel = 6 - ceilMultiple; //then retrieve integer relation
    else if(ceilMultiple == 3)
      rel = 3; //then retrieve integer relation
    else if(ceilMultiple >= 5 && ceilMultiple <= 8)
      rel = 1;
    else rel = 0;
  }

  else rel = 0; //else retrieve 0 (periods non metricaly related)

  return rel;
}

mrs_real
PhaseLock::calcRelationalScore(mrs_natural i, mrs_realvec rawScoreVec)
{
  mrs_real score = 0.0;
  score = 2*5*rawScoreVec(i); //self-relation = 10*

  mrs_natural metricalRel;
  for(int j = 0; j < nrPeriodHyps_; j++)
  {
    if(j != i && initPeriods_(i) > MINIMUMREAL && initPeriods_(j) > MINIMUMREAL)
    {
      metricalRel = metricalRelation((mrs_natural)initPeriods_(i), (mrs_natural)initPeriods_(j));
      score += rawScoreVec(j) * metricalRel;
    }
  }

  return score;
}


void
PhaseLock::regularFunc(realvec& in, realvec& out)
{
  //matrix with all the N generated beat hypotheses in the induction stage
  beatHypotheses_ = ctrl_beatHypotheses_->to<mrs_realvec>();

  //MATLAB_PUT(beatHypotheses_, "BeatHypotheses");
  //MATLAB_PUT(in, "Flux_FlowThrued2");

  //Build N hypotheses signals (phase + k*periods):
  mrs_realvec lastBeatPoint(nInitHyp_);
  mrs_realvec lastLocalPeriod(nInitHyp_);
  for(int h = 0; h < nInitHyp_; h++)
  {
    mrs_natural phase = (mrs_natural) beatHypotheses_(h, 1);
    mrs_natural period = (mrs_natural) beatHypotheses_(h, 0);
    if(phase > 0 && period > 1)
    {
      //if(timeElapsed_ == 1228)
      //cout << "h: " << h << "; ph: " << phase << "; per: " << period << endl;

      //calculate k for reversing phase till minimum (by subtracting period multiples)
      mrs_natural k = (mrs_natural) (((mrs_real)((inSamples_-1-inductionTime_)-phase)/(mrs_real)period)+MINIMUMREAL);

      if((phase + ((mrs_natural) ((k+1) * period))) < inSamples_) //to avoid surpassing accumulator
        k += 1; //start on 2nd beat prediction (due to unconsistencies on the very beggining of the accumulator)

      firstBeatPoint_(h) = phase + ((mrs_natural) (k * period));

      //cout << "H: " << h << "-1BP: " << firstBeatPoint_(h) << endl;

      //adjust initPhase around innerMargin
      beatCount_(h) = 1;
      mrs_real localPeakAmp = in((mrs_natural)firstBeatPoint_(h));
      mrs_natural localPeak = (mrs_natural) firstBeatPoint_(h);
      //mrs_natural errorTmp = 0;
      for (int t = (mrs_natural)firstBeatPoint_(h)-innerMargin_; t <= (mrs_natural)firstBeatPoint_(h)+innerMargin_; t++)
      {
        //Limit analysis to induction window
        if(t >= (inSamples_-1-inductionTime_))
        {
          if(in(t) > localPeakAmp)
          {
            //cout << "localPeak: " << localPeak << "; Amp: " << localPeakAmp << "; newPeak: " << t << "; newAmp: " << in(t) << endl;
            localPeakAmp = in(t);
            localPeak = t;
          }
        }
      }
      //errorTmp = (mrs_natural) firstBeatPoint_(h) - localPeak;
      //cout << "INIT_ERROR: " << errorTmp << endl;
      //cout << "Adj: " << localPeak << "; REALinitPhase: " << localPeak - (inSamples_-1 - timeElapsed_) << endl;

      trackingScore_(h) += localPeakAmp * ((mrs_real)period / (mrs_real)maxPeriod_); //Assume perfect initial beat prediction

      //if(timeElapsed_ == 1228)
      //cout << "TrackingScore1: " << trackingScore_(h) << " [" << localPeakAmp * ((mrs_real)period / (mrs_real)maxPeriod_) << "]" << endl;

      //cout << "h:" << h << "-k:" << k << "(" << (((mrs_real)((inSamples_-1-inductionTime_)-phase)/(mrs_real)period)+MINIMUMREAL) << ")"
      //	<< ";period: " << beatHypotheses_(h, 0) << ";phase: " << firstBeatPoint_(h) << "(" << (mrs_natural) beatHypotheses_(h, 1) << ")"
      //	<< ";adPhase: " << localPeak << ";S: " << trackingScore_(h) << endl;

      firstBeatPoint_(h) = localPeak; //Keep first beat point (adjusted to maximum wihtin innerMargin)

      //cout << "FirstBeat: " << firstBeatPoint_(h) << endl;
      hypSignals_(h, (mrs_natural) firstBeatPoint_(h)) = 1.0;

      outterWinLft_ = (mrs_natural) ceil(period * lftOutterMargin_); //(% of IBI)
      outterWinRgt_ = (mrs_natural) ceil(period * rgtOutterMargin_); //(% of IBI)
      mrs_natural beatPoint = (mrs_natural) firstBeatPoint_(h);
      mrs_real scoreFraction = 0.0;
      mrs_natural error = 0;
      mrs_real errorIn = MINIMUMREAL; //to avoid divide by zero
      mrs_natural periodTmp = 0;
      //mrs_natural it=1; //used for cout debugging
      do
      {
        //cout << "Period: " << period << endl;
        //cout << "Hypothesis (" << h << "): Phase: " << phase << " period_: " << period << "; corFactor: " << corFactor_ << endl;
        periodTmp = period + ((mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)));

        //cout << "NewPeriod: " << periodTmp << "; errorIn: " << errorIn << "; adj: " << ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)) << endl;
        //if the updated period don't respect the limits then keeps the former value
        if(periodTmp >= minPeriod_ && periodTmp <= maxPeriod_)
          period = periodTmp;

        //(for safechecking)
        if((beatPoint + (period + (mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)))) >= inSamples_)
        {
          lastBeatPoint(h) = beatPoint; //Keep current lastPhase
          lastLocalPeriod(h) = period; //Keep current lastPeriod
          break;
        }

        beatPoint += (period + (mrs_natural) ((errorIn*corFactor_) + ((errorIn/abs(errorIn)) * 0.5)));
        errorIn = MINIMUMREAL; //to avoid divide by zero

        //cout << "curBeatPoint: " << beatPoint << endl;

        lastBeatPoint(h) = beatPoint;
        lastLocalPeriod(h) = period;
        //Check maximum around outter window
        localPeakAmp = in(beatPoint);
        localPeak = beatPoint;
        for (int t = beatPoint-outterWinLft_; t <= beatPoint+outterWinRgt_; t++)
        {
          if(t > (inSamples_-1-inductionTime_) && t < inSamples_)
          {
            if(in(t) > localPeakAmp)
            {
              localPeakAmp = in(t);
              localPeak = t;
            }
          }
        }

        //Check if maximum is inside inner window
        //calc positive regular score and adjust hypothesis
        error = localPeak-beatPoint;
        if(localPeak >= beatPoint-innerMargin_ && localPeak <= beatPoint+innerMargin_)
        {
          //only consider error in innerWindow for adjustment
          errorIn = error;
          scoreFraction = (mrs_real) ((mrs_real)abs(error) / outterWinRgt_);
          trackingScore_(h) += (((1 - scoreFraction) * localPeakAmp) * ((mrs_real)period / (mrs_real)maxPeriod_));

          if(errorIn == 0)
            errorIn = MINIMUMREAL;

          //cout << "SCORE_FRACTION: " << (((1 - scoreFraction) * localPeakAmp) * ((mrs_real)period / (mrs_real)maxPeriod_)) << endl;
        }
        //if outside inner window calc negative regular score
        else
        {
          scoreFraction = (mrs_real) ((mrs_real)abs(error)/ outterWinRgt_);
          trackingScore_(h) += ((-1 * scoreFraction * localPeakAmp) * ((mrs_real)period / (mrs_real)maxPeriod_));

          //cout << "SCORE_FRACTION: " << ((-1 * scoreFraction * localPeakAmp) * ((mrs_real)period / (mrs_real)maxPeriod_)) << endl;
        }

        hypSignals_(h, beatPoint) = 1.0;
        beatCount_(h)++;

        //if(timeElapsed_ == 1228)
        //cout << "H:" << h << "-it:" << it << "; Error: " << error << "; Max: " << localPeakAmp
        //	<< "; S: " << trackingScore_(h) << "; BeatPoint: " << beatPoint << "; period: " << period << endl;
        //it++;
        //don't evaluate beat if the outterWin around it surprasses the induction window
      } while(beatPoint < inSamples_);
    }
  }

  //MATLAB_PUT(beatHypotheses_, "BeatHypotheses");

  //Retrieve best M (nrPeriodHyps_) {period_, phase} pairs, by period:
  for(int i = 0; i < nrPeriodHyps_; i++)
  {
    //cout << "i- " << i << "; period: " << beatHypotheses_(i*nrPhasesPerPeriod_, 0) << endl;
    //if the given period_ > 0 (= valid period_)
    if(beatHypotheses_(i*nrPhasesPerPeriod_, 0) > 0)
    {
      for(int j = i*nrPhasesPerPeriod_; j < (i*nrPhasesPerPeriod_)+nrPhasesPerPeriod_; j++)
      {
        // if given phase > 0 (= valid phase)
        if(beatHypotheses_(j, 1) > 0)
        {
          if(trackingScore_(j) > maxLocalTrackingScore_(i))
          {
            maxLocalTrackingScore_(i) = trackingScore_(j);
            maxLocalTrackingScoreInd_(i) = j;
          }
        }
      }

      initPeriods_(i) = beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 0); //keep each init period_ hypothesis
      lastPeriods_(i) = lastLocalPeriod((mrs_natural)maxLocalTrackingScoreInd_(i));
      //initPhases_(i) = firstBeatPoint_((mrs_natural)maxLocalTrackingScoreInd_(i)) - (inSamples_-1 - inductionTime_);
      //lastPhases_(i) = lastBeatPoint((mrs_natural)maxLocalTrackingScoreInd_(i)) - (inSamples_-1 - inductionTime_) + lastPeriods_(i);
      initPhases_(i) = firstBeatPoint_((mrs_natural)maxLocalTrackingScoreInd_(i)) - (inSamples_-1 - timeElapsed_);
      lastPhases_(i) = lastBeatPoint((mrs_natural)maxLocalTrackingScoreInd_(i)) - (inSamples_-1 - timeElapsed_) + lastPeriods_(i);
      //for safechecking (really unlikely to happen) if lastPhases still don't surprasses the induction window then sum other period
      if(lastPhases_(i) <= timeElapsed_)
        lastPhases_(i) += lastPeriods_(i);

      //metricalSalience_(i) = beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 2) * 10000; //(x10000 for real value)
      //rawScore_(i) = pow(metricalSalience_(i),2) * maxLocalTrackingScore_(i) / sqrt(beatCount((mrs_natural)maxLocalTrackingScoreInd_(i)));
      rawScore_(i) =  maxLocalTrackingScore_(i);// * (initPeriods_(i) / (mrs_real) maxPeriod_);
      metricalSalience_(i) = beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 2);
      //rawScore_(i) = metricalSalience_(i) * (initPeriods_(i) / 105);

      //cout << i << "(" << (mrs_natural)maxLocalTrackingScoreInd_(i) << ")- corrScore:" << maxLocalTrackingScore_(i) << "; Phase: "
      //	<< beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 1) << "; initPeriod: " << initPeriods_(i) << "; initPhase:"
      //	<< initPhases_(i) << "; lastPeriod: " << lastPeriods_(i) << "; lastPhases: " << lastPhases_(i) << endl;
    }
  }

  mrs_real maxMetricalRelScore = NA; //to allow negative scores
  mrs_natural avgPeriod = 0;
  mrs_real avgLocalTrackingScore_ = 0.0;
  mrs_real maxGlobalTrackingScore_ = NA; //to allow negative scores
  mrs_natural maxMetricalRelScoreInd = -1;
  for(int i = 0; i < nrPeriodHyps_; i++)
  {
    //if the given period_ > 0 && given phase > 0 (= valid period_ && valid phase)
    if(initPeriods_(i) > MINIMUMREAL && initPhases_(i) > MINIMUMREAL)
    {
      metricalRelScore_(i) = calcRelationalScore(i, rawScore_);

      if(metricalRelScore_(i) > maxMetricalRelScore)
      {
        maxMetricalRelScore = metricalRelScore_(i);
        maxMetricalRelScoreInd = i;
      }

      if(maxLocalTrackingScore_(i) > maxGlobalTrackingScore_)
        maxGlobalTrackingScore_ = maxLocalTrackingScore_(i);

      avgLocalTrackingScore_ += maxLocalTrackingScore_(i);
      avgPeriod += (mrs_natural) initPeriods_(i);

      //cout << "MetrRelScore" << i << ":" << metricalRelScore_(i) << "; iPeriod:" << initPeriods_(i)
      //	<< "; iPhase:" << initPhases_(i) << "(" << beatHypotheses_((mrs_natural)maxLocalTrackingScoreInd_(i), 1)
      //	<< "); MaxMetrRelScore:" << maxMetricalRelScore << "(" << maxMetricalRelScoreInd << ")" << endl;
    }
  }

  //if requested output induction best period hypothesis (as estimated by ACF and after adjustment in induction)
  if(strcmp(inductionOut_.c_str(), "-1") != 0 && !dumbInduction_)
  {
    ostringstream ossInitPeriod, ossLastPeriod;
    fstream outStream;
    mrs_natural indInitPeriod = (mrs_natural) (((60.0 / initPeriods_(maxMetricalRelScoreInd))
                                * (srcFs_ / hopSize_)) + 0.5); // (+ 0.5 for round integer)
    mrs_natural indLastPeriod = (mrs_natural) (((60.0 / lastPeriods_(maxMetricalRelScoreInd))
                                * (srcFs_ / hopSize_)) + 0.5); // (+ 0.5 for round integer)

    //write best initPeriod calculation (from ACF peaks)
    ossInitPeriod << inductionOut_ << "_indInitTempo.txt";
    outStream.open(ossInitPeriod.str().c_str(), ios::out|ios::trunc);
    outStream << indInitPeriod;
    outStream.close();

    //write best lastPeriod calculation (after adjusting the initial best period till the end of induction)
    ossLastPeriod << inductionOut_ << "_indLastTempo.txt";
    outStream.open(ossLastPeriod.str().c_str(), ios::out|ios::trunc);
    outStream << indLastPeriod;
    outStream.close();
  }

  avgLocalTrackingScore_ /= nrPeriodHyps_;
  avgPeriod /= nrPeriodHyps_;

  for(int i = 0; i < nrPeriodHyps_; i++)
  {
    if(backtrace_)
    {
      //Period:
      out(i, 0) = initPeriods_(i);
      //Phase:
      out(i, 1) = initPhases_(i);
    }
    else
    {
      //Period:
      out(i, 0) = lastPeriods_(i);
      //Phase:
      out(i, 1) = lastPhases_(i);
    }

    //After-Induction Score Calulcation: ===========================================================
    mrs_real finalScore; //final score
    if(!dumbInduction_) //if not in dumbInduction consider metrical relations
    {
      mrs_real metricalRelFraction = 0.0;
      metricalRelFraction = (metricalRelScore_(i) / maxMetricalRelScore);

      //to avoid negative score inversions
      if(metricalRelScore_(i) < 0 && maxMetricalRelScore > 0 && maxGlobalTrackingScore_ > 0
          && abs(metricalRelScore_(i)) > maxMetricalRelScore)
        metricalRelFraction = -1;
      if(metricalRelScore_(i) < 0 && maxMetricalRelScore < 0 && maxGlobalTrackingScore_ > 0)
        metricalRelFraction = (maxMetricalRelScore / metricalRelScore_(i));

      //initially consider a proportion of the maximum tracking score for this induction window
      if(curBestScore_ == NA)
        finalScore = metricalRelFraction * maxGlobalTrackingScore_;
      else //at each trigger consider a proportion of the current best score
      {
        if(curBestScore_ > 0) //to avoid best score inversion
          finalScore = (metricalRelFraction * curBestScore_) * triggerBestScoreFactor_;
        else
          finalScore = (metricalRelFraction * curBestScore_) / triggerBestScoreFactor_;

        //cout << "relFrac: " << metricalRelFraction << "; curBestScore: " << curBestScore_ << "; finalScore: " << finalScore
        //	<< "; triggerBestScoreFactor: " << triggerBestScoreFactor_ << endl;
      }

      //cout << i << "-> " << finalScore << "; MetrScore: " << metricalRelScore_(i) << "; MaxMetrRelScore: "
      //	<< maxMetricalRelScore << "; MaxCorrScore: " << maxGlobalTrackingScore_ << "; initPeriod: " << initPeriods_(i)
      //	<< "; initPhase: " << initPhases_(i) << "; lastPeriod: " << lastPeriods_(i) << "; lastPhase: "
      //	<< lastPhases_(i) << endl;

      //cout << "scoreFunc:" << scoreFunc_ << "; avgMaxSum:" << avgMaxSum << "; avgPeriod:" << avgPeriod << endl;
    }
    else //if dumbInduction don't consider metrical relations
    {
      //cout << "DUMB INDUCTION" << endl;

      if(curBestScore_ == NA) //initially consider a proportion of the maximum tracking score for this induction window
        finalScore = rawScore_(i);
      else //at each trigger consider a proportion of the current best score
      {
        //final score = a fraction of the curBestScore given by the proportion of this agent's induction score
        //to the maximum induction score, and by a user-defined triggerBestScoreFactor

        //to avoid rawScoreFraction negative inversions
        mrs_real rawScoreFraction;
        //(to avoid divide by 0 if hypothesis exists)
        if(initPeriods_(i) > MINIMUMREAL && initPhases_(i) > MINIMUMREAL)
        {
          if(rawScore_(i) == 0.0) rawScore_(i) = MINIMUMREAL;
          if(maxGlobalTrackingScore_ == 0.0) maxGlobalTrackingScore_ = MINIMUMREAL;
        }

        //if rawScore negative and maxRawScore positive && rawScore or maxRawScore < 1
        if((rawScore_(i) < 0 && maxGlobalTrackingScore_ > 0) && (fabs(rawScore_(i)) < 1 || maxGlobalTrackingScore_ < 1))
          rawScoreFraction = rawScore_(i) * maxGlobalTrackingScore_;
        else
        {
          if(fabs(rawScore_(i)) < maxGlobalTrackingScore_) //if abs(rawScore) < maxRawScore
            rawScoreFraction = rawScore_(i) / maxGlobalTrackingScore_;
          else //if abs(rawScore) > maxRawScore
            rawScoreFraction = maxGlobalTrackingScore_ / rawScore_(i);
        }

        if(curBestScore_ > 0) //to avoid best score inversion
          finalScore = (rawScoreFraction * curBestScore_) * triggerBestScoreFactor_;
        else
          finalScore = (rawScoreFraction * curBestScore_) / triggerBestScoreFactor_;

        //cout << "rawSc: " << rawScore_(i) << "; maxRawSc: " << maxGlobalTrackingScore_ << "; rawFr: " << rawScoreFraction
        //	<< "; curBestScore: " << curBestScore_ << "; finalScore: " << finalScore << "; triggerScFact: "
        //	<< triggerBestScoreFactor_ << endl;
      }

      //reset dumb induction
      ctrl_dumbInduction_->setValue(false);
    }

    //aditional initial score normalization dependent of score function (due to their relative weights)
    if(strcmp(scoreFunc_.c_str(), "correlation") == 0 || strcmp(scoreFunc_.c_str(), "squareCorr") == 0 )
      finalScore *= 5; //("correlation" and "squareCorr" are, in average, 5times more reactive than "regular")

    out(i, 2) = finalScore;

    //cout << "i- " << i << "FINALSCORE: " << finalScore << endl;
  }

  //MATLAB_PUT(in, "Flux_FlowThrued2");
  //MATLAB_PUT(hypSignals_, "HypSignals");
  //MATLAB_PUT(beatHypotheses_, "BeatHypotheses");
  //MATLAB_PUT(out, "FinalHypotheses");
}

void
PhaseLock::forceInitPeriods(mrs_string mode)
{
  (void) mode; // seems to be done with mode_ instead?
  cerr << "\nInitial period(s) given by ground-truth file at: " << ctrl_gtBeatsFile_->to<mrs_string>() << endl;

  beatHypotheses_ = ctrl_beatHypotheses_->to<mrs_realvec>();
  if(strcmp(mode_.c_str(), "p") == 0)
  {
    //just pass gt period
    if(backtrace_) //if in non-causal or backtrace mode
    {
      for(int h = 0; h < nInitHyp_; h++) //give init gt period
        beatHypotheses_(h, 0) = (mrs_real) gtInitPeriod_;

      cerr << "Period as ibi of given first 2 beats: ";
      cerr << ((60.0/gtInitPeriod_)*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
    }
    else
    {
      for(int h = 0; h < nInitHyp_; h++) //give last gt period
        beatHypotheses_(h, 0) = (mrs_real) gtLastPeriod_;

      cerr << "Period as ibi of given last 2 beats: ";
      cerr << ((60.0/gtLastPeriod_)*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
    }

    nrPeriodHyps_ = 1;
  }
  else if(strcmp(mode_.c_str(), "p_mr") == 0)
  {
    nrPeriodHyps_ = 5;
    nInitHyp_ = nrPeriodHyps_ * nrPhasesPerPeriod_; //Nr. of initial hypothesis
    mrs_natural p = 0, ph = 0;

    mrs_real gtPeriod;
    if(backtrace_) //noncausal or backtrace mode
    {
      gtPeriod = gtInitPeriod_;
      cerr << "Periods as ibi of given first 2 beats + others metrical related: ";
    }
    else //causal mode
    {
      gtPeriod = gtLastPeriod_;
      cerr << "Periods as ibi of given last 2 beats + others metrical related: ";
    }

    double periods[] = {
      (double) gtPeriod,
      (double) (mrs_natural) (gtPeriod * 2.0),
      (double) (mrs_natural) (gtPeriod * 0.5),
      (double) (mrs_natural) (gtPeriod * 3.0),
      (double) (mrs_natural) (gtPeriod * 0.333)
    };
    //assume gt period + (4) others metrical related (2x, 1/2x, 3x, 1/3x)
    for(int i = 0; i < nrPeriodHyps_; i++)
    {
      for(int h = i*nrPhasesPerPeriod_; h < (i*nrPhasesPerPeriod_)+nrPhasesPerPeriod_; h++)
      {
        beatHypotheses_(h, 0) = periods[p];
        beatHypotheses_(h, 1) = beatHypotheses_(ph, 1);
        //cout << "i-" << i <<"; h-" << h << "; p-" << p << ": " << ((60.0/periods[p])*(srcFs_ / hopSize_)) << "; ph: " << beatHypotheses_(h, 1) << endl;
        ph++;
      }
      p++;
      ph = 0;
    }

    //redefine tempo ranges to account for imposed values
    if(periods[3] > maxPeriod_) maxPeriod_ = ((mrs_natural) (periods[3] + 0.5));
    if(periods[4] < minPeriod_) minPeriod_ = ((mrs_natural) (periods[4] + 0.5));
    updControl(ctrl_maxPeriod_, maxPeriod_);
    updControl(ctrl_minPeriod_, minPeriod_);

    cerr << ((60.0/periods[0])*(srcFs_ / hopSize_)) << "; " << ((60.0/periods[1])*(srcFs_ / hopSize_)) << "; " <<
         ((60.0/periods[2])*(srcFs_ / hopSize_)) << "; " << ((60.0/periods[3])*(srcFs_ / hopSize_)) << "; " <<
         ((60.0/periods[4])*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
  }
  else if(strcmp(mode_.c_str(), "p_nr") == 0)
  {
    nrPeriodHyps_ = 5;
    nInitHyp_ = nrPeriodHyps_ * nrPhasesPerPeriod_; //Nr. of initial hypothesis
    mrs_natural p = 0, ph = 0;

    mrs_real gtPeriod;
    if(backtrace_) //noncausal or backtrace mode
    {
      gtPeriod = gtInitPeriod_;
      cerr << "Periods as ibi of given first 2 beats + others non-related: ";
    }
    else //causal mode
    {
      gtPeriod = gtLastPeriod_;
      cerr << "Periods as ibi of given last 2 beats + others non-related: ";
    }

    double periods[] = {
      (double) gtPeriod,
      (double) (mrs_natural) (gtInitPeriod_ * 1.8),
      (double) (mrs_natural) (gtPeriod * 1.2),
      (double) (mrs_natural) (gtPeriod * 2.3),
      (double) (mrs_natural) (gtPeriod * 0.7)
    };
    //assume gt period + (4) others metrical related (2x, 1/2x, 3x, 1/3x)
    for(int i = 0; i < nrPeriodHyps_; i++)
    {
      for(int h = i*nrPhasesPerPeriod_; h < (i*nrPhasesPerPeriod_)+nrPhasesPerPeriod_; h++)
      {
        beatHypotheses_(h, 0) = periods[p];
        beatHypotheses_(h, 1) = beatHypotheses_(ph, 1);
        //cout << "i-" << i <<"; h-" << h << "; p-" << p << ": " << ((60.0/periods[p])*(srcFs_ / hopSize_)) << "; ph: " << beatHypotheses_(h, 1) << endl;
        ph++;
      }
      p++;
      ph = 0;
    }

    //redefine tempo ranges to account for imposed values
    if(periods[3] > maxPeriod_) maxPeriod_ = ((mrs_natural) (periods[3] + 0.5));
    if(periods[4] < minPeriod_) minPeriod_ = ((mrs_natural) (periods[4] + 0.5));
    updControl(ctrl_maxPeriod_, maxPeriod_);
    updControl(ctrl_minPeriod_, minPeriod_);

    cerr << ((60.0/periods[0])*(srcFs_ / hopSize_)) << "; " << ((60.0/periods[1])*(srcFs_ / hopSize_)) << "; " <<
         ((60.0/periods[2])*(srcFs_ / hopSize_)) << "; " << ((60.0/periods[3])*(srcFs_ / hopSize_)) << "; " <<
         ((60.0/periods[4])*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
  }

  updControl(ctrl_beatHypotheses_, beatHypotheses_);
}

void
PhaseLock::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  //timeElapsed_ is constantly updated with the referee's next time frame
  timeElapsed_ = ctrl_tickCount_->to<mrs_natural>();

  //cout << "PLock: " << timeElapsed_ << "; bestScoreFact: " << triggerBestScoreFactor_ << endl;

  //cout << "PLock: " << timeElapsed_ << "; Ind: " << inductionTime_ << "; maxPer: " << maxPeriod_ << "; minPer: " << minPeriod_ << endl;

  //Output only defined just after induction time
  //until then output is undefined...
  for (o=0; o < onObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      out(o,t) = -1.0;
    }
  }

  //After the given induction stage, generate and return the correspondent beat hypotheses
  //[occuring in the beginning of the analysis or if triggered in "trigger induction" mode]
  //Calculate the N (nrPeriodHyps_) best (period_, phase) pairs + respective scores:
  triggerInduction_ = ctrl_triggerInduction_->to<mrs_bool>();
  if(triggerInduction_)
  {
    dumbInduction_ = ctrl_dumbInduction_->to<mrs_bool>();

    //cout << "TRIGGER @ " << timeElapsed_ << endl;

    cerr << "\nRequested Induction in \"" << mode_ << "\" mode at: " << (((timeElapsed_* hopSize_)-hopSize_/2) / srcFs_) << "s" << endl; //(" << timeElapsed_ << ")" << endl;

    //maxScore_ = calcGTNormScore(in);
    if(strcmp(mode_.c_str(), "2b") == 0 || strcmp(mode_.c_str(), "2b2") == 0) //gt period+phase (adjusted)
    {
      mrs_realvec gtHypotheses = readGTFile(ctrl_gtBeatsFile_->to<mrs_string>()); //process gt file
      handleGTHypotheses(in, out, ctrl_gtBeatsFile_->to<mrs_string>(), gtHypotheses);

      if(backtrace_)
      {
        //Period:
        out(0, 0) = gtInitPeriod_; //keep period as ibi from first 2 beats in gt file
        //Phase:
        out(0, 1) = gtInitPhase_; //first/second beat in gt file

        if(strcmp(mode_.c_str(), "2b") == 0)
        {
          if(gtAfter2ndBeat_)
            cerr << "Initial phase backtraced from second beat of given ground-truth file: ";
          else
            cerr << "Initial phase backtraced from first beat of given ground-truth file: ";
        }
        else if(strcmp(mode_.c_str(), "2b2") == 0)
        {
          if(gtAfter2ndBeat_)
            cerr << "Initial phase as second beat of given ground-truth file: ";
          else
            cerr << "Initial phase as first beat of given ground-truth file: ";
        }

        cerr << (((gtInitPhase_ * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
        cerr << "Ground-truth period: " << ((60.0/gtInitPeriod_)*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
      }
      else //no backtrace (causal mode)
      {
        //Period:
        out(0, 0) = gtLastPeriod_; //period adjusted from ibi given by first 2 beats in gt file
        //Phase:
        out(0, 1) = gtLastPhase_; //adjusted from first/second beat in gt file

        if(strcmp(mode_.c_str(), "2b") == 0)
        {
          if(gtAfter2ndBeat_)
            cerr << "Initial phase adjusted from second beat after induction time, of given ground-truth file: ";
          else
            cerr << "Initial phase adjusted from first beat after induction time, of given ground-truth file: ";

          cerr << (((gtLastPhase_ * hopSize_) - adjustment_) / srcFs_) << "s" << endl;
          cerr << "Ground-truth period (adjusted): " << ((60.0/gtLastPeriod_)*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
        }
        else if(strcmp(mode_.c_str(), "2b2") == 0)
        {
          cerr << "Initial phase as first beat after induction time, of given ground-truth file: ";
          cerr << (((gtLastPhase_ * hopSize_) - adjustment_) / srcFs_) << "s" << "[" << gtLastPhase_ << "]" << endl;
          cerr << "Ground-truth period: " << ((60.0/gtLastPeriod_)*(srcFs_ / hopSize_)) << " (BPMs)" << endl;
        }
        //cout << "CALC Initial phase: " << gtInitPhase_ << "(" << (((gtInitPhase_ * hopSize_) - adjustment_) / srcFs_) << ")" << endl;
      }

      //score
      out(0, 2) = gtScore_; //initialScore

      //cout << "gtLPe: " << gtLastPeriod_ << "(" << out(0,0) << ")" << "gtLPh: " << gtLastPhase_ << "(" << out(0,1) << ")" <<
      //	"gtSc: " << gtScore_ << "(" << out(0,3) << ")" << endl;
    }
    else if(strcmp(mode_.c_str(), "1b") == 0
            || strcmp(mode_.c_str(), "1b1") == 0 ) //gt phase
    {
      mrs_realvec gtHypotheses = readGTFile(ctrl_gtBeatsFile_->to<mrs_string>()); //process gt file
      handleGTHypotheses(in, out, ctrl_gtBeatsFile_->to<mrs_string>(), gtHypotheses);
    }
    else if((strcmp(mode_.c_str(), "p") == 0) || (strcmp(mode_.c_str(), "p_mr") == 0)
            || (strcmp(mode_.c_str(), "p_nr") == 0))
    {
      mrs_realvec gtHypotheses = readGTFile(ctrl_gtBeatsFile_->to<mrs_string>()); //process gt file
      handleGTHypotheses(in, out, ctrl_gtBeatsFile_->to<mrs_string>(), gtHypotheses);

      //force gt period (given by gt file - ibi of first 2 beats) in beatHypotheses vector
      forceInitPeriods(mode_);

      //run as regular induction
      regularFunc(in, out);
    }
    else if(strcmp(mode_.c_str(), "regular") == 0)
      regularFunc(in , out);

    cerr << "===================FINISH INDUCTION=====================" << endl;

    /*
    ostringstream oss;
    fstream outStream;
    oss << "phaselock_test.txt";
    for(int i = 0; i < in.getCols(); i++)
    {
    	if(i == 0)
    	{
    		outStream.open(oss.str().c_str(), ios::out|ios::trunc);
    		outStream << in(i) << endl;
    		outStream.close();
    	}
    	else
    	{
    		outStream.open(oss.str().c_str(), ios::out|ios::app);
    		outStream << in(i) << endl;
    		outStream.close();
    	}
    }
    */
  }

  //MATLAB_PUT(in, "Flux_FlowThrued");
  //MATLAB_PUT(out, "PhaseLockOut");
  //MATLAB_EVAL("hold on;");
  //MATLAB_EVAL("plot(Flux_FlowThrued), g");
  //MATLAB_EVAL("FluxFlowTS = [FluxFlowTS, Flux_FlowThrued];");
}
