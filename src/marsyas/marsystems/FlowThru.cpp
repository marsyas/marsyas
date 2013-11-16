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

#include "FlowThru.h"

using std::ostringstream;
using namespace Marsyas;
using std::cout;
using std::endl;


FlowThru::FlowThru(mrs_string name):MarSystem("FlowThru",name)
{
  isComposite_ = true;
  addControls();
}

FlowThru::FlowThru(const FlowThru& a):MarSystem(a)
{
  ctrl_innerOut_ = getctrl("mrs_realvec/innerOut");
}

FlowThru::~FlowThru()
{

}

MarSystem*
FlowThru::clone() const
{
  return new FlowThru(*this);
}

void
FlowThru::addControls()
{
  addctrl("mrs_realvec/innerOut", realvec(), ctrl_innerOut_);
}


void
FlowThru::myUpdate(MarControlPtr sender)
{
  //forward flow propagation
  //just pass input flow to the output
  MarSystem::myUpdate(sender);

  child_count_t child_count = marsystems_.size();
  if (child_count)
  {
    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update();

    // update dataflow component MarSystems in order
    for(child_count_t i=1; i < child_count; ++i) {
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inObsNames_,
                              marsystems_[i-1]->ctrl_onObsNames_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inObservations_,
                              marsystems_[i-1]->ctrl_onObservations_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inSamples_,
                              marsystems_[i-1]->ctrl_onSamples_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_israte_,
                              marsystems_[i-1]->ctrl_osrate_);
      marsystems_[i]->update();
    }

    //link the output of the last child MarSystem to the innerOut of FlowThru Composite
    //(linkTo() will automatically unlink ctrl_innerOut from any previous link target before
    //relinking it to the last MarSystem)
    ctrl_innerOut_->linkTo(marsystems_[child_count-1]->ctrl_processedData_);



    for(child_count_t i=0; i< child_count; ++i)
    {
      MarControlAccessor acc(marsystems_[i]->ctrl_processedData_, NOUPDATE);
      realvec& processedData = acc.to<mrs_realvec>();

      if (processedData.getRows() != marsystems_[i]->ctrl_onObservations_->to<mrs_natural>()  ||
          processedData.getCols() != marsystems_[i]->ctrl_onSamples_->to<mrs_natural>())
      {

        processedData.create(marsystems_[i]->ctrl_onObservations_->to<mrs_natural>(),
                             marsystems_[i]->ctrl_onSamples_->to<mrs_natural>());
      }

      if(i==child_count-1)
      {
        MarControlAccessor acc(ctrl_innerOut_, NOUPDATE);
        realvec& innerOut = acc.to<mrs_realvec>();
        innerOut.create(marsystems_[i]->ctrl_onObservations_->to<mrs_natural>(),marsystems_[i]->ctrl_onSamples_->to<mrs_natural>());
      }
    }
  }
}

void
FlowThru::myProcess(realvec& in, realvec& out)
{
  // Add assertions about sizes [!]

  //input should be passed thru the output untouched!
  out = in;

  child_count_t child_count = marsystems_.size();
  if(child_count >= 1)
  {
    for (child_count_t i = 0; i < child_count; ++i)
    {
      if (i==0)
      {
        MarControlAccessor acc(marsystems_[i]->ctrl_processedData_);
        realvec& slice = acc.to<mrs_realvec>();
        marsystems_[i]->process(in, slice);
      }
      else if (i == child_count-1)
      {
        MarControlAccessor accSlice(marsystems_[i-1]->ctrl_processedData_, true, true);
        realvec& slice = accSlice.to<mrs_realvec>();
        MarControlAccessor accInnerOut(ctrl_innerOut_);
        realvec& innerOut = accInnerOut.to<mrs_realvec>();


        marsystems_[i]->process(slice, innerOut);
      }
      else
      {
        MarControlAccessor acc1(marsystems_[i-1]->ctrl_processedData_, true, true);
        realvec& slice1 = acc1.to<mrs_realvec>();
        MarControlAccessor acc2(marsystems_[i]->ctrl_processedData_);
        realvec& slice2 = acc2.to<mrs_realvec>();
        marsystems_[i]->process(slice1, slice2);
      }
    }
  }
  else if(child_count == 0) //composite has no children!
  {
    MRSWARN("FlowThru::process: composite has no children MarSystems - passing input to output without changes.");
  }
}
