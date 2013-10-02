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

#include "OnsetTimes.h"
#include "../common_source.h"
#include <algorithm>
#include <functional>

using namespace std;
using namespace Marsyas;

OnsetTimes::OnsetTimes(mrs_string name):MarSystem("OnsetTimes", name)
{
  addControls();
  timeElapsed_ = 0;
  count_ = 0;
  lastInductionTime_ = 0;
  maxCount_ = 0;
}

OnsetTimes::OnsetTimes(const OnsetTimes& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_n1stOnsets_ = getctrl("mrs_natural/n1stOnsets");
  ctrl_lookAheadSamples_ = getctrl("mrs_natural/lookAheadSamples");
  ctrl_nPeriods_ = getctrl("mrs_natural/nPeriods");
  ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
  ctrl_accSize_ = getctrl("mrs_natural/accSize");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
  ctrl_triggerInduction_ = getctrl("mrs_bool/triggerInduction");

  count_ = a.count_;
  lastInductionTime_ = a.lastInductionTime_;
  triggerInduction_ = a.triggerInduction_;
  size_ = a.size_;
  maxCount_ = a.maxCount_;
  phasesRaw_ = a.phasesRaw_;
}

OnsetTimes::~OnsetTimes()
{
}

MarSystem*
OnsetTimes::clone() const
{
  return new OnsetTimes(*this);
}

void
OnsetTimes::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/n1stOnsets", 1, ctrl_n1stOnsets_);
  setctrlState("mrs_natural/n1stOnsets", true);
  addctrl("mrs_natural/lookAheadSamples", 1, ctrl_lookAheadSamples_);
  addctrl("mrs_natural/nPeriods", 1, ctrl_nPeriods_);
  setctrlState("mrs_natural/nPeriods", true);
  addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
  setctrlState("mrs_natural/inductionTime", true);
  addctrl("mrs_natural/accSize", -1, ctrl_accSize_);
  setctrlState("mrs_natural/accSize", true);
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_bool/triggerInduction", false, ctrl_triggerInduction_);
  setctrlState("mrs_bool/triggerInduction", true);
}

void
OnsetTimes::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("OnsetTimes.cpp - OnsetTimes:myUpdate");
  n_ = ctrl_n1stOnsets_->to<mrs_natural>();
  nPeriods_ = ctrl_nPeriods_->to<mrs_natural>();

  //if the nr of requested bpm hypotheses is bigger
  //than the requested phases we need to fill the rest of the vector with
  //0s -> to avoid crash in tempohypotheses Fanout
  size_ = 2 * max(n_, nPeriods_);
  setctrl("mrs_natural/onSamples",  size_);

  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  inductionSize_ = ctrl_inductionTime_->to<mrs_natural>();
  accSize_ = ctrl_accSize_->to<mrs_natural>();
  triggerInduction_ = ctrl_triggerInduction_->to<mrs_bool>();
}

void
OnsetTimes::delSurpassedOnsets()
{
  //clean surpassed onsets given current induction window
  for (int i=0; i < n_; i++)
  {
    mrs_real onset = phasesRaw_(i);
    if(onset > 0) //ignore empty bins
    {
      //cout << "t: " << timeElapsed_ << "====> i: " << i << "-> " << onset << ";";
      mrs_real onsetAdj = onset + (accSize_ - 1 - timeElapsed_);
      //if onset still inside current induction window (to be sure)
      if(onsetAdj < (accSize_ -1 - inductionSize_) || onsetAdj > (accSize_ - 1))
      {
        //cout << "(" << onsetAdj << ") [" << (accSize_ -1 - inductionSize_) << "; " << (accSize_ - 1) << "]" << endl;
        phasesRaw_(i) = 0.0; //if not => delete onset
        maxCount_--; //subtract deleted values to realvec

        if(count_ > maxCount_) //assure that count never surpasses maximum count (which may happen when one phase deleted)
          count_ = maxCount_;
      }
      //else cout << endl;
    }
  }

  //cout << "UNSORTED RAW-> maxCount: " << maxCount_ << "; Count: " << count_ << endl;
  //for (int i=0; i < n_; i++)
  //{
  //	cout << i << "-> " << phasesRaw_(i) << "; ";
  //}

  //sort the whole realvec in descending order to take eventual initial zeros
  sort(phasesRaw_.getData(), phasesRaw_.getData()+n_, greater<int>());
  //sort back to ascending order
  sort(phasesRaw_.getData(), phasesRaw_.getData()+maxCount_);
}

void
OnsetTimes::myProcess(realvec& in, realvec& out)
{
  //timeElapsed_ is constantly updated with the referee's next time frame
  timeElapsed_ = ctrl_tickCount_->to<mrs_natural>();

  //at every induction step -> update onset positions according to offset
  if(triggerInduction_)
  {

    //cout << "OnTimes-> Induction at: " << timeElapsed_ << "; accSize: " << accSize_ << "; min: "
    //	<< (accSize_ -1 - inductionSize_) << "; max: " << (accSize_ - 1) << "; phS: " << phasesRaw_.getCols() << endl;

    //cout << "BEGINING RAW-> maxCount: " << maxCount_ << "; Count: " << count_ << endl;
    //for (int i=0; i < n_; i++)
    //{
    //	cout << i << "-> " << phasesRaw_(i) << "; ";
    //}


    //delSurpassedOnsets();


    //cout << "SORTED RAW-> maxCount: " << maxCount_ << "; Count: " << count_ << endl;
    //for (int i=0; i < n_; i++)
    //{
    //	cout << i << "-> " << phasesRaw_(i) << "; ";
    //}


    mrs_realvec phasesAdj(size_);
    //Adjust given phases (onsets) to current induction window:
    for (int i=0; i < n_; i++)
    {
      mrs_real onset = phasesRaw_(i);
      //cout << "O: " << onset << "; ";
      if(onset > 0) //ignore empty bins
      {
        //cout << "i: " << i << "-> " << onset << " (";
        phasesAdj(0, (2*i)+1) = onset + (accSize_ - 1 - timeElapsed_);
        phasesAdj(0, 2*i) = 1.0;

        //cout << "(" << phasesAdj(0, (2*i)+1) << "); ";
      }

    }
    //cerr << endl;

    //out = phasesAdj;

    /*
    cout << "END-> maxCount: " << maxCount_ << endl;
    for (int i=0; i < n_; i++)
    {
    	cout << i << "-> " << phasesAdj(0, (2*i)+1) << "; ";
    }
    */
    out = phasesAdj;
  }

  //cout << "OTime: " << timeElapsed_ << endl;

  lookAhead_ = ctrl_lookAheadSamples_->to<mrs_natural>();

  mrs_natural inc = 0; //nr. of first ignored onsets
  if((timeElapsed_ - lookAhead_) > 0 && in(0,0) == 1.0) //avoid onset at frame 0
  {
    if(phasesRaw_.getCols() < n_)
      phasesRaw_.stretch(n_);

    //cout << "count: " << count_ << "; inc: " << inc << "; n: " << n_
    //	<< "; t-l: " << (timeElapsed_-lookAhead_) << "; PR: " << (phasesRaw_(0, (count_-inc-1)) + 5) << endl;
    //cout << "LOOKAHEAD: " << lookAhead_ << endl;
    //cout << "ONSET AT: " << timeElapsed_ << endl;
    //if task isn't still done && (first peak || peak distance at least 5 frames from last peak)
    //if(count_ == inc || (count_ > inc && count_ < n_ + inc && ((timeElapsed_-lookAhead_)+(accSize_-1 - inductionTime_)) > out(0, 2*(count_-inc)-1) + 5))
    //if(count_ == inc || (count_ > inc && count_ < n_ + inc && ((timeElapsed_-lookAhead_)+(accSize_-1 - inductionSize_)) > out(0, 2*(count_-inc)-1) + 5))
    if(count_ == inc || (count_ > inc && count_ < n_ + inc && ((timeElapsed_-lookAhead_) > phasesRaw_(0, (count_-inc-1)) + 5)))
    {
      //cout << "Out Last Arg: " << out(0, 2*(count_-inc)+1) << " Current Arg: " << timeElapsed_ - lookAhead_ << endl;
      //out(0, 2*(count_-inc)) = in(0,0);

      //onsetTime equals to peakerTime within current induction step (curInductionTime_-lookAhead_)
      //+ difference between last point in the
      //accumulator/ShiftInput (accSize_-1) and the considered inductionTime
      //out(0, 2*(count_-inc)+1) = (timeElapsed_-lookAhead_)+(accSize_-1 - inductionSize_);
      //out(0, 2*(count_-inc)+1) = ((timeElapsed_-lastInductionTime_) - lookAhead_)+(accSize_-1 - inductionSize_);
      //out(0, 2*(count_-inc)+1) = (curInductionTime_-lookAhead_)+(accSize_-1 - inductionTime_);
      //out(0, 2*(count_-inc)+1) = ((timeElapsed_-lastInductionTime_) - lookAhead_) + (accSize_-1);
      //out(0, 2*(count_-inc)+1) = (timeElapsed_-lookAhead_);

      //use phases_ realvec
      phasesRaw_(0, (count_-inc)) = (timeElapsed_-lookAhead_);

      //cout << "t: " << timeElapsed_ << "|" << (timeElapsed_-lookAhead_) << "- " << ((((timeElapsed_-lookAhead_) * 512.0) - 512.0/2) / 44100.0) << "(" << count_ << ") Onset at: " << (timeElapsed_-lookAhead_)+(862-1 - inductionSize_)
      //	<< "(" << phasesRaw_(0, (count_-inc)) << ")" << endl;

      count_++;
      if(count_ > maxCount_) //update maxCount for keeping max nr. of phases from all induction windows
        maxCount_ = count_;
    }
    else if(count_ >= n_) //if onsets' realvec is full and new onsets arrive check if the older onsets have already been surpassed
    {
      delSurpassedOnsets();

      //cout << "SORTED RAW-> maxCount: " << maxCount_ << "; Count: " << count_ << endl;
      //for (int i=0; i < n_; i++)
      //{
      //	cout << i << "-> " << phasesRaw_(i) << "; ";
      //}

      //add new onset
      phasesRaw_(0, (count_-inc)) = (timeElapsed_-lookAhead_);

      count_++;
      if(count_ > maxCount_) //update maxCount for keeping max nr. of phases from all induction windows
        maxCount_ = count_;
    }
  }
}
