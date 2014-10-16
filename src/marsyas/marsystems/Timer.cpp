/*
** Copyright (C) 1998-2014 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Timer.h"
#include "../common_source.h"

using namespace Marsyas;

Timer::Timer(mrs_string name):MarSystem("Timer", name)
{
  addControls();
}

Timer::Timer(const Timer& a) : MarSystem(a)
{
  ctrl_time_ = getctrl("mrs_real/time");
  ctrl_trigger_ = getctrl("mrs_bool/trigger");
}

Timer::~Timer()
{
}

MarSystem*
Timer::clone() const
{
  return new Timer(*this);
}

void
Timer::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/time", 0.0, ctrl_time_);
  addctrl("mrs_bool/trigger", false, ctrl_trigger_);
}


void
Timer::myUpdate(MarControlPtr sender)
{
  // no change to network flow
  MarSystem::myUpdate(sender);
}

void
Timer::myProcess(realvec& in, realvec& out)
{

  if (trigger_duration_ == 0.0)
      ctrl_trigger_->setValue(false);

  time_ += inSamples_ / israte_;
  ctrl_time_->setValue(time_);  
  trigger_duration_ += inSamples_ / israte_;


  if (trigger_duration_ > 2.0) 
    {
      ctrl_trigger_->setValue(true);
      trigger_duration_ = 0.0;
      // time_ = 0.0;
    }



  for (mrs_natural o=0; o < inObservations_; o++)
  {
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      out(o,t) = in(o,t);
    }
  }
}
