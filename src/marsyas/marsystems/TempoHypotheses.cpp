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

#include "TempoHypotheses.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

TempoHypotheses::TempoHypotheses(mrs_string name):MarSystem("TempoHypotheses", name)
{
  addControls();
  timeElapsed_ = 0;
  foundPeriods_ = false;
  foundPhases_ = false;
}

TempoHypotheses::TempoHypotheses(const TempoHypotheses& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_nPhases_ = getctrl("mrs_natural/nPhases");
  ctrl_nPeriods_ = getctrl("mrs_natural/nPeriods");
  ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_dumbInduction_ = getctrl("mrs_bool/dumbInduction");
  ctrl_dumbInductionRequest_ = getctrl("mrs_bool/dumbInductionRequest");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
  ctrl_triggerInduction_ = getctrl("mrs_bool/triggerInduction");
  ctrl_accSize_ = getctrl("mrs_natural/accSize");
  ctrl_maxPeriod_ = getctrl("mrs_natural/maxPeriod");
  ctrl_minPeriod_ = getctrl("mrs_natural/minPeriod");

  foundPeriods_ = a.foundPeriods_;
  foundPhases_ = a.foundPhases_;
  dumbInductionRequest_ = a.dumbInductionRequest_;
  triggerInduction_ = a.triggerInduction_;
}

TempoHypotheses::~TempoHypotheses()
{
}

MarSystem*
TempoHypotheses::clone() const
{
  return new TempoHypotheses(*this);
}

void
TempoHypotheses::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/nPhases", 1, ctrl_nPhases_);
  setctrlState("mrs_natural/nPhases", true);
  addctrl("mrs_natural/nPeriods", 1, ctrl_nPeriods_);
  setctrlState("mrs_natural/nPeriods", true);
  addctrl("mrs_natural/inductionTime", -1, ctrl_inductionTime_);
  setctrlState("mrs_natural/inductionTime", true);
  addctrl("mrs_natural/hopSize", 1, ctrl_hopSize_);
  addctrl("mrs_real/srcFs", 1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_bool/dumbInduction", false, ctrl_dumbInduction_);
  addctrl("mrs_bool/dumbInductionRequest", false, ctrl_dumbInductionRequest_);
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_bool/triggerInduction", false, ctrl_triggerInduction_);
  setctrlState("mrs_bool/triggerInduction", true);
  addctrl("mrs_natural/accSize", -1, ctrl_accSize_);
  addctrl("mrs_natural/maxPeriod", -1, ctrl_maxPeriod_);
  setctrlState("mrs_natural/maxPeriod", true);
  addctrl("mrs_natural/minPeriod", -1, ctrl_minPeriod_);
  setctrlState("mrs_natural/minPeriod", true);
}

void
TempoHypotheses::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("TempoHypotheses.cpp - TempoHypotheses:myUpdate");

  nPhases_ = ctrl_nPhases_->to<mrs_natural>();
  nPeriods_ = ctrl_nPeriods_->to<mrs_natural>();
  inductionSize_ = ctrl_inductionTime_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();
  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  triggerInduction_ = ctrl_triggerInduction_->to<mrs_bool>();
  accSize_ = ctrl_accSize_->to<mrs_natural>();
  dumbInductionRequest_ = ctrl_dumbInductionRequest_->to<mrs_bool>();
  maxPeriod_ = ctrl_maxPeriod_->to<mrs_natural>();
  minPeriod_ = ctrl_minPeriod_->to<mrs_natural>();

  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_natural/onSamples", 3);
  setctrl("mrs_natural/onObservations", nPhases_ * nPeriods_);
}


void
TempoHypotheses::myProcess(realvec& in, realvec& out)
{
  //timeElapsed_ is constantly updated with the referee's next time frame
  timeElapsed_ = ctrl_tickCount_->to<mrs_natural>();

  //cout << "THyp: " << timeElapsed_ << "; Ind: " << inductionSize_ << "; accSize: " << accSize_
  //	<< "; maxPer: " << maxPeriod_ << "; minPer: " << minPeriod_ << endl;

  triggerInduction_ = ctrl_triggerInduction_->to<mrs_bool>();
  if(triggerInduction_)
  {
    mrs_natural maxPeriod = 0;

    //reset flags
    foundPeriods_ = false;
    foundPhases_ = false;
    if(!dumbInductionRequest_) //if not in dumb induction mode (requested by user)
    {
      //retrieve Max Period Peak and check if found periods and/or phases
      mrs_real maxPeriodPeak = 0.0;
      for (int i=0; i < nPeriods_; i++)
      {
        if(in(0, 2*i+1) > 1) //if found any period (period > 1 because it may appear as decimals meaning 0)
          foundPeriods_ = true;

        if(in(0, 2*i) > maxPeriodPeak)
          maxPeriodPeak = in(0, 2*i);

        //keep maximum period
        if(in(0, 2*i+1) > maxPeriod)
          maxPeriod = (mrs_natural)in(0, 2*i+1);

        int z = 0;
        //cout << "TH-Phases: " << endl;
        for (int j = (i * nPhases_); j < ((i+1) * nPhases_); j++)
        {
          //cout << "i: " << in(1, 2*z+1) << "; ";
          if(in(1, 2*z+1) > 0) //if found any phases
            foundPhases_ = true;
          //foundPhases_ = false;
          z++;
        }
        //cout << endl;
      }

      //cout << "FoundPer: " << foundPeriods_ << "; foundPh: " << foundPhases_ << endl;

      if(foundPeriods_) //if found periods
      {
        for (int i=0; i < nPeriods_; i++)
        {
          int z = 0;
          for (int j = (i * nPhases_); j < ((i+1) * nPhases_); j++)
          {
            out(j, 0) = in(0, 2*i+1); //Periods
            out(j, 1) = in(1, 2*z+1); //Phases
            out(j, 2) = in(0, 2*i);// / maxPeriodPeak; //Normalized period peak magnitudes

            z++;
          }
        }
      }
    }

    //if no periods found or in dumb induction mode (requested by user)
    if(!foundPeriods_ || dumbInductionRequest_)
    {
      //Manual assorted values for filling BPM hypotheses vector when none are generated
      int manualBPMs_[] = {120, 60, 240, 100, 160, 200, 80, 140, 180, 220, 150};

      if(!foundPeriods_ && !dumbInductionRequest_)
        cerr << "\nUnable to find salient periodicities within the given induction window..." << endl;
      if(dumbInductionRequest_)
        cerr << "\nDumb Induction Mode..." << endl;

      cerr << "...Replacing induction with the following BPMs: ";

      mrs_natural assignedPerCount = 0;
      for (unsigned int i=0; i < (sizeof(manualBPMs_) / sizeof(int)); i++)
      {
        if(assignedPerCount == nPeriods_) break;

        mrs_natural manualPeriod = (mrs_natural) (((mrs_real) 60 / (manualBPMs_[i] * hopSize_)) * (srcFs_));

        //cout << i << "-> manBPM: " << manualBPMs_[i] << "[" << manualPeriod << "] maxPer: "
        //	<< maxPeriod_ << "; minPer: " << minPeriod_ << "; ASS: " << assignedPerCount << endl;

        //assure that the chosen manual periods are within the user-defined BPM range
        if(manualPeriod >= minPeriod_ && manualPeriod <= maxPeriod_)
        {
          cerr << manualBPMs_[i] << "; ";

          int z = 0;
          for (int j = (assignedPerCount * nPhases_); j < ((assignedPerCount+1) * nPhases_); j++)
          {
            out(j, 0) = manualPeriod; //Periods
            out(j, 1) = in(1, 2*z+1); //Phases
            out(j, 2) = 1.0; //equal (max) peak sizes to all manual periods
            z++;

            if(out(j,0) > maxPeriod) //save maximum manual period
              maxPeriod = (mrs_natural) out(j,0);
          }

          assignedPerCount++;
        }
      }

      //request dumbInduction to PhaseLock
      ctrl_dumbInduction_->setValue(true);
    }

    if(!foundPhases_) //if no phases found
    {
      //cerr << "\nUnable to find potential phases (onsets) within the given induction window..." << endl;
      //cerr << "...Assuming the maximum nr. of possible phases." << endl;

      //calculate minimum spacing between possible phases
      mrs_natural spacing = (mrs_natural) ceil(((mrs_real)maxPeriod / (mrs_real)nPhases_));
      //start phases on the analysis start point of the given induction window
      mrs_natural accBeginning = (accSize_-1-inductionSize_);

      //fill phases vector
      mrs_realvec phases(nPhases_);
      mrs_natural index = 0;
      for(int ph = accBeginning; ph <= accBeginning+maxPeriod+spacing; ph += spacing)
      {
        if(index == nPhases_) break;

        phases(index) = ph; //Phases
        index++;
      }

      for (int i=0; i < nPeriods_; i++)
      {
        int z = 0;
        for (int j = (i * nPhases_); j < ((i+1) * nPhases_); j++)
        {
          out(j, 1) = phases(z); //Phases
          z++;
        }
      }
      //MATLAB_PUT(in, "BeatData");
      //MATLAB_PUT(out, "TempoHypotheses");
    }

    //MATLAB_PUT(out, "TempoHypotheses2");
    //MATLAB_PUT(in, "BeatData");
    //MATLAB_EVAL("plot(BeatData)");
  }

  /*
  //FOR TESTING ONSETS IN REPEATED INDUCTION=========================
  MATLAB_PUT((mrs_natural)triggerInduction_, "Induction_flag");
  MATLAB_EVAL("Induction = Induction2;");
  MATLAB_EVAL("Induction(end) = Induction_flag*max(PeakerOnset_in);");
  */
}








