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

#include "Fanout.h"

using namespace std;
using namespace Marsyas;

Fanout::Fanout(string name):MarSystem("Fanout", name)
{
  isComposite_ = true;
  addControls();
}

Fanout::Fanout(const Fanout& a): MarSystem(a) 
{
  ctrl_enabled_ = getctrl("mrs_realvec/enabled");
}

Fanout::~Fanout()
{
  deleteSlices();
}

void 
Fanout::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); iter++)
    {
      delete *(iter);
    }
  slices_.clear();
}

MarSystem* 
Fanout::clone() const 
{
  return new Fanout(*this);
}

void 
Fanout::addControls()
{
  addctrl("mrs_natural/disable", -1);
  setctrlState("mrs_natural/disable", true);
  addctrl("mrs_natural/enable", -1);
  setctrlState("mrs_natural/enable", true);

  addctrl("mrs_string/enableChild", ",");
  setctrlState("mrs_string/enableChild", true);
  addctrl("mrs_string/disableChild", ",");
  setctrlState("mrs_string/disableChild", true);
	
  addctrl("mrs_realvec/enabled", realvec(), ctrl_enabled_);
}

void 
Fanout::myUpdate(MarControlPtr sender)
{	
  MarControlAccessor acc(ctrl_enabled_);
  mrs_realvec& enabled = acc.to<mrs_realvec>();

  if (enabled.getSize() < marsystemsSize_)
    {
      enabled.create(marsystemsSize_);
      enabled.setval(1.0);
    }
  if (marsystemsSize_ != 0) 
    {
      localIndices_.create(marsystemsSize_);
    }
  
  //check child MarSystems to disable (passed as a string) 
  disableChild_ = getctrl("mrs_string/disableChild")->to<mrs_string>();
  disableChildIndex_ = -1;
  for (unsigned int i=0; i < marsystems_.size(); i++) 
    {
      string s;
      s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
      if (disableChild_ == s) 
	disableChildIndex_ = i;
    }
  if (disableChildIndex_ != -1) 
    {
      enabled(disableChildIndex_) = 0.0;
      localIndices_(disableChildIndex_) = 0.0;
      setctrl("mrs_string/disableChild", ",");
    }  
  if (disableChild_ == "all")
    {
      for (unsigned int i=0; i < marsystems_.size(); i++) 
	{
	  enabled(i) = 0.0;
	  localIndices_(i) = 0.0;
	  setctrl("mrs_string/disableChild", ",");
	}
    }
  //check child MarSystem to disable (passed as an index) 
  disable_ = getctrl("mrs_natural/disable")->to<mrs_natural>();
  if (disable_ != -1 && disable_ < marsystemsSize_) 
    {
      enabled(disable_) = 0.0;
      localIndices_(disable_) = 0.0;
      setctrl("mrs_natural/disable", -1);
    }
  else
    setctrl("mrs_natural/disable", -1);

  //check child MarSystems to enable (passed as a string) 
  enableChild_ = getctrl("mrs_string/enableChild")->to<mrs_string>();
  enableChildIndex_ = -1;
  for (unsigned int i=0; i < marsystems_.size(); i++) 
    {
      string s;
      s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
      if (enableChild_ == s) 
	enableChildIndex_ = i;
    }
  if (enableChildIndex_ != -1) 
    {
      enabled(enableChildIndex_) = 1.0;
      localIndices_(enableChildIndex_) = 1.0;
      setctrl("mrs_string/enableChild", ",");
    }  
  //check child MarSystem to enable (passed as an index)
  enable_ = getctrl("mrs_natural/enable")->to<mrs_natural>();  
  if (enable_ != -1 && enable_ < marsystemsSize_) 
    {
      enabled(enable_) = 1.0;
      localIndices_(enable_) = 1.0;
      setctrl("mrs_natural/enable", -1);
    }
  else
    setctrl("mrs_natural/enable", -1);

	
  if (marsystemsSize_ != 0)
    {
      //propagate in flow controls to first child
      marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
      marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
      marsystems_[0]->setctrl("mrs_real/israte", israte_);
      marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
      marsystems_[0]->update();

      // update dataflow component MarSystems in order
      ostringstream oss;
      mrs_natural onObservations = 0;
      if (enabled(0))
	{
	  onObservations += marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	  localIndices_(0) = marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	  oss << marsystems_[0]->getctrl("mrs_string/onObsNames");
	}
      for (mrs_natural i=1; i < marsystemsSize_; i++)
	{
	  marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[i-1]->getctrl("mrs_natural/inSamples"));
	  marsystems_[i]->setctrl("mrs_natural/inObservations", marsystems_[i-1]->getctrl("mrs_natural/inObservations"));
	  marsystems_[i]->setctrl("mrs_real/israte", marsystems_[i-1]->getctrl("mrs_real/israte"));
	  marsystems_[i]->setctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
	  marsystems_[i]->update(sender);
	  if (enabled(i))
	    {
	      onObservations += (marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
	      localIndices_(i) = marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	      oss << marsystems_[i]->getctrl("mrs_string/onObsNames");
	    }
	}

      // forward flow propagation
      setctrl(ctrl_onSamples_, marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
      setctrl(ctrl_onObservations_, onObservations);
      setctrl(ctrl_osrate_, marsystems_[0]->getctrl("mrs_real/osrate")->to<mrs_real>());
      setctrl(ctrl_onObsNames_, oss.str());

      // update buffers between components 
      if ((mrs_natural)slices_.size() < marsystemsSize_) 
	slices_.resize(marsystemsSize_, NULL);
      for (mrs_natural i=0; i< marsystemsSize_; i++)
	{
	  if (slices_[i] != NULL) 
	    {
	      if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>()  ||
		  (slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>())
		{
		  delete slices_[i];
		  slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
					   marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
		}
	    }
	  else 
	    {
	      slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
				       marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
	    }
	  (slices_[i])->setval(0.0);
	}
    }
  else //if composite is empty...
    MarSystem::myUpdate(sender);
}

void
Fanout::myProcess(realvec& in, realvec& out)
{
  if(marsystemsSize_>0)
    {
      mrs_natural outIndex = 0;
      MarControlAccessor acc(ctrl_enabled_);
      mrs_realvec& enabled = acc.to<mrs_realvec>();
      
      
      for (mrs_natural i = 0; i < marsystemsSize_; i++)
	{
	  if (enabled(i))
	    {
	      marsystems_[i]->process(in, *(slices_[i]));
	      
	      for (o=0; o < localIndices_(i); o++)
		for (t=0; t < onSamples_; t++)
		  out(outIndex + o,t) = (*(slices_[i]))(o,t);
	      
	      outIndex += (mrs_natural)localIndices_(i);      
	    }
	}
    }
  else //composite has no children!
    {
      MRSWARN("FanOut::process: composite has no children MarSystems - passing input to output without changes.");
      out = in;
    }
}
