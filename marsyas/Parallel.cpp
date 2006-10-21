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
    \class Parallel
    \brief Parallel composite of MarSystem objects
*/

#include "Parallel.h"

using namespace std;
using namespace Marsyas;

Parallel::Parallel(string name):Composite("Parallel",name)
{
  //type_ = "Parallel";
  //name_ = name;
}

Parallel::~Parallel()
{
  deleteSlices();
}

// Parallel::Parallel(const Parallel& a):Composite(a)
// {
// 	//lmartins: this is now done at Composite copy constructor
// 	//
// 	//   for (mrs_natural i=0; i< a.marsystemsSize_; i++)
// 	//     {
// 	//       addMarSystem((*a.marsystems_[i]).clone());
// 	//     }
// }

void Parallel::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter = slices_.begin(); iter != slices_.end(); iter++) {
    delete *(iter);
  }
  slices_.clear();
}

MarSystem* Parallel::clone() const 
{
  return new Parallel(*this);
}

void Parallel::localUpdate()
{
  if (marsystemsSize_ != 0) 
	{
    marsystems_[0]->update();
    
    mrs_natural inObservations = marsystems_[0]->getctrl("mrs_natural/inObservations").toNatural();
    mrs_natural onObservations = marsystems_[0]->getctrl("mrs_natural/onObservations").toNatural();
    
    for (mrs_natural i=1; i < marsystemsSize_; i++) 
		{
      //lmartins: setctrl or updctrl?!? [?]
			marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
      marsystems_[i]->setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte")); //[!] israte
      marsystems_[i]->update();
      inObservations += marsystems_[i]->getctrl("mrs_natural/inObservations").toNatural();
      onObservations += marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural();
    }
    
    setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
    setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples"));
    setctrl("mrs_natural/inObservations", inObservations);
    setctrl("mrs_natural/onObservations", onObservations);
    setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));
    setctrl("mrs_real/osrate", marsystems_[0]->getctrl("mrs_real/osrate"));
    
    // update buffers for components
    
    if ((mrs_natural)slices_.size() < 2*marsystemsSize_) 
		{
      slices_.resize(2*marsystemsSize_, NULL);
    }
    
    for (mrs_natural i = 0; i < marsystemsSize_; i++) 
		{
      if (slices_[2*i] != NULL) 
			{
				if ((slices_[2*i])->getRows() != marsystems_[i]->getctrl("mrs_natural/inObservations").toNatural() || (slices_[2*i])->getCols() != marsystems_[i]->getctrl("mrs_natural/inSamples").toNatural()) 
				{
				delete slices_[2*i];
				slices_[2*i] = new realvec(marsystems_[i]->getctrl("mrs_natural/inObservations").toNatural(), marsystems_[i]->getctrl("mrs_natural/inSamples").toNatural());
				}
      }
      else 
			{
				slices_[2*i] = new realvec(marsystems_[i]->getctrl("mrs_natural/inObservations").toNatural(), marsystems_[i]->getctrl("mrs_natural/inSamples").toNatural());
      }
      
			(slices_[2*i])->setval(0.0);
      
			if (slices_[2*i+1] != NULL) 
			{
				if ((slices_[2*i+1])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural() || (slices_[2*i+1])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural()) 
				{
					delete slices_[2*i+1];
					slices_[2*i+1] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural(), marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural());
				}
      }
      else 
			{
				slices_[2*i+1] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural(), marsystems_[i]->getctrl("mrs_natural/onSamples").toNatural());
      }
      
			(slices_[2*i+1])->setval(0.0);
    }
		//defaultUpdate();
  }
}

void 
Parallel::updControl(string cname, MarControlValue value)
{ 
  // check for synonyms - call recursively to resolve them 
  map<string, vector<string> >::iterator ei;

  // remove prefix for synonyms
  string prefix = "/" + getType() + "/" + getName() + "/";
  string::size_type pos = cname.find(prefix, 0);
  string shortcname;
  
  if (pos == 0) 
    shortcname = cname.substr(prefix.length(), cname.length());
  
  ei = synonyms_.find(shortcname);
  if (ei != synonyms_.end())
  {
    vector<string> synonymList = synonyms_[shortcname];
    vector<string>::iterator si;
    for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			updControl(prefix + *si, value);
		}
  }
  else
  {
    string prefix = "/" + getType() + "/" + getName();
    string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
    string nchildcontrol = childcontrol.substr(1, childcontrol.length());  
    
    bool controlFound = false;
    
    // check local controls 
    if (hasControlLocal(cname))
		{
			controlFound = true;
			oldval_ = getControl(cname);
			setControl(cname, value);
			
			if (hasControlState(cname) && (value != oldval_)) 
			{
				update(); //update composite
				// commented out by gtzan - probably redundant
				//lmartins: AGREE! => code already executed by update()... [!]
				/* 
				dbg_ = getctrl("mrs_bool/debug").toBool();
				mute_ = getctrl("mrs_bool/mute").toBool();
				if ((inObservations_ != inTick_.getRows()) ||
						(inSamples_ != inTick_.getCols())      ||
						(onObservations_ != outTick_.getRows()) ||
						(onSamples_ != outTick_.getCols()))
				{
					inTick_.create(inObservations_, inSamples_);
					outTick_.create(onObservations_, onSamples_);
				}
				*/ 
			}
		}
		//Parallel Specific [?]
    if(nchildcontrol == "mrs_natural/inObservations")
		{
			mrs_natural v = value.toNatural()/marsystemsSize_;
			marsystems_[0]->updctrl(nchildcontrol, v);
			update();
			return;
		}
		// lmartins: should find a way to avoid this hard-coded check... [!]
    // default controls - semantics of composites 
    if ((nchildcontrol == "mrs_natural/inSamples")||
				//(nchildcontrol == "mrs_natural/inObservations")||
				(nchildcontrol == "mrs_real/israte")||
				//(nchildcontrol == "mrs_natural/onSamples")|| //lmartins: does it make any sense to check for this control?![?]
				//(nchildcontrol == "mrs_natural/onObservations")|| //lmartins: does it make any sense to check for this control?![?]
				//(nchildcontrol == "mrs_real/osrate")|| //lmartins: does it make any sense to check for this control?![?]
				//(nchildcontrol == "mrs_bool/debug")|| //lmartins: does it make any sense to check for this control?![?]
				(nchildcontrol == "mrs_string/inObsNames"))//||
				//(nchildcontrol == "mrs_string/onObsNames")) //lmartins: does it make any sense to check for this control?![?]
		{
			marsystems_[0]->updctrl(nchildcontrol, value);
			update();
			return;
		}
 		else//if(!hasControlLocal(cname)) 
		{
			//if control control is not from composite,
			//check if it exists among the children composites and update them
			for (mrs_natural i=0; i< marsystemsSize_; i++)
			{
				if (marsystems_[i]->hasControl(childcontrol))
				{
					controlFound = true;
					MarControlValue oldval;
					oldval = marsystems_[i]->getControl(childcontrol);
					marsystems_[i]->updControl(childcontrol, value); //updcontrol or setcontrol?! [!]
					if (marsystems_[i]->hasControlState(childcontrol) && 
						 (value != oldval))
					{
						update();
					}
				}
			}
		}
    
    if (!controlFound) 
		{
			MRSWARN("Composite::updControl - Unsupported control name = " + cname);
			MRSWARN("Composite::updControl - Composite name = " + getName());
		}
  }
}


void Parallel::process(realvec& in, realvec& out)
{
  checkFlow(in, out);
  
  mrs_natural inIndex = 0;
  mrs_natural outIndex = 0;
  mrs_natural localIndex = 0;
  
  if (marsystemsSize_ == 1) 
	{
    marsystems_[0]->process(in, out);
  }
  else if (marsystemsSize_ > 1) 
	{
    for (mrs_natural i = 0; i < marsystemsSize_; i++) 
		{
      localIndex = marsystems_[i]->getctrl("mrs_natural/inObservations").toNatural();
      for (o = 0; o < localIndex; o++) 
			{
				for (t = 0; t < inSamples_; t++) //lmartins: was t < onSamples [!]
				{
					(*(slices_[2*i]))(o,t) = in(inIndex + o,t);
				}
      }
      inIndex += localIndex;
      marsystems_[i]->process(*(slices_[2*i]), *(slices_[2*i+1]));
      localIndex = marsystems_[i]->getctrl("mrs_natural/onObservations").toNatural();
      for (o = 0; o < localIndex; o++) 
			{
				for (t = 0; t < onSamples_; t++) 
				{
					out(outIndex + o,t) = (*(slices_[2*i+1]))(o,t);
				}
      }
      outIndex += localIndex;
    }
  }
}

