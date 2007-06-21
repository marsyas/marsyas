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
\class Fanout
\ingroup Composites
\brief Fanout of MarSystem objects

Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 

Controls:
- \b mrs_natural/disable	: passes in the number of the child
  MarSystem to disable.
- \b mrs_natural/enable	: pass in the number of the child MarSystem to
  enable.  (by default all children are enabled)
- \b mrs_bool/probe	: deprecated.
*/

#include "Fanout.h"

using namespace std;
using namespace Marsyas;

Fanout::Fanout(string name):MarSystem("Fanout", name)
{
	isComposite_ = true;
	addControls();
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
	addctrl("mrs_bool/probe", false);
	setctrlState("mrs_bool/probe", true);
	addctrl("mrs_natural/disable", -1);
	setctrlState("mrs_natural/disable", true);
	addctrl("mrs_natural/enable", -1);
	setctrlState("mrs_natural/enable", true);
}

void 
Fanout::myUpdate(MarControlPtr sender)
{
	probe_ = getctrl("mrs_bool/probe")->toBool();
	if (enabled_.getSize() != marsystemsSize_)
	{
		enabled_.create(marsystemsSize_);
		enabled_.setval(1.0);
		localIndices_.create(marsystemsSize_);
	}

	disable_ = getctrl("mrs_natural/disable")->toNatural();
	if (disable_ != -1) 
	{
		enabled_(disable_) = 0.0;
		localIndices_(disable_) = 0.0;
		setctrl("mrs_natural/disable", -1);
	}
	enable_ = getctrl("mrs_natural/enable")->toNatural();
	if (enable_ != -1) 
	{
		enabled_(enable_) = 1.0;
		localIndices_(enable_) = 1.0;
		setctrl("mrs_natural/enable", -1);
	}


	mrs_natural onObservations = 0;

	if (marsystemsSize_ != 0)
	{
		marsystems_[0]->update();
		setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
		setctrl("mrs_natural/inObservations", marsystems_[0]->getctrl("mrs_natural/inObservations"));
		setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));  
		setctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
		ostringstream oss;
		oss << marsystems_[0]->getctrl("mrs_string/onObsNames");

		if (enabled_(0))
		{
			onObservations += marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural();
			localIndices_(0) = marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural();
		}

		for (mrs_natural i=1; i < marsystemsSize_; i++)
		{
			oss << marsystems_[i]->getctrl("mrs_string/onObsNames");

			//lmartins: replace updctrl() calls by setctrl() + update() ? ==> more efficient! [?]
			marsystems_[i]->updctrl("mrs_natural/inSamples", marsystems_[i-1]->getctrl("mrs_natural/inSamples"));
			marsystems_[i]->updctrl("mrs_natural/inObservations", marsystems_[i-1]->getctrl("mrs_natural/inObservations"));
			marsystems_[i]->updctrl("mrs_real/israte", marsystems_[i-1]->getctrl("mrs_real/israte"));
			marsystems_[i]->updctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
			marsystems_[i]->update();

			if (enabled_(i))
			{
				onObservations += (marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural());
				localIndices_(i) = marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural();
			}
		}

		setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl("mrs_natural/onObservations", onObservations);
		setctrl("mrs_real/osrate", marsystems_[0]->getctrl("mrs_real/osrate")->toReal());
		setctrl("mrs_string/onObsNames", oss.str());

		// update buffers between components 
		if ((mrs_natural)slices_.size() < marsystemsSize_) 
			slices_.resize(marsystemsSize_, NULL);

		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			if (slices_[i] != NULL) 
			{
				if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural()  ||
					(slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural())
				{
					delete slices_[i];
					slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), 
						marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
				}
			}
			else 
			{
				slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), 
					marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
			}
			(slices_[i])->setval(0.0);
		}
	}
}

void
Fanout::myProcess(realvec& in, realvec& out)
{
	mrs_natural outIndex = 0;
	mrs_natural localIndex = 0;

	for (mrs_natural i = 0; i < marsystemsSize_; i++)
	{
		if (enabled_(i))
		{
			marsystems_[i]->process(in, *(slices_[i]));

			for (o=0; o < localIndices_(i); o++)
				for (t=0; t < onSamples_; t++)
					out(outIndex + o,t) = (*(slices_[i]))(o,t);

			outIndex += localIndices_(i);      
		}
	}
}
