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

#include "ShiftInput.h"

using namespace std;
using namespace Marsyas;

ShiftInput::ShiftInput(string name):MarSystem("ShiftInput",name)
{
	winSize_ = 0;
	hopSize_ = 0;
	addControls();
}

ShiftInput::~ShiftInput()
{
}

ShiftInput::ShiftInput(const ShiftInput& a):MarSystem(a)
{
	winSize_ = 0;
	hopSize_ = 0;

	ctrl_reset_ = getctrl("mrs_bool/reset");
	ctrl_winSize_ = getctrl("mrs_natural/winSize");
}

MarSystem* 
ShiftInput::clone() const 
{
	return new ShiftInput(*this);
}

void
ShiftInput::addControls()
{
	addctrl("mrs_natural/winSize", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES, ctrl_winSize_);
	setctrlState("mrs_natural/winSize", true);

	//must be set true so we set the internal buffer to zeros the first
	//time it is used (it will be set to false after that)
	addctrl("mrs_bool/reset", true, ctrl_reset_);
}

void
ShiftInput::myUpdate(MarControlPtr sender)
{
	(void) sender;

	winSize_ = ctrl_winSize_->to<mrs_natural>();
	hopSize_ = ctrl_inSamples_->to<mrs_natural>();
	
	if(hopSize_ < winSize_)
		outSavedData_.stretch(ctrl_inObservations_->to<mrs_natural>(), winSize_- hopSize_);

	ctrl_onSamples_->setValue(ctrl_winSize_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ostringstream oss;
	oss << "HopSize" << hopSize_ << "_WinSize" << winSize_ << "_" << ctrl_inObsNames_;
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
}

void 
ShiftInput::myProcess(realvec& in, realvec& out)
{
	for(o=0; o<inObservations_; ++o)
	{
		if(hopSize_ < winSize_)
		{
			//check if we must clear the audio buffer (due to a reset call)
			if (ctrl_reset_->to<mrs_bool>()) 
			{
				outSavedData_.setval(0.0);
				ctrl_reset_->setValue(false);
			}
			//copy previous output stored data to the output
			for (t = 0; t < winSize_-hopSize_; t++)
			{
				out(o,t) = outSavedData_(o, t);
			}
			//add new hopSize samples from input to the end of output
			for (t=winSize_-hopSize_; t < winSize_; t++)
			{
				out(o, t) = in(o, t-(winSize_-hopSize_));
			}
			//store current output for next time
			for (t = 0; t < winSize_-hopSize_; t++) 
				outSavedData_(o, t) = out(o, t+hopSize_);
		}
		else
		{
			for(t=0; t<onSamples_; ++t)
				out(o,t) = in(o,t);
		}
	}

	//used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
	MATLAB_PUT(in, "ShiftInput_in");
	MATLAB_PUT(out, "ShiftInput_out");
}









