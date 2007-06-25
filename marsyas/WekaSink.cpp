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
	\class WekaSink
	\ingroup IO
	\brief Output sink (text) in Weka format

	Controls:
	- \b mrs_natural/precision [w] : precision of numbers to output.
	- \b mrs_string/filename [w] : name of output file.
	- \b mrs_natural/nLabels [rw] : number of labels.
	- \b mrs_natural/downsample [rw] : divides number of samples.
	- \b mrs_string/labelNames [rw] : names of labels.
	- \b mrs_bool/regresssion [rw] : classification or regression?
	- \b mrs_bool/putHeader [rw] : add extra information to the .arff
	  file.
*/

#include "WekaSink.h"

using namespace std;
using namespace Marsyas;

WekaSink::WekaSink(string name):MarSystem("WekaSink",name)
{
  mos_ = NULL;
  addControls();
}

WekaSink::~WekaSink()
{
  if (mos_ != NULL) 
    mos_->close();
  delete mos_;
}

WekaSink::WekaSink(const WekaSink& a):MarSystem(a)
{
  mos_ = NULL;

  ctrl_regression_ = getControl("mrs_bool/regression");
  ctrl_putHeader_ = getControl("mrs_bool/putHeader");
  ctrl_labelNames_ = getControl("mrs_string/labelNames");
  ctrl_nLabels_ = getControl("mrs_natural/nLabels");
  ctrl_precision_ = getControl("mrs_natural/precision");
  ctrl_downsample_ = getControl("mrs_natural/downsample");
  ctrl_filename_ = getControl("mrs_string/filename"); 
}

MarSystem* 
WekaSink::clone() const 
{
  return new WekaSink(*this);
}

void
WekaSink::addControls()
{
  addctrl("mrs_natural/precision", 6, ctrl_precision_);
  setctrlState("mrs_natural/precision", true);
  addctrl("mrs_string/filename", "weka.arff", ctrl_filename_);
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/nLabels", 2, ctrl_nLabels_);
  addctrl("mrs_natural/downsample", 1, ctrl_downsample_);
  setctrlState("mrs_natural/downsample", true);
  addctrl("mrs_string/labelNames", "Music,Speech", ctrl_labelNames_);
  setctrlState("mrs_string/labelNames", true);

  addctrl("mrs_bool/regression", false, ctrl_regression_);

  addControl("mrs_bool/putHeader", false, ctrl_putHeader_);
  setctrlState(ctrl_putHeader_, true);
}

void 
WekaSink::putHeader(string inObsNames)
{
  //updctrl(ctrl_putHeader_, false);
  ctrl_putHeader_->setValue(true);
  
  if ((filename_ != ctrl_filename_->toString()))
    {
      if (mos_ != NULL) 
	{
	  mos_->close();
	  delete mos_;
	  if (filename_ == "weka.arff")
	    remove(filename_.c_str());
	}
      filename_ = ctrl_filename_->toString();
      
      mos_ = new ofstream;
      mos_->open(filename_.c_str());
      
      (*mos_) << "@relation marsyas" << endl;
      mrs_natural nAttributes = ctrl_inObservations_->toNatural()-1;
      mrs_natural nLabels = ctrl_nLabels_->toNatural();
      
      mrs_natural i;
      for (i =0; i < nAttributes; i++)
	{
	  string inObsName;
	  string temp;
	  inObsName = inObsNames.substr(0, inObsNames.find(","));
	  temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
	  inObsNames = temp;
	  ostringstream oss;
	  // oss << "attribute" << i; 
	  oss << inObsName;
	  (*mos_) << "@attribute " << oss.str() << " real" << endl;
	}
      
      if (!ctrl_regression_->isTrue()) 
	{
	  (*mos_) << "@attribute output {";
	  for (i=0; i < nLabels; i++) 
	    {
	      ostringstream oss;
	      // oss << "label" << i;
	      oss << labelNames_[i];
	      (*mos_) << oss.str();
	      if (i < nLabels-1)
		(*mos_) << ",";
	      // (*mos_) << "@attribute output {music,speech}" << endl;
	    }
	  (*mos_) << "}" << endl;
	}
      else 
	{
	  (*mos_) << "@attribute output real" << endl;
	}
      (*mos_) << "\n\n@data" << endl;
    }
}

void
WekaSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("WekaSink.cpp - WekaSink:myUpdate");
  
  MarSystem::myUpdate(sender);
  
  
  string labelNames = ctrl_labelNames_->toString();
  
  labelNames_.clear();
  
  for (int i = 0; i < ctrl_nLabels_->toNatural(); i++)
    {
      string labelName;
      string temp;
      
      labelName = labelNames.substr(0, labelNames.find(","));
      temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
      labelNames = temp;
      labelNames_.push_back(labelName);
    }
  
  string onObsNames = ctrl_onObsNames_->toString();
  
  //if(!(getctrl("mrs_bool/mute")->toBool()))
  if(!ctrl_mute_->isTrue())
    putHeader(onObsNames);
  
  precision_ = ctrl_precision_->toNatural();
  downsample_ = ctrl_downsample_->toNatural();
}

void 
WekaSink::myProcess(realvec& in, realvec& out)
{
  //if (mute_) copy input to output
  //if(getctrl("mrs_bool/mute")->toBool())
  if(ctrl_mute_->isTrue())
    {
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    out(o,t) =  in(o,t);
	  }
      return;
    }
  
  static int count = 0;
  mrs_natural label = 0;
  mrs_bool notPrint = false;
  
  
  for (t = 0; t < inSamples_; t++)
    {
      for (o=0; o < inObservations_; o++)
	{
	  out(o,t) = in(o,t);
	  if (o < inObservations_-1)
	    {
	      if ((count % downsample_) == 0)
		{
		  if( out(o,t) != out(o,t) )	// Jen's NaN check for MIREX 05
		    {
		      // (*mos_) << fixed << setprecision(precision_) << 0. << ",";
		      // DO NOT OUTPUT FEATUERS 
		      // (*mos_) << fixed << setprecision(precision_) << 0. << ",";
		      notPrint = true;
		    }
		  
		  else
		    {
		      (*mos_) << fixed << setprecision(precision_) << out(o,t) << ",";
		      notPrint = false;
		    }
		}
	    }
	}
      
      label = (mrs_natural)in(inObservations_-1, t);
      ostringstream oss;
      if ((count % downsample_) == 0)
	{
	  if(!ctrl_regression_->isTrue())
	    {
	      if (!notPrint) 
		{
		  oss << labelNames_[label];
		  (*mos_) << oss.str();
		  (*mos_) << endl;
		}
	      else 
		{
		  cout << "skipping instance" << endl;
		}
	      
	      
	    }
	  else
	    {
	      (*mos_) << in(inObservations_-1, t);
	      (*mos_) << endl;
	    }
	}
    }
  count++;
}
