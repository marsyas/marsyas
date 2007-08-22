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

using namespace std;
using namespace Marsyas;

GaussianClassifier::GaussianClassifier(string name):MarSystem("GaussianClassifier",name)
{
  prev_mode_= "predict";
  addControls();
}


GaussianClassifier::GaussianClassifier(const GaussianClassifier& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
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
  addctrl("mrs_natural/nLabels", 1, ctrl_nLabels_);
  setctrlState("mrs_natural/nLabels", true);
  means_.create(1);
  covars_.create(1);
  addctrl("mrs_realvec/means", means_, ctrl_means_);
  addctrl("mrs_realvec/covars", covars_, ctrl_covars_);
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);
}


void
GaussianClassifier::myUpdate(MarControlPtr sender)
{
  MRSDIAG("GaussianClassifier.cpp - GaussianClassifier:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->toNatural();
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();

  mrs_natural mrows = (getctrl("mrs_realvec/means")->toVec()).getRows();
  mrs_natural mcols = (getctrl("mrs_realvec/means")->toVec()).getCols();
  mrs_natural nrows = means_.getRows();
  mrs_natural ncols = means_.getCols();

  if ((nlabels != mrows) || 
      (inObservations != mcols))
    {
      means_.create(nlabels, inObservations);
      covars_.create(nlabels, inObservations);
      updctrl("mrs_realvec/means", means_);//[?]
      updctrl("mrs_realvec/covars", covars_);//[?]      
      labelSizes_.create(nlabels);  
    }

  if ((nlabels != nrows) || 
      (inObservations != ncols))
    {
      means_.create(nlabels, inObservations);
      covars_.create(nlabels, inObservations);
    }

  string mode = getctrl("mrs_string/mode")->toString();
  if (mode == "predict")
    {
      means_ = getctrl("mrs_realvec/means")->toVec();
      covars_ = getctrl("mrs_realvec/covars")->toVec();
    }
}

void 
GaussianClassifier::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_real v;
  string mode = ctrl_mode_->to<string>();
  mrs_natural nlabels = ctrl_nLabels_->to<mrs_natural>();
  
  mrs_natural l;
  mrs_natural prediction = 0;
  mrs_real label;

  mrs_real diff;
  mrs_real sq_sum=0.0;

  if (mode == "train")  
    {
      for (t = 0; t < inSamples_; t++)  
	{
	  label = in(inObservations_-1, t);


	  for (o=0; o < inObservations_-1; o++)
	    {
	      v = in(o,t);

	      means_((mrs_natural)label,o) = means_((mrs_natural)label,o) + v;
	      covars_((mrs_natural)label,o) = covars_((mrs_natural)label,o) + v*v;
	      out(0,t) = (mrs_real)label;	      
	      out(1,t) = (mrs_real)label;

	    }
	  labelSizes_((mrs_natural)label) = labelSizes_((mrs_natural)label) + 1;

	}
    }


  if ((prev_mode_ == "train") && (mode == "predict"))
    {
      
      for (l=0; l < nlabels; l++)
	for (o=0; o < inObservations_; o++)
	  {
	    
	    means_(l,o) = means_(l,o) / labelSizes_(l);
	    covars_(l,o) = covars_(l,o) / labelSizes_(l);
	    covars_(l, o) = covars_(l,o) - 
	      (means_(l,o) * means_(l,o));
	    if (covars_(l,o) != 0.0)
	      {
		covars_(l,o) = (mrs_real)(1.0 / covars_(l,o));
	      }
	  }
      
      ctrl_means_->setValue(means_);
      ctrl_covars_->setValue(covars_);
    }




  if (mode == "predict")
    {
      mrs_real min = MAXREAL;
      for (t = 0; t < inSamples_; t++)  
	{
	  label = in(inObservations_-1, t);

	  for (l=0; l < nlabels; l++)
	    {
	      sq_sum = 0.0;

	      for (o=0; o < inObservations_-1; o++)
		{
		  v = in(o,t);
		  diff = (v - means_(l,o));

		  
		  sq_sum += (diff * covars_(l,o) * diff);
		}
	      if (sq_sum < min)
		{
		  min = sq_sum;
		  prediction = l;
		}
	    }
	  
	  out(0,t) = (mrs_real)prediction;
	  out(1,t) = (mrs_real)label;
	}
    }
  
  prev_mode_ = mode;



}







	

	
