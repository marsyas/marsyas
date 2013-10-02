/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.princeton.edu>
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

#include "ZeroRClassifier.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

ZeroRClassifier::ZeroRClassifier(mrs_string name):MarSystem("ZeroRClassifier",name)
{
  addControls();
}

ZeroRClassifier::~ZeroRClassifier()
{
}

MarSystem*
ZeroRClassifier::clone() const
{
  return new ZeroRClassifier(*this);
}

void
ZeroRClassifier::addControls()
{
  addctrl("mrs_string/mode", "train");
  addctrl("mrs_natural/nClasses", 1);
  setctrlState("mrs_natural/nClasses", true);
  addctrl("mrs_bool/done", false);
  addctrl("mrs_natural/prediction", 0);
}

void
ZeroRClassifier::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("ZeroRClassifier.cpp - ZeroRClassifier:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural nlabels = getctrl("mrs_natural/nClasses")->to<mrs_natural>();

  if ((mrs_natural)labelSizes_.getSize() != nlabels)
    labelSizes_.create(nlabels);
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();
  if (mode == "predict")
  {

  }
}

void
ZeroRClassifier::myProcess(realvec& in, realvec& out)
{
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();
  mrs_natural nlabels = getctrl("mrs_natural/nClasses")->to<mrs_natural>();
  mrs_natural l, t;
  mrs_natural prediction = 0;

  mrs_real label;

  if ((prev_mode_ == "predict") && (mode == "train"))
  {
    labelSizes_.setval(0.0);
  }

  if (mode == "train")
  {
    for (t=0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);
      if(label >= 0)
      {
        labelSizes_((int)label) = labelSizes_((int)label) + 1;
      }
      out(0,t) = label;
      out(1,t) = label;
    }
  }

  if ((prev_mode_ == "train") && (mode == "predict"))
  {
    int max = -1;
    for (l=0; l < nlabels; l++)
    {
      if (labelSizes_(l) > max)
      {
        prediction = l;
        max = (int)labelSizes_(l);
      }
    }
    updControl("mrs_natural/prediction", prediction);
  }

  if (mode == "predict")
  {
    for (t=0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);
      prediction = getctrl("mrs_natural/prediction")->to<mrs_natural>();
      out(0,t) = (mrs_real)prediction;
      out(1,t) = label;
    }
  }
  prev_mode_ = mode;
}
