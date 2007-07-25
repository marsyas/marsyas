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

/** 
	\class PeakViewSource
	\ingroup Special
	\brief MarSystem to read .peak files and at each tick output the peaks in each frame

	Controls:
	- \b mrs_string/filename [w] : .peak file name to read

*/
#include "PeakViewSource.h"
#include "peakView.h"

using namespace std;
using namespace Marsyas;

PeakViewSource::PeakViewSource(string name):MarSystem("PeakViewSource", name)
{
	addControls();
	filename_ = "EMPTY_STRING";
	frameIdx_ = 0;
	numFrames_ = 0;
	frameSize_ = 0;
}

PeakViewSource::PeakViewSource(const PeakViewSource& a) : MarSystem(a)
{
	ctrl_filename_ = getctrl("mrs_string/filename");
	ctrl_pos_= getctrl("mrs_natural/pos");
	ctrl_size_ = getctrl("mrs_natural/size");
	ctrl_notEmpty_ = getctrl("mrs_bool/notEmpty"); 

	filename_ = a.filename_;
	frameIdx_ = a.frameIdx_;
	numFrames_ = a.numFrames_;
	frameSize_ = a.frameSize_;
}

PeakViewSource::~PeakViewSource()
{
}

MarSystem* 
PeakViewSource::clone() const 
{
	return new PeakViewSource(*this);
}

void 
PeakViewSource::addControls()
{
	addctrl("mrs_string/filename", "defaultfile", ctrl_filename_);
	setctrlState("mrs_string/filename", true);

	addctrl("mrs_bool/notEmpty", false, ctrl_notEmpty_);
	addctrl("mrs_natural/size", 0, ctrl_size_);
	addctrl("mrs_natural/pos", 0, ctrl_pos_);
}

void
PeakViewSource::defaultConfig()
{
	ctrl_onSamples_->setValue(1, NOUPDATE);
	ctrl_onObservations_->setValue(0, NOUPDATE);
	ctrl_osrate_->setValue(0.0, NOUPDATE);
	ctrl_onObsNames_->setValue(",", NOUPDATE);

	ctrl_pos_->setValue(0, NOUPDATE);
	ctrl_size_->setValue(0, NOUPDATE);
	ctrl_notEmpty_->setValue(false, NOUPDATE);
	peakData_.create(0);
	frameIdx_ = 0;
	numFrames_ = 0;
}

void
PeakViewSource::myUpdate(MarControlPtr sender)
{
	//check for a new filename (this is also true for the first call to myUpdate())
	if(ctrl_filename_->to<mrs_string>() != filename_)
	{
		//check if this is the first call to myUpdate()
		if(ctrl_filename_->to<mrs_string>() == "defaultfile")
		{
			defaultConfig();
		}
		else //not the first call to myUpdate()
		{
			//try to read entire file into memory [!]
			peakView peakDataView(peakData_);
			if(peakDataView.peakRead(ctrl_filename_->to<mrs_string>()))
			{
				numFrames_ = peakDataView.getNumFrames();
				frameSize_ = peakDataView.getFrameSize();

				mrs_natural frameMaxNumPeaks = peakDataView.getFrameMaxNumPeaks();

				ctrl_onSamples_->setValue(1, NOUPDATE);
				ctrl_onObservations_->setValue(frameMaxNumPeaks * peakView::nbPkParameters, NOUPDATE);
				ctrl_osrate_->setValue(peakDataView.getFs(), NOUPDATE);
				ostringstream oss;
				for(mrs_natural j=0; j< peakView::nbPkParameters; ++j) //j = param index
				{
					for (mrs_natural i=0; i < frameMaxNumPeaks; i++) //i = peak index
						oss << peakView::getParamName(j) << "_" << i+j*frameMaxNumPeaks << ",";
				}
				ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

				filename_ = ctrl_filename_->to<mrs_string>();
				frameIdx_ = 0;
				ctrl_size_->setValue(numFrames_*frameSize_, NOUPDATE);
				ctrl_pos_->setValue(0, NOUPDATE);
				ctrl_notEmpty_->setValue(true, NOUPDATE);
			}
			else //failed to load the file
			{
				MRSERR("PeakViewSource::myUpdate() : error opening file: " << filename_);
				defaultConfig();
			}
		}
	}

	//[TODO]: Rewind? Reset? Done? [!]
	//[TODO]: read directly from file and not load entire .peak file into memory
	//[TODO]: allow changing pos control
}

void 
PeakViewSource::myProcess(realvec& in, realvec& out)
{
	//at each tick, output peaks for corresponding frame
	if(ctrl_notEmpty_->isTrue())
	{
		ctrl_pos_->setValue(frameIdx_*frameSize_);
		
		for(o=0; o < peakData_.getRows(); ++o)
			out(o,0) = peakData_(o, frameIdx_);

		frameIdx_++;
		if(frameIdx_ == numFrames_)//if EOF
			ctrl_notEmpty_->setValue(false);
	}
}








