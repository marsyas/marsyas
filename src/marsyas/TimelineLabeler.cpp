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

#include "TimelineLabeler.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace Marsyas;

TimelineLabeler::TimelineLabeler(string name):MarSystem("TimelineLabeler", name)
{
	addControls();
	labelFiles_ = ",";
	numClasses_ = 0;
}

TimelineLabeler::TimelineLabeler(const TimelineLabeler& a) : MarSystem(a)
{
	ctrl_labelFiles_ = getctrl("mrs_string/labelFiles");
	ctrl_currentLabelFile_ = getctrl("mrs_natural/currentLabelFile");
	ctrl_labelNames_ = getctrl("mrs_string/labelNames");
	ctrl_currentLabel_ = getctrl("mrs_natural/currentLabel");
	ctrl_nLabels_ = getctrl("mrs_natural/nLabels");

	labelFiles_ = ",";
	numClasses_ = 0;
}

TimelineLabeler::~TimelineLabeler()
{
}

MarSystem*
TimelineLabeler::clone() const
{
	return new TimelineLabeler(*this);
}

void
TimelineLabeler::addControls()
{
	addctrl("mrs_string/labelFiles", ",", ctrl_labelFiles_);
	ctrl_labelFiles_->setState(true);

	addctrl("mrs_natural/currentLabelFile", 0, ctrl_currentLabelFile_);
	ctrl_currentLabelFile_->setState(true);

	addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
	addctrl("mrs_natural/currentLabel", 0, ctrl_currentLabel_);
	addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
}

void
TimelineLabeler::myUpdate(MarControlPtr sender)
{
	MRSDIAG("TimelineLabeler.cpp - TimelineLabeler:myUpdate");

	MarSystem::myUpdate(sender);

	//fill labelFilesVec
	mrs_string newLabelFiles = ctrl_labelFiles_->to<mrs_string>();
	if(labelFiles_ != newLabelFiles)
	{
		labelFiles_ = newLabelFiles;
		mrs_natural i;
		labelFilesVec_.clear();
		while(newLabelFiles.length() != 0 )
		{
			i = newLabelFiles.find(",");
			labelFilesVec_.push_back(newLabelFiles.substr(0, i).c_str());
			newLabelFiles = newLabelFiles.substr(i+1 , newLabelFiles.length()-i-1);
		}
	}

	//load currentLabelFile into the internal timeline (if not already loaded)
	if(labelFilesVec_.size() && (labelFilesVec_[ctrl_currentLabelFile_->to<mrs_natural>()]) != timeline_.filename())
	{
		if(timeline_.load(labelFilesVec_[ctrl_currentLabelFile_->to<mrs_natural>()]))
		{
			//get the number of classes in the currently loaded timeline
			numClasses_ = (mrs_natural)timeline_.numClasses();

			//get the labels of the classes in the currently loaded timeline
			ostringstream sstr;
			vector<mrs_string> classNames = timeline_.getRegionNames();
			for(mrs_natural i=0; i < numClasses_; ++i)
				sstr << classNames[i] << ",";
			ctrl_labelNames_->setValue(sstr.str(), NOUPDATE);

			ctrl_currentLabelFile_ = ctrl_currentLabelFile_->to<mrs_natural>();
		}
		else //some problem occurred when reading the timeline file...
		{
			MRSWARN("TimelineLabeler::myUpdate() - error reading label file " << labelFilesVec_[ctrl_currentLabelFile_->to<mrs_natural>()]);
			numClasses_ = 0;
			ctrl_labelNames_->setValue(",", NOUPDATE);
		}
	}
	
	ctrl_nLabels_->setValue(numClasses_, NOUPDATE);
	samplePos_ = 0;
	curRegion_ = 0;
}

void
TimelineLabeler::myProcess(realvec& in, realvec& out)
{
	//bypass audio input to output 
	out = in;

	if(numClasses_ == 0)
	{
		ctrl_currentLabel_->setValue(0);
		return;
	}

	//get current region boundaries
	//mrs_natural regionStart = (mrs_natural)(timeline_.start(curRegion_) * timeline_.lineSize_); //region start sample
	mrs_natural regionEnd = (mrs_natural)(timeline_.regionEnd(curRegion_) * timeline_.lineSize()); //region end sample

	//check if this audio frame belongs to current region or to the next one
	if (samplePos_+inSamples_/2 < regionEnd)
		ctrl_currentLabel_->setValue(timeline_.regionClass(curRegion_));
	else
		ctrl_currentLabel_->setValue(timeline_.regionClass(++curRegion_));

	samplePos_ += inSamples_;
}



