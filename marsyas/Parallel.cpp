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

#include "Parallel.h"

using namespace std;
using namespace Marsyas;

Parallel::Parallel(string name):MarSystem("Parallel",name)
{
	isComposite_ = true;
}

Parallel::~Parallel()
{
	deleteSlices();
}

void Parallel::deleteSlices()
{
	vector<realvec *>::const_iterator iter;
	for (iter = slices_.begin(); iter != slices_.end(); iter++) {
		delete *(iter);
	}
	slices_.clear();
}

MarSystem* Parallel::clone() const 
{
	return new Parallel(*this);
}

void Parallel::myUpdate(MarControlPtr sender)
{
	if (marsystemsSize_ != 0) 
	{
		//propagate in flow controls to first child
		marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
		marsystems_[0]->setctrl("mrs_real/israte", israte_);
		//marsystems_[0]->updctrl("mrs_natural/inObservations", inObservations_);
		//marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
		marsystems_[0]->update();

		mrs_natural inObservations = marsystems_[0]->getctrl("mrs_natural/inObservations")->toNatural();
		mrs_natural onObservations = marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural();

		ostringstream oss;
		oss << marsystems_[0]->getctrl("mrs_string/onObsNames");

		for (mrs_natural i=1; i < marsystemsSize_; i++) 
		{
			marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
			marsystems_[i]->setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte")); //[!] israte
			marsystems_[i]->update();
			oss << marsystems_[i]->getctrl("mrs_string/onObsNames");

			inObservations += marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural();
			onObservations += marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural();
		}

		//forward flow propagation
		setctrl(ctrl_onSamples_, marsystems_[0]->getctrl("mrs_natural/onSamples"));
		setctrl(ctrl_onObservations_, onObservations);
		setctrl(ctrl_osrate_, marsystems_[0]->getctrl("mrs_real/osrate"));
		setctrl(ctrl_onObsNames_, oss.str());

		// update buffers for children MarSystems
		if ((mrs_natural)slices_.size() < 2*marsystemsSize_) 
		{
			slices_.resize(2*marsystemsSize_, NULL);
		}

		for (mrs_natural i = 0; i < marsystemsSize_; i++) 
		{
			if (slices_[2*i] != NULL) 
			{
				if ((slices_[2*i])->getRows() != marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural() ||
					  (slices_[2*i])->getCols() != marsystems_[i]->getctrl("mrs_natural/inSamples")->toNatural()) 
				{
					delete slices_[2*i];
					slices_[2*i] = new realvec(marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural(), 
																		 marsystems_[i]->getctrl("mrs_natural/inSamples")->toNatural());
				}
			}
			else 
				slices_[2*i] = new realvec(marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural(), 
																	 marsystems_[i]->getctrl("mrs_natural/inSamples")->toNatural());

			(slices_[2*i])->setval(0.0);

			if (slices_[2*i+1] != NULL) 
			{
				if ((slices_[2*i+1])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural() ||
					  (slices_[2*i+1])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural()) 
				{
					delete slices_[2*i+1];
					slices_[2*i+1] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), 
																			 marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
				}
			}
			else 
				slices_[2*i+1] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), 
																		 marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
			
			(slices_[2*i+1])->setval(0.0);
		}
	}
	else //if composite is empty...
		MarSystem::myUpdate(sender);
}

void Parallel::myProcess(realvec& in, realvec& out)
{
	mrs_natural inIndex = 0;
	mrs_natural outIndex = 0;
	mrs_natural localIndex = 0;

	if (marsystemsSize_ == 1) 
	{
		marsystems_[0]->process(in, out);
	}
	else if (marsystemsSize_ > 1) 
	{
		for (mrs_natural i = 0; i < marsystemsSize_; i++) 
		{
			localIndex = marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural();
			for (o = 0; o < localIndex; o++) 
			{
				for (t = 0; t < inSamples_; t++) //lmartins: was t < onSamples [!]
				{
					(*(slices_[2*i]))(o,t) = in(inIndex + o,t);
				}
			}
			inIndex += localIndex;
			marsystems_[i]->process(*(slices_[2*i]), *(slices_[2*i+1]));
			localIndex = marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural();
			for (o = 0; o < localIndex; o++) 
			{
				for (t = 0; t < onSamples_; t++) 
				{
					out(outIndex + o,t) = (*(slices_[2*i+1]))(o,t);
				}
			}
			outIndex += localIndex;
		}
	}
	else if(marsystemsSize_ == 0) //composite has no children!
	{
		MRSWARN("Parallel::process: composite has no children MarSystems - passing input to output without changes.");
		out = in;
	}
}

