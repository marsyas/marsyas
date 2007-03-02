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

/** 
    \class ZeroRClassifier

    When the mode control is set to "predict" then the output 
    of the classifier will output the class with the most instances. 

    This MarSystems serves as ground truth for classification/regression 
    MarSystem results.

*/

#include "ZeroRClassifier.h"

using namespace std;
using namespace Marsyas;

ZeroRClassifier::ZeroRClassifier(string name):MarSystem("ZeroRClassifier",name)
{
  //type_ = "ZeroRClassifier";
  //name_ = name;

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
  addctrl("mrs_natural/nLabels", 1);
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_bool/done", false);
  addctrl("mrs_natural/prediction", 0);
}

void
ZeroRClassifier::myUpdate(MarControlPtr sender)
{
  MRSDIAG("ZeroRClassifier.cpp - ZeroRClassifier:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
   
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();

  if (labelSizes_.getSize() != nlabels)
    labelSizes_.create(nlabels);  
  string mode = getctrl("mrs_string/mode")->toString();
  if (mode == "predict")
    {
    
    }
}

void 
ZeroRClassifier::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  string mode = getctrl("mrs_string/mode")->toString();
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();
  mrs_natural l;
  mrs_natural prediction = 0;
  
  mrs_real label;
  
  if (mode == "train")  
    {
      for (t=0; t < inSamples_; t++)
	{
	  label = in(inObservations_-1, t);	
	  labelSizes_((int)label) = labelSizes_((int)label) + 1;
	  out(0,t) = label;
	  out(1,t) = label;
	}
    }


  if (mode == "predict")
    {
      for (t=0; t < inSamples_; t++)
	{    
	  label = in(inObservations_-1, t);	  
	  prediction = getctrl("mrs_natural/prediction")->toNatural();
	  out(0,t) = (mrs_real)prediction;
	  out(1,t) = label;
	}

    }
  
  if (getctrl("mrs_bool/done")->toBool())
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
      updctrl("mrs_natural/prediction", prediction);
    }
}







	

	
