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
    \class ClassOutputSink
    \brief ClassOutputSink

*/



#include "ClassOutputSink.h"
using namespace std;



ClassOutputSink::ClassOutputSink(string name)
{
  type_ = "ClassOutputSink";
  name_ = name;
  mos_ = NULL;
  addControls();
}


ClassOutputSink::~ClassOutputSink()
{
  if (mos_ != NULL) 
    mos_->close();
  delete mos_;
}




MarSystem* 
ClassOutputSink::clone() const 
{
  return new ClassOutputSink(*this);
}

void 
ClassOutputSink::addControls()
{
  addDefaultControls();
  addctrl("natural/memSize", 40);
  addctrl("natural/nLabels", 2);

  addctrl("string/filename", "mugle.mf");
  setctrlState("string/filename", true);
  setctrlState("natural/nLabels", true);
  addctrl("string/labelNames", "Music,Speech");
  setctrlState("string/labelNames", true);
  addctrl("bool/silent", true);
}


void 
ClassOutputSink::putHeader()
{
  if ((filename_ != getctrl("string/filename").toString()))
    {
      if (mos_ != NULL) 
	{
	  mos_->close();
	  delete mos_;
	  if (filename_ == "mugle.mf")
	    remove(filename_.c_str());
	}

      
      filename_ = getctrl("string/filename").toString();
  
      mos_ = new ofstream;
      mos_->open(filename_.c_str());
      
    }
}


void
ClassOutputSink::update()
{
  MRSDIAG("ClassOutputSink.cpp - ClassOutputSink:update");
  cout << "ClassOutputSink::update" << endl;
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  string labelNames = getctrl("string/labelNames").toString();
  
  labelNames_.clear();

  string temp;  

  for (int i = 0; i < getctrl("natural/nLabels").toNatural(); i++)
    {
      string labelName;

      
      labelName = labelNames.substr(0, labelNames.find(","));
      temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
      labelNames = temp;
      labelNames_.push_back(labelName);
    }  
  count_ = 0;

  
  putHeader();
  defaultUpdate();
}


void 
ClassOutputSink::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  natural p;
  natural g;
  
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) = in(o,t);
      }
  
  if (!getctrl("bool/silent").toBool())
    {
      for (o = 0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)	
	  {
	    p = (natural)in(o,t);
	    g = (natural)in(1,t);
	    if (o < inObservations_-1)
	      cout << "Predicted: " << labelNames_[p] << endl;
	    (*mos_) << labelNames_[p] << endl;
	  }
    }
  
}







	

	

	
