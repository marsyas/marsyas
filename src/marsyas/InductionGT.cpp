/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "InductionGT.h"
using namespace std;
using namespace Marsyas;

InductionGT::InductionGT(mrs_string name):MarSystem("InductionGT", name)
{
  addControls();
  t_ = 0;
  maxScore_ = -100.0;
}

InductionGT::InductionGT(const InductionGT& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_sourceFile_ = getctrl("mrs_string/sourceFile");
  ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");

  t_ = a.t_;
  ibi_ = a.ibi_;
  beatTime1_ = a.beatTime1_;
  beatTime2_ = a.beatTime2_;
  maxScore_ = a.maxScore_;
  phase_ = a.phase_;
}

InductionGT::~InductionGT()
{
}

MarSystem*
InductionGT::clone() const
{
  return new InductionGT(*this);
}

void
InductionGT::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_string/sourceFile", "input.txt", ctrl_sourceFile_);
  setctrlState("mrs_string/sourceFile", true);
  addctrl("mrs_natural/inductionTime", 0, ctrl_inductionTime_);
  setctrlState("mrs_natural/inductionTime", true);
  addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);

}

void
InductionGT::myUpdate(MarControlPtr sender)
{
  (void)sender;	 // avoid unused parameter warning 
  
  MRSDIAG("InductionGT.cpp - InductionGT:myUpdate");

  sourceFile_ = ctrl_sourceFile_->to<mrs_string>().c_str();

  ctrl_onSamples_->setValue(3, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();
}


void
InductionGT::myProcess(realvec& in, realvec& out)
{
  t_++;
  mrs_natural o,t;

  //Output only defined just after induction time
  //until then output is undefined...
  for (o=0; o < onObservations_; o++)
  {
	for (t = 0; t < onSamples_; t++)
	{
	  out(o,t) = -1.0;
	}
  }

  if(t_ == inductionTime_)
  {
	ostringstream oss;
	oss << ctrl_sourceFile_->to<mrs_string>();
	inStream_.open(oss.str().c_str());

	getline (inStream_, line_);

	//for beat groundtruth files (in line, separated by spaces):
	beatTime1_ = strtod(line_.substr(0, line_.find(" ")).c_str(), NULL);
	beatTime2_ = strtod(line_.substr(line_.find(" ")+1, line_.find(" ", 1)).c_str(), NULL);
	//==========================================================
	//cout << "beat1: " << beatTime1_ << "; beat2: " << beatTime2_ << endl;

	//if to assure that it could read from file => beatFile in column (ifnot => beatFile in row)
	if(beatTime1_ == beatTime2_ || beatTime2_ == 0 || beatTime2_ > 40)
	{
	  beatTime1_ = atof(line_.c_str());
	  getline (inStream_, line_);
	  beatTime2_ = atof(line_.c_str());
	}

	ibi_ = (mrs_natural) (((beatTime2_ - beatTime1_) * srcFs_ / hopSize_));
	phase_ = (mrs_natural) (beatTime1_ * srcFs_ / hopSize_);

	//Retrieve best score within induction window -> for starting score normalized with remaining analysis
	for(mrs_natural i = 0; i <= inObservations_; i++)
	{
	  if(in(i, 2) > maxScore_)
		maxScore_ = in(i, 2); //max score from PhaseLock (if actual tempo would be measured)
	}

	//sum_ = 10.0;
	cout << "\nInduction replaced by ground-truth file at: " << oss.str().c_str() << endl;
	//cout << "BEAT1: " << beatTime1_ << "(" << phase_ << "); BEAT2: " << beatTime2_ << "; IBI: " << ibi_ << "; SCORE:" << maxScore_ << endl;

	//Period:
	out(0, 0) = ibi_; //period in frames
	//Phase:
	out(0, 1) = phase_; //phase in frames
	//score
	out(0, 2) = 2*maxScore_; //initialScore

	inStream_.close();
  }

  //MATLAB_PUT(out, "InductionGT");
}
