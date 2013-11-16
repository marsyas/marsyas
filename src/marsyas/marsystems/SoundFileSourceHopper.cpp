/*
** Copyright (C) 2009 Stefaan Lippens
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

#include "SoundFileSourceHopper.h"
#include "SoundFileSource.h"
#include "MixToMono.h"
#include "ShiftInput.h"

using namespace std;
using namespace Marsyas;

SoundFileSourceHopper::SoundFileSourceHopper(mrs_string name) : MarSystem("SoundFileSourceHopper", name)
{

  /// Add any specific controls needed by this MarSystem.
  addControls();

  // Add the MarSystems we need as children.
  isComposite_ = true;
  addMarSystem(new SoundFileSource("src"));
  addMarSystem(new MixToMono("mix2mono"));
  addMarSystem(new ShiftInput("hopper"));

  // Provide some shortcuts to SoundFileSource internals.
  this->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
  this->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

}

SoundFileSourceHopper::SoundFileSourceHopper(const SoundFileSourceHopper& a) : MarSystem(a)
{
  isComposite_ = true;
  // All member MarControlPtr have to be explicitly reassigned in
  // the copy constructor.
  ctrl_windowSize_ = getControl("mrs_natural/windowSize");
  ctrl_hopSize_ = getControl("mrs_natural/hopSize");
  ctrl_mixToMono_ = getControl("mrs_bool/mixToMono");

  // Add the MarSystems we need as children.
  isComposite_ = true;
  addMarSystem(new SoundFileSource("src"));
  addMarSystem(new MixToMono("mix2mono"));
  addMarSystem(new ShiftInput("hopper"));

  // Provide some shortcuts to SoundFileSource internals.
  this->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
  this->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

}


SoundFileSourceHopper::~SoundFileSourceHopper()
{
}

MarSystem*
SoundFileSourceHopper::clone() const
{
  return new SoundFileSourceHopper(*this);
}

void
SoundFileSourceHopper::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addControl("mrs_natural/windowSize", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES, ctrl_windowSize_);
  setControlState("mrs_natural/windowSize", true);

  addControl("mrs_natural/hopSize", (mrs_natural)(MRS_DEFAULT_SLICE_NSAMPLES/2), ctrl_hopSize_);
  setControlState("mrs_natural/hopSize", true);

  addControl("mrs_bool/mixToMono", false, ctrl_mixToMono_);
  setControlState("mrs_bool/mixToMono", true);

}

/**
 * SoundFileSourceHopper::myUpdate() is largely based on Series::myUpdate().
 * One important difference is that the input flow settings of the
 * SoundFileSourceHopper are ignored, The flow settings should be defined
 * by the internal SoundFileSource MarSystem after all.
 */
void
SoundFileSourceHopper::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  // Set the hop size as window size for the SoundFileSource (and leave the
  // other stuff (like osrate, onObservations, ...) up to the SoundFileSource.
  // We have to make sure we do this when the SoundFileSource is already
  // added to the marsystems_ list.
  if (marsystems_.size() >= 1) {
    marsystems_[0]->setControl("mrs_natural/inSamples", ctrl_hopSize_);
    marsystems_[0]->update();
  }

  // Set the window size in the ShiftInput Marsystem (if it is already
  // added to the marsystems_ list).
  if (marsystems_.size() >= 3) {
    marsystems_[2]->setControl("mrs_natural/winSize", ctrl_windowSize_);
    marsystems_[2]->update();
  }

  // Update data flows in order.
  // src and tgt stand for the source and target MarSystems in the for loop.
  // Note: if MixToMono is disabled, we skip the MixToMono MarSystem
  // (which is at index 1).
  mrs_natural i_src = 0;
  mrs_natural i_tgt = ctrl_mixToMono_->to<mrs_bool>() ? 1 : 2;
  for ( ; i_tgt < (mrs_natural)marsystems_.size(); i_src = i_tgt, i_tgt++) {
    // Get the source and target MarSystems.
    MarSystem* m_src = marsystems_[i_src];
    MarSystem* m_tgt = marsystems_[i_tgt];
    // Copy over the control values from source to target.
    m_tgt->setctrl(m_tgt->ctrl_inObsNames_, m_src->ctrl_onObsNames_);
    m_tgt->setctrl(m_tgt->ctrl_inObservations_, m_src->ctrl_onObservations_);
    m_tgt->setctrl(m_tgt->ctrl_inSamples_, m_src->ctrl_onSamples_);
    m_tgt->setctrl(m_tgt->ctrl_israte_, m_src->ctrl_osrate_);
    // And update the target MarSystem with the new settings.
    m_tgt->update();
  }

  // Set the container's output controls based on the last internal MarSystem.
  MarSystem* m_last = marsystems_[marsystems_.size() - 1];
  updControl(ctrl_onObsNames_, m_last->ctrl_onObsNames_, NOUPDATE);
  updControl(ctrl_onSamples_, m_last->ctrl_onSamples_, NOUPDATE);
  updControl(ctrl_onObservations_, m_last->ctrl_onObservations_, NOUPDATE);
  updControl(ctrl_osrate_, m_last->ctrl_osrate_, NOUPDATE);

  // Allocate the appropriate amount of memory for the intermediate slices.
  for (size_t i=0; i < marsystems_.size() - 1; ++i)
  {
    MarSystem* m = marsystems_[i];
    MarControlAccessor acc(m->ctrl_processedData_, NOUPDATE);
    realvec& processedData = acc.to<mrs_realvec>();
    mrs_natural rows = m->ctrl_onObservations_->to<mrs_natural>();
    mrs_natural cols = m->ctrl_onSamples_->to<mrs_natural>();
    if (processedData.getRows() != rows || processedData.getCols() != cols )
    {
      processedData.create(rows, cols);
    }
  }

}

void
SoundFileSourceHopper::myProcess(realvec& in, realvec& out)
{
  (void) in;

  // First step: do the processing of the SoundFileSource.
  MarControlAccessor acc(marsystems_[0]->ctrl_processedData_);
  realvec& slice_out = acc.to<mrs_realvec>();
  // `slice_out` is the slice that receives the data from the SoundFileSource
  // Note that we provide `slice_out` also as input argument of
  // this `process` call instead of the traditional realvec `in`.
  // We do this to circumvent the (optional) flow check in `process`.
  // SoundFileSourceHopper does not use its `inSamples_` attribute
  // to set slice sizes, but uses the `hopSize` control instead.
  // Consequently, the realvec `in` given as argument from upstream
  // (based on the inSamples_ attribute) will typically not match the slice
  // size we expect and the flow check will fail. `slice_out` however
  // does have the size we expect and because the SoundFileSource just
  // ignores its input anyway, we can use this trick here.
  marsystems_[0]->process(slice_out, slice_out);

  // The other steps: do the processing of MixToMono (if required)
  // and ShiftInput (and maybe more MarSystems).
  child_count_t i_prev = 0;
  child_count_t i_curr = ctrl_mixToMono_->to<mrs_bool>() ? 1 : 2;
  child_count_t child_count = marsystems_.size();
  for ( ; i_curr < child_count; i_prev = i_curr, i_curr++) {
    MarSystem* m_prev = marsystems_[i_prev];
    MarSystem* m_curr = marsystems_[i_curr];

    // Get the processed data slice from the previous MarSystem.
    MarControlAccessor acc_prev(m_prev->ctrl_processedData_, true, true);
    realvec& data_prev = acc_prev.to<mrs_realvec>();

    if (i_curr < child_count - 1)
    {
      MarControlAccessor acc_curr(m_curr->ctrl_processedData_);
      realvec& data_curr = acc_curr.to<mrs_realvec>();
      m_curr->process(data_prev, data_curr);
    }
    else
    {
      m_curr->process(data_prev, out);
    }

  }

}
