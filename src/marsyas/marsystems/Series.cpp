/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Series.h"

using namespace std;
using namespace Marsyas;

Series::Series(mrs_string name):MarSystem("Series",name)
{
  isComposite_ = true;
  addControls();
}

Series::~Series()
{
}

MarSystem*
Series::clone() const
{
  return new Series(*this);
}

void
Series::addControls()
{
}


void
Series::myUpdate(MarControlPtr sender)
{
  child_count_t child_count = marsystems_.size();
  if (child_count)
  {
    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->setctrl("mrs_natural/inStabilizingDelay", inStabilizingDelay_);
    marsystems_[0]->update();

    // update dataflow component MarSystems in order
    for (child_count_t i=1; i < child_count; ++i)
    {
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inObsNames_,
                              marsystems_[i-1]->ctrl_onObsNames_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inObservations_,
                              marsystems_[i-1]->ctrl_onObservations_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inSamples_,
                              marsystems_[i-1]->ctrl_onSamples_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_israte_,
                              marsystems_[i-1]->ctrl_osrate_);
      marsystems_[i]->setctrl(marsystems_[i]->ctrl_inStabilizingDelay_,
                              marsystems_[i-1]->ctrl_onStabilizingDelay_);
      marsystems_[i]->update();
    }

    //forward flow propagation
    updControl(ctrl_onObsNames_, marsystems_[child_count-1]->ctrl_onObsNames_, NOUPDATE);
    updControl(ctrl_onSamples_, marsystems_[child_count-1]->ctrl_onSamples_, NOUPDATE);
    updControl(ctrl_onObservations_, marsystems_[child_count-1]->ctrl_onObservations_, NOUPDATE);
    updControl(ctrl_osrate_, marsystems_[child_count-1]->ctrl_osrate_, NOUPDATE);
    updControl(ctrl_onStabilizingDelay_, marsystems_[child_count-1]->ctrl_onStabilizingDelay_, NOUPDATE);

    for (child_count_t i=0; i< child_count-1; ++i)
    {
      MarControlAccessor acc(marsystems_[i]->ctrl_processedData_, NOUPDATE);
      realvec& processedData = acc.to<mrs_realvec>();

      if (processedData.getRows() != marsystems_[i]->ctrl_onObservations_->to<mrs_natural>()  ||
          processedData.getCols() != marsystems_[i]->ctrl_onSamples_->to<mrs_natural>())
      {
        processedData.create(marsystems_[i]->ctrl_onObservations_->to<mrs_natural>(),
                             marsystems_[i]->ctrl_onSamples_->to<mrs_natural>());
      }
    }
  }
  else //if composite is empty...
    MarSystem::myUpdate(sender);
}

void
Series::myProcess(realvec& in, realvec& out)
{
  // Add assertions about sizes [!]

  child_count_t child_count = marsystems_.size();
  if (child_count == 1)
    marsystems_[0]->process(in,out);
  else if(child_count > 1)
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
        MarControlAccessor acc(marsystems_[i-1]->ctrl_processedData_, true, true);
        realvec& slice = acc.to<mrs_realvec>();
        marsystems_[i]->process(slice, out);
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
    MRSWARN("Series::process: composite has no children MarSystems - passing input to output without changes.");
    out = in;
  }


}
