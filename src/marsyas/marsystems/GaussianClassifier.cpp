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

#include "GaussianClassifier.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

GaussianClassifier::GaussianClassifier(mrs_string name):MarSystem("GaussianClassifier",name)
{
  prev_mode_= "predict";
  addControls();
}


GaussianClassifier::GaussianClassifier(const GaussianClassifier& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_nClasses_ = getctrl("mrs_natural/nClasses");
  ctrl_means_ = getctrl("mrs_realvec/means");
  ctrl_covars_ = getctrl("mrs_realvec/covars");
  prev_mode_ = "predict";
}


GaussianClassifier::~GaussianClassifier()
{
}


MarSystem*
GaussianClassifier::clone() const
{
  return new GaussianClassifier(*this);
}

void
GaussianClassifier::addControls()
{
  addctrl("mrs_string/mode", "train", ctrl_mode_);
  setctrlState("mrs_string/mode", true);

  addctrl("mrs_natural/nClasses", 1, ctrl_nClasses_);
  setctrlState("mrs_natural/nClasses", true);
  addctrl("mrs_realvec/means", realvec(), ctrl_means_);
  addctrl("mrs_realvec/covars", realvec(), ctrl_covars_);
}


void
GaussianClassifier::myUpdate(MarControlPtr sender)
{
  mrs_natural o;
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("GaussianClassifier.cpp - GaussianClassifier:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)3);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural nClasses = getctrl("mrs_natural/nClasses")->to<mrs_natural>();

  setctrl("mrs_natural/onObservations", (mrs_natural)2 + nClasses);

  mrs_natural mrows = (getctrl("mrs_realvec/means")->to<mrs_realvec>()).getRows();
  mrs_natural mcols = (getctrl("mrs_realvec/means")->to<mrs_realvec>()).getCols();
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  if (active_) {
    // Don't bother resetting anything if this system is inactive.
    // This allows us to load an entire .mpl file without incorrectly
    // resetting the means and covars a bunch of times.

    if ((nClasses != mrows) || (inObservations_ != mcols))
    {
      MarControlAccessor acc_means(ctrl_means_);
      MarControlAccessor acc_covars(ctrl_covars_);

      realvec& means = acc_means.to<mrs_realvec>();
      realvec& covars = acc_covars.to<mrs_realvec>();

      means.create(nClasses, inObservations_);
      covars.create(nClasses, inObservations_);
      labelSizes_.create(nClasses);
    }

    if ((prev_mode_ == "train") && (mode == "predict"))
    {

      MarControlAccessor acc_means(ctrl_means_);
      MarControlAccessor acc_covars(ctrl_covars_);

      realvec& means = acc_means.to<mrs_realvec>();
      realvec& covars = acc_covars.to<mrs_realvec>();


      for (int l=0; l < nClasses; l++)
      {
        for (o=0; o < inObservations_; o++)
        {
          means(l,o) = means(l,o) / labelSizes_(l);
          covars(l,o) = covars(l,o) / labelSizes_(l);
          covars(l, o) = covars(l,o) - (means(l,o) * means(l,o));
          if (covars(l,o) != 0.0)
          {
            covars(l,o) = (mrs_real)(1.0 / covars(l,o));
          }
        }
      }
      prev_mode_ = mode;
    }
  }
}

void
GaussianClassifier::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_real v;
  mrs_string mode = ctrl_mode_->to<mrs_string>();
  mrs_natural nClasses = ctrl_nClasses_->to<mrs_natural>();

  mrs_natural l;
  mrs_natural prediction = 0;
  mrs_real label;

  mrs_real diff;
  mrs_real sq_sum=0.0;

  MarControlAccessor acc_means(ctrl_means_);
  MarControlAccessor acc_covars(ctrl_covars_);
  realvec& means = acc_means.to<mrs_realvec>();
  realvec& covars = acc_covars.to<mrs_realvec>();



  // reset
  if ((prev_mode_ == "predict") && (mode == "train"))
  {
    means.setval(0.0);
    covars.setval(0.0);
    labelSizes_.setval(0.0);
  }

  if (mode == "train")
  {
    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);
      if(label>=0) //unlabeled data (i.e. label = -1) should be ignored
      {
        for (o=0; o < inObservations_-1; o++)
        {
          v = in(o,t);
          means((mrs_natural)label,o) = means((mrs_natural)label,o) + v;
          covars((mrs_natural)label,o) = covars((mrs_natural)label,o) + v*v;
          out(0,t) = (mrs_real)label;
          out(1,t) = (mrs_real)label;
          for (int j=0; j < nClasses; j++)
          {
            out(j,t) = (mrs_real)0;
            if (j == label)
              out(j,t) = (mrs_real)1;
          }
        }
        labelSizes_((mrs_natural)label) = labelSizes_((mrs_natural)label) + 1;
      }
    }
  }

  if (mode == "predict")
  {

    mrs_real min = MAXREAL;

    for (t = 0; t < inSamples_; t++)
    {
      label = in(inObservations_-1, t);

      for (l=0; l < nClasses; l++)
      {
        sq_sum = 0.0;

        for (o=0; o < inObservations_-1; o++)
        {
          v = in(o,t);
          diff = (v - means(l,o));
          sq_sum += (diff * covars(l,o) * diff);
        }

        if (sq_sum < min)
        {
          min = sq_sum;
          prediction = l;
        }

        // output the distances in the probability slots
        out (2+l, t) = sq_sum;
      }
      out(0,t) = (mrs_real)prediction;
      out(1,t) = (mrs_real)label;
    }
  }

  prev_mode_ = mode;
}
