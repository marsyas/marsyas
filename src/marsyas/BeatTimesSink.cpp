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

  ibiBPM_ = a.ibiBPM_;
  beatCount_ = a.beatCount_;
  ibiBPMSum_ = a.ibiBPMSum_;
  inc_ = a.inc_;
  ibiBPMVec_ = a.ibiBPMVec_;
  t_ = a.t_;
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
}

void
BeatTimesSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("BeatTimesSink.cpp - BeatTimesSink:myUpdate");
  MarSystem::myUpdate(sender);

  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  winSize_ = ctrl_winSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();

  //adjustment_ = (winSize_ - hopSize_) + floor((mrs_real) winSize_/2);
  adjustment_ = hopSize_/2;
  initialOut_ = true;
  initialOut2_ = true;
  initialOut3_ = true;

  mode_ = ctrl_mode_->to<mrs_string>();

  ibiBPMVec_.create(1000);
}


void 
BeatTimesSink::myProcess(realvec& in, realvec& out)
{
	//Frame (tick) counter:
	t_ = ctrl_tickCount_->to<mrs_natural>();

	//FlowThru input
	out = in;

	//If Beat:
	if(in(0,0) == 1.0) //only count after 1st beat
	{
		//For writing only beats after inc_ (to avoid writing first unconsistent beats)
		if(beatCount_ >= inc_)
		{
			//Output BeatTime (in Seconds) = ((t_ (inFrames) * hopSize_) - adjustment) / srcFs_
			srcFs_ = ctrl_srcFs_->to<mrs_real>();
			beatTime_ = ((t_ * hopSize_) - adjustment_) / srcFs_;
			
			//after the 1st beat calculate ibi
			if(!initialOut_ || !initialOut2_ || !initialOut3_)
				ibiBPM_ = 60 / (beatTime_ - lastBeatTime_); //inter-beat-interval (in BPMs)

			fstream outStream;
			fstream outStream2;
			fstream outStream3;

			if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+tempo") == 0))
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
					outStream.close();
				}
			}

			if((strcmp(mode_.c_str(), "meanTempo") == 0))
			{
				ostringstream oss2;
				ibiBPMSum_ += ibiBPM_;

				if(initialOut2_)
				{
					oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
					cout << "MeanTempo Output: " << oss2.str().c_str() << endl;
					
					outStream2.open(oss2.str().c_str(), ios::out|ios::trunc);
					outStream2.close();
					initialOut2_ = false;
				}

				if(beatCount_ > inc_+1)
				{
					mrs_natural output = (mrs_natural) ibiBPMSum_ / (beatCount_-inc_);
					
					oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
					//outStream.open(destFile_.c_str(), ios::out|ios::app);
					outStream2.open(oss2.str().c_str());
					outStream2 << output << endl;
					outStream2.close();
				}
			}

			if((strcmp(mode_.c_str(), "medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+tempo") == 0))
			{
				ostringstream oss3;
				if(initialOut3_)
				{
					oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
					outStream3.open(oss3.str().c_str(), ios::out|ios::trunc);
					outStream3.close();
					initialOut3_ = false;
				}

				if(beatCount_ > inc_+1)
				{
					for(mrs_natural i = beatCount_-1; i >=0 ; i--)
					{
						if(ibiBPM_ > ibiBPMVec_(i))
						{
							mrs_realvec tmp(beatCount_);
							for(mrs_natural j = 0; j < beatCount_; j++)
							{
								//copy all
								tmp(j) = ibiBPMVec_(j);
							}

							ibiBPMVec_(i+1) = ibiBPM_;

							for(mrs_natural z = i+1; z < beatCount_; z++)
								ibiBPMVec_(z+1) = tmp(z);

							break;
						}
					}

					mrs_natural output;
					if(beatCount_ % 2 == 0) 
						output = (mrs_natural) ibiBPMVec_((beatCount_ / 2)+1);
					else
					{
						output = (mrs_natural) (ibiBPMVec_((mrs_natural)floor((mrs_real)beatCount_ / 2)+1) 
							+ ibiBPMVec_((mrs_natural)ceil((mrs_real)beatCount_ / 2)+1)) / 2;
					}
					//MATLAB_PUT(ibiBPMVec_, "IBIVector");
					
					oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
					outStream3.open(oss3.str().c_str());
					outStream3 << output << endl;
					outStream3.close();
				}

				else if(beatCount_ == 1) //if only two beats => equal to ibi
				{
					oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
					outStream.open(oss3.str().c_str());
					outStream << (mrs_natural) ibiBPM_ << endl;
					outStream.close();
				}
				
			}

		//MATLAB_PUT(ibiBPMVec_, "IBIVector");
		lastBeatTime_ = beatTime_;
		}
		beatCount_ ++;
	}
	//MATLAB_PUT(out, "BeatTimesSink");
}







	
