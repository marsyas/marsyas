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
  //type_ = "Annotator";
  //name_ = name;

	addControls();
}


Annotator::Annotator(const Annotator& a):MarSystem(a) 
{
  ctrl_label_ = getctrl("mrs_natural/label");
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
  addctrl("mrs_natural/label", 0, ctrl_label_);
  // addctrl("mrs_string/labels","");
  // setctrlState("mrs_string/labels",true);
  labels_str_ = "";
  labels_index_ = 0; 
}


void
Annotator::myUpdate(MarControlPtr sender)
{
	(void) sender;
  MRSDIAG("Annotator.cpp - Annotator:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>()+1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);  


  /* if( labels_str_.compare( getctrl("mrs_string/labels")->to<mrs_string>() ) != 0 )
  {     
  labels_str_ = getctrl("mrs_string/labels")->to<mrs_string>();
  labels_.clear();
	  
	  while( labels_str_.length() != 0 )
	  {
	  mrs_natural i = labels_str_.find(",");
	  labels_.push_back( strtol( labels_str_.substr(0, i).c_str() , NULL , 10 ) );
	  labels_str_ = labels_str_.substr( i+1 , labels_str_.length()-i-1 );
	  }
	  
	  labels_index_ = 0;
  }
   */ 
}


void 
Annotator::myProcess(realvec& in, realvec& out)
{

  const mrs_natural& label = ctrl_label_->to<mrs_natural>();
  
  for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    out(o,t) =  in(o,t);
	  }
	
  for (t=0; t < inSamples_; t++) 
  {
	// if( labels_.size() == 0 )
	// {
		out(onObservations_-1, t) = (mrs_real)label;
	// }
	// else
	// {
	//	out(onObservations_-1, t) = (mrs_real)labels_[labels_index_];
	// 	labels_index_ = (labels_index_+1) % labels_.size();
	//}
  } 
}







	
	
