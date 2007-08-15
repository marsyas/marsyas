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

#include "Confidence.h"
#include "FileName.h"

using namespace std;
using namespace Marsyas;

Confidence::Confidence(string name):MarSystem("Confidence",name)
{
  //type_ = "Confidence";
  //name_ = name;
  
  print_ = false;
  forcePrint_ = false;
  
  predictions_ = 0;
  count_ = 0;
  write_=0;
  oriName_ = "MARSYAS_EMPTY";
  addControls();
}


Confidence::Confidence(const Confidence& a):MarSystem(a)
{
  ctrl_memSize_ = getctrl("mrs_natural/memSize");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  count_ = 0;
  print_ = false;
  forcePrint_ = false;
  write_=0;
  oriName_ = "MARSYAS_EMPTY";
}



Confidence::~Confidence()
{
}


MarSystem* 
Confidence::clone() const 
{
  return new Confidence(*this);
} 

void 
Confidence::addControls()
{
  addctrl("mrs_natural/memSize", 40, ctrl_memSize_);
  addctrl("mrs_natural/nLabels", 2, ctrl_nLabels_);
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_string/labelNames", "Music,Speech");
  setctrlState("mrs_string/labelNames", true);
  addctrl("mrs_bool/print", false);
  setctrlState("mrs_bool/print", true);
  addctrl("mrs_bool/forcePrint", false);
  setctrlState("mrs_bool/forcePrint", true);
  addctrl("mrs_string/fileName", "MARSYAS_EMPTY");
  setctrlState("mrs_string/fileName", true);
  addctrl("mrs_natural/write", 0);
  setctrlState("mrs_natural/write", true);
  addctrl("mrs_natural/hopSize", 512);
  setctrlState("mrs_natural/hopSize", true);
}

void
Confidence::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Confidence.cpp - Confidence:myUpdate");
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  confidences_.stretch(getctrl("mrs_natural/nLabels")->toNatural());
  string labelNames = getctrl("mrs_string/labelNames")->toString();
  
  labelNames_.clear();

  print_ = getctrl("mrs_bool/print")->toBool();
  forcePrint_ = getctrl("mrs_bool/forcePrint")->toBool();
    
  for (mrs_natural i = 0; i < getctrl("mrs_natural/nLabels")->toNatural(); i++)
    {
      string labelName;
      string temp;
      
      labelName = labelNames.substr(0, labelNames.find(","));
      temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
      labelNames = temp;
      labelNames_.push_back(labelName);
    }  


  if(getctrl("mrs_string/fileName")->toString().compare(oriName_))
    {
      if(write_)
	{
	  outputFileSyn_.close();
	  outputFileTran_.close();
	}
      oriName_ = getctrl("mrs_string/fileName")->toString();
      FileName Sfname(oriName_);
      string tmp = Sfname.nameNoExt() +"_synSeg.txt";
      cout << Sfname.nameNoExt() << endl;
      //      getchar();
      outputFileSyn_.open(tmp.c_str(), ios::out);
      tmp = Sfname.nameNoExt() +"_tranSeg.txt";
      outputFileTran_.open(tmp.c_str(), ios::out);
      write_ = 1;
  }

  hopDuration_ = getctrl("mrs_natural/hopSize")->toNatural() / getctrl("mrs_real/osrate")->toReal();
  nbFrames_ = -getctrl("mrs_natural/memSize")->toNatural()+1;
  lastLabel_ = "MARSYAS_EMPTY";
}

void 
Confidence::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  bool mute = ctrl_mute_->to<mrs_bool>();
  mrs_natural memSize = ctrl_memSize_->to<mrs_natural>();
  mrs_natural nLabels = ctrl_nLabels_->to<mrs_natural>();
  
  mrs_natural label;
  mrs_natural l;

   if (mute == false) 
    {
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    out(o,t) = in(o,t);
	    if (o==0) 
	      {
		label = (mrs_natural)in(o,t);
		// cout << "Label = " << label << endl;
		confidences_(label) = confidences_(label) + 1;
	      } 
	  }
      count_++;
      bool cond = ((count_ % memSize) == 0);
      if (cond || forcePrint_)
	{
	  mrs_real max_conf = 0;
	  mrs_natural max_l = 0;
	  for (l=0; l < nLabels; l++)
	    {
	      mrs_real conf = ((confidences_(l)) / count_);
	      if (conf > max_conf) 
		{
		  max_conf = conf;
		  max_l = l;
		}
	    }
	  if (print_) 
	    cout << nbFrames_*hopDuration_ << "\t" << labelNames_[max_l] << "\t" << 
	      ((confidences_(max_l) / count_)) * 100.0 << endl;
          if (write_)
	    { 
	    outputFileSyn_ << nbFrames_*hopDuration_ << "\t" << labelNames_[max_l] << "\t" << 
	      ((confidences_(max_l) / count_)) * 100.0 << endl;

	    if(lastLabel_ == "MARSYAS_EMPTY" || lastLabel_ != labelNames_[max_l])
	      {
	    outputFileTran_ << nbFrames_*hopDuration_ << "\t" << labelNames_[max_l] << endl;
	    lastLabel_ = labelNames_[max_l];
	      }
	    }
	  if (cond || forcePrint_)
	    {
	      count_ = 0;
	    }
	    
	  confidences_.setval(0.0);
	}
    }
    nbFrames_++; 
 }







	

	
