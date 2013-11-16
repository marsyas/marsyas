/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "FanOutIn.h"
#include <algorithm>

using std::ostringstream;
using std::vector;
using std::max;
using std::min;

using namespace Marsyas;

FanOutIn::FanOutIn(mrs_string name):MarSystem("FanOutIn", name)
{
  isComposite_ = true;
  wrongOutConfig_ = false;
  addControls();
}

FanOutIn::FanOutIn(const FanOutIn& a):MarSystem(a)
{
  ctrl_combinator_ = getctrl("mrs_string/combinator");
  wrongOutConfig_ = false;
}

FanOutIn::~FanOutIn()
{
  deleteSlices();
}

void
FanOutIn::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); iter++)
  {
    delete *(iter);
  }
  slices_.clear();
}

MarSystem*
FanOutIn::clone() const
{
  return new FanOutIn(*this);
}

void
FanOutIn::addControls()
{
  addctrl("mrs_natural/disable", -1);
  setctrlState("mrs_natural/disable", true);
  addctrl("mrs_natural/enable", -1);
  setctrlState("mrs_natural/enable", true);

  addctrl("mrs_string/enableChild", "");
  setctrlState("mrs_string/enableChild", true);
  addctrl("mrs_string/disableChild", "");
  setctrlState("mrs_string/disableChild", true);

  addctrl("mrs_string/combinator", "+", ctrl_combinator_);
}

void
FanOutIn::myUpdate(MarControlPtr sender)
{
  child_count_t child_count = marsystems_.size();
  if (enabled_.getSize() != child_count)
  {
    enabled_.create(child_count);
    enabled_.setval(1.0);
  }

  //check child MarSystems to disable (passed as a string)
  disableChild_ = getctrl("mrs_string/disableChild")->to<mrs_string>();
  disableChildIndex_ = -1;
  for (mrs_natural i=0; i < (mrs_natural)marsystems_.size(); ++i)
  {
    mrs_string s;
    s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
    if (disableChild_ == s)
      disableChildIndex_ = i;
  }
  if (disableChildIndex_ != -1)
  {
    enabled_(disableChildIndex_) = 0.0;
    setctrl("mrs_string/disableChild", "");
  }
  if (disableChild_ == "all")
  {
    for (mrs_natural i=0; i < (mrs_natural)marsystems_.size(); ++i)
    {
      enabled_(i) = 0.0;
      setctrl("mrs_string/disableChild", "");
    }
  }

  //check child MarSystem to disable (passed as an index)
  disable_ = getctrl("mrs_natural/disable")->to<mrs_natural>();
  if (disable_ != -1 && disable_ < (mrs_natural) child_count)
  {
    enabled_(disable_) = 0.0;
    setctrl("mrs_natural/disable", -1);
  }
  else
    setctrl("mrs_natural/disable", -1);

  //check child MarSystems to enable (passed as a string)
  enableChild_ = getctrl("mrs_string/enableChild")->to<mrs_string>();
  enableChildIndex_ = -1;
  for (mrs_natural i=0; i < (mrs_natural)marsystems_.size(); ++i)
  {
    mrs_string s;
    s = marsystems_[i]->getType() + "/" + marsystems_[i]->getName();
    if (enableChild_ == s)
      enableChildIndex_ = i;
  }
  if (enableChildIndex_ != -1)
  {
    enabled_(enableChildIndex_) = 1.0;
    setctrl("mrs_string/enableChild", "");
  }
  //check child MarSystem to enable (passed as an index)
  enable_ = getctrl("mrs_natural/enable")->to<mrs_natural>();
  if (enable_ != -1 && enable_ < (mrs_natural) child_count)
  {
    enabled_(enable_) = 1.0;
    setctrl("mrs_natural/enable", -1);
  }
  else
    setctrl("mrs_natural/enable", -1);

  if (child_count)
  {
    wrongOutConfig_ = false;

    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update();

    // update dataflow component MarSystems in order
    for (child_count_t i=1; i < child_count; ++i)
    {
      marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
      marsystems_[i]->setctrl("mrs_natural/inObservations", marsystems_[0]->getctrl("mrs_natural/inObservations"));
      marsystems_[i]->setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));
      marsystems_[i]->setctrl("mrs_string/inObsNames", marsystems_[0]->getctrl("mrs_string/inObsNames"));
      marsystems_[i]->update(sender);

      if(marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>() !=
          marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>() ||
          marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>() !=
          marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>())
      {
        //MRSERR("FanInOut::myUpdate - child MarSystem " + marsystems_[i]->getPrefix() +
        //	" ouput configuration is not the same as the one from the first child MarSystem(" +
        //	marsystems_[0]->getPrefix() + " ! Outputing zero valued result...");
        wrongOutConfig_ = true;
      }
    }

    // forward flow propagation
    setctrl(ctrl_onSamples_, marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    setctrl(ctrl_onObservations_, marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
    setctrl(ctrl_osrate_, marsystems_[0]->getctrl("mrs_real/osrate")->to<mrs_real>());
    ostringstream oss;
    oss << marsystems_[0]->getctrl("mrs_string/onObsNames");
    setctrl(ctrl_onObsNames_, oss.str());

    // update buffers between components
    if (slices_.size() < child_count)
      slices_.resize(child_count, NULL);
    for (child_count_t i=0; i< child_count; ++i)
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
FanOutIn::myProcess(realvec& in, realvec& out)
{
  child_count_t child_count = marsystems_.size();
  if(child_count)
  {
    if(ctrl_combinator_->to<mrs_string>() == "+")
      out.setval(0); //identity operator
    if(ctrl_combinator_->to<mrs_string>() == "*")
      out.setval(1); //identity operator
    if(ctrl_combinator_->to<mrs_string>() == "max")
      out.setval(-1.0*MAXREAL);
    if(ctrl_combinator_->to<mrs_string>() == "min")
      out.setval(MAXREAL);

    if(wrongOutConfig_)
    {
      //if there is  a child with a non matching output configuration, just output zeros
      MRSERR("FanInOut::myUpdate - at least one child MarSystem ouput \
						 configuration is not the same as the one from the first child \
						 MarSystem! Outputing zero valued result...");
      out.setval(0);
      return;
    }

    for (child_count_t i = 0; i < child_count; ++i)
    {
      if (enabled_(i))
      {
        marsystems_[i]->process(in, *(slices_[i]));
        if(ctrl_combinator_->to<mrs_string>() == "+")
        {
          out += *(slices_[i]);
        }
        if(ctrl_combinator_->to<mrs_string>() == "*")
        {
          out *= *(slices_[i]);
        }
        if(ctrl_combinator_->to<mrs_string>() == "max")
        {
          for(mrs_natural l=0; l<out.getRows(); ++l)
            for(mrs_natural c=0; c<out.getCols(); ++c)
              out(l,c) = max(out(l,c), (*(slices_[i]))(l,c));
        }
        if(ctrl_combinator_->to<mrs_string>() == "min")
        {
          for(mrs_natural l=0; l<out.getRows(); ++l)
            for(mrs_natural c=0; c<out.getCols(); ++c)
              out(l,c) = min(out(l,c), (*(slices_[i]))(l,c));
        }
      }
    }
  }
  else //composite has no children!
  {
    MRSWARN("FanOutIn::process: composite has no children MarSystems - passing input to output without changes.");
    out = in;
  }
}
