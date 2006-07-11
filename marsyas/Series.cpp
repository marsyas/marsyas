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
using namespace Marsyas;

Series::Series(string name):Composite("Series",name)
{
  //type_ = "Series";
  //name_ = name;

	addControls();
}

//Series::Series(const Series& a):Composite(a)
//{
// lmartins: This is now done at MarSystem copy Constructor
//
//   type_ = a.type_;
//   name_ = a.name_;
//   ncontrols_ = a.ncontrols_; 		
//   synonyms_ = a.synonyms_;
// 	dbg_ = a.dbg_;
// 	mute_ = a.mute_;
// 	active_ = a.active_;
  
//lmartins: this is now done at Composite copy constructor
//
//   for (mrs_natural i=0; i< a.marsystemsSize_; i++)
//     {
//       addMarSystem((*a.marsystems_[i]).clone());
//     }
//}

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
  addctrl("mrs_bool/probe", false);
  setctrlState("mrs_bool/probe", true);
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
mrs_real* 
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
Series::localUpdate()
{
  probe_ = getctrl("mrs_bool/probe").toBool();
  
  if (marsystemsSize_ != 0) 
  {
		// update dataflow component MarSystems in order 
    marsystems_[0]->update();      

    for (mrs_natural i=1; i < marsystemsSize_; i++)
		{
		  //lmartins: replaced updctrl() calls by setctrl() ==> more efficient!
			marsystems_[i]->updctrl("mrs_string/inObsNames", marsystems_[i-1]->getctrl("mrs_string/onObsNames"));
			marsystems_[i]->updctrl("mrs_natural/inSamples", marsystems_[i-1]->getctrl("mrs_natural/onSamples"));
			marsystems_[i]->updctrl("mrs_natural/inObservations", marsystems_[i-1]->getctrl("mrs_natural/onObservations"));
			marsystems_[i]->updctrl("mrs_real/israte", marsystems_[i-1]->getctrl("mrs_real/osrate"));
			marsystems_[i]->update();
		}

    // set controls based on first and last marsystem 
    setctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
    setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
    setctrl("mrs_natural/inObservations", marsystems_[0]->getctrl("mrs_natural/inObservations"));
    setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));
    
    setctrl("mrs_string/onObsNames", marsystems_[marsystemsSize_-1]->getctrl("mrs_string/onObsNames"));
    setctrl("mrs_natural/onSamples", marsystems_[marsystemsSize_-1]->getctrl("mrs_natural/onSamples").toNatural());
    setctrl("mrs_natural/onObservations", marsystems_[marsystemsSize_-1]->getctrl("mrs_natural/onObservations").toNatural());
    setctrl("mrs_real/osrate", marsystems_[marsystemsSize_-1]->getctrl("mrs_real/osrate").toReal());
    
    // update buffers (aka slices) between components 
    if ((mrs_natural)slices_.size() < marsystemsSize_) 
			slices_.resize(marsystemsSize_, NULL);
    
		for (mrs_natural i=0; i< marsystemsSize_-1; i++)
		{
			if (slices_[i] != NULL) 
			{
				if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural()  ||
						(slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural())
				{
					delete slices_[i];
					slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural(), 
								 marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural());
					(slices_[i])->setval(0.0);
				}
			}
			else 
			{
				ostringstream oss;
				oss << "mrs_realvec/input" << i;
				addctrl(oss.str(), empty_);      
      
				slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural(), 
							 marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural());
				(slices_[i])->setval(0.0);
			}
		}

    if ((probe_)&&(marsystemsSize_ > 1))
		{
			for (mrs_natural i=0; i< marsystemsSize_-1; i++)
			{
				ostringstream oss;
				oss << "mrs_realvec/input" << i;
				setctrl(oss.str(), *(slices_[i]));
			}
		}
    //defaultUpdate();      
  }
}

void
Series::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  // Add assertions about sizes [!]
  
	if (marsystemsSize_ == 1)
    marsystems_[0]->process(in,out);
  else
  {
    for (mrs_natural i = 0; i < marsystemsSize_; i++)
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
		for (mrs_natural i=0; i< marsystemsSize_-1; i++)
		{
			ostringstream oss;
			oss << "mrs_realvec/input" << i;
			setctrl(oss.str(), *(slices_[i]));
		}
	}
}

	
  
