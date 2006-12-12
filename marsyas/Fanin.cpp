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
\class Fanin
\brief Fanin of MarSystem objects

Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 
*/

#include "Fanin.h"

using namespace std;
using namespace Marsyas;

Fanin::Fanin(string name):MarSystem("Fanin", name)
{
	isComposite_ = true;
}

Fanin::~Fanin()
{
	deleteSlices();
}

void 
Fanin::deleteSlices()
{
	vector<realvec *>::const_iterator iter;
	for (iter= slices_.begin(); iter != slices_.end(); iter++)
	{
		delete *(iter);
	}
	slices_.clear();
}

MarSystem* 
Fanin::clone() const 
{
	return new Fanin(*this);
}

void 
Fanin::myUpdate()
{
	if (marsystemsSize_ != 0) 
	{
		marsystems_[0]->update();

		mrs_natural inObservations = marsystems_[0]->getctrl("mrs_natural/inObservations")->toNatural();

		for (mrs_natural i=1; i < marsystemsSize_; i++) 
		{
			//lmartins: setctrl or updctrl?!? [?]
			marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
			marsystems_[i]->setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte")); //[!] israte
			marsystems_[i]->update();
			inObservations += marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural();
		}

		setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
		setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples"));
		setctrl("mrs_natural/inObservations", inObservations);
		setctrl("mrs_natural/onObservations", 1);//sum the outputs of each child into  single observation!
		setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));
		setctrl("mrs_real/osrate", marsystems_[0]->getctrl("mrs_real/osrate"));

		// update slices for child MarSystems
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

bool 
Fanin::updControl(string cname, MarControlPtr newcontrol, bool upd)
{ 
	// get the control (local or from children)...
	MarControlPtr control = getControl(cname);

	// ...and check if the control really exists locally or among children
	if(control.isInvalid())
	{
		MRSWARN("MarSystem::updControl - Unsupported control name = " + cname);
		MRSWARN("MarSystem::updControl - Composite name = " + name_);
		return false;
	}

	// since the control exists somewhere, set its value...
	if(!control->setValue(newcontrol, upd))
		return false; //some error occurred in setValue()

	//in case this is a composite Marsystem,
	if(isComposite_)
	{
		// call update (only if the control has state,
		// upd is true, and if it's not a local control (otherwise update 
		// was already called by control->setValue())).
		if(upd && control->hasState() && !hasControlLocal(cname))
			update();

		// certain controls must also be propagated to its children
		// (must find a way to avoid this hard-coded control list, though! [!] )

		//Fanin Specific [!]
		if(cname == "mrs_natural/inObservations")
		{
			if (marsystemsSize_ > 0)
			{
				//Fanin Specific [!]
				mrs_natural val = newcontrol->toNatural() / marsystemsSize_;

				if(!marsystems_[0]->updctrl(cname, val, upd))
					return false;//some error occurred in updctrl()
				if(upd && marsystems_[0]->hasControlState(cname))
					update();
			}
		}
		if ((cname == "mrs_natural/inSamples")|| 
			//(cname == "mrs_natural/inObservations")||
			(cname == "mrs_real/israte")||
			(cname == "mrs_string/inObsNames"))
		{
			//if there is at least a child MarSystem in this composite...
			if (marsystemsSize_ > 0)
			{
				if(!marsystems_[0]->updctrl(cname, newcontrol, upd))
					return false;//some error occurred in updctrl()
				if(upd && marsystems_[0]->hasControlState(cname))
					update();
			}
		}
	}

	//success!
	return true;
}

void
Fanin::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);

	out.setval(0.0);

	// Add assertions about sizes
	realvec ob(1,inSamples_);

	for (o=0; o < inObservations_; o++)
	{
		// process each observation 
		for (t=0; t < inSamples_; t++)
			ob(0,t) = in(o,t);
		marsystems_[o]->process(ob, *(slices_[o]));
		for (t=0; t < onSamples_; t++)
			out(0,t) += (*(slices_[o]))(0,t);	  
	}
}




