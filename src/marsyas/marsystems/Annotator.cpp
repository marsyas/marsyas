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
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Annotator::Annotator(mrs_string name):MarSystem("Annotator", name)
{
  addControls();
}


Annotator::Annotator(const Annotator& a):MarSystem(a)
{
  ctrl_label_ = getControl("mrs_real/label");
  ctrl_labelInFront_ = getControl("mrs_bool/labelInFront");
  ctrl_annotationName_ = getControl("mrs_string/annotationName");
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
  addControl("mrs_real/label", 0.0, ctrl_label_);
  addControl("mrs_bool/labelInFront", false, ctrl_labelInFront_);
  addControl("mrs_string/annotationName", "annotation", ctrl_annotationName_);
}

void
Annotator::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Annotator.cpp - Annotator:myUpdate");

  // Do the default MarSystem configuration ...
  MarSystem::myUpdate(sender);

  // ... but add one to the number of output observations.
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() + 1, NOUPDATE);

  // Should the label go in front or at the back of the observations?
  labelInFront_ = ctrl_labelInFront_->to<mrs_bool>();


  // Add the annotation name to onObsNames.
  mrs_string annotationName = ctrl_annotationName_->to<mrs_string>();
  mrs_string onObsNames = ctrl_inObsNames_->to<mrs_string>();
  if (labelInFront_)
  {
    onObsNames = annotationName + mrs_string(",") + onObsNames;
  }
  else
  {
    onObsNames = onObsNames + mrs_string(",") + annotationName;
  }
  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

}

void
Annotator::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // Get the label to annotate the feature stream with.
  const mrs_real& label = ctrl_label_->to<mrs_real>();

  // Copy the input observations to the output and add the label.
  for (t = 0; t < inSamples_; t++)
  {
    for (o = 0; o < inObservations_; o++)
    {
      out((int)(labelInFront_) + o, t) =  in(o, t);
    }
    out(labelInFront_ ? 0 : onObservations_ - 1, t) = label;
  }
}
