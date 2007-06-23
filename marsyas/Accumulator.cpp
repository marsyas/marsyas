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
\class Accumulator 
\ingroup Composites
\brief Accumulate result of multiple ticks

Accumulate result of multiple tick process calls 
to internal MarSystem. Spit output only once when
all the results are accumulated. Used to change 
the rate of process requests.

For example, if \c nTimes is 5, then each time the Accumulator recieves
a tick(), it sends 5 tick()s to the MarSystems that are inside it.

Controls:
- \b mrs_natural/nTimes [rw] : the multiplier of ticks() for the internal
  MarSystems.
*/

#include "Accumulator.h"

using namespace std;
using namespace Marsyas;

Accumulator::Accumulator(string name):MarSystem("Accumulator", name)
{
	isComposite_ = true;
	addControls();
}

Accumulator::Accumulator(const Accumulator& a) : MarSystem(a)
{
	ctrl_nTimes_ = getctrl("mrs_natural/nTimes");
}

Accumulator::~Accumulator()
{
}

MarSystem* 
Accumulator::clone() const 
{
	return new Accumulator(*this);
}

void 
Accumulator::addControls()
{
	addctrl("mrs_natural/nTimes", 5, ctrl_nTimes_);
	setctrlState("mrs_natural/nTimes", true);
	nTimes_ = 5;
}

void
Accumulator::myUpdate(MarControlPtr sender)
{
	MRSDIAG("Accumulator.cpp - Accumulator:myUpdate");
	
	string onObsNames;
	
	nTimes_ = ctrl_nTimes_->to<mrs_natural>();

	if (marsystemsSize_ > 0)
	{
		//propagate in flow controls to first child
		marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
		marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
		marsystems_[0]->setctrl("mrs_real/israte", israte_);
		marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
		marsystems_[0]->update();

		// forward flow propagation 
		setctrl(ctrl_onSamples_, 
			nTimes_ * marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl(ctrl_onObservations_, 
			marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural());
		setctrl(ctrl_osrate_, 
			marsystems_[0]->getctrl("mrs_real/osrate"));

		onObsNames = marsystems_[0]->getctrl("mrs_string/onObsNames")->toString();
	}
	else
	{
		MarSystem::myUpdate(sender);
		setctrl(ctrl_onSamples_, ctrl_inSamples_->to<mrs_natural>()*nTimes_);
	}

	onObservations_ = ctrl_onObservations_->to<mrs_natural>();
	onSamples_ = ctrl_onSamples_->to<mrs_natural>();

	ostringstream oss;
	for (int i = 0; i < onObservations_; i++)
	{
		string onObsName;
		string temp;
		onObsName = onObsNames.substr(0, onObsNames.find(","));
		temp = onObsNames.substr(onObsNames.find(",")+1, onObsNames.length());
		onObsNames = temp;
		oss << "Acc" << nTimes_ << "_" << onObsName << ",";
	}
	setctrl(ctrl_onObsNames_, oss.str());

	tout_.create(onObservations_, onSamples_ / nTimes_);
}

void 
Accumulator::myProcess(realvec& in, realvec& out)
{
	for (c = 0; c < nTimes_; c++) 
	{
		marsystems_[0]->recvControls(); // HACK STU
		marsystems_[0]->process(in, tout_);
		for (o=0; o < onObservations_; o++)
			for (t = 0; t < onSamples_/nTimes_; t++)
			{
				out(o, t + c * (onSamples_/nTimes_)) = tout_(o,t);
			}
	}
}
