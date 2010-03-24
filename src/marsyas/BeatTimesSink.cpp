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

#include "BeatTimesSink.h"
#include <fstream>
#include <string.h>

using namespace std;
using namespace Marsyas;

BeatTimesSink::BeatTimesSink(string name):MarSystem("BeatTimesSink", name)
{
  addControls();
  ibiBPM_ = 0.0;
  ibiBPMSum_ = 0.0;
  beatCount_ = 0;
  t_ = 0;
  inc_ = 0; //initial beat counting...
  nonCausal_ = true;
  lastIbi_ = 0.0;
  initialOut_ = true;
  initialOut2_ = true;
  initialOut3_ = true;
}

BeatTimesSink::BeatTimesSink(const BeatTimesSink& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_winSize_ = getctrl("mrs_natural/winSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_destFileName_ = getctrl("mrs_string/destFileName");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
  ctrl_tempo_ = getctrl("mrs_real/tempo");
  ctrl_adjustment_ = getctrl("mrs_natural/adjustment");
  ctrl_bestFinalAgentHistory_= getctrl("mrs_realvec/bestFinalAgentHistory");
  ctrl_soundFileSize_= getctrl("mrs_natural/soundFileSize");
  ctrl_nonCausal_ = getctrl("mrs_bool/nonCausal");
  
  ibiBPM_ = a.ibiBPM_;
  beatCount_ = a.beatCount_;
  ibiBPMSum_ = a.ibiBPMSum_;
  inc_ = a.inc_;
  ibiBPMVec_ = a.ibiBPMVec_;
  initialOut_ = a.initialOut_;
  initialOut2_ = a.initialOut2_;
  initialOut3_ = a.initialOut3_;
}

BeatTimesSink::~BeatTimesSink()
{
}

MarSystem* 
BeatTimesSink::clone() const 
{
  return new BeatTimesSink(*this);
}

void 
BeatTimesSink::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_natural/winSize", -1, ctrl_winSize_);
  setctrlState("mrs_natural/winSize", true);
  addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_string/destFileName", "output", ctrl_destFileName_);
  addctrl("mrs_string/mode", "beats+tempo", ctrl_destFileName_);
  setctrlState("mrs_string/mode", true);
  addctrl("mrs_real/tempo", 80.0, ctrl_tempo_);
  addctrl("mrs_natural/adjustment", 0, ctrl_adjustment_);
  setctrlState("mrs_natural/adjustment", true);
  addctrl("mrs_realvec/bestFinalAgentHistory", realvec(), ctrl_bestFinalAgentHistory_);
  setctrlState("mrs_realvec/bestFinalAgentHistory", true);
  addctrl("mrs_natural/soundFileSize", 0, ctrl_soundFileSize_);
  setctrlState("mrs_natural/soundFileSize", true);
  addctrl("mrs_bool/nonCausal", false, ctrl_nonCausal_);
  setctrlState("mrs_bool/nonCausal", true);
}

void
BeatTimesSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("BeatTimesSink.cpp - BeatTimesSink:myUpdate");
  MarSystem::myUpdate(sender);

  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  winSize_ = ctrl_winSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();
  
  adjustment_ = (mrs_real) ctrl_adjustment_->to<mrs_natural>();

  mode_ = ctrl_mode_->to<mrs_string>();

  ibiBPMVec_.create(1000);

  bestFinalAgentHistory_ = ctrl_bestFinalAgentHistory_->to<mrs_realvec>();
  soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();
  nonCausal_ = ctrl_nonCausal_->to<mrs_bool>();
}

mrs_realvec
BeatTimesSink::addMedianVector(mrs_real ibiBPM)
{
	mrs_bool bigger = false;
	mrs_realvec tmp(beatCount_);
	for(mrs_natural j = 0; j < beatCount_-1; j++)
	{
		//copy all
		tmp(j) = ibiBPMVec_(j);
	}

	for(mrs_natural i = beatCount_-2; i >=0 ; i--)
	{
		if(ibiBPM > ibiBPMVec_(i))
		{
			ibiBPMVec_(i+1) = ibiBPM;

			for(mrs_natural z = i+1; z < beatCount_-1; z++)
				ibiBPMVec_(z+1) = tmp(z);

			bigger = true;
			break;
		}
	}

	if(!bigger)
	{
		ibiBPMVec_(0) = ibiBPM;
		for(mrs_natural z = 0; z < beatCount_-1; z++)
			ibiBPMVec_(z+1) = tmp(z);
	}

	return ibiBPMVec_;
}

void 
BeatTimesSink::myProcess(realvec& in, realvec& out)
{
	//Frame (tick) counter: (updated from BeatReferee's next time frame -1)
	t_ = ctrl_tickCount_->to<mrs_natural>()-1;

	//cout << "BSink: " << t_ << endl;

	//FlowThru input
	out = in;

	if(!nonCausal_)
	{
		//If Beat:
		if(in(0,0) == 1.0) //only count after 1st beat
		{
			//For writing only beats after inc_ (to avoid writing first unconsistent beats)
			if(beatCount_ >= inc_)
			{
				//Output BeatTime (in Seconds) = ((t_ (inFrames) * hopSize_) - adjustment) / srcFs_
				srcFs_ = ctrl_srcFs_->to<mrs_real>();
				beatTime_ = ((t_ * hopSize_) - adjustment_) / srcFs_;

				//cout << "Beat at: " << beatTime_ << " (s)" << endl;
				
				//after the 1st beat calculate ibi
				if(!initialOut_ || !initialOut2_ || !initialOut3_)
					ibiBPM_ = (60.0 / (beatTime_ - lastBeatTime_)); //inter-beat-interval (in BPMs)

				cout << "Beat at: " << beatTime_ << " (s) - " << ibiBPM_ << " (BPMs)" << endl;

				fstream outStream;
				fstream outStream2;
				fstream outStream3;

				if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					ostringstream oss;

					//initially a new output file is created
					//(If the file exists, its content is deleted and it is treated as a new file)
					if(initialOut_)
					{
						oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
						outStream.open(oss.str().c_str(), ios::out|ios::trunc);
						outStream << beatTime_ << endl;
						outStream.close();
						initialOut_ = false;
					}

					//output is appended in the end of the file
					else 
					{
						oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
						outStream.open(oss.str().c_str(), ios::out|ios::app);
						
						outStream << beatTime_ << " " << ibiBPM_ << endl;
						//outStream << beatTime_ << endl;
					}
				}

				if((strcmp(mode_.c_str(), "meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					ostringstream oss2;
					if(initialOut2_)
					{
						oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
						
						outStream2.open(oss2.str().c_str(), ios::out|ios::trunc);
						outStream2.close();
						initialOut2_ = false;
					}

					if(beatCount_ > inc_)
					{
						ibiBPMSum_ += ibiBPM_;
						mrs_natural output = (mrs_natural) ((ibiBPMSum_ / (beatCount_-inc_)) + 0.5);

						oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
						outStream2.open(oss2.str().c_str());
						outStream2 << output << endl;
						outStream2.close();
					}
				}

				if((strcmp(mode_.c_str(), "medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					ostringstream oss3;
					if(initialOut3_)
					{
						oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
						outStream3.open(oss3.str().c_str(), ios::out|ios::trunc);
						outStream3.close();
						initialOut3_ = false;
					}

					if(beatCount_ > inc_)
					{
						addMedianVector(ibiBPM_);

						mrs_natural output;
						output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
						tempo_ = output;
						ctrl_tempo_->setValue(tempo_, NOUPDATE);

						/*
						if(beatCount_ % 2 == 0) 
						{
							output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
							tempo_ = output;
							ctrl_tempo_->setValue(tempo_, NOUPDATE);
						}
						else
						{
							output = (mrs_natural) ((ibiBPMVec_((mrs_natural)floor((beatCount_ / 2.0))) 
								+ ibiBPMVec_((mrs_natural)ceil((beatCount_ / 2.0))) / 2.0) +0.5);
							tempo_ = output;
							ctrl_tempo_->setValue(tempo_, NOUPDATE);
						}
						*/

						oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
						outStream3.open(oss3.str().c_str());
						outStream3 << output << endl;
						outStream3.close();

						//MATLAB_PUT(ibiBPMVec_, "IBIVector1");
					}

					else if(beatCount_ == 2) //if only two beats => equal to ibi
					{
						oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
						outStream.open(oss3.str().c_str());
						outStream << (mrs_natural) (ibiBPM_ + 0.5); //(+0.5 for round integer)
						outStream.close();
						tempo_ = ibiBPM_;
						ctrl_tempo_->setValue(tempo_, NOUPDATE);
					}
				}
				

			//MATLAB_PUT(ibiBPMVec_, "IBIVector");
			lastBeatTime_ = beatTime_;
			}
			beatCount_ ++;
		}
	}
	if(nonCausal_)
	{
		if(t_ == soundFileSize_-1) //[! -1 for acouting on time of timing reset on backtrace mode]
		{
			//if no beats detected [to avoid writing beatTimes output file]
			if(bestFinalAgentHistory_(0) >= 0.0)
			{
				//reset beatCount, ibiBPMSum, and ibiBPMVec, from causal analysis
				for(mrs_natural i = 0; i < beatCount_; i++)
					ibiBPMVec_(i) = 0.0;
				beatCount_ = 0;
				ibiBPMSum_ = 0.0;
				
				//first beatTime:
				beatTime_ = ((bestFinalAgentHistory_(0) * hopSize_) - adjustment_) / srcFs_;
				lastBeatTime_ = beatTime_;
				beatTimeTmp_ = beatTime_;
				beatCount_++;

				fstream outStream;
				ostringstream oss;
				if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
					outStream.open(oss.str().c_str(), ios::out|ios::trunc);
					outStream << beatTime_ << endl;
				}

				//remaining beatTimes
				for(int i = 1; i < bestFinalAgentHistory_.getCols(); i++)
				{
					beatTime_ = ((bestFinalAgentHistory_(i) * hopSize_) - adjustment_) / srcFs_;
					
					mrs_real ibi = (beatTime_ - lastBeatTime_);

					mrs_real nextIbi = 0;
					mrs_real nextBeatTime = 0;
					if(i < bestFinalAgentHistory_.getCols()-1)
					{
						nextBeatTime = ((bestFinalAgentHistory_(i+1) * hopSize_) - adjustment_) / srcFs_;
						nextIbi = (nextBeatTime - beatTimeTmp_);
					}
					
					else //if last beat always write
					{	
						ibiBPM_ = (60.0 / ibi); //inter-beat-interval (in BPMs)
						outStream << beatTime_ << " " << ibiBPM_ << endl;

						addMedianVector(ibiBPM_); //for calculating medianTempo
						ibiBPMSum_ += ibiBPM_; //for calculating meanTempo
					}
					
					//cout << "i: " << i << "; beatTime: " << beatTime_ << "; ibi: " << ibi << "; lastIbi: " << lastIbi_ << endl;
					//to avoid supurious beats at the transitions (on best agent changes)
					//[prioritize replacing (next) agent]
					if(nextIbi > (0.5*ibi))
					{
						ibiBPM_ = (60.0 / ibi); //inter-beat-interval (in BPMs)

						if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
							|| (strcmp(mode_.c_str(), "beats+medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
						{
							outStream << beatTime_ << " " << ibiBPM_ << endl;
						}

						addMedianVector(ibiBPM_); //for calculating medianTempo
						ibiBPMSum_ += ibiBPM_; //for calculating meanTempo
						lastBeatTime_ = beatTime_;
						beatTimeTmp_ = nextBeatTime;

						beatCount_ ++;
					}
					lastIbi_ = ibi;
				}

				if((strcmp(mode_.c_str(), "meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					ostringstream oss2;
					fstream outStream2;
					oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
					outStream2.open(oss2.str().c_str(), ios::out|ios::trunc);
					outStream2 << (mrs_natural) ((ibiBPMSum_/beatCount_) + 0.5) << endl;
					outStream2.close();
				}

				if((strcmp(mode_.c_str(), "medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
					|| (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
				{
					mrs_natural output;
					output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
					tempo_ = output;
					ctrl_tempo_->setValue(tempo_, NOUPDATE);
					
					ostringstream oss3;
					fstream outStream3;
					oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
					outStream3.open(oss3.str().c_str(), ios::out|ios::trunc);
					outStream3 << output << endl;
					outStream3.close();

					//MATLAB_PUT(ibiBPMVec_, "IBIVector");
					//MATLAB_PUT(bestFinalAgentHistory_, "bestAgentHistory");
				}
			}
		}
	}
	//MATLAB_PUT(out, "BeatTimesSink");
}







	
