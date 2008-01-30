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

#include "Memory.h"

using namespace std;
using namespace Marsyas;


Memory::Memory(string name):MarSystem("Memory",name)
{
	end_ = 0;
	addControls();
}

Memory::~Memory()
{
}

Memory::Memory(const Memory& a):MarSystem(a)
{
	end_ = 0;
	ctrl_reset_ = getctrl("mrs_bool/reset");
	ctrl_memSize_ = getctrl("mrs_natural/memSize");
}

MarSystem* 
Memory::clone() const 
{
	return new Memory(*this);
}

void 
Memory::addControls()
{
	addctrl("mrs_natural/memSize", 40, ctrl_memSize_);
	setctrlState("mrs_natural/memSize", true);
	addctrl("mrs_bool/reset", false, ctrl_reset_);
	setctrlState("mrs_bool/reset", true);
}

void
Memory::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("Memory.cpp - Memory:myUpdate");


	mrs_natural memSize = ctrl_memSize_->to<mrs_natural>();

	if (memSize != 0) 
	{
		ctrl_onSamples_->setValue(ctrl_inSamples_->to<mrs_natural>() * memSize, 
			NOUPDATE);
		ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
		ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	}
	else 
	{
		ctrl_onSamples_->setValue(ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
		ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
		ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	}
	reset_ = ctrl_reset_->to<mrs_bool>();

	inObservations_ = ctrl_inObservations_->to<mrs_natural>();

	ostringstream oss;
	string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	for (int i = 0; i < inObservations_; i++)
	{
		string inObsName;
		string temp;
		inObsName = inObsNames.substr(0, inObsNames.find(","));
		temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
		inObsNames = temp;
		oss << "Mem" << memSize << "_" << inObsName << ",";
	}
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

}

void 
Memory::myProcess(realvec& in, realvec& out)
{
	mrs_natural memSize = ctrl_memSize_->to<mrs_natural>();

	if (reset_) 
	{
		out.setval(0.0);
		reset_ = false;
		ctrl_reset_->setValue(false, NOUPDATE);
		end_ = 0;
	}

	if (memSize != 0) 
	{
		for (t = 0; t < inSamples_; t++)
		{
			for (o=0; o < inObservations_; o++)
			{
				out(o, end_) = in(o,t);
			}
			end_ = (end_ + 1) % memSize; 		// circular buffer index  		
		}
	}
	else // memSize == 0 
	{
		for (t = 0; t < inSamples_; t++)
		{
			for (o=0; o < inObservations_; o++)
			{
				out(o,t) = in(o,t);
			}
		}
	}

	//MATLAB_PUT(out, "Memory_out");
	//MATLAB_EVAL("plot(Memory_out);");
}













