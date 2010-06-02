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

using namespace std;
using namespace Marsyas;

AimLocalMax::AimLocalMax(string name):MarSystem("AimLocalMax",name)
{
  initialized = false;
  addControls();
}


AimLocalMax::~AimLocalMax()
{
}

bool 
AimLocalMax::InitializeInternal() {
  strobe_timeout_samples_ = floor(ctrl_timeout_ms_->to<mrs_real>() * israte_ / 1000.0f);
  strobe_decay_samples_ = floor(ctrl_decay_time_ms_->to<mrs_real>() * israte_ / 1000.0f);
  ResetInternal();
}

void 
AimLocalMax::ResetInternal() {
  threshold_.clear();
  threshold_.resize(inObservations_, 0.0f);

  decay_constant_.clear();
  decay_constant_.resize(inObservations_, 1.0f);

  prev_sample_.clear();
  prev_sample_.resize(inObservations_, 10000.0f);
  curr_sample_.clear();
  curr_sample_.resize(inObservations_, 5000.0f);
  next_sample_.clear();
  next_sample_.resize(inObservations_, 0.0f);
}


MarSystem*
AimLocalMax::clone() const
{
  return new AimLocalMax(*this);
}

void 
AimLocalMax::addControls()
{
  addControl("mrs_real/decay_time_ms", 20.0f , ctrl_decay_time_ms_);
  addControl("mrs_real/timeout_ms", 3.0f , ctrl_timeout_ms_);
}

void
AimLocalMax::myUpdate(MarControlPtr sender)
{
  if (!initialized) {
    InitializeInternal();
    initialized = true;
  }

	(void) sender;
  MRSDIAG("AimLocalMax.cpp - AimLocalMax:myUpdate");
  // The extra observation is for the strobe channel
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() + 1, NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>());
  ctrl_onObsNames_->setValue("AimLocalMax_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}


void
AimLocalMax::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  int strobe_count = 0;
  int last_strobe;
  int samples_since_last;

  // Zero out the strobe channel
  for (t = 0; t < inSamples_; t++) {
    out(inObservations_,t) = 0.0;
  }

  for (o = 0; o < inObservations_; o++) {
    for (t = 0; t < inSamples_; t++) {
      out(o,t) = in(o,t);

      // curr_sample is the sample at time (i - 1)
      prev_sample_[o] = curr_sample_[o];
      curr_sample_[o] = next_sample_[o];
      next_sample_[o] = in(o, t);
      // Copy input signal to output signal
      out(o, t) = in(o, t);

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
          // int count = output_.strobe_count(o);
          if (strobe_count > 0) {
            // If there are previous strobes, then calculate the time since
            // the last one. If it's long enough, then this is a strobe point,
            // if not, then just move on.
            // int samples_since_last = (t - 1) - output_.strobe(o, strobe_count - 1);
            if (samples_since_last > strobe_timeout_samples_) {
              out(inObservations_, t) = 1.0;
              strobe_count++;
              last_strobe = t;
            }
          } else {
            out(inObservations_, t) = 1.0;
            strobe_count++;
            last_strobe = t;
          }
        }
      }

      // Update the threshold, decaying as necessary
      if (threshold_[o] > decay_constant_[o])
        threshold_[o] -= decay_constant_[o];
      else
        threshold_[o] = 0.0f;
    }
  }


}
