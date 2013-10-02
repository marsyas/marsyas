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

#include "AimLocalMax.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

AimLocalMax::AimLocalMax(mrs_string name):MarSystem("AimLocalMax",name)
{
  is_initialized = false;
  initialized_israte = 0.0;

  is_reset = false;
  reset_inobservations = -1;

  addControls();
}

AimLocalMax::AimLocalMax(const AimLocalMax& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_decay_time_ms_ = getctrl("mrs_real/decay_time_ms");
  ctrl_timeout_ms_ = getctrl("mrs_real/timeout_ms");

  is_initialized = false;
  initialized_israte = 0.0;

  is_reset = false;
  reset_inobservations = -1;

}


AimLocalMax::~AimLocalMax()
{
}

MarSystem*
AimLocalMax::clone() const
{
  return new AimLocalMax(*this);
}

void
AimLocalMax::addControls()
{
  addControl("mrs_real/decay_time_ms", 20.0 , ctrl_decay_time_ms_);
  addControl("mrs_real/timeout_ms", 3.0 , ctrl_timeout_ms_);
}

void
AimLocalMax::myUpdate(MarControlPtr sender)
{
  (void)sender;

  MRSDIAG("AimLocalMax.cpp - AimLocalMax:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);

  // The output from PZFC (and HCL) is a realvec with the first half
  // of observations being the signal, and the second half being the
  // channels.  So, the output of AimLocalMax is a realvec with 3
  // times the number of channels, with the first block of
  // observations being the signal (unchanged) the second block being
  // the centre frequencies (unchanged) and the third block being the
  // newly calculated strobe points.
  channel_count_ = ctrl_inObservations_->to<mrs_natural>() / 2;
  ctrl_onObservations_->setValue(channel_count_ * 3);

  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimLocalMax_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  //
  // Does the MarSystem need initialization?
  //
  if (initialized_israte != ctrl_israte_->to<mrs_real>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_israte = ctrl_israte_->to<mrs_real>();
  }

  //
  // Does the MarSystem need a reset?
  //
  if (reset_inobservations != ctrl_inObservations_->to<mrs_natural>()) {
    is_reset = false;
  }

  if (!is_reset) {
    ResetInternal();
    is_reset = true;
    reset_inobservations = ctrl_inObservations_->to<mrs_natural>();
  }

}


bool
AimLocalMax::InitializeInternal() {
  strobe_timeout_samples_ = (int) floor(ctrl_timeout_ms_->to<mrs_real>() * ctrl_israte_->to<mrs_real>() / 1000.0);
  strobe_decay_samples_ = (int) floor(ctrl_decay_time_ms_->to<mrs_real>() * ctrl_israte_->to<mrs_real>() / 1000.0);
  return true;

}

void
AimLocalMax::ResetInternal() {
  threshold_.clear();
  threshold_.resize(channel_count_, 0.0);

  decay_constant_.clear();
  decay_constant_.resize(channel_count_, 1.0);

  prev_sample_.clear();
  prev_sample_.resize(channel_count_, 10000.0);
  curr_sample_.clear();
  curr_sample_.resize(channel_count_, 5000.0);
  next_sample_.clear();
  next_sample_.resize(channel_count_, 0.0);
}



void
AimLocalMax::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  // sness - Need this because we don't have a SignalBuffer class like AIM-C has, so we
  // have to keep track of the strobes ourselves.
  strobe_count_.clear();
  strobe_count_.resize(channel_count_, 0.0);
  last_strobe_.clear();
  last_strobe_.resize(channel_count_, 0); // vector of ints

  // sness - Hmm, the original code isn't doing this, but I don't see
  // how this could possibly work if you don't reset these samples at
  // the beginning of a buffer.  Otherwise, you get carryover from the
  // last step that causes the line after the condition
  // "strobe_count_[o] > 0" to coredump.  Requires further
  // investigation.
  prev_sample_.clear();
  prev_sample_.resize(channel_count_, 10000.0);
  curr_sample_.clear();
  curr_sample_.resize(channel_count_, 5000.0);
  next_sample_.clear();
  next_sample_.resize(channel_count_, 0.0);

  // Skip over the signals and centre frequencies from PZFC (and HCL)
  mrs_natural skip_channels = channel_count_ + channel_count_;

  for (t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {
    for (o = 0; o < channel_count_; o++) {
      // Initialize the strobe
      out(o + skip_channels,t) = 0.0;

      // curr_sample is the sample at time (i - 1)
      prev_sample_[o] = curr_sample_[o];
      curr_sample_[o] = next_sample_[o];
      next_sample_[o] = in(o, t);

      // If the current sample is above threshold, the threshold is raised to
      // the level of the current sample, and decays from there.
      if (curr_sample_[o] >= threshold_[o]) {
        threshold_[o] = curr_sample_[o];
        decay_constant_[o] = threshold_[o] / strobe_decay_samples_;
        // If the current sample is also a peak, then it is a potential strobe
        // point.
        if (prev_sample_[o] < curr_sample_[o]
            && next_sample_[o] < curr_sample_[o]) {
          // If there are no strobes so far in this channel, then the sample
          // is definitely a strobe (this means that the timeout is not
          // respected across frame boundaries. This is a minor bug, but I
          // don't believe that it's serious enough to warrant updating the
          // samples since last strobe all the time.)
          if (strobe_count_[o] > 0) {
            // If there are previous strobes, then calculate the time since
            // the last one. If it's long enough, then this is a strobe point,
            // if not, then just move on.
            int samples_since_last = (t - 1) - last_strobe_[o];
            if (samples_since_last > strobe_timeout_samples_) {
              out(o + skip_channels, t-1) = 1.0;
              strobe_count_[o]++;
              last_strobe_[o] = t;
            }
          } else {
            out(o + skip_channels, t-1) = 1.0;
            strobe_count_[o]++;
            last_strobe_[o] = t;
          }
        }
      }

      // Update the threshold, decaying as necessary
      if (threshold_[o] > decay_constant_[o])
        threshold_[o] -= decay_constant_[o];
      else
        threshold_[o] = 0.0;
    }
  }

  // Now that we've added strobes as a second set of observations
  // after the first, copy the the data from the input to the output.
  for (t = 0; t < ctrl_inSamples_->to<mrs_natural>(); t++) {
    for (o = 0; o < skip_channels; o++) {
      out(o,t) = in(o,t);
    }
  }
}
