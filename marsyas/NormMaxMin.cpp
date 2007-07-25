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
\class NormMaxMin
\ingroup Processing
\brief Normalize my mapping min/max range to user specified range


*/

#include "NormMaxMin.h"

using namespace std;
using namespace Marsyas;

NormMaxMin::NormMaxMin(string name):MarSystem("NormMaxMin",name)
{

	init_ = false;

	addControls();
}

NormMaxMin::NormMaxMin(const NormMaxMin& a): MarSystem(a) 
{
	lowerPtr_ = getctrl("mrs_real/lower");
	upperPtr_ = getctrl("mrs_real/upper");
	trainPtr_ = getctrl("mrs_bool/train");
	initPtr_ = getctrl("mrs_bool/init");
	maximumsPtr_ = getctrl("mrs_realvec/maximums");
	minimumsPtr_ = getctrl("mrs_realvec/minimums");
}

NormMaxMin::~NormMaxMin()
{
}

MarSystem* 
NormMaxMin::clone() const 
{
	return new NormMaxMin(*this);
}

void 
NormMaxMin::addControls()
{
	addctrl("mrs_real/lower", 0.0, lowerPtr_);
	addctrl("mrs_real/upper", 1.0, upperPtr_);
	maximums_.create(1);
	minimums_.create(1);
	addctrl("mrs_realvec/maximums", maximums_, maximumsPtr_);
	addctrl("mrs_realvec/minimums", minimums_, minimumsPtr_);
	setctrlState("mrs_realvec/maximums", true);
	setctrlState("mrs_realvec/minimums", true);

	addctrl("mrs_bool/train", true, trainPtr_);
	setctrlState("mrs_bool/train", true);

	addctrl("mrs_bool/init", false, initPtr_);
	setctrlState("mrs_bool/init", true);
}

void
NormMaxMin::myUpdate(MarControlPtr sender)
{
	MRSDIAG("NormMaxMin.cpp - NormMaxMin:myUpdate");

	MarSystem::myUpdate(sender);

	inObservations_ = ctrl_inObservations_->toNatural();

	init_ = initPtr_->toBool();

	mrs_natural msize = maximumsPtr_->toVec().getSize();
	mrs_natural nsize = maximums_.getSize();

	if (msize != nsize) 
	{
		maximums_.stretch(msize);
		minimums_.stretch(msize);
	}

	if (!init_)
	{
		maximums_.stretch(inObservations_);
		minimums_.stretch(inObservations_);

		maximums_.setval(DBL_MIN);
		minimums_.setval(DBL_MAX);
		maximumsPtr_->setValue(maximums_, NOUPDATE);
		minimumsPtr_->setValue(minimums_, NOUPDATE);  
	}
	train_ = trainPtr_->toBool();

	if (!train_)
	{
		maximums_ = maximumsPtr_->toVec();
		minimums_ = minimumsPtr_->toVec();
	} 
}

void 
NormMaxMin::myProcess(realvec& in, realvec& out)
{
	init_ = true;
	setctrl(initPtr_, init_);

	lower_ = lowerPtr_->toReal();
	upper_ = upperPtr_->toReal();
	train_ = trainPtr_->toBool();

	if (lower_ > upper_) 
	{
		MRSWARN("Lower is greater than upper");
		return;
	}

	range_ = upper_ - lower_;

	if (train_)
	{
		// first pass calculate min/max limits
		for (o=0; o < inObservations_; o++)
			for (t = 0; t < inSamples_; t++)
			{
				if (in(o,t) > maximums_(o))
					maximums_(o) = in(o,t);
				if (in(o,t) < minimums_(o))	
					minimums_(o) = in(o,t);
				out(o,t) = in(o,t);

			}

			setctrl(maximumsPtr_, maximums_);
			setctrl(minimumsPtr_, minimums_);  
	}
	else
	{
		// second pass for normalization 
		for (o=0; o < inObservations_; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o,t) = lower_ + range_ * ((in(o,t) - minimums_(o)) / (maximums_(o) - minimums_(o)));
			}

	}
}









