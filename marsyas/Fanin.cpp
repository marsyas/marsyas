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
		setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
		mrs_natural obs = 0;
		for (mrs_natural i=0; i < marsystemsSize_; i++)
		{
			obs += marsystems_[i]->getctrl("mrs_natural/inObservations")->toNatural();//[?]
		}
		setctrl("mrs_natural/inObservations", obs); //[?] 
		setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));  

		// setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
		// setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
		// setctrl("mrs_real/israte", getctrl("mrs_real/israte"));  

		for (mrs_natural i=0; i < marsystemsSize_; i++) //i=0 [?]
		{
			//lmartins: replace updctrl() calls by setctrl()? ==> more efficient! [?]
			marsystems_[i]->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
			marsystems_[i]->updctrl("mrs_natural/inObservations", (mrs_natural)1); //[?]
			marsystems_[i]->updctrl("mrs_real/israte", getctrl("mrs_real/israte"));
			marsystems_[i]->update();
		}
		setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl("mrs_natural/onObservations", (mrs_natural)1);
		setctrl("mrs_real/osrate", marsystems_[0]->getctrl("mrs_real/osrate")->toReal());

		deleteSlices();

		// SHOULD ADD CHECK THAT THE OUTSLICEINF OF 
		// ALL THE MARSYSTEMS IS THE SAME 
		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			slices_.push_back(new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural() , marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural()));			
			(slices_[i])->setval(0.0);
		}
		//defaultUpdate();
	}
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




