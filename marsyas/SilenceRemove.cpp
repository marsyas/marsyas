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
\class SilenceRemove 

\brief SilenceRemove takes as argument another Marsystem 
and ticks it ignoring the output when it is silent. Effectively 
tick silence remove is the same as playing the sound with 
silences removed. 
*/

#include "SilenceRemove.h"

using namespace std;
using namespace Marsyas;

SilenceRemove::SilenceRemove(string name):MarSystem("SilenceRemove",name)
{
	isComposite_ = true;
	addControls();
}

SilenceRemove::~SilenceRemove()
{

}

MarSystem* 
SilenceRemove::clone() const 
{
	return new SilenceRemove(*this);
}

void 
SilenceRemove::addControls()
{
	addctrl("mrs_real/threshold", 0.01);
	setctrlState("mrs_real/threshold", true);
}

void
SilenceRemove::myUpdate(MarControlPtr sender)
{
	MRSDIAG("SilenceRemove.cpp - SilenceRemove:myUpdate");

	threshold_ = getctrl("mrs_real/threshold")->toReal();

	if (marsystemsSize_ > 0)
	{
		// set input characteristics 
		setctrl("mrs_natural/inSamples", 
			marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural());
		setctrl("mrs_natural/inObservations", 
			marsystems_[0]->getctrl("mrs_natural/inObservations"));
		setctrl("mrs_real/israte", 
			marsystems_[0]->getctrl("mrs_real/israte"));

		// set output characteristics
		setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl("mrs_natural/onObservations", marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural());
		setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
		marsystems_[0]->update(); //lmartins: shouldn't this have already been called?! [?]


		ctrl_notEmpty_ = marsystems_[0]->getctrl("mrs_bool/notEmpty");
	}
}

void 
SilenceRemove::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);
	mrs_real rms = 0.0;
	mrs_natural count = 0;

	do 
	{
		marsystems_[0]->process(in, out);

		for (o=0; o < onObservations_; o++)
			for (t = 0; t < onSamples_; t++)
			{
				rms += (out(o,t) * out(o,t));
				count++;
			}
			rms /= count;
			rms = sqrt(rms);
			count = 0;
	} while (rms < threshold_ && (ctrl_notEmpty_->isTrue())); 
}
