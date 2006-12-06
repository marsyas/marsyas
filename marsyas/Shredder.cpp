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
    \class Shredder 

    \brief Split the output of an accumulator

    Split the result an accumulator. Spit several outputs when
    the input is recieved. Used to restore
    the rate of process requests initially change by an accumulator
*/

#include "Shredder.h"

using namespace std;
using namespace Marsyas;
 
Shredder::Shredder(string name):MarSystem("Shredder", name)
{
  isComposite_ = true;
  addControls();
}

Shredder::~Shredder()
{
}

MarSystem* 
Shredder::clone() const 
{
  return new Shredder(*this);
}

void 
Shredder::addControls()
{
  addctrl("mrs_natural/nTimes", 5);
  setctrlState("mrs_natural/nTimes", true);
  nTimes_ = 5;
}

void
Shredder::myUpdate()
{
  MRSDIAG("Shredder.cpp - Shredder:myUpdate");
  
  nTimes_ = getctrl("mrs_natural/nTimes")->toNatural();

	// update dataflow component MarSystems in order 
	if (marsystemsSize_ > 0)
	{
		marsystems_[0]->update(); 

		// set input characteristics 
		setctrl("mrs_natural/inSamples", 
			marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural() * nTimes_);
		setctrl("mrs_natural/inObservations", 
			marsystems_[0]->getctrl("mrs_natural/inObservations"));
		setctrl("mrs_real/israte", 
			marsystems_[0]->getctrl("mrs_real/israte")->toNatural());

		// set output characteristics 
		setctrl("mrs_natural/onSamples", 
			marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl("mrs_natural/onObservations", 
			marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural());
		setctrl("mrs_real/osrate", 
			marsystems_[0]->getctrl("mrs_real/osrate"));
		setctrl("mrs_string/onObsNames", 
			marsystems_[0]->getctrl("mrs_string/onObsNames"));
	}

  tin_.create(marsystems_[0]->getctrl("mrs_natural/inObservations")->toNatural(), 
		marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural());
}

bool
Shredder::updControl(std::string cname, MarControlPtr newcontrol, bool upd)
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

		//Shredder Specific [!]
		if(cname == "mrs_natural/inSamples")
		{
			if (marsystemsSize_ > 0)
			{
				//Shredder Specific [!]
				mrs_natural val = newcontrol->to<mrs_natural>() / nTimes_;

				if(!marsystems_[0]->updctrl(cname, val, upd))
					return false;//some error occurred in updctrl()
				if(upd && marsystems_[0]->hasControlState(cname))
					update();
			}
		}
		if (//(cname == "mrs_natural/inSamples")|| 
			(cname == "mrs_natural/inObservations")||
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
Shredder::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);

	for (c = 0; c < nTimes_; c++) 
	{
		for (o=0; o < inObservations_; o++)
			for (t = 0; t < inSamples_/nTimes_; t++)
			{
				tin_(o,t) = in(o, t + c * (inSamples_/nTimes_)) ;
			}
		marsystems_[0]->recvControls(); // HACK STU
		marsystems_[0]->process(tin_, out);
		
	}
	/*
	MATLAB_PUT(in, "Schredder_in");
	MATLAB_PUT(out, "Schredder_out");
	MATLAB_EVAL("figure(1);imagesc(Schredder_in(1:2:end, :))");
	MATLAB_EVAL("figure(2);plot(Schredder_out)");
	*/
}







	

	
