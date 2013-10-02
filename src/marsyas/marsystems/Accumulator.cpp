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

#include "Accumulator.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Accumulator::Accumulator(mrs_string name): MarSystem("Accumulator", name)
{
  isComposite_ = true;
  addControls();

  keptOnSamples_ = 0;
}

Accumulator::Accumulator(const Accumulator& a): MarSystem(a)
{
  ctrl_nTimes_ = getctrl("mrs_natural/nTimes");
  ctrl_maxTimes_ = getctrl("mrs_natural/maxTimes");
  ctrl_minTimes_ = getctrl("mrs_natural/minTimes");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_flush_ = getctrl("mrs_bool/flush");
  ctrl_timesToKeep_ = getctrl("mrs_natural/timesToKeep");
  keptOnSamples_ = a.keptOnSamples_;
}

Accumulator::~Accumulator()
{
}

MarSystem*
Accumulator::clone() const
{
  return new Accumulator(*this);
}

void
Accumulator::addControls()
{
  addctrl("mrs_string/mode", "countTicks", ctrl_mode_);
  ctrl_mode_->setState(true);

  addctrl("mrs_natural/nTimes", 5, ctrl_nTimes_);
  ctrl_nTimes_->setState(true);

  addctrl("mrs_natural/timesToKeep", 0, ctrl_timesToKeep_);
  ctrl_timesToKeep_->setState(true);

  addctrl("mrs_bool/flush", false, ctrl_flush_);

  addctrl("mrs_natural/maxTimes", 5, ctrl_maxTimes_);
  ctrl_maxTimes_->setState(true);

  addctrl("mrs_natural/minTimes", 5, ctrl_minTimes_);
  ctrl_minTimes_->setState(true);
}


bool
Accumulator::addMarSystem(MarSystem *marsystem)
{
  if (marsystems_.size()) {
    MarSystem* mySystem = marsystems_[0];
    (void) mySystem; // suppress warning in case the macro is not expanded

    MRSWARN("Accumulator::addMarSystem: already added '"
            << mySystem->getAbsPath()
            << "' to the Accumulator NOT ADDING '"
            << marsystem->getName()
            << "'.");
    return false;
  }
  return MarSystem::addMarSystem(marsystem);


}

void
Accumulator::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Accumulator.cpp - Accumulator:myUpdate");

  mrs_string onObsNames;

  childOnSamples_ = 0;
  nTimes_ = ctrl_nTimes_->to<mrs_natural>();

  MarSystem::myUpdate(sender);

  if (marsystems_.size())
  {
    //propagate in flow controls to first (and single) child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update();

    childOnSamples_ = marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    //nTimes_ = ctrl_nTimes_->to<mrs_natural>();

    // forward flow propagation
    setctrl(ctrl_onSamples_, nTimes_ * childOnSamples_); //dynamic resizing of onSamples!
    setctrl(ctrl_onObservations_,
            marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
    setctrl(ctrl_osrate_,
            marsystems_[0]->getctrl("mrs_real/osrate"));

    onObsNames = marsystems_[0]->getctrl("mrs_string/onObsNames")->to<mrs_string>();
  }

  onObservations_ = ctrl_onObservations_->to<mrs_natural>();

  // Add prefix to the observation names.
  ostringstream prefix_oss;
  prefix_oss << "Acc" << nTimes_ << "_";
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(onObsNames, prefix_oss.str()), NOUPDATE);


  onSamples_ = ctrl_onSamples_->to<mrs_natural>();

  //create temporary buffer for child output
  childOut_.stretch(onObservations_, childOnSamples_);

  //create internal buffer for accumulating data to send to output
  //(set it to the specified maximum + any kept old)
  if (ctrl_mode_->to<mrs_string>() == "explicitFlush")
  {
    tout_.stretch(onObservations_,
                  (ctrl_timesToKeep_->to<mrs_natural>() + ctrl_maxTimes_->to<mrs_natural>()) * childOnSamples_);
  }
  else
  {
    tout_.create(0,0); //no memory is needed in this mode
  }
}

void
Accumulator::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,c,t;

  if (!marsystems_.size())
  {
    out = in;
    return;
  }

  if (ctrl_mode_->to<mrs_string>() == "explicitFlush")
  {
    mrs_natural timesCount = keptOnSamples_/childOnSamples_;

    while ((!ctrl_flush_->to<mrs_bool>() && timesCount < ctrl_maxTimes_->to<mrs_natural>())
           || timesCount < ctrl_minTimes_->to<mrs_natural>()
           || timesCount <= ctrl_timesToKeep_->to<mrs_natural>())
    {
      // child MarSystem should have a control linked to the Accumulator flush control
      // so it can signal the end of this loop (e.g. when an onset is detected or some other similar event)
      marsystems_[0]->process(in, childOut_);

      //accumulate output from child process()into temp buffer
      for (o=0; o < onObservations_; o++)
      {
        for (t = 0; t < childOnSamples_; t++)
        {
          tout_(o, t + timesCount * childOnSamples_) = childOut_(o,t);
        }
      }
      timesCount++;
    }

#ifdef MARSYAS_LOG_DIAGNOSTICS
    if (!ctrl_flush_->to<mrs_bool>())
    {
      MRWDIAG("Accumulator::myProcess() - maxTimes reached without a flush event!");
    }
#endif

    //adjust output number of samples dynamically (this calls update()!!)
    //to the number of accumulated samples (minus the ones to keep for next time)
    ctrl_nTimes_->setValue(timesCount - ctrl_timesToKeep_->to<mrs_natural>());

    keptOnSamples_ = ctrl_timesToKeep_->to<mrs_natural>() * childOnSamples_;

    //copy data in tmp buffer to the output
    for (o=0; o < onObservations_; ++o)
    {
      for (t = 0; t < ctrl_onSamples_->to<mrs_natural>(); ++t)
      {
        out(o,t) = tout_(o,t);
      }
    }


    //store samples to keep into the beginning of the temp buffer
    //for next call to process()
    for (t = 0; t < keptOnSamples_; ++t)
    {
      for (o=0; o < onObservations_; ++o)
      {
        tout_(o,t) = tout_(o, t + ctrl_onSamples_->to<mrs_natural>());
      }
    }

    //reset flush flag
    ctrl_flush_->setValue(false);
  }
  else if (ctrl_mode_->to<mrs_string>() == "countTicks")
  {
    //nTimes_ = ctrl_nTimes_->to<mrs_natural>();
    ctrl_flush_->setValue(false);
    for (c = 0; c < nTimes_; ++c)
    {

      marsystems_[0]->process(in, childOut_);

      for (o=0; o < onObservations_; o++)
      {
        for (t = 0; t < childOnSamples_; t++)
        {
          out(o, t + c * childOnSamples_) = childOut_(o,t);
        }
      }
    }
    ctrl_flush_->setValue(true);
  }

  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
  //MATLAB_PUT(out, "Accum_out");
  //MATLAB_EVAL("segmentData = [segmentData, Accum_out];");
}
