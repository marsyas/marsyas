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

#include "Annotator.h"

using namespace std;
using namespace Marsyas;

Annotator::Annotator(string name):MarSystem("Annotator", name)
{
	addControls();
}


Annotator::Annotator(const Annotator& a):MarSystem(a)
{
	ctrl_label_ = getControl("mrs_natural/label");
	ctrl_labelInFront_ = getControl("mrs_bool/labelInFront");
}

Annotator::~Annotator()
{
}

MarSystem*
Annotator::clone() const
{
	return new Annotator(*this);
}

void
Annotator::addControls()
{
	addControl("mrs_natural/label", 0, ctrl_label_);
	addControl("mrs_bool/labelInFront", false, ctrl_labelInFront_);
}

void
Annotator::myUpdate(MarControlPtr sender)
{
	MRSDIAG("Annotator.cpp - Annotator:myUpdate");

	// Do the default MarSystem configuration ...
	MarSystem::myUpdate(sender);

	// ... but add one to the number of output observations.
	ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() + 1, NOUPDATE);

}

void
Annotator::myProcess(realvec& in, realvec& out)
{
	// Get the label to annotate the feature stream with.
	const mrs_natural& label = ctrl_label_->to<mrs_natural>();

	// Should the label go in front or at the back of the observations?
	mrs_bool labelInFront = ctrl_labelInFront_->to<mrs_bool>();

	// Copy the input observations to the output and add the label.
	for (t = 0; t < inSamples_; t++)
	{
		for (o = 0; o < inObservations_; o++)
		{
			out((int)(labelInFront) + o, t) =  in(o, t);
		}
		out(labelInFront ? 0 : onObservations_ - 1, t) = (mrs_real) label;
	}
}
