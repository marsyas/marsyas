/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Fanout.h"
#include "common_source.h"

 
using std::ostringstream;
using std::vector;

using namespace Marsyas;

Fanout::Fanout(mrs_string name):MarSystem("Fanout", name)
{
	isComposite_ = true;
	addControls();
}

Fanout::Fanout(const Fanout& a): MarSystem(a)
{
	ctrl_enabled_ = getctrl("mrs_realvec/enabled");
	ctrl_muted_ = getctrl("mrs_realvec/muted");
}

Fanout::~Fanout()
{
	deleteSlices();
}

void
Fanout::deleteSlices()
{
	vector<realvec *>::const_iterator iter;
	for (iter= slices_.begin(); iter != slices_.end(); iter++)
	{
		delete *(iter);
	}
	slices_.clear();
}

MarSystem*
Fanout::clone() const
{
	return new Fanout(*this);
}

void
Fanout::addControls()
{
	addctrl("mrs_natural/disable", -1);
	setctrlState("mrs_natural/disable", true);
	addctrl("mrs_natural/enable", -1);
	setctrlState("mrs_natural/enable", true);

	addctrl("mrs_string/enableChild", ",");
	setctrlState("mrs_string/enableChild", true);
	addctrl("mrs_string/disableChild", ",");
	setctrlState("mrs_string/disableChild", true);

	addctrl("mrs_realvec/enabled", realvec(), ctrl_enabled_);
	addctrl("mrs_realvec/muted", realvec(), ctrl_muted_);
}

void
Fanout::myUpdate(MarControlPtr sender)
{
	MarControlAccessor acc(ctrl_enabled_);
	mrs_realvec& enabled = acc.to<mrs_realvec>();
	if (enabled.getSize() < marsystemsSize_)
	{
		enabled.create(marsystemsSize_);
		enabled.setval(1.0); //all children enabled by default
	}
	
	MarControlAccessor accMuted(ctrl_muted_);
	mrs_realvec& muted = accMuted.to<mrs_realvec>();
	if (muted.getSize() < marsystemsSize_)
	{
		muted.create(marsystemsSize_);
		muted.setval(0.0); //all children unmuted by default
	}
	
	if (marsystemsSize_ != 0)
	{
		localIndices_.create(marsystemsSize_);
	}

	//check child MarSystems to disable (passed as a string)
	disableChild_ = getctrl("mrs_string/disableChild")->to<mrs_string>();
	disableChildIndex_ = -1;
	for (size_t i=0; i < marsystems_.size(); ++i)
	{
		mrs_string s;
		s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
		if (disableChild_ == s)
		{
			disableChildIndex_ = i;
			MRSDIAG("Fanout::myUpdate(): DISABLING child: " + marsystems_[i]->getAbsPath());
		}
	}
	if (disableChildIndex_ != -1)
	{
		enabled(disableChildIndex_) = 0.0;
		localIndices_(disableChildIndex_) = 0.0;
		setctrl("mrs_string/disableChild", ",");
	}
	if (disableChild_ == "all")
	{
		for (size_t i=0; i < marsystems_.size(); ++i)
		{
			enabled(i) = 0.0;
			localIndices_(i) = 0.0;
			setctrl("mrs_string/disableChild", ",");
			MRSDIAG("Fanout::myUpdate(): DISABLING child: " + marsystems_[i]->getAbsPath());
		}
	}
	//check child MarSystem to disable (passed as an index)
	disable_ = getctrl("mrs_natural/disable")->to<mrs_natural>();
	if (disable_ != -1 && disable_ < (mrs_natural)marsystemsSize_)
	{
		enabled(disable_) = 0.0;
		localIndices_(disable_) = 0.0;
		setctrl("mrs_natural/disable", -1);
		MRSDIAG("Fanout::myUpdate(): DISABLING child: " + marsystems_[disable_]->getAbsPath());
	}
	else
		setctrl("mrs_natural/disable", -1);

	//check child MarSystems to enable (passed as a string)
	enableChild_ = getctrl("mrs_string/enableChild")->to<mrs_string>();
	enableChildIndex_ = -1;
	for (size_t i=0; i < marsystems_.size(); ++i)
	{
		mrs_string s;
		s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
		if (enableChild_ == s)
		{
			enableChildIndex_ = i;
			MRSDIAG("Fanout::myUpdate(): ENABLING child: " + marsystems_[i]->getAbsPath());
		}
	}
	if (enableChildIndex_ != -1)
	{
		enabled(enableChildIndex_) = 1.0;
		localIndices_(enableChildIndex_) = 1.0;
		setctrl("mrs_string/enableChild", ",");
	}
	//check child MarSystem to enable (passed as an index)
	enable_ = getctrl("mrs_natural/enable")->to<mrs_natural>();
	if (enable_ != -1 && enable_ < (mrs_natural)marsystemsSize_)
	{
		enabled(enable_) = 1.0;
		localIndices_(enable_) = 1.0;
		setctrl("mrs_natural/enable", -1);
	}
	else
		setctrl("mrs_natural/enable", -1);


	if (marsystemsSize_ != 0)
	{
		mrs_natural highestStabilizingDelay = ctrl_inStabilizingDelay_->to<mrs_natural>();
		//propagate in flow controls to first child
		marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
		marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
		marsystems_[0]->setctrl("mrs_real/israte", israte_);
		marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
		marsystems_[0]->setctrl("mrs_natural/inStabilizingDelay", inStabilizingDelay_);
		marsystems_[0]->update();

		// update dataflow component MarSystems in order
		ostringstream oss;
		mrs_natural onObservations = 0;
		if (enabled(0))
		{
			onObservations += marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
			localIndices_(0) = marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
			oss << marsystems_[0]->getctrl("mrs_string/onObsNames");
			mrs_natural localStabilizingDelay = marsystems_[0]->getctrl("mrs_natural/onStabilizingDelay")->to<mrs_natural>();
			if (highestStabilizingDelay < localStabilizingDelay)
				highestStabilizingDelay = localStabilizingDelay;

		}
		for (mrs_natural i=1; i < marsystemsSize_; ++i)
		{
			marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[i-1]->getctrl("mrs_natural/inSamples"));
			marsystems_[i]->setctrl("mrs_natural/inObservations", marsystems_[i-1]->getctrl("mrs_natural/inObservations"));
			marsystems_[i]->setctrl("mrs_real/israte", marsystems_[i-1]->getctrl("mrs_real/israte"));
			marsystems_[i]->setctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
			marsystems_[i]->setctrl("mrs_natural/inStabilizingDelay", inStabilizingDelay_);
			marsystems_[i]->update(sender);
			if (enabled(i))
			{
				onObservations += (marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
				localIndices_(i) = marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

				oss << marsystems_[i]->getctrl("mrs_string/onObsNames");
				mrs_natural localStabilizingDelay = marsystems_[i]->getctrl("mrs_natural/onStabilizingDelay")->to<mrs_natural>();
				if (highestStabilizingDelay < localStabilizingDelay)
					highestStabilizingDelay = localStabilizingDelay;
			}
		}

		// forward flow propagation
		setctrl(ctrl_onSamples_, marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
		setctrl(ctrl_onObservations_, onObservations);
		setctrl(ctrl_osrate_, marsystems_[0]->getctrl("mrs_real/osrate")->to<mrs_real>());
		setctrl(ctrl_onObsNames_, oss.str());
		setctrl(ctrl_onStabilizingDelay_, highestStabilizingDelay);

		// update buffers between components
		if ((mrs_natural)slices_.size() < marsystemsSize_)
			slices_.resize(marsystemsSize_, NULL);
		for (mrs_natural i=0; i< marsystemsSize_; ++i)
		{
			if (slices_[i] != NULL)
			{
				if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>()  ||
					(slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>())
				{
					delete slices_[i];
					slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
											 marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
				}
			}
			else
			{
				slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
										 marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
			}
			(slices_[i])->setval(0.0);
		}
	}
	else //if composite is empty...
		MarSystem::myUpdate(sender);
}

void
Fanout::myProcess(realvec& in, realvec& out)
{
	mrs_natural o,t;
	if (marsystemsSize_>0)
	{
		mrs_natural outIndex = 0;
		
		//MarControlAccessor acc(ctrl_enabled_);
		//mrs_realvec& enabled = acc.to<mrs_realvec>();
		
		MarControlAccessor accMuted(ctrl_muted_);
		mrs_realvec& muted = accMuted.to<mrs_realvec>();
		
		for (mrs_natural i = 0; i < marsystemsSize_; ++i)
		{
			if (localIndices_(i))//enabled child have a non-zero localIndex
			{
				//check if the child is unmuted, otherwise just use the previous output
				if(!muted(i))
				{
					marsystems_[i]->process(in, *(slices_[i]));
					
					for (o=0; o < localIndices_(i); o++)
						for (t=0; t < onSamples_; t++)
							out(outIndex + o,t) = (*(slices_[i]))(o,t);
				}
				outIndex += (mrs_natural)localIndices_(i);
			}
		}
	}
	else //composite has no children!
	{
		MRSWARN("FanOut::process: composite has no children MarSystems - passing input to output without changes.");
		out = in;
	}
}
