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
    \class Series
    \brief Series of MarSystem objects
    
    Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 
*/


#include "Series.h"
using namespace std;


Series::Series():Composite()
{
  type_ = "Series";
}


Series::Series(string name)
{
  type_ = "Series";
  name_ = name;
  
  dbg_ = 0;
  mute_ = 0;
  addControls();
}




Series::Series(const Series& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  synonyms_ = a.synonyms_;
  

  for (natural i=0; i< a.marsystemsSize_; i++)
    {
      addMarSystem((*a.marsystems_[i]).clone());
    }
  dbg_ = a.dbg_;
  mute_ = a.mute_;
}



Series::~Series()
{
  deleteSlices();
}




MarSystem* 
Series::clone() const 
{
  return new Series(*this);
}


void 
Series::addControls()
{
  addDefaultControls();
  addctrl("bool/probe", false);
  setctrlState("bool/probe", true);

  
	  
}


void 
Series::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); ++iter)
    {
      delete *(iter);
    }
  slices_.clear();
}


// STU
real* 
const Series::recvControls()
{
	if ( marsystemsSize_ != 0 ) {
		if (marsystems_[0]->getType() == "NetworkTCPSource" ) {
			return marsystems_[0]->recvControls();
		}
	}
	return 0;
}


void 
Series::update()
{

  probe_ = getctrl("bool/probe").toBool();
  
  if (marsystemsSize_ != 0) 
    {


      // update dataflow component MarSystems in order 
      marsystems_[0]->update();      
      
      
      for (natural i=1; i < marsystemsSize_; i++)
	{
	  marsystems_[i]->updctrl("string/inObsNames", marsystems_[i-1]->getctrl("string/onObsNames"));
	  

	  marsystems_[i]->updctrl("natural/inSamples", 
				  marsystems_[i-1]->getctrl("natural/onSamples"));
	  marsystems_[i]->updctrl("natural/inObservations", 
				  marsystems_[i-1]->getctrl("natural/onObservations"));
	  marsystems_[i]->updctrl("real/israte", 
				  marsystems_[i-1]->getctrl("real/osrate"));
	  
	  marsystems_[i]->update();
	}
      
      
      // set controls based on first and last marsystem 
      setctrl("string/inObsNames", marsystems_[0]->getctrl("string/inObsNames"));
      setctrl("natural/inSamples", marsystems_[0]->getctrl("natural/inSamples"));
      setctrl("natural/inObservations", marsystems_[0]->getctrl("natural/inObservations"));
      setctrl("real/israte", marsystems_[0]->getctrl("real/israte"));
      
      setctrl("string/onObsNames", marsystems_[marsystemsSize_-1]->getctrl("string/onObsNames"));
      setctrl("natural/onSamples", marsystems_[marsystemsSize_-1]->getctrl("natural/onSamples").toNatural());
      setctrl("natural/onObservations", marsystems_[marsystemsSize_-1]->getctrl("natural/onObservations").toNatural());
      setctrl("real/osrate", marsystems_[marsystemsSize_-1]->getctrl("real/osrate").toReal());
      

      // update buffers between components 
      if ((natural)slices_.size() < marsystemsSize_) 
	slices_.resize(marsystemsSize_, NULL);
      
      


      for (natural i=0; i< marsystemsSize_-1; i++)
	{
	  if (slices_[i] != NULL) 
	    {
	      if ((slices_[i])->getRows() != marsystems_[i]->getctrl("natural/onObservations").toNatural()  ||
		  (slices_[i])->getCols() != marsystems_[i]->getctrl("natural/onSamples").toNatural())
		{
		  delete slices_[i];
		  slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), 
					   marsystems_[i]->getctrl("natural/onSamples").toNatural());
		  
		  (slices_[i])->setval(0.0);
		}
	    }
	  else 
	    {

	      ostringstream oss;
	      oss << "realvec/input" << i;
	      addctrl(oss.str(), empty);      
	      
	      slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), 
				       marsystems_[i]->getctrl("natural/onSamples").toNatural());
	      (slices_[i])->setval(0.0);
	    }

	  
	}
      

      if ((probe_)&&(marsystemsSize_ > 1))
	{
	  for (natural i=0; i< marsystemsSize_-1; i++)
	    {
	      ostringstream oss;
	      oss << "realvec/input" << i;
	      setctrl(oss.str(), *(slices_[i]));
	    }
	  
	}
      
      
      defaultUpdate();      
    }
}

  



void
Series::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  // Add assertions about sizes
  if (marsystemsSize_ == 1)
    marsystems_[0]->process(in,out);
  else
    {
      for (natural i = 0; i < marsystemsSize_; i++)
	{
	  if (i==0)
	    {
	      marsystems_[i]->process(in, *(slices_[i]));
	    }
	  else if (i == marsystemsSize_-1)
	    {
	      marsystems_[i]->process(*(slices_[i-1]), out);	  
	    }
	  else
	    marsystems_[i]->process(*(slices_[i-1]), *(slices_[i]));
	}
    }
  
  if ((probe_)&&(marsystemsSize_ > 1))
    {

      for (natural i=0; i< marsystemsSize_-1; i++)
	{
	  ostringstream oss;
	  oss << "realvec/input" << i;
	  setctrl(oss.str(), *(slices_[i]));
	}
      
      

    }
  
  


  
}

	
  
