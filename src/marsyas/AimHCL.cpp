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

#include "AimHCL.h"

using namespace std;
using namespace Marsyas;

AimHCL::AimHCL(string name):MarSystem("AimHCL",name)
{
  addControls();
}


AimHCL::~AimHCL()
{
}


MarSystem*
AimHCL::clone() const
{
  return new AimHCL(*this);
}

void 
AimHCL::addControls()
{
  addControl("mrs_bool/do_lowpass", true , ctrl_do_lowpass_);
  addControl("mrs_bool/do_log", false , ctrl_do_log_);
  addControl("mrs_real/lowpass_cutoff", 1200.0 , ctrl_lowpass_cutoff_);
  addControl("mrs_natural/lowpass_order", 2 , ctrl_lowpass_order_);
}

bool 
AimHCL::InitializeInternal() {
  time_constant_ = 1.0f / (2.0f * PI * ctrl_lowpass_cutoff_->to<mrs_real>());
  // channel_count_ = input.channel_count();
  // output_.Initialize(input);
  ResetInternal();
  return true;
}

void 
AimHCL::ResetInternal() {
  xn_ = 0.0f;
  yn_ = 0.0f;
  yns_.clear();
  yns_.resize(inObservations_);
  for (int c = 0; c < inObservations_; ++c) {
    yns_[c].resize(ctrl_lowpass_order_->to<mrs_natural>(), 0.0f);
  }
}

void
AimHCL::myUpdate(MarControlPtr sender)
{
  if (!initialized) {
    InitializeInternal();
    initialized = true;
  }

  (void) sender;
  MRSDIAG("AimHCL.cpp - AimHCL:myUpdate");
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>());
  ctrl_onObsNames_->setValue("AimHCL_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}


//
// With do_log, the signal is first scaled up so that values <1.0 become
// negligible. This just rescales the sample values to fill the range of a
// 16-bit signed integer, then we lose the bottom bit of resolution. If the
// signal was sampled at 16-bit resolution, there shouldn't be anything to
// speak of there anyway. If it was sampled using a higher resolution, then
// some data will be discarded.
//
void
AimHCL::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  for (o = 0; o < inObservations_; o++) {
    for (t = 0; t < inSamples_; t++) {
      // out(o,t) = in(o,t);
      if (in(o,t) < 0.0f) {
        out(o, t) = 0.0f;
      } else {
        float s = in(o,t);
        if (ctrl_do_log_->to<mrs_bool>()) {
          s *= pow(2.0f, 15);
          if (s < 1.0f) s = 1.0f;
          s = 20.0f * log10(s);
        }
        out(c, t) = s;
      }
    }
    if (ctrl_do_lowpass_->to<mrs_bool>()) {
      float b = exp(-1.0f / (israte_ * time_constant_));
      float gain = 1.0f / (1.0f - b);
      for (int j = 0; j < ctrl_lowpass_order_->to<mrs_natural>(); j++) {
        for (int k = 0; k < onSamples_; ++k) {
          xn_ = out(o,k);
          yn_ = xn_ + b * yns_[o][j];
          yns_[o][j] = yn_;
          out(c, k) = yn_ / gain;
        }
      }
    }
  }
}
