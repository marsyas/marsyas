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

#include "SonicVisualiserSink.h"
#include <fstream>
#include <string.h>

using namespace std;
using namespace Marsyas;

SonicVisualiserSink::SonicVisualiserSink(string name):MarSystem("SonicVisualiserSink", name)
{
  addControls();
}

SonicVisualiserSink::SonicVisualiserSink(const SonicVisualiserSink& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_destFileName_ = getctrl("mrs_string/destFileName");
}

SonicVisualiserSink::~SonicVisualiserSink()
{
}

MarSystem* 
SonicVisualiserSink::clone() const 
{
  return new SonicVisualiserSink(*this);
}

void 
SonicVisualiserSink::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/hopSize", 1, ctrl_hopSize_);
  addctrl("mrs_real/srcFs", 1.0, ctrl_srcFs_);
  addctrl("mrs_string/mode", "frames", ctrl_mode_);
  addctrl("mrs_string/destFileName", "output_beatTimes.txt", ctrl_destFileName_);
}

void
SonicVisualiserSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SonicVisualiserSink.cpp - SonicVisualiserSink:myUpdate");
  MarSystem::myUpdate(sender);

  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  t_ = 0;
  counter_ = 0;
  time_.create(100000);
  value_.create(100000);
}


void 
SonicVisualiserSink::myProcess(realvec& in, realvec& out)
{
	//Frame (tick) counter:
	t_++;

	//FlowThru input
	out = in;

	//Output BeatTime (in Seconds) = (t_ (inFrames) * hopSize_) / srcFs_
	srcFs_ = ctrl_srcFs_->to<mrs_real>();
	
	mode_ = ctrl_mode_->to<mrs_string>();
	//for (o=0; o < inObservations_; o++)
	//{
	//	for (t=0; t < inSamples_; t++)
	//	{
			if(strcmp(mode_.c_str(), "seconds") == 0)
			{
				//time_(counter_) = ((t_+t) * hopSize_) / srcFs_; //time in seconds
				mrs_natural winSize_ = 2048;
				mrs_natural adjustment = hopSize_/2;//(winSize_ - hopSize_) + floor((mrs_real) winSize_/2);

				time_(counter_) = ((t_ * hopSize_) - adjustment) / srcFs_; //time in seconds
			}

			//value_(counter_) = out(o, t);
			value_(counter_) = out(0, 0);
			
	//		if(t == inSamples_ -1)
	//			break;
			
	//		else counter_++;
	//	}
	//}
	
	//Save to an output file using fstream
	fstream outStream;

	//initially a new output file is created
	//(If the file exists, its content is deleted and it is treated as a new file)
	if(t_ == 430)
	{
		outStream.open(ctrl_destFileName_->to<mrs_string>().c_str(), ios::out|ios::trunc);
		cout << "PATH: " << ctrl_destFileName_->to<mrs_string>().c_str() << endl;
	}

	//else -> output is appended in the end of the file
	else
		outStream.open(ctrl_destFileName_->to<mrs_string>().c_str(), ios::out|ios::app);

	if(strcmp(mode_.c_str(), "seconds") == 0)
		outStream << time_(counter_) << " " << value_(counter_) << endl;

	if(strcmp(mode_.c_str(), "frames") == 0)
		outStream << value_(counter_) << endl;

	outStream.close();

	//increment counter:
	counter_++;

	//MATLAB_PUT(out, "SonicVisualiserSink");
}







	
