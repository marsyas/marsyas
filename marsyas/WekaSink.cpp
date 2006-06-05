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
   \brief Output sink (text) in Weka format

*/


#include "WekaSink.h"
using namespace std;




WekaSink::WekaSink(string name)
{
  type_ = "WekaSink";
  name_ = name;

  mos_ = NULL;
  addControls();
}


WekaSink::~WekaSink()
{
  
  if (mos_ != NULL) 
    mos_->close();
  delete mos_;

}


WekaSink::WekaSink(const WekaSink& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  
  inSamples_ = a.inSamples_;
  inObservations_ = a.inObservations_;
  onSamples_ = a.onSamples_;
  onObservations_ = a.onObservations_;
  dbg_ = a.dbg_;
  mute_ = a.mute_;
  
  
  mos_ = NULL;


}



MarSystem* 
WekaSink::clone() const 
{
  return new WekaSink(*this);
}

void 
WekaSink::addControls()
{
  addDefaultControls();
  addctrl("natural/precision", 6);
  setctrlState("natural/precision", true);
  addctrl("string/filename", "weka.arff");
  setctrlState("string/filename", true);
  addctrl("natural/nLabels", 2);
  addctrl("natural/downsample", 1);
  setctrlState("natural/downsample", true);
  addctrl("string/labelNames", "Music,Speech");
  setctrlState("string/labelNames", true);
}

void 
WekaSink::putHeader(string inObsNames)
{
  if ((filename_ != getctrl("string/filename").toString()))
    {
      if (mos_ != NULL) 
	{
	  mos_->close();
	  delete mos_;
	  if (filename_ == "weka.arff")
	    remove(filename_.c_str());
	}

      
      filename_ = getctrl("string/filename").toString();
  

      

      mos_ = new ofstream;
      mos_->open(filename_.c_str());

      (*mos_) << "@relation marsyas" << endl;
      natural nAttributes = getctrl("natural/inObservations").toNatural()-1;
      natural nLabels = getctrl("natural/nLabels").toNatural();
      
      natural i;
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
      (*mos_) << "\n\n@data" << endl;
    }
  
}



void
WekaSink::update()
{
  MRSDIAG("WekaSink.cpp - WekaSink:update");
  


  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));

  setctrl("string/onObsNames", getctrl("string/inObsNames"));


  string labelNames = getctrl("string/labelNames").toString();
  
  labelNames_.clear();
  
  for (int i = 0; i < getctrl("natural/nLabels").toNatural(); i++)
    {
      string labelName;
      string temp;
      
      labelName = labelNames.substr(0, labelNames.find(","));
      temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
      labelNames = temp;
      labelNames_.push_back(labelName);
    }


  string onObsNames = getctrl("string/inObsNames").toString();

  mute_ = getctrl("bool/mute").toBool();
  
  if (!mute_) 
    putHeader(onObsNames);

  precision_ = getctrl("natural/precision").toNatural();
  downsample_ = getctrl("natural/downsample").toNatural();
  defaultUpdate();
}




void 
WekaSink::process(realvec& in, realvec& out)
{

  if (mute_) 				// copy input to output 
    {
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    out(o,t) =  in(o,t);
	  }
      return;
    }
  

  static int count = 0;
  
  checkFlow(in,out);


  natural label = 0;
  
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
		  	(*mos_) << fixed << setprecision(precision_) << 0. << ",";
		  else
		  	(*mos_) << fixed << setprecision(precision_) << out(o,t) << ",";
		}
	    }
	}
      
      
      label = (natural)in(inObservations_-1, t);

      
      ostringstream oss;
      if ((count % downsample_) == 0)
	{
	  oss << labelNames_[label];
	  (*mos_) << oss.str();
	  (*mos_) << endl;
	}
    }
  
  
  count++;
}
















	
	
	
